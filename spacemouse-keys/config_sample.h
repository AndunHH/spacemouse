#ifndef CONFIG_h
#define CONFIG_h

#include "release.h"

#define PARAM_IN_EEPROM 1
#define ENABLE_PROGMODE 1

#undef  DEBUG_KEYS
#undef  DEBUG_ADC

/* The user specific settings, like pin mappings or special configuration variables and sensitivities are stored in config.h.
   This file is meant for the << JOYSTICK SPACEMOUSE >>
   >>> Some comments distinguish between resistive-JOYSTICKs "RJS" and hall-JOYSTICKs "HJS" to give examples for values. <<<
   Please adjust your settings and save it as --> config.h <-- !
*/


/* Calibration Instructions
============================
Follow this file from top to bottom to calibrate your space mouse.
You can find some pictures for the calibration process here:
https://github.com/AndunHH/spacemouse/wiki/Ergonomouse-Build#calibration

Debugging Instructions
=========================
To activate one of the following debugging modes, you can either:
- Change STARTDEBUG here in the code and compile/download again
  OR
- Compile and upload the program. Go to the serial monitor and type the number and hit enter to select the debug mode.

Debug Modes:
------------
-1: Debugging off. Set to this once everything is working.
0:  Nothing...

1:  Report raw joystick values. 0-1023 raw ADC 10-bit values
11: Calibrate / Zero the SpaceMouse and get a dead-zone suggestion (This is also done on every startup in the setup())

2:  Report centered joystick values. Values should be approximately -500 to +500, jitter around 0 at idle.
20: semi-automatic min-max calibration.

3:  Report centered joystick values. Filtered for deadzone. Approximately -350 to +350, locked to zero at idle, modified with a function.

4:  Report translation and rotation values. Approximately -350 to +350 depending on the parameter.
5:  Report debug 3 and 4 side by side for direct cause and effect reference.
6:  Report velocity and keys after possible kill-key feature
61: Report velocity and keys after kill-switch or ExclusiveMode
7:  Report the frequency of the loop() -> how often is the loop() called in one second?
8:  Report the bits and bytes send as button codes
9:  Report details about the encoder wheel, if ROTARY_AXIS > 0 or ROTARY_KEYS>0
*/

#define STARTDEBUG 0  // Can also be set over the serial interface, while the programm is running!

// Hardware uses joystick sensors (resistive or hall) instead of HallEffect sensors and magnets
#undef HALLEFFECT

/* First Calibration: Joystick axis pin assignment, electrical check
=====================================================================
Default Assembly when looking from above on top of the space mouse
   back    resulting axis (of the mouse, not from the single joystick)
    C           Y+
    |           .
 B--+--D   X-...Z+...X+
    |           .
    A           Y-
  front

Each joysticks has
- a horizontal axis from left(0) to right(1023) = Y
- a vertical axis from top(0) to bottom(1023) = X
(This definition of X and Y may not correspond to the print on your joysticks... We will find out which signal is X and Y now.)

1. Try to write down the two axis of every joystick with the corresponding pin numbers you chose. 
(A4 and A5 are not used in the example by TeachingTech).
2. Compile the script, type 1 into the serial interface and hit enter to enable debug output 1.
3. At the joystick in front of you (A), move the joystick from the top -> down to bottom (X) and observe the debug output:
  a) AX goes from 0 to 1023 -> Everything is correct.
  b) AX goes from 1023 to 0 -> You need to invert AX, see INVERTLIST below.
  c) Another output is showing movement: Swap the pins in the PINLIST. Probably you have to swap the first and second element, as AX and AY may be swapped.
  
If you have the joystick TeachingTech recommended:
  The pins labelled X and Y on the joystick are NOT the X and Y needed here, but swapped. First joysticks Y: AX and X: AY.

4. Continue with AY: Move the joystick from left -> right and the values shall increase from 0 to 1023 and follow the instructions 3.a) to 3.c) above.
5. Repeat this with every axis and every joystick until you have a valid PINLIST and maybe an INVERTLIST
6. If you use hall-effect-joysticks, you should observe the value shown when the joystick is not touched.
   At this zeropoint it should read roundabout 512. If it doesn't, you can mechanically adjust the joysticks -
   there are many tutorial videos online that show the procedure.
*/

