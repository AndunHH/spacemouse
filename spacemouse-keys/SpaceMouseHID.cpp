/*
This class behaves as HID Device with two endpoints for in and out

It was created by reverse-engineering a Space Navigator and relating to the HID Library by Nico Hood for reference. https://github.com/NicoHood/HID

This code is based on https://forum.arduino.cc/t/solved-unable-to-receive-hid-reports-from-computer-using-pluggableusb/596793
*/

#include <Arduino.h>
#include "config.h"

#include "SpaceMouseHID.h"

SpaceMouseHID_::SpaceMouseHID_() : PluggableUSBModule(2, 1, endpointTypes)
{
	endpointTypes[0] = EP_TYPE_INTERRUPT_IN;
	endpointTypes[1] = EP_TYPE_INTERRUPT_OUT;
	PluggableUSB().plug(this);
	nextState = ST_INIT; // init state machine with init state
	ledState = false;
}

int SpaceMouseHID_::getInterface(uint8_t *interfaceNumber)
{
	interfaceNumber[0] += 1;
	SpaceMouseHIDDescriptor interfaceDescriptor = {
		D_INTERFACE(USBControllerInterface, 2, USB_DEVICE_CLASS_HUMAN_INTERFACE, 0, 0),
		SPACEMOUSE_D_HIDREPORT(sizeof(SpaceMouseReportDescriptor)),
		D_ENDPOINT(USB_ENDPOINT_IN(USBControllerEndpointIn), USB_ENDPOINT_TYPE_INTERRUPT, USB_EP_SIZE, 0),
		D_ENDPOINT(USB_ENDPOINT_OUT(USBControllerEndpointOut), USB_ENDPOINT_TYPE_INTERRUPT, USB_EP_SIZE, 0),
	};
	return USB_SendControl(0, &interfaceDescriptor, sizeof(interfaceDescriptor));
}

int SpaceMouseHID_::getDescriptor(USBSetup &setup)
{
	// code copied and modified from NicoHood's HID-Project
	// check if it is a HID class Descriptor request
	if (setup.bmRequestType != REQUEST_DEVICETOHOST_STANDARD_INTERFACE)
	{
		return 0;
	}
	if (setup.wValueH != HID_REPORT_DESCRIPTOR_TYPE)
	{
		return 0;
	}

	// In a HID Class Descriptor wIndex cointains the interface number
	if (setup.wIndex != pluggedInterface)
	{
		return 0;
	}

	protocol = HID_REPORT_PROTOCOL;

	return USB_SendControl(TRANSFER_PGM, SpaceMouseReportDescriptor, sizeof(SpaceMouseReportDescriptor));
}

bool SpaceMouseHID_::setup(USBSetup &setup)
{
	// code copied from NicoHood's HID-Project
	if (pluggedInterface != setup.wIndex)
	{
		return false;
	}

	uint8_t request = setup.bRequest;
	uint8_t requestType = setup.bmRequestType;

	if (requestType == REQUEST_DEVICETOHOST_CLASS_INTERFACE)
	{
		if (request == HID_GET_REPORT)
		{
			// TODO: HID_GetReport();
			return true;
		}
		if (request == HID_GET_PROTOCOL)
		{
			// TODO: Send8(protocol);
			return true;
		}
	}

	if (requestType == REQUEST_HOSTTODEVICE_CLASS_INTERFACE)
	{
		if (request == HID_SET_PROTOCOL)
		{	
			protocol = setup.wValueL;
			return true;
		}
		if (request == HID_SET_IDLE)
		{
			idle = setup.wValueL;
			return true;
		}
		if (request == HID_SET_REPORT)
		{
			// If you press "Calibrate" in the windows driver of a _SpaceNavigator_ the following setup request is sent:
			// wValue: 0x0307
			// wIndex: 0 (0x0000)
			// wLength: 2
			// Data Fragment: 0700
			// Unfortunately, we are simulating a _SpaceMouse Pro Wireless (cabled)_, because it has more than two buttons
			// With this SM pro, the windows driver is NOT sending this status report and their is no point in waiting for it...
			return true;
		}
	}

	return false;
}

int SpaceMouseHID_::write(const uint8_t *buffer, size_t size)
{
	return USB_Send(USBControllerTX, buffer, size);
}

/// @brief Send a HID Report
/// @param id Report Id of the data to be sent
/// @param data Pointer to the data array
/// @param len  Length of the data
/// @return Length of data sent (including 1 byte for report id)
int SpaceMouseHID_::SendReport(uint8_t id, const void *data, int len)
{
	auto ret = USB_Send(USBControllerTX, &id, 1);
	if (ret < 0)
		return ret;
	auto ret2 = USB_Send(USBControllerTX | TRANSFER_RELEASE, data, len);
	if (ret2 < 0)
		return ret2;
	return ret + ret2;
}

