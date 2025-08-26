# SpaceMouse Wirless
This page contains informations about a 3Dconnexion Space Mouse Wirless, which has been observed via wireshark:

SpaceMouse Wirelss HID Report
Recorded with Wireshark. 
Decoded with https://eleccelerator.com/usbdescreqparser/


## Update Rate
In move_and_rot.pcapng:
The translation and rotation seem to be sent at a speed of 12.5 packets each 200 ms = 62,5 packets/s = 16 ms update rate.

## Buttons and LEDs
Not further analysed jet, but seems to conform to [SpaceNavigator.](SpaceNavigator.md). I.e.: Report 3 rot keys, Report 4 for LED.


## Other findings
From other records: Report 0x17 reports the battery

    Report ID: 0x17
    0110 0100 = Usage: Battery Strength: 100 
    Vendor Data: 01
    Padding: 00


## Device Descriptor
```
DEVICE DESCRIPTOR
    bLength: 18
    bDescriptorType: 0x01 (DEVICE)
    bcdUSB: 0x0200
    bDeviceClass: Device (0x00)
    bDeviceSubClass: 0
    bDeviceProtocol: 0 (Use class code info from Interface Descriptors)
    bMaxPacketSize0: 32
    idVendor: 3Dconnexion (0x256f)
    idProduct: SpaceMouse Wireless (cabled) (0xc62e)
    bcdDevice: 0x0441
    iManufacturer: 1
    iProduct: 2
    iSerialNumber: 0
    bNumConfigurations: 1
```

