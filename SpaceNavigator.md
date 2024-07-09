# Space Navigator
This page contains informations about a 3Dconnexion SpaceNavigator, which has been observed via wireshark:

`sudo modprobe usbmon` 
`sudo wireshark`

Test on update rate of the spacemouse with wireshark and spacenavd on ubuntu. src 1.8.1

## Data transmitted by Space Navigator
First package: (no translation seen here)
`01 00 00 00 00 00 00`

Second package: (rotation around "right down" = "second axis")
`02 00 00 e2 ff 00 00`

Pushing Button 1:
`03 01 00`

Releasing Button 1:
`03 00 00`

Pushing Button 2:
`03 02 00`

Release Button 2:
`03 00 00`

Pushing both buttons at the same time: 
`03 03 00`

If in spnavcfg the LED is turned on, the host sends to 1.8.2
`04 01`

Turning LED off:
`04 00`

## Timing of the data sent by Space Navigator
Report Id 1 for translation is sent.
8 ms later Report Id 2 for rotation is sent.
If keys are pressed: After 8 ms Report Id 3 is sent. Otherwise this is skipped and after 8 ms it restarts with Report Id 1.

This leads to a constant rate of 125 packages per second with alternating report id, as long there non-zero data.

After the last package with non-zero data, translation and rotation and sent three times with only zeros and in the steady 8 ms intervall.

## Device Descriptor of Space Navigator
```
DEVICE DESCRIPTOR
    bLength: 18
    bDescriptorType: 0x01 (DEVICE)
    bcdUSB: 0x0200
    bDeviceClass: Device (0x00)
    bDeviceSubClass: 0
    bDeviceProtocol: 0 (Use class code info from Interface Descriptors)
    bMaxPacketSize0: 8
    idVendor: Logitech, Inc. (0x046d)
    idProduct: 3Dconnexion Space Navigator 3D Mouse (0xc626)
    bcdDevice: 0x0435
    iManufacturer: 1
    iProduct: 2
    iSerialNumber: 0
    bNumConfigurations: 1
```

## HID Report of Space Navigator
Decoded with https://eleccelerator.com/usbdescreqparser/
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
0x75, 0x10,        //     Report Size (16)
0x95, 0x03,        //     Report Count (3)
0x81, 0x06,        //     Input (Data,Var,Rel,No Wrap,Linear,Preferred State,No Null Position)
0xC0,              //   End Collection
0xA1, 0x00,        //   Collection (Physical)
0x85, 0x02,        //     Report ID (2)
0x09, 0x33,        //     Usage (Rx)
0x09, 0x34,        //     Usage (Ry)
0x09, 0x35,        //     Usage (Rz)
0x75, 0x10,        //     Report Size (16)
0x95, 0x03,        //     Report Count (3)
0x81, 0x06,        //     Input (Data,Var,Rel,No Wrap,Linear,Preferred State,No Null Position)
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
0xC0,              //   End Collection
0xC0,              // End Collection

// 217 bytes



