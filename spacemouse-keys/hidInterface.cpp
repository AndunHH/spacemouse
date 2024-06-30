#include <Arduino.h>
#include "config.h"
#include "hidInterface.h"
// Include inbuilt Arduino HID library by NicoHood: https://github.com/NicoHood/HID
#include "HID.h"

#if (NUMKEYS > 0)
// Array with the bitnumbers, which should be assign keys to buttons
uint8_t bitNumber[NUMHIDKEYS] = BUTTONLIST;
#endif

#if HIDUPDATERATE_MS > 0
unsigned long lastHIDsentRep = 0; // time from millis(), when the last HID report was sent
unsigned long now = 0;            // time from millis()
#endif

// Function to send translation and rotation data to the 3DConnexion software using the HID protocol outlined earlier.
// Three sets of data are sent: translation, rotation and key events
// For each, a 16bit integer is split into two using bit shifting. The first is mangitude and the second is direction.
// With regard to the _hidReportDescriptor: the values should be within +/- 350.
// returns true, if new data was sent
bool send_command(int16_t rx, int16_t ry, int16_t rz, int16_t x, int16_t y, int16_t z, uint8_t *keys, int debug)
{
#if HIDUPDATERATE_MS > 0
  now = millis();
#endif

#if JIGGLEVALUES > 0
  // Manipulate the non-zero values a little bit, see more details in config.h
  static boolean jiggle;
#endif

  bool hasSentNewData = false;
  uint8_t trans[6] = {(byte)(x & 0xFF), (byte)(x >> 8), (byte)(y & 0xFF), (byte)(y >> 8), (byte)(z & 0xFF), (byte)(z >> 8)};
  uint8_t rot[6] = {(byte)(rx & 0xFF), (byte)(rx >> 8), (byte)(ry & 0xFF), (byte)(ry >> 8), (byte)(rz & 0xFF), (byte)(rz >> 8)};

#if HIDUPDATERATE_MS > 0
  if (((x != 0 || y != 0 || z != 0 || rx != 0 || ry != 0 || rz != 0) && (now - lastHIDsentRep > HIDUPDATERATE_MS)) || (now - lastHIDsentRep > HIDUPDATERATESLOW_MS))
  // send new data every HIDUPDATERATE_MS
  // send zero data only every HIDUPDATERATESLOW_MS
  {
#if JIGGLEVALUES > 0
    // check, if the data shall be jiggled, if they are not zero for spacenavd
    if (jiggle)
    { // jiggling is only done every second send
      for (int i = 0; i < 5; i = i + 2)
      {
        if (trans[i] != 0)
        {
          trans[i] = trans[i] ^ ((uint8_t)1); // toggle the smallest bit
        }
        if (rot[i] != 0)
        {
          rot[i] = rot[i] ^ ((uint8_t)1); // toggle the smallest bit
        }
      }
    }
#endif
    lastHIDsentRep = now;
    HID().SendReport(1, trans, 6); // send new values
    HID().SendReport(2, rot, 6);
    hasSentNewData = true; // return value
  }
#else
  HID().SendReport(1, trans, 6); // send new values
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

#if JIGGLEVALUES > 0
  if (hasSentNewData)
  {
    // Toogle the jiggle flag
    jiggle = !jiggle;
  }
#endif

  return hasSentNewData;
}