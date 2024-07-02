// hidInterface.h

// This portion sets up the communication with the 3DConnexion software. The communication protocol is created here.
// hidReportDescriptor webpage can be found here: https://eleccelerator.com/tutorial-about-usb-hid-report-descriptors/
// Altered physical, logical range to ranges the 3DConnexion software expects by Daniel_1284580.
#define HIDMAXBUTTONS 24 // was 32 by Daniel_1284580, but 24 in the comment... must be multiple of 8!

static const uint8_t _hidReportDescriptor[] PROGMEM = {
    0x05, 0x01,          // Usage Page (Generic Desktop)
    0x09, 0x08,          // Usage (Multi-Axis)
    0xA1, 0x01,          // Collection (Application)
    0xa1, 0x00,          // Collection (Physical)
    0x85, 0x01,          // Report ID (1)
    0x16, 0xA2, 0xFE,    // Logical Minimum (-350) (0xFEA2 in little-endian)
    0x26, 0x5E, 0x01,    // Logical Maximum (350) (0x015E in little-endian)
    0x36, 0x88, 0xFA,    // Physical Minimum (-1400) (0xFA88 in little-endian)
    0x46, 0x78, 0x05,    // Physical Maximum (1400) (0x0578 in little-endian)
    0x09, 0x30,          // Usage (X)
    0x09, 0x31,          // Usage (Y)
    0x09, 0x32,          // Usage (Z)
    0x75, 0x10,          // Report Size (16)
    0x95, 0x03,          // Report Count (3)
    0x81, 0x02,          // Input (variable,absolute)
    0xC0,                // End Collection
    0xa1, 0x00,          // Collection (Physical)
    0x85, 0x02,          // Report ID (2)
    0x16, 0xA2, 0xFE,    // Logical Minimum (-350)
    0x26, 0x5E, 0x01,    // Logical Maximum (350)
    0x36, 0x88, 0xFA,    // Physical Minimum (-1400)
    0x46, 0x78, 0x05,    // Physical Maximum (1400)
    0x09, 0x33,          // Usage (RX)
    0x09, 0x34,          // Usage (RY)
    0x09, 0x35,          // Usage (RZ)
    0x75, 0x10,          // Report Size (16)
    0x95, 0x03,          // Report Count (3)
    0x81, 0x02,          // Input (variable,absolute)
    0xC0,                // End Collection
    0xa1, 0x00,          // Collection (Physical)
    0x85, 0x03,          //  Report ID (3)
    0x15, 0x00,          //   Logical Minimum (0)
    0x25, 0x01,          //    Logical Maximum (1)
    0x75, 0x01,          //    Report Size (1)
    0x95, HIDMAXBUTTONS, //    Report Count (24) // has been 32 in earlier versions
    0x05, 0x09,          //    Usage Page (Button)
    0x19, 1,             //    Usage Minimum (Button #1)
    0x29, HIDMAXBUTTONS, //    Usage Maximum (Button #24) // has been 32 in earlier versions
    0x81, 0x02,          //    Input (variable,absolute)
    0xC0,
    0xC0};

bool send_command(int16_t rx, int16_t ry, int16_t rz, int16_t x, int16_t y, int16_t z, uint8_t *keys, int debug);