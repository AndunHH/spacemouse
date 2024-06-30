#include <Arduino.h>
#include "config.h"
#include "hidInterface.h"
// Include inbuilt Arduino HID library by NicoHood: https://github.com/NicoHood/HID
#include "HID.h"

#if (NUMKEYS > 0)
// Array with the bitnumbers, which should be assign keys to buttons
uint8_t bitNumber[NUMHIDKEYS] = BUTTONLIST;
#endif

// Function to send translation and rotation data to the 3DConnexion software using the HID protocol outlined earlier. Two sets of data are sent: translation and then rotation.
// For each, a 16bit integer is split into two using bit shifting. The first is mangitude and the second is direction.
void send_command(int16_t rx, int16_t ry, int16_t rz, int16_t x, int16_t y, int16_t z, uint8_t *keys, int debug)
{
  uint8_t trans[6] = {(byte)(x & 0xFF), (byte)(x >> 8), (byte)(y & 0xFF), (byte)(y >> 8), (byte)(z & 0xFF), (byte)(z >> 8)};
  HID().SendReport(1, trans, 6);
  uint8_t rot[6] = {(byte)(rx & 0xFF), (byte)(rx >> 8), (byte)(ry & 0xFF), (byte)(ry >> 8), (byte)(rz & 0xFF), (byte)(rz >> 8)};
  HID().SendReport(2, rot, 6);

#if (NUMKEYS > 0)
  static uint8_t data[HIDMAXBUTTONS / 8];     // array to be sent over hid
  for (int i = 0; i < HIDMAXBUTTONS / 8; i++) // init or empty this array
  {
    data[i] = 0;
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

  HID().SendReport(3, data, HIDMAXBUTTONS / 8);

#endif
}