// Check the correct wiring with the debug output=1
#define PINLIST \
  { A1, A0, A3, A2, A7, A6, A9, A8 }
//  AX, AY, BX, BY, CX, CY, DX, DY

// Set to 1 to invert one joystick axis. 
// Usually all _X values shall be inverted or none of them.
// Usually all _Y values shall be inverted or none of them.
#define INVERTLIST \
  { 0, 0,  0,  0,  0,  0,  0,  0}
// AX, AY, BX, BY, CX, CY, DX, DY

/* Second calibration: Tune Deadzone
=====================================
Deadzone to filter out unintended movements. Increase if the mouse has small movements when it should be idle or the mouse is too sensitive to subtle movements.
Semi-automatic: Set debug = 11. Don't touch the mouse and observe the automatic output.
Manual:         Set debug = 2.  Don't touch the mouse but observe the values. They should be nearly to zero.
                                Every value around zero which is noise or should be neglected afterwards is in the following deadzone.
*/
#define DEADZONE 0  // Recommended to have this as small as possible to allow full range of motion.

/* Third calibration: Getting MIN and MAX values
=================================================
Can be done manual (debug = 2) or semi-automatic (debug = 20)

Semi-automatic (debug=20)
-------------------------
1. Compile the sketch and upload it.
2. Go to the serial monitor type 20 and hit enter. -> debug is set to 20.
3. Move the Spacemouse around for 15s to get a min and max value.
4. Verify, that the minimums are around -400 to -520 and the maxVals around +400 to +520.
   (repeat or check again, if you have too small values!)
5. Copy the output from the console into your config.h below.

Manual min/max calibration (debug = 2)
--------------------------------------
Recommended calibration procedure for min/max ADC levels
1. Compile the sketch and upload it. Go to the serial monitor type 2 and hit enter. -> debug is set to 2.
2. Get a piece of paper and write the following chart:
 Chart:
 maxVals      | minVals
-------------------------------
 AX+:         | AX-:
 AY+:         | AY-:
 BX+:         | BX-:
 BY+:         | BY-:
 CX+:         | CX-:
 CY+:         | CY-:
 DX+:         | DX-:
 DY+:         | DY-:

3. (a) Start out with AX (positive values)
   (b) Start moving the your Spacemouse and try increasing the value of AX till you can't get a higher value out of it.
   (c) this is your positive maximum value for AX so write it down for AX
4. Do the same for AY,BX,BY,....DY
5. Do the same for your negative values to populate the minVals
6. Write all the positive values starting from the top into the array maxValues below
7. Write all the negative values starting from the top into the array minValues below
8. Compile and upload. You finished calibrating.
*/

// Insert measured Values like this:
//              {  AX,   AY,   BX,   BY,   CX,   CY,   DX,   DY}
#define MINVALS {-265, -260, -250, -230, -250, -510, -250, -230}
#define MAXVALS { 265,  510,  250,  450,  250,  260,  250,  450}

/* Fourth calibration: Sensitivity
===================================
Use debug mode 4 or use for example your CAD program to verify changes.

The sensitivity values are used in a division, thus
  - Use a fraction to make the axis MORE sensitive. F.e. 0.5 makes the axis twice as sensitive.
  - Use a value larger than 1 to make it LESS sensitive. F.e. 5 makes the axis five times less sensitive.

Recommended calibration procedure for sensitivity
--------------------------------------------------
NEW: you can use a parameter menu on the serial interface,
     the constants below are only a starting point,
     the parameters you entered can be saved into the EEPROM of the SpaceMouse itself
     
1. Make sure modFunc is on level 0, see below. Upload the sketch. Then open serial monitor, type 4 for and hit enter.
   You will see Values TX, TY, TZ, RX, RY, RZ
2. Start moving your SpaceMouse. You will notice values changing.
3. Starting with TX try increasing this value as much as possible by moving your Spacemouse around. If you get around 350 thats great.
   If not change parameter SENS_TX. Repeat until it is around 350 for maximum motion.
4. Repeat steps 3 for TY, TZ, RX, RY, RZ
5. Verification: Move the Joystick in funny ways. All you should get for either TX,TX,TZ,RX,RY,RZ should be approximately between -350 to 350.
6. save your settings to the devices EEPROM have them permanently on the SpaceMouse
6.a) you may show the parameters on screen as #DEFINE's and copy them back to this file (as new starting point for empty devices)
7. You have finished sensitivity calibration. You can now test your SpaceMouse with your favorite program (e.g. Cad software, Slicer)
8. Aftermath: You notice the movements are hard to control - try using Modification Functions "modFunc".
[Suggestion: ModFunc level 3]
*/

