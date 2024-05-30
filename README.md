# spacemouse
Repository for the space mouse based on four joysticks.

![overview](https://github.com/AndunHH/spacemouse/assets/76771247/562b9763-a6fa-47f1-af30-2dd4bbc053b8)


See 
https://www.printables.com/de/model/864950-open-source-spacemouse-space-mushroom-remix
and 
https://www.printables.com/de/model/883967-tt-spacemouse-v2-lid-with-mounting-for-4-mx-switch


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

## Spacemouse emulation
Teaching Tech followed the instructions here from nebhead: https://gist.github.com/nebhead/c92da8f1a8b476f7c36c032a0ac2592a
with two key differences:

1. Teaching Tech changed the word 'DaemonBite' to 'Spacemouse' in all references.
2. Teaching Tech changed the VID and PID values as per jfedor's instructions: vid=0x256f, pid=0xc631 (SpaceMouse Pro Wireless (cabled))

Daniel_1284580 recomments changing leonardo.upload.tool=avrdude to leonardo.upload.tool.serial=avrdude to get no error when compiling
When compiling and uploading, Teaching Tech select Arduino AVR boards (in Sketchbook) > Spacemouse and then the serial port.
You will also need to download and install the 3DConnexion software: https://3dconnexion.com/us/drivers-application/3dxware-10/
If all goes well, the 3DConnexion software will show a SpaceMouse Pro wireless when the Arduino is connected.


# License
Shield: [![CC BY-NC-SA 4.0][cc-by-nc-sa-shield]][cc-by-nc-sa]

This work is licensed under a
[Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International License][cc-by-nc-sa].

[![CC BY-NC-SA 4.0][cc-by-nc-sa-image]][cc-by-nc-sa]

[cc-by-nc-sa]: http://creativecommons.org/licenses/by-nc-sa/4.0/
[cc-by-nc-sa-image]: https://licensebuttons.net/l/by-nc-sa/4.0/88x31.png
[cc-by-nc-sa-shield]: https://img.shields.io/badge/License-CC%20BY--NC--SA%204.0-lightgrey.svg