0000   05 01 09 08 a1 01 a1 00 85 01 16 a2 fe 26 5e 01   .............&^.
0010   36 88 fa 46 78 05 55 0c 65 11 09 30 09 31 09 32   6..Fx.U.e..0.1.2
0020   75 10 95 03 81 06 c0 a1 00 85 02 09 33 09 34 09   u...........3.4.
0030   35 75 10 95 03 81 06 c0 a1 02 85 03 05 01 05 09   5u..............
0040   19 01 29 02 15 00 25 01 35 00 45 01 75 01 95 02   ..)...%.5.E.u...
0050   81 02 95 0e 81 03 c0 a1 02 85 04 05 08 09 4b 15   ..............K.
0060   00 25 01 95 01 75 01 91 02 95 01 75 07 91 03 c0   .%...u.....u....
0070   06 00 ff 09 01 a1 02 15 80 25 7f 75 08 09 3a a1   .........%.u..:.
0080   02 85 05 09 20 95 01 b1 02 c0 a1 02 85 06 09 21   .... ..........!
0090   95 01 b1 02 c0 a1 02 85 07 09 22 95 01 b1 02 c0   ..........".....
00a0   a1 02 85 08 09 23 95 07 b1 02 c0 a1 02 85 09 09   .....#..........
00b0   24 95 07 b1 02 c0 a1 02 85 0a 09 25 95 07 b1 02   $..........%....
00c0   c0 a1 02 85 0b 09 26 95 01 b1 02 c0 a1 02 85 13   ......&.........
00d0   09 2e 95 01 b1 02 c0 c0 c0                        .........
```

# Other related stuff
## Device List for other devices
Check [ANTz wiki](https://github.com/openantz/antz/wiki/3D-Mouse#device-list) for a device list. 
Interestingly, our code is not fully compliant with this list, as we use event#1 and event#2 with 6 byte, despite we are suggesting to be a SpaceMouse Pro Wireless (cabled), which has only one 12 byte event#1.
This seems to be no problem, as our HID descriptor is declaring the values with report id#1 and id#2.

## About relative and absolute input data
[Full discussion](https://github.com/FreeSpacenav/spacenavd/issues/108)
> When I wrote the first version of spacenavd, I only had a space navigator, and the first version of spacenavd only worked with "relative" inputs because that was what the space navigator emitted. I'm saying "relative" in quotes because they weren't really relative, they were just reported as such, but the values were always absolute displacements per axis. 3Dconnexion probably realized at some stage that reporting them as "relative" is incorrect, and changed to absolute in newer devices. 
>
> I'm pretty sure all modern 3Dconnexion devices report absolute axis usage.

> The original Space Navigator reports it's data as relative positions. The original Space Navigator is very sensitive and is jiggling a lot i.e. the same value is only send repeatedly very for some milli-seconds.
>
> My emulated "SpaceMouse Pro Wireless (cabled)" (or at least our inherited) hid report descriptor reports absolute values
Our emulation is very sturdy and if you hold it in position, the same value are easily calculate for a second.
>
> When using spacenavd and the simple example, cube or even in FreeCAD:
>
>Relative reports are evaluated with every event, even if they are the same as before.
Absolute reports are only evaluated, if they differ from the previous report. This is merely visible with the SpaceNavigator, but is very annoying for our emulation, as it is reporting same values very often. I didn't figured out, if this dropping of events is done by spacenavd or linux itself...

> Solution: Change our emulated mouse to Relative Positions, even if this is not "up to date". But it avoids the necessity to jiggle the values.



## HID descriptor of a spacemouse wireless
Taken from [here](https://pastebin.com/GD5mEKW6)
```
$ sudo usbhid-dump -d 256f:c62e
001:011:000:DESCRIPTOR         1667648810.573469
 05 01 09 08 A1 01 A1 00 85 01 16 A2 FE 26 5E 01
 36 88 FA 46 78 05 55 0C 65 11 09 30 09 31 09 32
 09 33 09 34 09 35 75 10 95 06 81 02 C0 A1 02 85
 03 05 01 05 09 19 01 29 02 15 00 25 01 35 00 45
 01 75 01 95 02 81 02 95 0E 81 03 C0 A1 02 85 04
 05 08 09 4B 15 00 25 01 95 01 75 01 91 02 95 01
 75 07 91 03 C0 A1 02 85 17 15 00 25 64 55 00 65
 00 05 06 09 20 75 08 95 01 81 02 15 00 25 01 06
 00 FF 09 27 75 01 95 01 81 02 75 07 81 03 C0 06
 00 FF 09 01 A1 02 15 80 25 7F 75 08 09 3A A1 02
 85 05 09 20 95 01 B1 02 C0 A1 02 85 06 09 21 95
 01 B1 02 C0 A1 02 85 07 09 22 95 01 B1 02 C0 A1
 02 85 08 09 23 95 07 B1 02 C0 A1 02 85 09 09 24
 95 07 B1 02 C0 A1 02 85 0A 09 25 95 07 B1 02 C0
 A1 02 85 0B 09 26 95 01 B1 02 C0 A1 02 85 13 09
 2E 95 01 B1 02 C0 A1 02 85 19 09 31 95 04 B1 02
 C0 A1 02 85 1A 09 32 95 07 B1 02 C0 C0 C0
 
Parsed relevant sections:
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
 
it goes on with LED control, battery indicators and so on...
```