#define SENS_TX     2.50  // RJS:2.50  HJS:5.00
#define SENS_TY     2.50  // RJS:2.50  HJS:5.00
#define SENS_PTZ    15.00 // RJS:15.0  HJS:8.00  // sensitivity for positive translation z
#define SENS_NTZ    7.00  // RJS:7.00  HJS:6.00  // sensitivity for negative translation z
#define GATE_NTZ    0.01  // RJS:0.01  HJS:8.00  // gate value, which negative z movements will be ignored (like an additional deadzone for -z).
#define GATE_RX     1     // RJS:1     HJS:2     // Value under which rotX values will be forced to zero
#define GATE_RY     1     // RJS:1     HJS:2     // Value under which roty values will be forced to zero
#define GATE_RZ     1     // RJS:1     HJS:2     // Value under which rotz values will be forced to zero
#define SENS_RX     0.75  // RJS:0.75  HJS:1.50
#define SENS_RY     0.75  // RJS:0.75  HJS:1.50
#define SENS_RZ     2.00  // RJS:2.00  HJS:3.00


/* Fifth calibration: Modifier Function
========================================
NEW: you can use a parameter menu on the serial interface,
     the constants below are only a starting point,
     the parameters you entered can be saved into the EEPROM of the SpaceMouse itself

Modify resulting behaviour of SpaceMouse outputs to suppress small movements around zero and enforce big movements even more.

Check the README.md for more details and a plot of the different functions.
(This function is applied on the resulting velocities and not on the direct input from the joysticks)

This should be at level 0 when starting the calibration!
0: linear                      y = x                                     [Standard behaviour: No modification]

1: "squared" function:         y = abs(x)^a * sign(x)                    [altered squared function working in positive and negative direction]
	-> MOD_A is the exponent "a": 1 is linear, 2 is squared, 3 is cubed - anything between is permitted, you can fine-tune the shape of the curve

   tangent function:           y = tan(b * x) / tan(b)                   [results in a flat curve near zero but increases the more you are away from zero]
    -> use modFunc = 3 and set MOD_A = 1.0 to get this

3: "squared" tangent function: y = tan(b * (abs(x)^a *sign(X))) / tan(b) [results in a flatter curve near zero but increases alot the more you are away from zero]
	-> MOD_B is factor "b" and tunes the form of the tangens-function

   cubed tangent function:     y = tan(b * (abs(x)^3 *sign(X))) / tan(b) [results in a flatter curve near zero but increases extreme the more you are away from zero]
    -> set MOD_A = 3.0 to get this
Recommendation after tuning: MODFUNC 3
*/

#define MODFUNC   0     // Used as default value as long as the data hasn't been saved in the EEPROM
#define MOD_A     1.15  // exponent "a", recommended: 1.0 ... 3.0  [anything from linear to x^3]  -> bigger value flattens the curve more near zero (on MODFUNC 1 and 3)
#define MOD_B     1.15  // factor "b",   recommended: 1.0 ... 1.57 [tan(0..1) up to tan(0..pi/2)] -> bigger value increases the curve more away from zero (on MODFUNC 3)

/* Sixth Calibration: Direction
================================
NEW: you can use a parameter menu on the serial interface,
     the constants below are only a starting point,
     the parameters you entered can be saved into the EEPROM of the SpaceMouse itself

Modify the direction of translation/rotation depending on the CAD program you are using on your PC.
This should be done, when you are done with the pin assignment!

If all defines are set to 0 the resulting X, Y and Z axis correspond to the pictures shown in the README.md.
The suggestion in the comments for "3Dc" are often needed on windows PCs with 3dconnexion driver to get expected behavior.
*/

