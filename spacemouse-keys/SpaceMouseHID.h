#ifndef SpaceMouseHID_h
#define SpaceMouseHID_h
#include "Arduino.h"


// make sure that it is a supported Architecture
#ifndef ARDUINO_ARCH_AVR
#error "Unsupported Architecture"
#endif

#include "PluggableUSB.h"
#include "HID.h"

#define USBController_D_HIDREPORT(length) { 9, 0x21, 0x11, 0x01, 0, 1, 0x22, lowByte(length), highByte(length) }
typedef struct {
	InterfaceDescriptor hid;
  HIDDescDescriptor desc;
  EndpointDescriptor in;
	EndpointDescriptor out;
} USBControllerHIDDescriptor;

typedef union {
	uint8_t dataInHeader[0];
	uint8_t dataInBlock[0];
	uint8_t dataOutHeader[0];
	uint8_t dataOutBlock[0];
	struct {
		uint8_t dataIn;
		uint8_t dataOut;
	};
} USBControllerDataPacket;

/*static const uint8_t USBControllerReportDescriptor[] PROGMEM = {
	0x06, 0x00, 0xff,              // USAGE_PAGE (vendor defined page 1)
	0x09, 0x00,                    // USAGE (Undefined)
	0xa1, 0x01,                    // COLLECTION (Application)
	0x85, 0x01,                    //   REPORT_ID (1)
	0x09, 0x00,                    //   USAGE (Undefined)
	0x15, 0x00,                    //   LOGICAL_MINIMUM (0)
	0x26, 0xff, 0x00,              //   LOGICAL_MAXIMUM (255)
	0x75, 0x08,                    //   REPORT_SIZE (8)
	0x95, 0x01,                    //   REPORT_COUNT (1)
	0x81, 0x06,                    //   INPUT (Data,Var,Rel)
	0x85, 0x02,                    //   REPORT_ID (2)
	0x09, 0x00,                    //   USAGE (Undefined)
	0x15, 0x00,                    //   LOGICAL_MINIMUM (0)
	0x26, 0xff, 0x00,              //   LOGICAL_MAXIMUM (255)
	0x75, 0x08,                    //   REPORT_SIZE (8)
	0x95, 0x01,                    //   REPORT_COUNT (1)
	0x91, 0x06,                    //   OUTPUT (Data,Var,Rel)
	0xc0                           // END_COLLECTION
};*/

static const uint8_t USBControllerReportDescriptor[] PROGMEM = {
    0x05, 0x01,       // Usage Page (Generic Desktop)
    0x09, 0x08,       // Usage (Multi-Axis)
    0xA1, 0x01,       // Collection (Application)
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
#ifdef ADV_HID_REL
    0x81, 0x06, //     Input (Data,Var,Rel,No Wrap,Linear,Preferred State,No Null Position) // see above
#else
    0x81, 0x02, // Input (variable,absolute)     // see above
#endif
    0xC0,             // End Collection
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
#ifdef ADV_HID_REL
    0x81, 0x06, //     Input (Data,Var,Rel,No Wrap,Linear,Preferred State,No Null Position) // see above
#else
    0x81, 0x02, // Input (variable,absolute)     // see above
#endif
    0xC0,                // End Collection
    0xa1, 0x00,          // Collection (Physical)
    0x85, 0x03,          //  Report ID (3)
    0x15, 0x00,          //   Logical Minimum (0)
    0x25, 0x01,          //    Logical Maximum (1)
    0x75, 0x01,          //    Report Size (1)
    0x95, HIDMAXBUTTONS,          //    Report Count (32) 
    0x05, 0x09,          //    Usage Page (Button)
    0x19, 1,             //    Usage Minimum (Button #1)
    0x29, HIDMAXBUTTONS,          //    Usage Maximum (Button #24) 
    0x81, 0x02,          //    Input (variable,absolute)
    0xC0,             // End Collection

    0xA1, 0x02, //   Collection (Logical)
    0x85, 0x04, //     Report ID (4)
    0x05, 0x08, //     Usage Page (LEDs)
    0x09, 0x4B, //     Usage (Generic Indicator)
    0x15, 0x00, //     Logical Minimum (0)
    0x25, 0x01, //     Logical Maximum (1)
    0x95, 0x01, //     Report Count (1)
    0x75, 0x01, //     Report Size (1)
    0x91, 0x02, //     Output (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
    0x95, 0x01, //     Report Count (1)
    0x75, 0x07, //     Report Size (7)
    0x91, 0x03, //     Output (Const,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
    0xC0,       //   End Collection
0xc0      // END_COLLECTION
};

#define USBControllerInterface pluggedInterface
#define USBControllerEndpointIn pluggedEndpoint
#define USBControllerEndpointOut (pluggedEndpoint + 1)
#define USBControllerTX USBControllerEndpointIn
#define USBControllerRX USBControllerEndpointOut

class SpaceMouseHID : public PluggableUSBModule {
public:
	SpaceMouseHID();
	int write(const uint8_t *buffer, size_t size);
    int SendReport(uint8_t id, const void* data, int len);
	int read();
private:

protected:
	uint8_t endpointTypes[2];
	uint8_t protocol;
	uint8_t idle;

	int getInterface(uint8_t *interfaceNumber);
	int getDescriptor(USBSetup &setup);
	bool setup(USBSetup &setup);
};

#endif // SpaceMouseHID_h import guard