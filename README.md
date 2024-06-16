# Open Source Spacemouse with Keys
Repository for the space mouse based on four joysticks with an addition for keys (not jet in the picture)

![overview](https://github.com/AndunHH/spacemouse/assets/76771247/562b9763-a6fa-47f1-af30-2dd4bbc053b8)

This repository for the necessary arduino sources is mainly based on 
the work by [TeachingTech](https://www.printables.com/de/model/864950-open-source-spacemouse-space-mushroom-remix) and the additional code for keys by [LivingTheDream](https://www.printables.com/de/model/883967-tt-spacemouse-v2-lid-with-mounting-for-4-mx-switch) and is intended as basis for further development, because proper source code management is needed.

## Additional Features
- [x] Semi-Automatic calibration methods
- [x] Support for keys, that can triggger functions on your pc
- [x] Support for "kill-keys", that disable translation or rotation directly in the mouse

Coming up:
- [ ] Support for wheel to zoom (merging from [JoseLuizGZA](https://github.com/JoseLuisGZA/ErgonoMouse/))

Wanted, with unclear solution ... ?
- [ ] Reverse Direction and Speed options in 3dConnexion Software is not working, because our spacemouse is not accepting this settings.

# Getting Started
1. You purchase the [electronics](#electronics) and [print some parts](#printed-parts), which is not scope of this repository
2. [Create a custom board](#custom-board-to-emulate-the-space-mouse) in your Arduino IDE, that emulates the original space mouse
3. [Download or clone this github repository](#cloning-the-github-repo)
4. [Rename the config_sample.h to config.h](#create-your-own-config-file)
5. [Try to compile and flash your board](#compiling-and-flashing-the-firmware)
6. [Assign the pins of the joysticks and go through the calibration](#calibrate-your-hardware)
7. [Use your space mouse](#use-the-spacemouse)

## Custom board to emulate the space mouse
The boards.txt file needs an additional Board definition, which tells the processor to report the USB identifiers correctly and immitate the 3dconnexion space-mouse.

### Boards.txt on linux
You find the boards.txt in ```~/.arduino15/packages/SparkFun/hardware/avr/1.1.13```.
If this folder doesn't exist you need to install board support for SparkFun Arduinos.

### boards.txt on mac
Please read https://gist.github.com/maunsen/8dbee2bddef027b04a450241c7d36668

### boards.txt on windows
C:\Users<USER>\AppData\Local\Arduino15\packages\arduino\hardware

### Code to add to boards.txt
Here is the addition, which needs to be copied into the boards.txt (e.g. at the bottom). 
```
# Add this to the bottom your boards.txt

################################################################################
################################## Spacemouse based on Pro Micro ###################################
################################################################################
spacemouse.name=SpaceMouse

spacemouse.upload.tool=avrdude
spacemouse.upload.protocol=avr109
spacemouse.upload.maximum_size=28672
spacemouse.upload.maximum_data_size=2560
spacemouse.upload.speed=57600
spacemouse.upload.disable_flushing=true
spacemouse.upload.use_1200bps_touch=true
spacemouse.upload.wait_for_upload_port=true

spacemouse.bootloader.tool=avrdude
spacemouse.bootloader.unlock_bits=0x3F
spacemouse.bootloader.lock_bits=0x2F
spacemouse.bootloader.low_fuses=0xFF
spacemouse.bootloader.high_fuses=0xD8

spacemouse.build.board=AVR_PROMICRO
spacemouse.build.core=arduino:arduino
spacemouse.build.variant=promicro
spacemouse.build.mcu=atmega32u4
spacemouse.build.usb_product="Spacemouse Pro Wireless (cabled)"
spacemouse.build.usb_manufacturer="3Dconnexion"
spacemouse.build.vid=0x256f
spacemouse.build.extra_flags={build.usb_flags}

############################# Spacemouse Pro Micro 5V / 16MHz #############################
# deleted 3.3V / 8 Mhz variant to avoid bricking

spacemouse.build.pid.0=0xc631
spacemouse.build.pid.1=0xc631
spacemouse.build.pid=0xc631
spacemouse.build.f_cpu=16000000L

spacemouse.bootloader.extended_fuses=0xCB
spacemouse.bootloader.file=caterina/Caterina-promicro16.hex
```


### Further reading / FAQ regarding the boards.txt:

- [TeachingTech](https://www.printables.com/de/model/864950-open-source-spacemouse-space-mushroom-remix) video for proper instructions
- Teaching Tech followed the instructions here from [nebhead](https://gist.github.com/nebhead/c92da8f1a8b476f7c36c032a0ac2592a) with two key differences:
	- Changed the word 'DaemonBite' to 'Spacemouse' in all references.
  	- Changed the VID and PID values as per jfedor's instructions: vid=0x256f, pid=0xc631 (SpaceMouse Pro Wireless (cabled))
-  Some [people](https://gist.github.com/nebhead/c92da8f1a8b476f7c36c032a0ac2592a?permalink_comment_id=5069434#gistcomment-5069434) need to change ```spacemouse.upload.tool=avrdude``` to ```spacemouse.upload.tool.serial=avrdude``` to get no error when compiling

## Cloning the github repo
Clone the github repo to your computer: Scroll-Up to the green "<> Code" Button and select, if you wish to clone or just download the code.

## Create your own config file
Copy the config_sample.h and rename it to config.h.
This is done to avoid the personal config file being overwritten when pulling new updates from this repository. You probably have to update the config.h file with new additions from the config_sample.h, but your pin assignment will not stay.

## Compiling and flashing the firmware
- Open the Arduino IDE (1.8.19 and 2.3.2 are tested on Ubuntu).
- Open spacemouse-keys.ino
- Select Tools -> Board -> SparkFun AVR Boards -> Spacemouse.
- (If you followed another boards.txt instructions, which also allow 3.3V with 8 Mhz: Make sure to select the proper processor: 5V 16MHz)  
- Select the correct port (see troubleshooting section, which might be necessary for first upload)
- Compile the firmware

### Troubleshooting uploading
If you have the problem, that the port can not be found, the bootloader of your board is probably not reachable. The problem is, that the arduino pro micro has a very short time to get into the bootloader of 800 ms.
Therefore you need to connect the reset pin twice to gnd. Than you have 8 s to initially set the com port and upload your sketch. It is also quite a fast timing and needs some number of tries. 

You can read the details for this reset here: https://learn.sparkfun.com/tutorials/pro-micro--fio-v3-hookup-guide/troubleshooting-and-faq#ts-reset

## Calibrate your hardware 
After compiling and uploading the programm to your hardware, you can connect via the serial monitor. In the upper line, you can send the desired debug mode to the board and observe the output. "-1" stops the debug output.

Read and follow the instructions throughout the config.h file and write down your results. Recompile after every step.

1. Check and correct your pin out -> Refer to the pictures in the (Electronics)[#electronics] section below.
2. Tune dead zone to avoid jittering
3. Getting min and max values for your joysticks 
	- There is a semi-automatic approach, which returns you the minimum and maximum values seen within 15s.
4. Adjust sensitivity

This calibration is supported by various debug outputs which can toggle on or off before compiling or during run time by sending the corresponding number via the serial interface.

## Use the spacemouse
### Download the 3dconnexion driver on windows and mac
You will also need to download and install the 3DConnexion software: https://3dconnexion.com/us/drivers-application/3dxware-10/
If all goes well, the 3DConnexion software will show a SpaceMouse Pro wireless when the Arduino is connected.

### spacenav for linux users
Checkout https://wiki.freecad.org/3Dconnexion_input_devices and https://github.com/FreeSpacenav/spacenavd.

# Software Main Idea 
1. The software reads the eight ADC values of the four joy sticks
2. During start-up the zero-position of the joystick is measured and subtracted from the adc-value. -> The values now range from e.g. -500 to +500
3. A dead zone in the middle is applied to avoid small noisy movements. (E.g. every value between +/- 3 is fixed to zero)
4. The movement of the joysticks is mapped from the original about ca. +/- 500 digits to exactly +/- 350. (Therefore the real min and max values will be calibrated) Now all further calculations can be done with this normalized values between +/-350.
5. We calculate the translation and rotation based on this.
6. Applying the modifiers to minimize very small rotations or translations.
7. Kill, swap or invert movements
8. Sending the velocities and keys to the PC

# Printed parts
* https://www.printables.com/de/model/864950-open-source-spacemouse-space-mushroom-remix
* Check out the many remixes, if you have other joystick dev-boards, because the joystick must be perfectly centered!
* https://www.printables.com/de/model/883967-tt-spacemouse-v2-lid-with-mounting-for-4-mx-switch


# Electronics

The spacemouse is connected to an arduino Pro Micro 16 Mhz. Check out the wiring diagram by [TeachingTech](https://www.printables.com/de/model/864950-open-source-spacemouse-space-mushroom-remix/) or with this added keys:
![WiringSpaceMouse](https://github.com/AndunHH/spacemouse/assets/76771247/e013d9e8-8c01-40e2-9800-abb08bb271dd)

The calculations in the programm expect AX to be the vertical joystick in front of you and AY the horizontal in front of you. B, C and D are clockwise around the spacemouse.
Maybe your joystick axis are named X and Y in an other orientation. That doesn't matter. Connect them and use the config file to put the pin the vertical joystick in front of you (=AX) to the first position. In teaching techs example, this is A1 and A0 follows second for AY.

![analog](https://github.com/AndunHH/spacemouse/assets/76771247/150d7ad8-c8fc-4b01-9628-41bf6c72f0c1)

The calculation in this programm results in X, Y and Z calculated as shown in the middle of the picture. 
If this doesn't suit your programm change it by using the INVX or SWITCHYZ. 
```
TRANSX = -CY + AY
TRANSY = -BY + DY 
TRANSZ = -AX - BX - CX - DX
ROTX = -CX + AX
ROTY = -BX + DX
ROTZ = AY + BY + CY + DY
```
# See also

* Another approach for the spacemouse with an esp32: https://github.com/horvatkm/space_mouse_esp32s2

# History
This code is the combination of multiple works by others:
1. Original code for the Space Mushroom by Shiura on Thingiverse: https://www.thingiverse.com/thing:5739462
2. The next two from the comments on the instructables page: https://www.instructables.com/Space-Mushroom-Full-6-DOFs-Controller-for-CAD-Appl/
3. and the comments of Thingiverse: https://www.thingiverse.com/thing:5739462/comments
4. Code to emulate a 3DConnexion Space Mouse by jfedor: https://pastebin.com/gQxUrScV
5. This code was then remixed by BennyBWalker to include the above two sketches: https://pastebin.com/erhTgRBH
6. Four joystick remix code by fdmakara: https://www.thingiverse.com/thing:5817728
7. Teaching Techs work involves mixing all of these: https://www.printables.com/de/model/864950-open-source-spacemouse-space-mushroom-remix 
The basis is fdmakara's four joystick movement logic, with jfedor/BennyBWalker's HID SpaceMouse emulation. The four joystick logic sketch was setup for the joystick library instead of HID, so elements of this were omitted where not needed.  The outputs were jumbled no matter how Teaching Tech plugged them in, so Teaching Tech spent a lot of time adding debugging code to track exactly what was happening. On top of this, Teching Tech has added more control of speed/direction and comments/links to informative resources to try and explain what is happening in each phase. 
8. Code to include meassured min and max values for each Joystick by Daniel_1284580 (In Software Version V1 and newer)
9. Improved code to make it more userfriendly by Daniel_1284580 (In Software Version V2 and newer)
10. Improved Code, improved comments and added written tutorials in comments, By LivingTheDream: https://www.printables.com/de/model/883967-tt-spacemouse-v2-lid-with-mounting-for-4-mx-switch/files Implemented new algorithm "modifier function" for better motioncontrol by Daniel_1284580 (In Software Version V3) 
11. Moved the Deadzone detection into the inital ADC conversion and calculate every value everytime and use the modifier for better seperation between the access, By Andun_HH.
12. Added two additional buttons integrated into the knob to kill either translation or rotation at will and prevent unintended movements, by JoseLuisGZA.
13. Coming up: Added Encoder to use with a wheel on top of the main knob an simulate pulls on any of the axis (main use is simulating zoom like the mouse wheel), by [JoseLuizGZA](https://github.com/JoseLuisGZA/ErgonoMouse/).

# License
Because TeachingTech published his source code on Printables under this license, it also applies here:

[![CC BY-NC-SA 4.0][cc-by-nc-sa-shield]][cc-by-nc-sa]

This work is licensed under a
[Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International License][cc-by-nc-sa].

[![CC BY-NC-SA 4.0][cc-by-nc-sa-image]][cc-by-nc-sa]

[cc-by-nc-sa]: http://creativecommons.org/licenses/by-nc-sa/4.0/
[cc-by-nc-sa-image]: https://licensebuttons.net/l/by-nc-sa/4.0/88x31.png
[cc-by-nc-sa-shield]: https://img.shields.io/badge/License-CC%20BY--NC--SA%204.0-lightgrey.svg