// Switch between 0 or 1 as desired
#define INVX  1 // pan left/right                          RJS:1     HJS:0
#define INVY  1 // pan up/down                             RJS:0     HJS:1
#define INVZ  1	// zoom in/out                           RJS:0     HJS:1
#define INVRX 1 // Rotate around X axis (tilt front/back)  RJS:0     HJS:1
#define INVRY 1 // Rotate around Y axis (tilt left/right)  RJS:1     HJS:0
#define INVRZ 1 // Rotate around Z axis (twist left/right) RJS:1     HJS:0

// Switch Zoom direction with Up/Down Movement
#define SWITCHYZ 0  // change to 1 to switch Y and Z axis
#define SWITCHXY 0  // change to 1 to switch X and Y axis

// ------------------------------------------------------------------------------------
// Joystick drift compensation
//   Compensates drifting zero-position (drifting electronics, unprecise mechanics)
//   All values may be edited in the parameter-menu.
//
//   check the following conditions for the duration of COMP_WAIT:
//     - raw-value doesn't move more than COMP_CDIFF from the center-value
//     - raw-value doesn't move more than COMP_MDIFF itself
//   if they were not violated, we consider the SpaceMouse is not touched, so we do:
//     > take the mean-value of the next COMP_NR raw-values for each joystick-axis
//     > calculate offsets for each axis to bring the axis mean-value to the axis center-value (re-center the joysticks)
#define COMP_EN      1   // enable the compensation
#define COMP_NR     50   // number of points to build the mean-value
#define COMP_WAIT  200   // [ms] time to wait and monitor before compensating (smaller value=>faster re-centering, but may cut off small moves)
#define COMP_MDIFF   4   // [incr] maximum range of raw-values to be considered as only drift
#define COMP_CDIFF  50   // [incr] maximum distance from the center-value to be only drift (never compensates above this offset)

/* Exclusive mode
==================
Exclusive mode only permit to send translation OR rotation, but never both at the same time.
This can solve issues with classic joysticks where you get unwanted translation or rotation at the same time.

It choose to send the one with the biggest absolute value.
*/
#define EXCLUSIVE   0             // RJS:1     HJS:0
#define EXCL_HYST   5

/* PRIO-Z-EXCLUSIVE MODE:
=========================
If prio-z-exclusive-mode is on, rotations are only calculated, if no z-move is detected
Recommended for resistive joysticks.
When pushing or pulling, the knob produced transient rotational components that stops when the z-translation gets the priority. So when pulling, we get first a rotation then the desired translation.
So this mode sees that min. 3 of 4 joysticks all move up (or down) and use it as an indicator that the knob is mainly pushed/pulled. So before any (ghost-)rotational component can be calculated, it is sorted out.
*/
#define EXCL_PRIOZ 0      // RJS:1     HJS:0

/* Key Support
===============
If you attached keys to your Spacemouse, configure them here.
You can use the keys to report them via USB HID to the PC (either classically pressed or emulated with an encoder)
or as kill-keys (described below).
*/

//How many classic keys are there in total? (0=no keys, feature disabled)
#define NUMKEYS 0

// Define the PINS for the classic keys on the Arduino
// The first pins from KEYLIST may be reported via HID
#define KEYLIST \
    { 15, 14, 16, 10 }

/* Report KEYS over USB HID to the PC
 -------------------------------------
*/

// How many keys reported? Classical + ROTARY_KEYS in total.
#define NUMHIDKEYS 0

// In order to define which key is assigned to which button, elements of the following list must be entered in the BUTTONLIST below
#define SM_MENU     0 // Key "Menu"
#define SM_FIT      1 // Key "Fit"
#define SM_T        2 // Key "Top"
#define SM_R        4 // Key "Right"
#define SM_F        5 // Key "Front"
#define SM_RCW      8 // Key "Roll 90°CW"
#define SM_1        12 // Key "1" 
#define SM_2        13 // Key "2" 
#define SM_3        14 // Key "3" 
#define SM_4        15 // Key "4"
#define SM_ESC      22 // Key "ESC"
#define SM_ALT      23 // Key "ALT"
#define SM_SHFT     24 // Key "SHIFT"
#define SM_CTRL     25 // Key "CTRL"
#define SM_ROT      26 // Key "Rotate" 