/// @brief Reads a single byte from the interface, if available
/// @return Returns the byte or zero
int SpaceMouseHID_::readSingleByte()
{
	if (USB_Available(USBControllerRX))
	{
		return USB_Recv(USBControllerRX);
	}
	else
	{
		return 0;
	}
}

/// @brief Try to read some reports and print them
/// @return  Returns nothing
void SpaceMouseHID_::printAllReports()
{
	uint8_t numBytes = USB_Available(USBControllerRX);
	if (numBytes >= 2)
	{
		uint8_t data[2] = {0};
		USB_Recv(USBControllerRX, data, numBytes);
		for (int i = 0; i < numBytes; i++)
		{
			Serial.print(data[i], HEX);
			Serial.print(", ");
		}
		Serial.println(" ");
	}
	else
	{
		// Serial.print(".");
	}
}

/// @brief Check for LED hid reports (report Id: 4). This empties the RX buffer.
/// @return  Returns the led status
bool SpaceMouseHID_::updateLEDState()
{
	uint8_t numBytes = USB_Available(USBControllerRX);
	if (numBytes >= 2)
	{
		uint8_t data[2] = {0};
		USB_Recv(USBControllerRX, data, 2);
		if (data[0] == 4) // LED report id: 4
		{
			if (data[1] == 1) // if 1, led on!
			{
				ledState = true;
				//Serial.println("led on!");
			}
			else
			{
				ledState = false;
				//Serial.println("led off!");
			}
		}
	}
	return ledState;
}

/// @brief Get the LED state, which shall be updated regularly by calling updateLEDstate()
/// @return Boolean LED state
bool SpaceMouseHID_::getLEDState()
{
	return ledState;
}

bool SpaceMouseHID_::send_command(int16_t rx, int16_t ry, int16_t rz, int16_t x, int16_t y, int16_t z, uint8_t *keys, int debug)
{
	unsigned long now = millis();

	bool hasSentNewData = false; // this value will be returned

#if (NUMKEYS > 0)
	static uint8_t keyData[HIDMAXBUTTONS / 8];	   // key data to be sent via HID
	static uint8_t prevKeyData[HIDMAXBUTTONS / 8]; // previous key data
	prepareKeyBytes(keys, keyData, debug);		   // sort the bytes from keys into the bits in keyData
#endif

#ifdef ADV_HID_JIGGLE
	static bool toggleValue; // variable to track if values shall be jiggled or not
#endif

	switch (nextState) // state machine
	{
	case ST_INIT:
		// init the variables
		lastHIDsentRep = now;
		nextState = ST_START;
#ifdef ADV_HID_JIGGLE
		toggleValue = false;
#endif
		break;
	case ST_START:
		// Evaluate everytime, without waiting for 8ms
		if (countTransZeros < 3 || countRotZeros < 3 || (x != 0 || y != 0 || z != 0 || rx != 0 || ry != 0 || rz != 0))
		{
			// if one of the values is not zero,
			// or not all zero data packages are sent (sent 3 of them)
			// start sending data
			nextState = ST_SENDTRANS;
		}
		else
		{
// if nothing is to be sent, check for keys. If no keys, don't change state
#if (NUMKEYS > 0)
			if (memcmp(keyData, prevKeyData, HIDMAXBUTTONS / 8) != 0)
			// compare key data to previous key data
			{
				nextState = ST_SENDKEYS;
			}
#endif
			if (nextState == ST_START && IsNewHidReportDue(now))
			{
				// if we are not leaving the start state and
				// we are waiting here for more than the update rate,
				// keep the timestamp for the last sent package nearby
				lastHIDsentRep = now - HIDUPDATERATE_MS;
			}
		}
		break;
	case ST_SENDTRANS:
		// send translation data, if the 8 ms from the last hid report have past
		if (IsNewHidReportDue(now))
		{
			uint8_t trans[12] = {(byte)(x & 0xFF), (byte)(x >> 8), (byte)(y & 0xFF), (byte)(y >> 8), (byte)(z & 0xFF), (byte)(z >> 8),
				(byte)(rx & 0xFF), (byte)(rx >> 8), (byte)(ry & 0xFF), (byte)(ry >> 8), (byte)(rz & 0xFF), (byte)(rz >> 8)};

#ifdef ADV_HID_JIGGLE
			jiggleValues(trans, toggleValue); // jiggle the non-zero values, if toggleValue is true
											  // the toggleValue is toggled after sending the rotations, down below
#endif
			SendReport(1, trans, 12); // send new translational values
			lastHIDsentRep += HIDUPDATERATE_MS;
			hasSentNewData = true; // return value

			// if only zeros where send, increment zero counter, otherwise reset it
			if (x == 0 && y == 0 && rz == 0)
			{
				countTransZeros++;
			}
			else
			{
				countTransZeros = 0;
			}
			nextState = ST_START;
		}
		break;
	case ST_SENDROT:
		// send rotational data, if the 8 ms from the last hid report have past
		if (IsNewHidReportDue(now))
		{
			uint8_t rot[6] = {(byte)(rx & 0xFF), (byte)(rx >> 8), (byte)(ry & 0xFF), (byte)(ry >> 8), (byte)(rz & 0xFF), (byte)(rz >> 8)};

#ifdef ADV_HID_JIGGLE
			jiggleValues(rot, toggleValue); // jiggle the non-zero values, if toggleValue is true
			toggleValue ^= true;			// toggle the indicator to jiggle only every second report send
#endif

			SendReport(2, rot, 6);
			lastHIDsentRep += HIDUPDATERATE_MS;
			hasSentNewData = true; // return value
			// if only zeros where send, increment zero counter, otherwise reset it
			if (rx == 0 && ry == 0 && rz == 0)
			{
				countRotZeros++;
			}
			else
			{
				countRotZeros = 0;
			}
// check if the next state should be keys
#if (NUMKEYS > 0)
			if (memcmp(keyData, prevKeyData, HIDMAXBUTTONS / 8) != 0)
			// compare key data to previous key data
			{
				nextState = ST_SENDKEYS;
			}
			else
			{
				// go back to start
				nextState = ST_START;
			}
#else
			// if no keys are used, go to start state after rotations
			nextState = ST_START;
#endif
		}
		break;
#if (NUMKEYS > 0)
	case ST_SENDKEYS:
		// report the keys, if the 8 ms since the last report have past
		if (IsNewHidReportDue(now))
		{
			SendReport(3, keyData, HIDMAXBUTTONS / 8);
			lastHIDsentRep += HIDUPDATERATE_MS;
			memcpy(prevKeyData, keyData, HIDMAXBUTTONS / 8); // copy actual keyData to previous keyData
			hasSentNewData = true;							 // return value
			nextState = ST_START;							 // go back to start
		}
		break;
#endif
	default:
		nextState = ST_START; // go back to start in error?!
		// send nothing if all data is zero
		break;
	}
	return hasSentNewData;
}