## HID Report of SpaceMouse Wireless
```
0x05, 0x01,        // Usage Page (Generic Desktop Ctrls)
0x09, 0x08,        // Usage (Multi-axis Controller)
0xA1, 0x01,        // Collection (Application)
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
0xA1, 0x02,        //   Collection (Logical)
0x85, 0x03,        //     Report ID (3)
0x05, 0x01,        //     Usage Page (Generic Desktop Ctrls)
0x05, 0x09,        //     Usage Page (Button)
0x19, 0x01,        //     Usage Minimum (0x01)
0x29, 0x02,        //     Usage Maximum (0x02)
0x15, 0x00,        //     Logical Minimum (0)
0x25, 0x01,        //     Logical Maximum (1)
0x35, 0x00,        //     Physical Minimum (0)
0x45, 0x01,        //     Physical Maximum (1)
0x75, 0x01,        //     Report Size (1)
0x95, 0x02,        //     Report Count (2)
0x81, 0x02,        //     Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
0x95, 0x0E,        //     Report Count (14)
0x81, 0x03,        //     Input (Const,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
0xC0,              //   End Collection
0xA1, 0x02,        //   Collection (Logical)
0x85, 0x04,        //     Report ID (4)
0x05, 0x08,        //     Usage Page (LEDs)
0x09, 0x4B,        //     Usage (Generic Indicator)
0x15, 0x00,        //     Logical Minimum (0)
0x25, 0x01,        //     Logical Maximum (1)
0x95, 0x01,        //     Report Count (1)
0x75, 0x01,        //     Report Size (1)
0x91, 0x02,        //     Output (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
0x95, 0x01,        //     Report Count (1)
0x75, 0x07,        //     Report Size (7)
0x91, 0x03,        //     Output (Const,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
0xC0,              //   End Collection
0xA1, 0x02,        //   Collection (Logical)
0x85, 0x17,        //     Report ID (23)
0x15, 0x00,        //     Logical Minimum (0)
0x25, 0x64,        //     Logical Maximum (100)
0x55, 0x00,        //     Unit Exponent (0)
0x65, 0x00,        //     Unit (None)
0x05, 0x06,        //     Usage Page (Generic Dev Ctrls)
0x09, 0x20,        //     Usage (Battery Strength)
0x75, 0x08,        //     Report Size (8)
0x95, 0x01,        //     Report Count (1)
0x81, 0x02,        //     Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
0x15, 0x00,        //     Logical Minimum (0)
0x25, 0x01,        //     Logical Maximum (1)
0x06, 0x00, 0xFF,  //     Usage Page (Vendor Defined 0xFF00)
0x09, 0x27,        //     Usage (0x27)
0x75, 0x01,        //     Report Size (1)
0x95, 0x01,        //     Report Count (1)
0x81, 0x02,        //     Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
0x75, 0x07,        //     Report Size (7)
0x81, 0x03,        //     Input (Const,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
0xC0,              //   End Collection
0x06, 0x00, 0xFF,  //   Usage Page (Vendor Defined 0xFF00)
0x09, 0x01,        //   Usage (0x01)
0xA1, 0x02,        //   Collection (Logical)
0x15, 0x80,        //     Logical Minimum (-128)
0x25, 0x7F,        //     Logical Maximum (127)
0x75, 0x08,        //     Report Size (8)
0x09, 0x3A,        //     Usage (0x3A)
0xA1, 0x02,        //     Collection (Logical)
0x85, 0x05,        //       Report ID (5)
0x09, 0x20,        //       Usage (0x20)
0x95, 0x01,        //       Report Count (1)
0xB1, 0x02,        //       Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
0xC0,              //     End Collection
0xA1, 0x02,        //     Collection (Logical)
0x85, 0x06,        //       Report ID (6)
0x09, 0x21,        //       Usage (0x21)
0x95, 0x01,        //       Report Count (1)
0xB1, 0x02,        //       Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
0xC0,              //     End Collection
0xA1, 0x02,        //     Collection (Logical)
0x85, 0x07,        //       Report ID (7)
0x09, 0x22,        //       Usage (0x22)
0x95, 0x01,        //       Report Count (1)
0xB1, 0x02,        //       Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
0xC0,              //     End Collection
0xA1, 0x02,        //     Collection (Logical)
0x85, 0x08,        //       Report ID (8)
0x09, 0x23,        //       Usage (0x23)
0x95, 0x07,        //       Report Count (7)
0xB1, 0x02,        //       Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
0xC0,              //     End Collection
0xA1, 0x02,        //     Collection (Logical)
0x85, 0x09,        //       Report ID (9)
0x09, 0x24,        //       Usage (0x24)
0x95, 0x07,        //       Report Count (7)
0xB1, 0x02,        //       Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
0xC0,              //     End Collection
0xA1, 0x02,        //     Collection (Logical)
0x85, 0x0A,        //       Report ID (10)
0x09, 0x25,        //       Usage (0x25)
0x95, 0x07,        //       Report Count (7)
0xB1, 0x02,        //       Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
0xC0,              //     End Collection
0xA1, 0x02,        //     Collection (Logical)
0x85, 0x0B,        //       Report ID (11)
0x09, 0x26,        //       Usage (0x26)
0x95, 0x01,        //       Report Count (1)
0xB1, 0x02,        //       Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
0xC0,              //     End Collection
0xA1, 0x02,        //     Collection (Logical)
0x85, 0x13,        //       Report ID (19)
0x09, 0x2E,        //       Usage (0x2E)
0x95, 0x01,        //       Report Count (1)
0xB1, 0x02,        //       Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
0xC0,              //     End Collection
0xA1, 0x02,        //     Collection (Logical)
0x85, 0x19,        //       Report ID (25)
0x09, 0x31,        //       Usage (0x31)
0x95, 0x04,        //       Report Count (4)
0xB1, 0x02,        //       Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
0xC0,              //     End Collection
0xA1, 0x02,        //     Collection (Logical)
0x85, 0x1A,        //       Report ID (26)
0x09, 0x32,        //       Usage (0x32)
0x95, 0x07,        //       Report Count (7)
0xB1, 0x02,        //       Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
0xC0,              //     End Collection
0xC0,              //   End Collection
0xC0,              // End Collection
// 270 bytes
```
