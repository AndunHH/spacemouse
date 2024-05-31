// The user specific settings, like pin mappings or special configuration variables and sensitivities are stored in config.h.
// Please adjust your settings and save it as config.h

// Calibration instructions
// Follow this file from top to bottom to calibrate your space mouse

// Default Assembly when looking from above:
//    C           Y+
//    |           .
// B--+--D   X-...Z+...X+
//    |           .
//    A           Y-

// First step: Wiring. Matches the first eight analogue pins of the Arduino Pro Micro (atmega32u4)
int PINLIST[8] = {
  // The positions of the reads
  A1, // X-axis A (A is in fron of you: X is the horizontal axis)
  A0, // Y-axis A (A: vertical axis Y)
  A3, // X-axis B (B: on the left side, when viewed from top)
  A2, // Y-axis B
  A7, // X-axis C (C is "away from user" at the back)
  A6, // Y-axis C
  A9, // X-axis D (D: on the right side)
  A8  // Y-axis D
};
// Check the correct wiring with the debug output=1

// Debugging
// 0: Debugging off. Set to this once everything is working.
// 1: Output raw joystick values. 0-1023 raw ADC 10-bit values
// 2: Output centered joystick values. Values should be approx -500 to +500, jitter around 0 at idle.
// 20: semi-automatic min-max calibration
// 3: Output centered joystick values. Filtered for deadzone. Approx -350 to +350, locked to zero at idle, modified with a function.
// 4: Output translation and rotation values. Approx -350 to +350 depending on the parameter.
// 5: Output debug 4 and 5 side by side for direct cause and effect reference.
int debug = 1;

// Modifier Function
// Modify resulting behaviour of Joystick input values
// DISCLAIMER: This should be at level 0 when starting the calibration
// 0: linear y = x [Standard behaviour: No modification]
// 1: squared function y = x^2*sign(x) [altered squared function working in positive and negative direction]
// 2: tangent function: y = tan(x) [Results in a flat curve near zero but increases the more you are away from zero]
// 3: squared tangent function: y = tan(x^2*sign(X)) [Results in a flatter curve near zero but increases alot the more you are away from zero]
// 4: cubed tangent function: y = tan(x^3) [Results in a very flat curve near zero but increases drastically the more you are away from zero]
int modFunc = 0;

// Second calibration: Tune Deadzone
// Deadzone to filter out unintended movements. Increase if the mouse has small movements when it should be idle or the mouse is too senstive to subtle movements.
// Set debug = 2. Don't touch the mouse but observe the values. They should be nearly to zero. Every value around zero which is noise or should be neglected afterwards is in the following deadzone.
int DEADZONE = 3; // Recommended to have this as small as possible for V2 to allow smaller knob range of motion.

// Third calibration: getting min and max values
// Can be done manual (debug = 2) or semi-automatic (debug = 20)
// Semi-automatic (debug=20)
// 1. Set debug = 20
// 2. Compile the sketch, upload it and wait for confirmation in the serial console.
// 3. Move the spacemouse around for 15s to get a min and max value.
// 4. Verify, that the minimums are around -400 to -520 and the maxVals around +400 to +520.
// (repeat or check again, if you have small values!)
// 5. Copy the output from the console into your config.h (below the manual instructions)

// Manual min/max calibration (debug = 2)
// Recommended calibration procedure for min/max adc levels
// 1. Change debug to level 2 and upload sketch. Then open Serial Monitor.
// 2. Get a piece of paper and write the following chart:
// Chart:
//  maxVals      | minVals
//--------------------------------
//  AX+:         | AX-:
//  AY+:         | AY-:
//  BX+:         | BY-:
//  BY+:         | BY-:
//  CX+:         | CY-:
//  CY+:         | CY-:
//  DX+:         | DY-:
//  DY+:         | DY-:
//
// 3. (a) Start out with AX (positive Values)
//    (b) Start moving the your spacemouse and try increasing the Value of AX till you can't get a higher value out of it.
//    (c) this is your positive maximum value for AX so write it down for AX
//4. Do the same for AY,BX,BY,....DY
// 5. Do the same for your negative Values to populate the minVals
// 6. Write all the positive Values starting from the top into the Array maxValues
// 7. Write all the negative Values starting from the top into the Array minValues
// 8. You finished calibrating.

// Insert measured Values like this: {AX,AY,BX,BY,CY,CY,DX,DY}.
int minVals[8] = { -512, -512, -512, -512, -512, -512, -512, -512};
int maxVals[8] = { +512, +512, +512, +512, +512, +512, +512, +512};

// Fourth calibration: Sensitivity
// Independent sensitivity multiplier for each axis movement. Use degbug mode 4 or use for example your cad program to verify changes.
// eg use lower value like 0.5 to make axis more sensitive, use higher value like 5 to make it less sensitive
// The Values you can change (those Values worked for me, you can or should change them to your preferences):

// Recommended calibration procedure for sensitivity
//  1. Make sure modFunc is on level 0!! Change debug to level 4 and upload sketch. Then open Serial Monitor. You will see Values TX, TY, TZ, RX, RY, RZ
//  2. Start moving your spacemouse. You will notice values changing.
//  3. Starting with TX try increasing this value as much as possible by moving your spacemouse around. If you get around 350 thats great. If not change pos_transX_sensitivy and reupload sketch. Repeat until it is around 350 for maximum motion.
//  4. Repeat steps 3 for TY,TZ,RX,RY,RZ
//  5. Verification: Move the Jockstick in funny ways. All you should get for eather TX,TX,TZ,RX,RY,RZ should be aprox. between -350 to 350.
//  6. You have finished sensitivity calibration. You can now test your spacemouse with your favorite program (e.g. Cad software, Slicer)
//  7. Aftermath: You notice the movements are hard to control. Try using Modification Functions (have a look at the beginning of the sketch) [I like level 3 the most. Experiment to find your favorite function]

float transX_sensitivity = 2;
float transY_sensitivity = 2;
float pos_transZ_sensitivity = 0.5;
float neg_transZ_sensitivity = 5; //I want low sensitiviy for down, therefore a high value.
float gate_neg_transZ = 15; // gate value, which negative z movements will be ignored (like an additional deadzone for -z).

float rotX_sensitivity = 1.5;
float rotY_sensitivity = 1.5;
float rotZ_sensitivity = 2;

// Direction
// Modify the direction of translation/rotation depending on preference. This can also be done per application in the 3DConnexion software afterwards
// Switch between true/false as desired.
bool invX = false; // pan left/right
bool invY = false; // pan up/down
bool invZ = false; // zoom in/out
bool invRX = false; // Rotate around X axis (tilt front/back)
bool invRY = false; // Rotate around Y axis (tilt left/right)
bool invRZ = false; // Rotate around Z axis (twist left/right)
//Switch Zooming with Up/Down Movement
//DISCLAIMER: This will make your spacemouse work like in the original code from TeachingTech, but if you try the 3DConnexion tutorial in the Spacemouse Home Software you will notice it won't work.
bool switchYZ = false; //change to true for switching movement

//
// -----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//LivingTheDream added
// Define the keycodes for each key
int numKeys = 4;
int KEYLIST[4] = {
  14,
  15,
  10,
  5
};