// check if a new HID report shall be send
bool SpaceMouseHID_::IsNewHidReportDue(unsigned long now)
{
	// calculate the difference between now and the last time it was sent
	// such a difference calculation is safe with regard to integer overflow after 48 days
	return (now - lastHIDsentRep >= HIDUPDATERATE_MS);
}

// function to add jiggle to the values, if they are not zero.
// jiggle means to set the last bit to zero or one, depending on the parameter lastBit
// lastBit shall be toggled between true and false between repeating calls
bool SpaceMouseHID_::jiggleValues(uint8_t val[6], bool lastBit)
{
	for (uint8_t i = 0; i < 6; i = i + 2)
	{
		if ((val[i] != 0 || val[i + 1] != 0) && lastBit)
		{
			// value is not zero, set last bit to one
			val[i] = val[i] | 1;
		}
		else
		{
			// value is already zero and needs not jiggling, or the last bit shall be forced to zero
			val[i] = val[i] & (0xFE);
		}
	}
	return true;
}

#if (NUMKEYS > 0)
// Takes the data in keys and sort them into the bits of keyData
// Which key from keyData should belong to which byte is defined in bitNumber = BUTTONLIST see config.h
void SpaceMouseHID_::prepareKeyBytes(uint8_t *keys, uint8_t *keyData, int debug)
{
  for (int i = 0; i < HIDMAXBUTTONS / 8; i++) // init or empty this array
  {
    keyData[i] = 0;
  }

  for (int i = 0; i < NUMHIDKEYS; i++)
  {
    // check for every key if it is pressed
    if (keys[i])
    {
      // set the according bit in the data bytes
      // byte no.: bitNumber[i] / 8
      // bit no.:  bitNumber[i] modulo 8
      keyData[(bitNumber[i] / 8)] = (1 << (bitNumber[i] % 8));
      if (debug == 8)
      {
        // debug the key board outputs
        Serial.print("bitnumber: ");
        Serial.print(bitNumber[i]);
        Serial.print(" -> keyData[");
        Serial.print((bitNumber[i] / 8));
        Serial.print("] = ");
        Serial.print("0x");
        Serial.println(keyData[(bitNumber[i] / 8)], HEX);
      }
    }
  }
}
#endif


SpaceMouseHID_ SpaceMouseHID;