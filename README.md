# Open Source Spacemouse with Keys
Repository for the space mouse based on four joysticks with an addition for keys (not jet in the picture)

![overview](https://github.com/AndunHH/spacemouse/assets/76771247/562b9763-a6fa-47f1-af30-2dd4bbc053b8)

This repository for the necessary arduino sources is mainly based on 
the work by [TeachingTech](https://www.printables.com/de/model/864950-open-source-spacemouse-space-mushroom-remix) and the additional code for keys by [LivingTheDream](https://www.printables.com/de/model/883967-tt-spacemouse-v2-lid-with-mounting-for-4-mx-switch) and is intended as basis for further development, because proper source code management is needed.

# Calibration
First, you need to calibrate your space-mouse. This is described in the config_sample.h which is copied and renamed to config.h.

## Short calibration overview

1. Check and correct your pin out
2. Tune dead zone to avoid jittering
3. Getting min and max values for your joysticks
4. Adjust sensitivity

This calibration is supported by various debug outputs which can toggle on or off before compiling or during run time by sending the corresponding number via the serial interface.


# Software Main Idea 
1. The software reads the eight ADC values of the four joy sticks
2. During start-up the zero-position of the joystick is measured and subtracted from the adc-value. -> The values now range from e.g. -500 to +500
3. A dead zone in the middle is applied to avoid small noisy movements. (E.g. every value between +/- 3 is fixed to zero)
4. The movement of the joysticks is mapped from the original about ca. +/- 500 digits to exactly +/- 350. (Therefore the real min and max values will be calibrated) Now all further calculations can be done with this normalized values between +/-350.
5. We calculate the translation and rotation based on this.
6. Applying the modifiers to minimize very small rotations or translations.

# Printed parts
* https://www.printables.com/de/model/864950-open-source-spacemouse-space-mushroom-remix
* Check out the many remixes, if you have other joystick dev-boards, because the joystick must be perfectly centered!
* https://www.printables.com/de/model/883967-tt-spacemouse-v2-lid-with-mounting-for-4-mx-switch


# Electronics

The spacemouse is connected to an arduino pro micro 16 Mhz. Check out the wiring diagram by [TeachingTech](https://www.printables.com/de/model/864950-open-source-spacemouse-space-mushroom-remix/) or with this added keys:
![WiringSpaceMouse](https://github.com/AndunHH/spacemouse/assets/76771247/e013d9e8-8c01-40e2-9800-abb08bb271dd)

As a reference: the pins and values the joysticks create on the analoge pins of the arduino, when the pin mapping is A0 to A7 straight. The original version from teaching tech has pin-out A1, A0, A3, A2 etc. 
All the different joysticks name there x and y axis differently. Just connect every axis to an ADC and sort them afterwards in your config.h.
![analog](https://github.com/AndunHH/spacemouse/assets/76771247/7948f8bc-7c1c-4189-9ef6-1216a6b8f164)

## Spacemouse emulation
Teaching Tech followed the instructions here from nebhead: https://gist.github.com/nebhead/c92da8f1a8b476f7c36c032a0ac2592a
with two key differences:

1. Teaching Tech changed the word 'DaemonBite' to 'Spacemouse' in all references.
2. Teaching Tech changed the VID and PID values as per jfedor's instructions: vid=0x256f, pid=0xc631 (SpaceMouse Pro Wireless (cabled))

Daniel_1284580 recomments changing leonardo.upload.tool=avrdude to leonardo.upload.tool.serial=avrdude to get no error when compiling
When compiling and uploading, Teaching Tech select Arduino AVR boards (in Sketchbook) > Spacemouse and then the serial port.
You will also need to download and install the 3DConnexion software: https://3dconnexion.com/us/drivers-application/3dxware-10/
If all goes well, the 3DConnexion software will show a SpaceMouse Pro wireless when the Arduino is connected.

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

# License
Because TeachingTech published his source code on Printables under this license, it also applies here:

[![CC BY-NC-SA 4.0][cc-by-nc-sa-shield]][cc-by-nc-sa]

This work is licensed under a
[Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International License][cc-by-nc-sa].

[![CC BY-NC-SA 4.0][cc-by-nc-sa-image]][cc-by-nc-sa]

[cc-by-nc-sa]: http://creativecommons.org/licenses/by-nc-sa/4.0/
[cc-by-nc-sa-image]: https://licensebuttons.net/l/by-nc-sa/4.0/88x31.png
[cc-by-nc-sa-shield]: https://img.shields.io/badge/License-CC%20BY--NC--SA%204.0-lightgrey.svg
