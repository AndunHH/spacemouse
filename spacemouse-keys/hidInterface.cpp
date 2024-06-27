#include <Arduino.h>
#include "config.h"
#include "hidInterface.h"
// Include inbuilt Arduino HID library by NicoHood: https://github.com/NicoHood/HID
#include "HID.h"

// Array with the bitnumbers, which should be assign keys to buttons
uint8_t bitNumber[NUMHIDKEYS] = BUTTONLIST;
#if HIDUPDATERATE_MS > 0
unsigned long lastHIDsentRep1 = 0; // time from millis(), when the last HID report was sent
unsigned long lastHIDsentRep2 = 0; // time from millis(), when the last HID report was sent
#endif

// Function to send translation and rotation data to the 3DConnexion software using the HID protocol outlined earlier.
// Three sets of data are sent: translation, rotation and key events
// For each, a 16bit integer is split into two using bit shifting. The first is mangitude and the second is direction.
// With regard to the _hidReportDescriptor: the values should be within +/- 350.
// returns true, if new data was sent
bool send_command(int16_t rx, int16_t ry, int16_t rz, int16_t x, int16_t y, int16_t z, uint8_t *keys, int debug)
{
  bool hasSentNewData = false;
  uint8_t trans[6] = {(byte)(x & 0xFF), (byte)(x >> 8), (byte)(y & 0xFF), (byte)(y >> 8), (byte)(z & 0xFF), (byte)(z >> 8)};

#if HIDUPDATERATE_MS > 0 // send new data only every HIDUPDATERATE_MS ms
  if (millis() - lastHIDsentRep1 > HIDUPDATERATE_MS)
  {
    lastHIDsentRep1 = millis();
    HID().SendReport(1, trans, 6); // send new values
    hasSentNewData = true;         // return value
  }
#else
  HID().SendReport(1, trans, 6); // send new values
  hasSentNewData = true;         // return value
#endif

  uint8_t rot[6] = {(byte)(rx & 0xFF), (byte)(rx >> 8), (byte)(ry & 0xFF), (byte)(ry >> 8), (byte)(rz & 0xFF), (byte)(rz >> 8)};

#if HIDUPDATERATE_MS > 0 // send new data only every HIDUPDATERATE_MS ms
  if (millis() - lastHIDsentRep2 > HIDUPDATERATE_MS)
  {
    lastHIDsentRep2 = millis();
    HID().SendReport(2, rot, 6);
    hasSentNewData = true; // return value
  }
#else
  HID().SendReport(2, rot, 6);
  hasSentNewData = true; // return value
#endif

#if (NUMKEYS > 0)
  static uint8_t data[HIDMAXBUTTONS / 8]; // array to be sent over hid
  static uint8_t prevData[HIDMAXBUTTONS / 8];
  for (int i = 0; i < HIDMAXBUTTONS / 8; i++) // init or empty this array
  {
    data[i] = 0;
    // do not fill prevData with zeros, as this would invalidate the approach to store old data
  }

  for (int i = 0; i < NUMHIDKEYS; i++)
  {
    // check for every key if it is pressed
    if (keys[i])
    {
      // set the according bit in the data bytes
      // byte no.: bitNumber[i] / 8
      // bit no.:  bitNumber[i] modulo 8
      data[(bitNumber[i] / 8)] = (1 << (bitNumber[i] % 8));
      if (debug == 8)
      {
        // debug the key board outputs
        Serial.print("bitnumber: ");
        Serial.print(bitNumber[i]);
        Serial.print(" -> data[");
        Serial.print((bitNumber[i] / 8));
        Serial.print("] = ");
        Serial.print("0x");
        Serial.println(data[(bitNumber[i] / 8)], HEX);
      }
    }
  }

  if (memcmp(data, prevData, HIDMAXBUTTONS / 8) != 0)
  { // compare previous and actual data
    HID().SendReport(3, data, HIDMAXBUTTONS / 8);
    memcpy(prevData, data, HIDMAXBUTTONS / 8); // copy actual data to previous data
    hasSentNewData = true;                     // return value
  }
#endif
  return hasSentNewData;
}