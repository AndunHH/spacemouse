/*
This class behaves as HID Device with two endpoints for in and out

This code is based on https://forum.arduino.cc/t/solved-unable-to-receive-hid-reports-from-computer-using-pluggableusb/596793
*/

#include "Arduino.h"
#include "config.h"
#include "hidInterface.h"
#include "SpaceMouseHID.h"

SpaceMouseHID::SpaceMouseHID() : PluggableUSBModule(2, 1, endpointTypes)
{
	endpointTypes[0] = EP_TYPE_INTERRUPT_IN;
	endpointTypes[1] = EP_TYPE_INTERRUPT_OUT;
	PluggableUSB().plug(this);
}

int SpaceMouseHID::getInterface(uint8_t *interfaceNumber)
{
	interfaceNumber[0] += 1;
	USBControllerHIDDescriptor interfaceDescriptor = {
		D_INTERFACE(USBControllerInterface, 2, USB_DEVICE_CLASS_HUMAN_INTERFACE, 0, 0),
		USBController_D_HIDREPORT(sizeof(USBControllerReportDescriptor)),
		D_ENDPOINT(USB_ENDPOINT_IN(USBControllerEndpointIn), USB_ENDPOINT_TYPE_INTERRUPT, USB_EP_SIZE, 0),
		D_ENDPOINT(USB_ENDPOINT_OUT(USBControllerEndpointOut), USB_ENDPOINT_TYPE_INTERRUPT, USB_EP_SIZE, 0),
	};
	return USB_SendControl(0, &interfaceDescriptor, sizeof(interfaceDescriptor));
}

int SpaceMouseHID::getDescriptor(USBSetup &setup)
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

	return USB_SendControl(TRANSFER_PGM, USBControllerReportDescriptor, sizeof(USBControllerReportDescriptor));
}

bool SpaceMouseHID::setup(USBSetup &setup)
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
			// do what??
			return true;
		}
	}

	return false;
}

int SpaceMouseHID::write(const uint8_t *buffer, size_t size)
{
	return USB_Send(USBControllerTX, buffer, size);
}

/// @brief Send a HID Report
/// @param id Report Id of the data to be sent
/// @param data Pointer to the data array
/// @param len  Length of the data
/// @return Length of data sent (including 1 byte for report id)
int SpaceMouseHID::SendReport(uint8_t id, const void *data, int len)
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
int SpaceMouseHID::readSingleByte()
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

/// @brief Try to read a report Id with two bytes
/// @param reportId Which shall be the first byte?
/// @return  Returns the byte after the report id, if successfull. Returns -1 if nothing was read. Returns -2, if the reportId is wrong (data are thrown away):
int SpaceMouseHID::readReport(uint8_t reportId)
{
	uint8_t numBytes = USB_Available(USBControllerRX);
	if (numBytes >= 2)
	{
		uint8_t data[2] = {0};
		USB_Recv(USBControllerRX, data, 2);
		/*Serial.print(data[0]);
		Serial.print(" ");
		Serial.print(data[1]);
		Serial.println(" ");*/
		if (data[0] == reportId)
		{
			return data[1];
		}
		else
		{
			return -2;
		}
	}
	else
	{
		return -1;
	}
}

SpaceMouseHID spaceMouse;