// BUTTONLIST must have at least as many elements as NUMHIDKEYS
// The keys from KEYLIST or ROTARY_KEYS are assigned to buttons here:
#define BUTTONLIST {SM_T, SM_R, SM_F}

/* Kill-Key Feature
--------------------
Are there buttons to set the translation or rotation to zero?
How many kill keys are there? (disabled: 0; enabled: 2)
*/

// usually you take the last two buttons from KEYLIST as kill-keys
#define NUMKILLKEYS 0

// Index of the kill key for rotation
#define KILLROT 2

// Index of the kill key for translation
#define KILLTRANS 3

// Note: Technically you can report the kill-keys via HID as "usual" buttons, but that doesn't make much sense...

/*  Example for NO KEYS
 *  There are zero keys in total:  NUMKEYS 0
 *  KEYLIST { }
 *  NUMHIDKEYS 0
 *  BUTTONLIST { }
 *  NUMKILLKEYS 0
 *  KILLROT and KILLTRANS don't matter... KILLROT 0 and KILLTRANS 0
 */

/*  Example for three usual buttons and no kill-keys
 *  There are three keys in total:  NUMKEYS 3
 *  The keys which shall be reported to the pc are connected to pin 15, 14 and 16
 *  KEYLIST {15, 14, 16}
 *  Therefore, the first three pins from the KEYLIST apply for the HID: NUMHIDKEYS 3
 *  Those three Buttons shall be "FIT", "T" and "R": BUTTONLIST {SM_FIT, SM_T, SM_R}
 *  No keys for kill-keys NUMKILLKEYS 0
 *  KILLROT and KILLTRANS don't matter... KILLROT 0 and KILLTRANS 0
 */

/*
 *  Example for two usual buttons and two kill-keys:
 *  There are four keys in total:  NUMKEYS 4
 *  The keys which shall be reported to the pc are connected to pin 15 and 14
 *  The keys which shall be used to kill translation or rotation are connected to pin 16 and 10
 *  KEYLIST {15, 14, 16, 10}
 *  Therefore, the first two pins from the KEYLIST apply for the HID: NUMHIDKEYS 2
 *  Those two Buttons shall be "3", "4": BUTTONLIST {SM_3, SM_4}
 *  Two keys are used as kill-keys: NUMKILLKEYS 2
 *  The first kill key has the third position in the KEYLIST and due to zero-based counting third-1 => KILLROT 2
 *  The second kill key has the last position in the KEYLIST with index 3 -> KILLTRANS 3
 */

// Some simple tests for the definition of the keys
#if (NUMKILLKEYS > NUMKEYS)
#error "Number of Kill Keys can not be larger than total number of keys"
#endif
#if (NUMKILLKEYS > 0 && ((KILLROT > NUMKEYS) || (KILLTRANS > NUMKEYS)))
#error "Index of killkeys must be smaller than the total number of keys"
#endif

// time in ms which is needed to allow a new button press
#define DEBOUNCE_KEYS_MS 200

/* Encoder Wheel
================
You can attach an encoder to the mouse, which acts as an input device for one movement.
Needs the encoder library by Paul Stoffregen (https://www.pjrc.com/teensy/td_libs_Encoder.html).
*/

// Define the encoder pins
// Swap those two pins to change direction of encoder
#define ENCODER_CLK 2
#define ENCODER_DT 3

/*
Axis to replace with encoder
0. None -> disable this feature completely
1. transX
2. transY (zoom in "Forward / Backward" Zoom Direction configuration, see SWITCHYZ)
3. transZ (simulates zoom in "Up / Down" Zoom Direction configuration , see SWITCHYZ)
4. rotX
5. rotY
6. rotZ
(Those are the positions in the velocity array +1, as defined in kinematics.h)
*/
#define ROTARY_AXIS 0

/* To calculate a velocity from the encoder position, the output is faded over so many loop() iterations, as defined in #RAXIS_ECH
Small number = short duration of zooming <-> Big Number = longer duration of zooming
Compare this number with the update frequency of the script, reported by debug=7:
  If RAXIS_ECH = frequency: the zoom is faded for 1 second.
*/
#define RAXIS_ECH 200

