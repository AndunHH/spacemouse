/*
This class behaves as HID Device with two endpoints for in and out

It was created by reverse-engineering a Space Navigator and relating to the HID Library by Nico Hood for reference. https://github.com/NicoHood/HID

This code is based on https://forum.arduino.cc/t/solved-unable-to-receive-hid-reports-from-computer-using-pluggableusb/596793
*/

#ifndef SpaceMouseHID_h
#define SpaceMouseHID_h
#include <Arduino.h>

// make sure that it is a supported Architecture
#ifndef ARDUINO_ARCH_AVR
#error "Unsupported Architecture"
#endif

#include "PluggableUSB.h"
#include "HID.h"

#define SPACEMOUSE_D_HIDREPORT(length)                                     \
    {                                                                      \
        9, 0x21, 0x11, 0x01, 0, 1, 0x22, lowByte(length), highByte(length) \
    }
typedef struct
{
    InterfaceDescriptor hid;
    HIDDescDescriptor desc;
    EndpointDescriptor in;
    EndpointDescriptor out;
} SpaceMouseHIDDescriptor;

// The USB VID and PID for this emulated space mouse pro must be set in the boards.txt in arduino IDE or in set_hwids.py in platformIO.

static const uint8_t SpaceMouseReportDescriptor[] PROGMEM = {
    0x05, 0x01,       // Usage Page (Generic Desktop)
    0x09, 0x08,       // Usage (Multi-Axis)
    0xA1, 0x01,       // Collection (Application)
                      // Report 1: Translation
0xA1, 0x00,        //   Collection (Physical)
0x85, 0x01,        //     Report ID (1)
0x16, 0xA2, 0xFE,  //     Logical Minimum (-350)
0x26, 0x5E, 0x01,  //     Logical Maximum (350)
0x36, 0x88, 0xFA,  //     Physical Minimum (-1400)
0x46, 0x78, 0x05,  //     Physical Maximum (1400)
0x55, 0x0C,        //     Unit Exponent (-4)
0x65, 0x11,        //     Unit (System: SI Linear, Length: Centimeter)
0x09, 0x30,        //     Usage (X)
0x09, 0x31,        //     Usage (Y)
0x09, 0x32,        //     Usage (Z)
0x09, 0x33,        //     Usage (Rx)
0x09, 0x34,        //     Usage (Ry)
0x09, 0x35,        //     Usage (Rz)
0x75, 0x10,        //     Report Size (16)
0x95, 0x06,        //     Report Count (6)
0x81, 0x02,        //     Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
0xC0,              //   End Collection
                         // Report 3: Keys  // find #define HIDMAXBUTTONS 32 in config_sample.h
    0xa1, 0x00,          // Collection (Physical)
    0x85, 0x03,          //  Report ID (3)
    0x15, 0x00,          //   Logical Minimum (0)
    0x25, 0x01,          //    Logical Maximum (1)
    0x75, 0x01,          //    Report Size (1)
    0x95, HIDMAXBUTTONS, //    Report Count (32)
    0x05, 0x09,          //    Usage Page (Button)
    0x19, 1,             //    Usage Minimum (Button #1)
    0x29, HIDMAXBUTTONS, //    Usage Maximum (Button #24)
    0x81, 0x02,          //    Input (variable,absolute)
    0xC0,                // End Collection
                         // Report 4: LEDs
    0xA1, 0x02,          //   Collection (Logical)
    0x85, 0x04,          //     Report ID (4)
    0x05, 0x08,          //     Usage Page (LEDs)
    0x09, 0x4B,          //     Usage (Generic Indicator)
    0x15, 0x00,          //     Logical Minimum (0)
    0x25, 0x01,          //     Logical Maximum (1)
    0x95, 0x01,          //     Report Count (1)
    0x75, 0x01,          //     Report Size (1)
    0x91, 0x02,          //     Output (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
    0x95, 0x01,          //     Report Count (1)
    0x75, 0x07,          //     Report Size (7)
    0x91, 0x03,          //     Output (Const,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
    0xC0,                //   End Collection
    0xc0              // END_COLLECTION
};

#define USBControllerInterface pluggedInterface
#define USBControllerEndpointIn pluggedEndpoint
#define USBControllerEndpointOut (pluggedEndpoint + 1)
#define USBControllerTX USBControllerEndpointIn
#define USBControllerRX USBControllerEndpointOut

// Send a HID report every 8 ms
#define HIDUPDATERATE_MS 8

// State machine to track, which report to send next
enum SpaceMouseHIDStates
{
    ST_INIT,      // init variables
    ST_START,     // start to check if something is to be sent
    ST_SENDTRANS, // send translations
    ST_SENDROT,   // send rotations
    ST_SENDKEYS   // send keys
};

class SpaceMouseHID_ : public PluggableUSBModule
{
public:
    SpaceMouseHID_();
    int write(const uint8_t *buffer, size_t size);
    int SendReport(uint8_t id, const void *data, int len);
    int readSingleByte();
    void printAllReports();
    bool updateLEDState();
    bool getLEDState();
    bool send_command(int16_t rx, int16_t ry, int16_t rz, int16_t x, int16_t y, int16_t z, uint8_t *keys, int debug);

private:
    bool IsNewHidReportDue(unsigned long now);
    bool jiggleValues(uint8_t val[6], bool lastBit);

    SpaceMouseHIDStates nextState;
#if (NUMKEYS > 0)
    // Array with the bitnumbers, which should assign keys to buttons
    uint8_t bitNumber[NUMHIDKEYS] = BUTTONLIST;
    void prepareKeyBytes(uint8_t *keys, uint8_t *keyData, int debug);
#endif
    uint8_t countTransZeros = 0; // count how many times, the zero data has been sent
    uint8_t countRotZeros = 0;

    unsigned long lastHIDsentRep; // time from millis(), when the last HID report was sent

    bool ledState;

protected:
    uint8_t endpointTypes[2];
    uint8_t protocol; 
    uint8_t idle;

    int getInterface(uint8_t *interfaceNumber);
    int getDescriptor(USBSetup &setup);
    bool setup(USBSetup &setup);
};

extern SpaceMouseHID_ SpaceMouseHID;

#endif // SpaceMouseHID_h import guard