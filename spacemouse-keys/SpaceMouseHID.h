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

static const uint8_t SpaceMouseReportDescriptor[] PROGMEM = {
    0x05, 0x01,       // Usage Page (Generic Desktop)
    0x09, 0x08,       // Usage (Multi-Axis)
    0xA1, 0x01,       // Collection (Application)
                      // Report 1: Translation
    0xa1, 0x00,       // Collection (Physical)
    0x85, 0x01,       // Report ID (1)
    0x16, 0xA2, 0xFE, // Logical Minimum (-350) (0xFEA2 in little-endian)
    0x26, 0x5E, 0x01, // Logical Maximum (350) (0x015E in little-endian)
    0x36, 0x88, 0xFA, // Physical Minimum (-1400) (0xFA88 in little-endian)
    0x46, 0x78, 0x05, // Physical Maximum (1400) (0x0578 in little-endian)
    0x09, 0x30,       // Usage (X)
    0x09, 0x31,       // Usage (Y)
    0x09, 0x32,       // Usage (Z)
    0x75, 0x10,       // Report Size (16)
    0x95, 0x03,       // Report Count (3)
#ifdef ADV_HID_REL    // see Advanced HID settings in config_sample.h
    0x81, 0x06,       //     Input (Data,Var,Rel,No Wrap,Linear,Preferred State,No Null Position)
#else
    0x81, 0x02, // Input (variable,absolute)
#endif
    0xC0,             // End Collection
                      // Report 2: Rotation
    0xa1, 0x00,       // Collection (Physical)
    0x85, 0x02,       // Report ID (2)
    0x16, 0xA2, 0xFE, // Logical Minimum (-350)
    0x26, 0x5E, 0x01, // Logical Maximum (350)
    0x36, 0x88, 0xFA, // Physical Minimum (-1400)
    0x46, 0x78, 0x05, // Physical Maximum (1400)
    0x09, 0x33,       // Usage (RX)
    0x09, 0x34,       // Usage (RY)
    0x09, 0x35,       // Usage (RZ)
    0x75, 0x10,       // Report Size (16)
    0x95, 0x03,       // Report Count (3)
#ifdef ADV_HID_REL    // see Advanced HID settings in config_sample.h
    0x81, 0x06,       //     Input (Data,Var,Rel,No Wrap,Linear,Preferred State,No Null Position)
#else
    0x81, 0x02, // Input (variable,absolute)
#endif
    0xC0,                // End Collection
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
    // The following commeted block comes from a Space Navigator and defines something vendor specific.
    // They are suspected to have something to do with speed or configuration, but no usb traffic can be seen here.
    // Therefore they are not active...
    /*0x06, 0x00, 0xFF, //   Usage Page (Vendor Defined 0xFF00)
    0x09, 0x01,       //   Usage (0x01)
    0xA1, 0x02,       //   Collection (Logical)
    0x15, 0x80,       //     Logical Minimum (-128)
    0x25, 0x7F,       //     Logical Maximum (127)
    0x75, 0x08,       //     Report Size (8)
    0x09, 0x3A,       //     Usage (0x3A)
    0xA1, 0x02,       //     Collection (Logical)
    0x85, 0x05,       //       Report ID (5)
    0x09, 0x20,       //       Usage (0x20)
    0x95, 0x01,       //       Report Count (1)
    0xB1, 0x02,       //       Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
    0xC0,             //     End Collection
    0xA1, 0x02,       //     Collection (Logical)
    0x85, 0x06,       //       Report ID (6)
    0x09, 0x21,       //       Usage (0x21)
    0x95, 0x01,       //       Report Count (1)
    0xB1, 0x02,       //       Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
    0xC0,             //     End Collection
    0xA1, 0x02,       //     Collection (Logical)
    0x85, 0x07,       //       Report ID (7)
    0x09, 0x22,       //       Usage (0x22)
    0x95, 0x01,       //       Report Count (1)
    0xB1, 0x02,       //       Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
    0xC0,             //     End Collection
    0xA1, 0x02,       //     Collection (Logical)
    0x85, 0x08,       //       Report ID (8)
    0x09, 0x23,       //       Usage (0x23)
    0x95, 0x07,       //       Report Count (7)
    0xB1, 0x02,       //       Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
    0xC0,             //     End Collection
    0xA1, 0x02,       //     Collection (Logical)
    0x85, 0x09,       //       Report ID (9)
    0x09, 0x24,       //       Usage (0x24)
    0x95, 0x07,       //       Report Count (7)
    0xB1, 0x02,       //       Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
    0xC0,             //     End Collection
    0xA1, 0x02,       //     Collection (Logical)
    0x85, 0x0A,       //       Report ID (10)
    0x09, 0x25,       //       Usage (0x25)
    0x95, 0x07,       //       Report Count (7)
    0xB1, 0x02,       //       Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
    0xC0,             //     End Collection
    0xA1, 0x02,       //     Collection (Logical)
    0x85, 0x0B,       //       Report ID (11)
    0x09, 0x26,       //       Usage (0x26)
    0x95, 0x01,       //       Report Count (1)
    0xB1, 0x02,       //       Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
    0xC0,             //     End Collection
    0xA1, 0x02,       //     Collection (Logical)
    0x85, 0x13,       //       Report ID (19)
    0x09, 0x2E,       //       Usage (0x2E)
    0x95, 0x01,       //       Report Count (1)
    0xB1, 0x02,       //       Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
    0xC0,             //     End Collection
    0xC0,             //   End Collection*/
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
    int readReport(uint8_t reportId);
    void printAllReports();
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

protected:
    uint8_t endpointTypes[2];
    uint8_t protocol;
    uint8_t idle;

    int getInterface(uint8_t *interfaceNumber);
    int getDescriptor(USBSetup &setup);
    bool setup(USBSetup &setup);
};

// Replacement for global singleton.
// This function prevents static-initialization-order-fiasco
// https://isocpp.org/wiki/faq/ctors#static-init-order-on-first-use
SpaceMouseHID_ &SpaceMouseHID();

#endif // SpaceMouseHID_h import guard