/* Strength of the simulated pull
Recommended range: 0 - 350
  Reason for max=350: The HID Interface reports logical max as +350, see hidInterface.h
Recommended strength = 200
*/
#define RAXIS_STR 200

/* ROTARY_KEYS
===============
Use the encoder and emulate a key stroke by turning the encoder.
ROTARY_KEYS 1 = enabled, 0 = disabled
*/
#define ROTARY_KEYS 0
// which key from the BUTTONLIST shall be emulated?
// First direction  (0 = first element from BUTTONLIST, 1 = second element, etc.)
#define ROTARY_KEY_IDX_A 2
// counter direction
#define ROTARY_KEY_IDX_B 3
// duration of simulated key
#define ROTARY_KEY_STRENGTH 19

/* LED support
===============
You can attach:
a) a simple LED to the mouse. LED shall be connected to 5V and the controller port.
b) a fancy LED strip, like the nanopixel. Check the FASTLED library for supported chips / led strips.

Which pin shall be used as LED? This pin is used either as a digital pin (a) or as the data pin (b).
Change from "//define" to "#define" to activate the LED feature.
*/
// #define LEDpin 5

/* Simple LED
-------------
// If you have connected a single LED to the controller port and GND, invert it by uncommenting this #define
*/
// #define LEDinvert

/* LED strip with data pin
---------------------------
The connected LED is not just a stupid LED, but an intelligent one, like a neopixel controlled by FASTLED library. If set, the LEDRING gives the number of LEDs on the ring.
*/

// #define LEDRING 24
//  The LEDpin is used as a data pin

// The LEDs light up, if a certain movement is reached:
#define VelocityDeadzoneForLED 15

// About how many LEDs must the ring by turned to align?
#define LEDclockOffset 0

// how often shall the LEDs be updated
#define LEDUPDATERATE_MS 150


/* Advanced debug output settings
=================================
The following settings allow customization of debug output behavior */

// Generate a debug line only every DEBUGDELAY ms 
#define DEBUGDELAY 100

// The standard behavior "\r" for the debug output is, that the values are always written into the same line to get a clean output. Easy readable for the human.
#define DEBUG_LINE_END "\r"
// If you need to report some debug outputs to trace errors, you can change the debug output to "\r\n" to get a newline with each debug output. (old behavior)
//define DEBUG_LINE_END "\r\n"

/* Advanced USB HID settings
=============================
The following settings are advanced and don't need to changed for normal windows users.
*/

/* ADV_HID_REL and ADV_HID_JIGGLE change how the values are reported over HID protocol, see hidInterface.cpp and .h

For windows users: DON'T CHANGE / DON'T ENABLE THIS, if you don't understand what it does.

For linux / spacenavd user: Suggestions to enable #define ADV_HID_JIGGLE

Translation and rotation values are either declared as absolute or relative values in the hid descriptor in hidInterface.h.

Relative declaration (may be activated by ADV_HID_REL)
-------------------------------------------------------
With linux and spacenavd: If the space mouse didn't return to absolutely zero in one axis this axis will still report movement,
when another direction is pushed, because only the changed values are emitted as events by the linux kernel.

Despite that, values events are emitted with every report send, even if they didn't changed.

Absolute declaration (default)
------------------------------
Every value is always reporting the absolute position.
This means, in contrast to relative, an axis that is left alone is reported again as zero.
On the other hand, events are only emitted, if at least some value changes.
This is not always the case, when the space mouse is held still at a non-zero position.
Solution: Jiggling (may be activated by ADV_HID_JIGGLE)
Every non-zero value is reported as it is and +1 in the next report, repeating with +0 in the next iteration and +1 in the next...
This little extra noise is called "jiggling" and ensures that a value declared as absolute is resent with every report, because is not equal to the last value.
*/

// Switch declaration of values to relative, if the following symbol is defined:
// #define ADV_HID_REL

// Add Jiggling to the value reported, if the following symbol is defined:
// #define ADV_HID_JIGGLE

#endif // CONFIG_h
