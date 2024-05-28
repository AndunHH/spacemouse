// This code is the combination of multiple works by others:
// 1. Original code for the Space Mushroom by Shiura on Thingiverse: https://www.thingiverse.com/thing:5739462
//    The next two from the comments on the instructables page: https://www.instructables.com/Space-Mushroom-Full-6-DOFs-Controller-for-CAD-Appl/
//    and the comments of Thingiverse: https://www.thingiverse.com/thing:5739462/comments
// 2. Code to emulate a 3DConnexion Space Mouse by jfedor: https://pastebin.com/gQxUrScV
// 3. This code was then remixed by BennyBWalker to include the above two sketches: https://pastebin.com/erhTgRBH
// 4. Four joystick remix code by fdmakara: https://www.thingiverse.com/thing:5817728
// Teaching Techs work invloves mixing all of these. The basis is fdmakara's four joystick movement logic, with jfedor/BennyBWalker's HID SpaceMouse emulation.
// The four joystick logic sketch was setup for the joystick library instead of HID, so elements of this were omitted where not needed.
// The outputs were jumbled no matter how Teaching Tech plugged them in, so Teaching Tech spent a lot of time adding debugging code to track exactly what was happening.
// On top of this, Teching Tech has added more control of speed/direction and comments/links to informative resources to try and explain what is happening in each phase:
// https://www.printables.com/de/model/864950-open-source-spacemouse-space-mushroom-remixhttps://www.printables.com/de/model/864950-open-source-spacemouse-space-mushroom-remix 

// Spacemouse emulation
// Teaching Tech followed the instructions here from nebhead: https://gist.github.com/nebhead/c92da8f1a8b476f7c36c032a0ac2592a
// with two key differences:
// 1. Teaching Tech changed the word 'DaemonBite' to 'Spacemouse' in all references.
// 2. Teaching Tech changed the VID and PID values as per jfedor's instructions: vid=0x256f, pid=0xc631 (SpaceMouse Pro Wireless (cabled))
// 3. Daniel_1284580 recomments changing leonardo.upload.tool=avrdude to leonardo.upload.tool.serial=avrdude to get no error when compiling
// When compiling and uploading, Teaching Tech select Arduino AVR boards (in Sketchbook) > Spacemouse and then the serial port.
// You will also need to download and install the 3DConnexion software: https://3dconnexion.com/us/drivers-application/3dxware-10/
// If all goes well, the 3DConnexion software will show a SpaceMouse Pro wireless when the Arduino is connected.

//Additional work from other makers
// 1. Code to include meassured min and max values for each Joystick by Daniel_1284580 (In Software Version V1 and newer)
// 2. Improved code to make it more userfriendly by Daniel_1284580 (In Software Version V2 and newer)
// 3. Improved Code, improved comments and added written tutorials in comments. Implemented new algorithm "modifier function" for better motioncontrol by Daniel_1284580 (In Software Version V3)

// Include inbuilt Arduino HID library by NicoHood: https://github.com/NicoHood/HID 
#include "HID.h"
//Include math operators for doing better calculation algorithms. Arduino math is a standard library already included.
#include <math.h>
#define sign(x) ((x) < 0 ? -1 : ((x) > 0 ? 1 : 0)) //Define Signum Function

// ---------------------------------------------------------Values you can change--------------------------------------------------------------------------
// Debugging
// 0: Debugging off. Set to this once everything is working.
// 1: Output raw joystick values. 0-1023 raw ADC 10-bit values
// 2: Output centered joystick values. Values should be approx -500 to +500, jitter around 0 at idle.
// 3: Output centered joystick values. Filtered for deadzone. Approx -500 to +500, locked to zero at idle.
// 4: Output translation and rotation values. Approx -800 to 800 depending on the parameter.
// 5: Output debug 4 and 5 side by side for direct cause and effect reference.
int debug = 5;

// Modifier Function 
// Modify resulting behaviour of Joystick input values
// DISCLAIMER: This should be at level 0 when calibrating "Independent Sensitivity Multiplier".
// 0: linear y = x [Standard behaviour: No modification]
// 1: squared function y = x^2*sign(x) [altered squared function working in positive and negative direction]
// 2: tangent function: y = tan(x) [Results in a flat curve near zero but increases the more you are away from zero]
// 3: squared tangent function: y = tan(x^2*sign(X)) [Results in a flatter curve near zero but increases alot the more you are away from zero]
// 4: cubed tangent function: y = tan(x^3) [Results in a very flat curve near zero but increases drastically the more you are away from zero]
int modFunc = 3;  //3 

// Direction
// Modify the direction of translation/rotation depending on preference. This can also be done per application in the 3DConnexion software.
// Switch between true/false as desired.
// Values got changed from TeachingTechs original software because of the 3DConnexion tutorial in the Spacemouse Home Software not working the right way.
bool invX = true; // pan left/right
bool invY = true; // pan up/down
bool invZ = false; // zoom in/out
bool invRX = true; // Rotate around X axis (tilt front/back)
bool invRY = true; // Rotate around Y axis (tilt left/right)
bool invRZ = false; // Rotate around Z axis (twist left/right)
//Original Values from TT in original Software 
/*
bool invX = false; // pan left/right
bool invY = false; // pan up/down
bool invZ = true; // zoom in/out
bool invRX = true; // Rotate around X axis (tilt front/back)
bool invRY = false; // Rotate around Y axis (tilt left/right)
bool invRZ = true; // Rotate around Z axis (twist left/right)
*/

//Switch Zooming with Up/Down Movement
//DISCLAIMER: This will make your spacemouse work like in the original code from TeachingTech, but if you try the 3DConnexion tutorial in the Spacemouse Home Software you will notice it won't work.
bool switchYZ = false; //change to true for switching movement 

// Speed - Had to be removed when the V2 shorter range of motion. The logic reduced sensitivity on small movements. Use 3DConnexion slider instead for V2.
// Modify to change sensitibity/speed. Default and maximum 100. Works like a percentage ie. 50 is half as fast as default. This can also be done per application in the 3DConnexion software.
//int16_t speed = 100; 

// Min and max values to be populated by you testing the positions in Debug mode 3.
// Insert measured Values like this: {AX,AY,BX,BY,CY,CY,DX,DY}.
int minVals[8]={-519,-521,-512,-501,-520,-522,-508,-507};
int maxVals[8]={504,502,511,522,503,501,515,516};
//Recommended calibration prozedure 
// 1. Change debug to level 3 and upload sketch. Then open Serial Monitor. 
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
// Example: 
//    maxVals      | minVals
//  --------------------------------
//    AX+: 504     | AX-:
//    AY+: 502     | AY-:
//    BX+: 511     | BY-:
//    BY+: 522     | BY-:
//    CX+: 503     | CY-:
//    CY+: 501     | CY-:
//    DX+: 515     | DY-:
//    DY+: 516     | DY-:
// 5. Do the same for your negative Values 
// Example: 
//    maxVals      | minVals
//  --------------------------------
//    AX+:      | AX-: -519  
//    AY+:      | AY-: -521
//    BX+:      | BX-: -512
//    BY+:      | BY-: -501
//    CX+:      | CX-: -520
//    CY+:      | CY-: -522
//    DX+:      | DX-: -508
//    DY+:      | DY-: -507
// 6. Write all the positive Values starting from the top into the Array maxValues
// 7. Write all the negative Values starting from the top into the Array minValues
// 8. You finished calibrating. Move on to Indipendent sensitivity multiplier

// Indipendent sensitivity multiplier for each axis movement. Use degbug mode 4 or use for example your cad program to verify changes.
/* Factory Settings:
float pos_transX_sensetivity = 2; // eg use lower value like 0.5 to make axis more sensitive, use higher value like 5 to make it less sensitive
float neg_transX_sensetivity = 2;
float pos_transY_sensetivity = 2;
float neg_transY_sensetivity = 2;
float pos_transZ_sensetivity = 4;
float neg_transZ_sensetivity = 4;
float pos_rotX_sensetivity = 1;
float neg_rotX_sensetivity = 1;
float pos_rotY_sensetivity = 1;
float neg_rotY_sensetivity = 1;
float pos_rotZ_sensetivity = 2;
float neg_rotZ_sensetivity = 2; */
// The Values you can change (those Values worked for me, you can or should change them to your preferences):
float pos_transX_sensetivity = 2;
float neg_transX_sensetivity = 2;
float pos_transY_sensetivity = 2;
float neg_transY_sensetivity = 2;
float pos_transZ_sensetivity = 1;
float neg_transZ_sensetivity = 2;
float pos_rotX_sensetivity = 1;
float neg_rotX_sensetivity = 1;
float pos_rotY_sensetivity = 1;
float neg_rotY_sensetivity = 1;
float pos_rotZ_sensetivity = 2;
float neg_rotZ_sensetivity = 2;
// Recommended calibration prozedure
//  1. Make sure modFunc is on level 0!! Change debug to level 4 and upload sketch. Then open Serial Monitor. You will see Values TX, TY, TZ, RX, RY, RZ
//  2. Start moving your spacemouse. You will notice Values changing. 
//  3. Starting with TX try increasing this value as much as possible by moving your spacemouse around. If you get around 350 thats great. If not change pos_transX_sensitivy and reupload sketch. Repeat until it is around 350 for maximum motion. 
//  4. Now try decreasing TX as much as possible. If you get around -350 thats great. If not change neg_transX_sensitivity until it is around -350 for maximum motion.
//  5. Repeat steps 3 and 4 for TY,TZ,RX,RY,RZ 
//  6. Verification: Move the Jockstick in funny ways. All you should get for eather TX,TX,TZ,RX,RY,RZ should be aprox. between -350 to 350. 
//  7. You have finished sensitivity calibration. You can now test your spacemouse with your favorite program (e.g. Cad software, Slicer)
//  8. Aftermath: You notice the movements are hard to control. Try using Modification Functions (have a look at the beginning of the sketch) [I like level 3 the most. Experiment to find your favorite function]


// Deadzone to filter out unintended movements. Increase if the mouse has small movements when it should be idle or the mouse is too senstive to subtle movements.
int DEADZONE = 2; // Recommended to have this as small as possible for V2 to allow smaller knob range of motion.

// Default Assembly when looking from above:
//    C           Y+
//    |           .
// B--+--D   X-...Z+...X+
//    |           .
//    A           Y-
//
// -----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

// Wiring. Matches the first eight analogue pins of the Arduino Pro Micro (atmega32u4)
int PINLIST[8] = { // The positions of the reads
  A0, // X-axis A
  A1, // Y-axis A
  A2, // X-axis B
  A3, // Y-axis B
  A6, // X-axis C
  A7, // Y-axis C
  A8, // X-axis D
  A9  // Y-axis D
};

//LivingTheDream added
// Define the keycodes for each key
int numKeys = 4;
int KEYLIST[4] = {
  14, 
  15, 
  10, 
  5
};

// This portion sets up the communication with the 3DConnexion software. The communication protocol is created here.
// hidReportDescriptor webpage can be found here: https://eleccelerator.com/tutorial-about-usb-hid-report-descriptors/ 
// Altered physical, logical range to ranges the 3DConnexion software expects by Daniel_1284580.
static const uint8_t _hidReportDescriptor[] PROGMEM = {
  0x05, 0x01,           // Usage Page (Generic Desktop)
  0x09, 0x08,           // Usage (Multi-Axis)
  0xa1, 0x01,           // Collection (Application)
  0xa1, 0x00,           // Collection (Physical)
  0x85, 0x01,           // Report ID
  0x16, 0xAA, 0xFE,     // Logical Minimum (-350) (0xFEAA in little-endian)
  0x26, 0x5E, 0x01,     // Logical Maximum (350) (0x015E in little-endian)
  0x36, 0x88, 0xFA,     // Physical Minimum (-1400) (0xFA88 in little-endian)
  0x46, 0x70, 0x05,     // Physical Maximum (1400) (0x0570 in little-endian)
  0x09, 0x30,           // Usage (X)
  0x09, 0x31,           // Usage (Y)
  0x09, 0x32,           // Usage (Z)
  0x75, 0x10,           // Report Size (16)
  0x95, 0x03,           // Report Count (3)
  0x81, 0x02,           // Input (variable,absolute)
  0xC0,                 // End Collection
  0xa1, 0x00,           // Collection (Physical)
  0x85, 0x02,           // Report ID
  0x16, 0xAA, 0xFE,     // Logical Minimum (-350)
  0x26, 0x5E, 0x01,     // Logical Maximum (350)
  0x36, 0x88, 0xFA,     // Physical Minimum (-1400)
  0x46, 0x70, 0x05,     // Physical Maximum (1400)
  0x09, 0x33,           // Usage (RX)
  0x09, 0x34,           // Usage (RY)
  0x09, 0x35,           // Usage (RZ)
  0x75, 0x10,           // Report Size (16)
  0x95, 0x03,           // Report Count (3)
  0x81, 0x02,           // Input (variable,absolute)
  0xC0,                 // End Collection
  0xa1, 0x00,           // Collection (Physical)
  0x85, 0x03,           //  Report ID
  0x15, 0x00,           //   Logical Minimum (0)
  0x25, 0x01,           //    Logical Maximum (1)
  0x75, 0x01,           //    Report Size (1)
  0x95, 32,             //    Report Count (24)
  0x05, 0x09,           //    Usage Page (Button)
  0x19, 1,              //    Usage Minimum (Button #1)
  0x29, 32,             //    Usage Maximum (Button #24)
  0x81, 0x02,           //    Input (variable,absolute)
  0xC0,
  0xC0
};

// Axes are matched to pin order.
#define AX 0
#define AY 1
#define BX 2
#define BY 3
#define CX 4
#define CY 5
#define DX 6
#define DY 7

//LivingThe Dream added
// Keys matched to pins (thouse corresponde to the 3D Connection Software names of the keys)
#define KEY_LEFT_MENU 14
#define KEY_LEFT_ROTATEVIEW 15
#define KEY_LEFT_UNKNOWN 10
#define KEY_LEFT_FIT 5

//Please do not change this anymore. Use indipendent sensitivity multiplier.
int totalSentitivity=350;
// Centerpoint variable to be populated during setup routine.
int centerPoints[8];

//LivingThe Dream added
int keyState[4];
//Needed for key evaluation
int8_t keyOut[4];
int key_waspressed[4];
float timestamp[4];

//Modifier Functions
int modifierFunction(int x) {
  //making sure function input never exedes range of -350 to 350
  x = constrain(x,-350,350);
  double result;
  if(modFunc==0){
    //no modification
    result=x;
  }
  if(modFunc==1){
  // using squared function y = x^2*sign(x)
    result = 350*pow(x/350.0,2)*sign(x); //sign putting out -1 or 1 depending on sign of value. (Is needed because x^2 will always be positive)
  }
  if(modFunc==2){
  // unsing tan function: tan(x)
    result = 350*tan(x/350.0);
  }
  if(modFunc==3){
  // unsing squared tan function: tan(x^2*sign(x))
    result = 350*tan(pow(x/350.0,2)*sign(x)); //sign putting out -1 or 1 depending on sign of value. (Is needed because x^2 will always be positive)
  }
  if(modFunc==4){
    //unsing cubed tan function: tan(x^3) 
    result = 350*tan(pow(x/350.0,3));
  }
  
  //make sure values between-350 and 350 are allowed
  result = constrain(result, -350, 350);
  //converting doubles to int again
  return (int)round(result);
}

// Function to read and store analogue voltages for each joystick axis.
void readAllFromJoystick(int *rawReads){
  for(int i=0; i<8; i++){
    rawReads[i] = analogRead(PINLIST[i]);
  }
}

// LivingTheDream added Daniel_1284580 modified into a forloop
// Function to read and store the digital states for each of the keys
void readAllFromKeys(int *keyVals){
  for(int i=0; i<numKeys; i++){
    keyVals[i] = digitalRead(KEYLIST[i]);
  }
}

void setup() {
//LivingTheDream added
  /* Setting up the switches */ 
  for (int i = 0; i < numKeys; i++) { 
    pinMode(KEYLIST[i], INPUT_PULLUP);
  }

  // HID protocol is set.
  static HIDSubDescriptor node(_hidReportDescriptor, sizeof(_hidReportDescriptor));
  HID().AppendDescriptor(&node);
  // Begin Seral for debugging
  Serial.begin(250000);
  delay(100);
  // Read idle/centre positions for joysticks.
  readAllFromJoystick(centerPoints);
  readAllFromJoystick(centerPoints);
}

// Function to send translation and rotation data to the 3DConnexion software using the HID protocol outlined earlier. Two sets of data are sent: translation and then rotation.
// For each, a 16bit integer is split into two using bit shifting. The first is mangitude and the second is direction.
void send_command(int16_t rx, int16_t ry, int16_t rz, int16_t x, int16_t y, int16_t z, int8_t k1, int8_t k2, int8_t k3, int8_t k4) {
  uint8_t trans[6] = { x & 0xFF, x >> 8, y & 0xFF, y >> 8, z & 0xFF, z >> 8 };
  HID().SendReport(1, trans, 6);
  uint8_t rot[6] = { rx & 0xFF, rx >> 8, ry & 0xFF, ry >> 8, rz & 0xFF, rz >> 8 };
  HID().SendReport(2, rot, 6);

  // LivingTheDream added
  uint8_t key[4] = {keyOut[0], keyOut[1], keyOut[2], keyOut[3]};
  HID().SendReport(3, key, 4);  
}

void loop() {
  int rawReads[8], centered[8], keyVals[numKeys];
  // Joystick values are read. 0-1023
  readAllFromJoystick(rawReads);

    // LivingTheDream added reading of key presses
  readAllFromKeys(keyVals);

  // Report back 0-1023 raw ADC 10-bit values if enabled
  if(debug == 1){ 
    Serial.print("AX:");
    Serial.print(rawReads[0]);
    Serial.print(",");
    Serial.print("AY:");
    Serial.print(rawReads[1]);
    Serial.print(",");
    Serial.print("BX:");
    Serial.print(rawReads[2]);
    Serial.print(",");
    Serial.print("BY:");
    Serial.print(rawReads[3]);
    Serial.print(",");
    Serial.print("CX:");
    Serial.print(rawReads[4]);
    Serial.print(",");
    Serial.print("CY:");
    Serial.print(rawReads[5]);
    Serial.print(",");
    Serial.print("DX:");
    Serial.print(rawReads[6]);
    Serial.print(",");
    Serial.print("DY:");
    Serial.println(rawReads[7]);
  }

  // Subtract centre position from measured position to determine movement.
  for(int i=0; i<8; i++) centered[i] = rawReads[i] - centerPoints[i]; // 

  // Report centered joystick values if enabled. Values should be approx -500 to +500, jitter around 0 at idle.
  if(debug == 2){
    Serial.print("AX:");
    Serial.print(centered[0]);
    Serial.print(",");
    Serial.print("AY:");
    Serial.print(centered[1]);
    Serial.print(",");
    Serial.print("BX:");
    Serial.print(centered[2]);
    Serial.print(",");
    Serial.print("BY:");
    Serial.print(centered[3]);
    Serial.print(",");
    Serial.print("CX:");
    Serial.print(centered[4]);
    Serial.print(",");
    Serial.print("CY:");
    Serial.print(centered[5]);
    Serial.print(",");
    Serial.print("DX:");
    Serial.print(centered[6]);
    Serial.print(",");
    Serial.print("DY:");
    Serial.println(centered[7]);
  }
  // Filter movement values. Set to zero if movement is below deadzone threshold.
  for(int i=0; i<8; i++){
    if(centered[i]<DEADZONE && centered[i]>-DEADZONE) centered[i] = 0;
  }
  // Report centered joystick values. Filtered for deadzone. Approx -500 to +500, locked to zero at idle
  if(debug == 3){
    Serial.print("AX:");
    Serial.print(centered[0]);
    Serial.print(",");
    Serial.print("AY:");
    Serial.print(centered[1]);
    Serial.print(",");
    Serial.print("BX:");
    Serial.print(centered[2]);
    Serial.print(",");
    Serial.print("BY:");
    Serial.print(centered[3]);
    Serial.print(",");
    Serial.print("CX:");
    Serial.print(centered[4]);
    Serial.print(",");
    Serial.print("CY:");
    Serial.print(centered[5]);
    Serial.print(",");
    Serial.print("DX:");
    Serial.print(centered[6]);
    Serial.print(",");
    Serial.print("DY:");
    Serial.println(centered[7]);
  }

  // Doing all through arithmetic contribution by fdmakara
  // Integer has been changed to 16 bit int16_t to match what the HID protocol expects.
  int16_t transX, transY, transZ, rotX, rotY, rotZ; // Declare movement variables at 16 bit integers
  // Original fdmakara calculations
  //transX = (-centered[AX] +centered[CX])/1;
  //transY = (-centered[BX] +centered[DX])/1;
  //transZ = (-centered[AY] -centered[BY] -centered[CY] -centered[DY])/2;
  //rotX = (-centered[AY] +centered[CY])/2;
  //rotY = (+centered[BY] -centered[DY])/2;
  //rotZ = (+centered[AX] +centered[BX] +centered[CX] +centered[DX])/4;
  // Teaching Techs altered calculations based on debug output. Final divisor can be changed to alter sensitivity for each axis.
  /*
  transX = -(-centered[CY] +centered[AY])/1;  
  transY = (-centered[BY]+centered[DY])/1;
  if((abs(centered[AX])>DEADZONE)&&(abs(centered[BX])>DEADZONE)&&(abs(centered[CX])>DEADZONE)&&(abs(centered[DX])>DEADZONE)){
    transZ = (-centered[AX] -centered[BX] -centered[CX] -centered[DX])/1;
    transX = 0;
    transY = 0;
  } else {
    transZ = 0;
  } 
  rotX = (-centered[AX] +centered[CX])/1;
  rotY = (+centered[BX] -centered[DX])/1;
  if((abs(centered[AY])>DEADZONE)&&(abs(centered[BY])>DEADZONE)&&(abs(centered[CY])>DEADZONE)&&(abs(centered[DY])>DEADZONE)){
    rotZ = (+centered[AY] +centered[BY] +centered[CY] +centered[DY])/2;
    rotX = 0;
    rotY = 0;
  } else {
    rotZ = 0;
  }
  */
  // More Complex Calculations with min max values by Daniel_1284580.
  // transX
    //positive translation in x
  if((centered[CY]<-DEADZONE)&&(centered[AY]>DEADZONE)){
    transX = -(-map(centered[CY],minVals[CY],0,-totalSentitivity,0)+map(centered[AY],0,maxVals[AY],0,totalSentitivity))/pos_transX_sensetivity;
    transX=modifierFunction(transX); //recalculate with modifier function
    //negative translation in x
  }else if((centered[CY]>DEADZONE)&&(centered[AY]<-DEADZONE)){
    transX = -(-map(centered[CY],minVals[CY],0,-totalSentitivity,0)+map(centered[AY],0,maxVals[AY],0,totalSentitivity))/neg_transX_sensetivity;
    transX=modifierFunction(transX); //recalculate with modifier function
  }else{
    transX=0;
  }
  // transY
    //positive translation in y
  if((centered[BY]<-DEADZONE)&&(centered[DY]>DEADZONE)){
    transY = -(-map(centered[BY],minVals[BY],0,-totalSentitivity,0)+map(centered[DY],0,maxVals[DY],0,totalSentitivity))/pos_transY_sensetivity;
    transY=modifierFunction(transY); //recalculate with modifier function
    //negative translation in y
  }else if((centered[BY]>DEADZONE)&&(centered[DY]<-DEADZONE)){
    transY = -(-map(centered[BY],minVals[BY],0,-totalSentitivity,0)+map(centered[DY],0,maxVals[DY],0,totalSentitivity))/neg_transX_sensetivity;
    transY=modifierFunction(transY); //recalculate with modifier function
  }else{
    transY=0;
  }
  // transZ
    //positive translation in z
  if((centered[AX]>DEADZONE)&&(centered[BX]>DEADZONE)&&(centered[CX]>DEADZONE)&&(centered[DX]>DEADZONE)){
    transZ = (-map(centered[AX],0,maxVals[AX],0,totalSentitivity) -map(centered[BX],0,maxVals[BX],0,totalSentitivity) -map(centered[CX],0,maxVals[CX],0,totalSentitivity) -map(centered[DX],0,maxVals[DX],0,totalSentitivity))/pos_transZ_sensetivity;
    transZ=modifierFunction(transZ); //recalculate with modifier function
    transX = 0;
    transY = 0;
    //negative translation in z
  } else if((centered[AX]<-DEADZONE)&&(centered[BX]<-DEADZONE)&&(centered[CX]<-DEADZONE)&&(centered[DX]<-DEADZONE)){
    transZ = (-map(centered[AX],minVals[AX],0,-totalSentitivity,0) -map(centered[BX],minVals[BX],0,-totalSentitivity,0) -map(centered[CX],minVals[CX],0,-totalSentitivity,0) -map(centered[DX],minVals[DX],0,-totalSentitivity,0))/neg_transZ_sensetivity;
    transZ=modifierFunction(transZ); //recalculate with modifier function
    transX = 0;
    transY = 0;
  }else{
    transZ = 0;
  } 
  // rotX
    //positive rotation in x
  if((centered[CX]<-DEADZONE)&&(centered[AX]>DEADZONE)){
    rotX = -(-map(centered[CX],minVals[CX],0,-totalSentitivity,0)+map(centered[AX],0,maxVals[AX],0,totalSentitivity))/pos_rotX_sensetivity;
    rotX=modifierFunction(rotX); //recalculate with modifier function
  }else if((abs(centered[CX])<DEADZONE)&&(centered[AX]>DEADZONE)){
    // cx is zero, but ax is pushed down -> positive rotation x
    rotX = -map(centered[AX],0,maxVals[AX],0,totalSentitivity)/pos_rotX_sensetivity;
    rotX=modifierFunction(rotX); //recalculate with modifier function
  }else if((centered[CX]>DEADZONE)&&(centered[AX]<-DEADZONE)){
    //negative rotation in x
    rotX = -(-map(centered[CX],minVals[CX],0,-totalSentitivity,0)+map(centered[AX],0,maxVals[AX],0,totalSentitivity))/neg_rotX_sensetivity;
    rotX=modifierFunction(rotX); //recalculate with modifier function
   }else if((centered[CX]>DEADZONE)&&(abs(centered[AX])<DEADZONE)){
    // ax is zero, but cx is pushed down -> negative rotation x
    rotX = map(centered[CX],minVals[CX],0,-totalSentitivity,0)/neg_rotX_sensetivity;
    rotX=modifierFunction(rotX); //recalculate with modifier function
  }else{
    rotX=0;
  }
  // rotY
    //positive rotation in y
  if((centered[DX]<-DEADZONE)&&(centered[BX]>DEADZONE)){
    rotY = -(-map(centered[BX],minVals[BX],0,-totalSentitivity,0)+map(centered[DX],0,maxVals[DX],0,totalSentitivity))/pos_rotY_sensetivity;
    rotY=modifierFunction(rotY); //recalculate with modifier function  
  }else if((abs(centered[DX])<DEADZONE)&&(centered[BX]>DEADZONE)){
    // Dx is zero, but Bx is pushed down -> positive rotation y
    rotY = -(-map(centered[BX],minVals[BX],0,-totalSentitivity,0)+map(centered[DX],0,maxVals[DX],0,totalSentitivity))/pos_rotY_sensetivity;
    rotY=modifierFunction(rotY); //recalculate with modifier function  
  }else if((centered[DX]>DEADZONE)&&(centered[BX]<-DEADZONE)){
    //negative rotation in y
    rotY = -(-map(centered[BX],minVals[BX],0,-totalSentitivity,0)+map(centered[DX],0,maxVals[DX],0,totalSentitivity))/neg_rotY_sensetivity;
    rotY=modifierFunction(rotY); //recalculate with modifier function
   }else if((centered[DX]>DEADZONE)&&(abs(centered[BX])<DEADZONE)){
    // Bx is zero, but dx is pushed down -> negative rotation y
    rotY = -(-map(centered[BX],minVals[BX],0,-totalSentitivity,0)+map(centered[DX],0,maxVals[DX],0,totalSentitivity))/neg_rotY_sensetivity;
    rotY=modifierFunction(rotY); //recalculate with modifier function
  }else{
    rotY=0;
  }
  // rotZ
    //positive rotation in z
  if((centered[AY]>DEADZONE)&&(centered[BY]>DEADZONE)&&(centered[CY]>DEADZONE)&&(centered[DY]>DEADZONE)){
    rotZ = (+map(centered[AY],0,maxVals[AY],0,totalSentitivity) +map(centered[BY],0,maxVals[BY],0,totalSentitivity) +map(centered[CY],0,maxVals[CY],0,totalSentitivity) +map(centered[DY],0,maxVals[DY],0,totalSentitivity))/pos_rotZ_sensetivity;
    rotZ=modifierFunction(rotZ); //recalculate with modifier function
    rotX = 0;
    rotY = 0;
    //negative rotation in z
  }else if((centered[AY]<-DEADZONE)&&(centered[BY]<-DEADZONE)&&(centered[CY]<-DEADZONE)&&(centered[DY]<-DEADZONE)){
    rotZ = (+map(centered[AY],minVals[AX],0,-totalSentitivity,0) +map(centered[BY],minVals[BY],0,-totalSentitivity,0) +map(centered[CY],minVals[CY],0,-totalSentitivity,0) +map(centered[DY],minVals[DY],0,-totalSentitivity,0))/neg_rotZ_sensetivity;
    rotZ=modifierFunction(rotZ); //recalculate with modifier function
    rotX = 0;
    rotY = 0;
  }else {
    rotZ = 0;
  }

//LivingTheDream added
  //Button Evaluation
  for(int i=0; i<numKeys; i++){
    if(keyVals[i]!=keyState[i]){
      // Making sure button cannot trigger multiple times which would result in overloading HID.
      if(key_waspressed[i]==0){
        keyOut[i]=1;
        key_waspressed[i]=1;
        timestamp[i]=millis();
      }else{
        keyOut[i]=0;
      }
    }else{
      if(key_waspressed[i]==1){
        //Debouncing
        if(millis()-timestamp[i]>1000){
          key_waspressed[i]=0;
        }
      }
      
    }
  }
// Alter speed to suit user preference - Use 3DConnexion slider instead for V2.
  //transX = transX/100*speed;
  //transY = transY/100*speed;
  //transZ = transZ/100*speed;
  //rotX = rotX/100*speed;
  //rotY = rotY/100*speed;
  //rotZ = rotZ/100*speed;
// Invert directions if needed
  if(invX == true){ transX = transX*-1;};
  if(invY == true){ transY = transY*-1;};
  if(invZ == true){ transZ = transZ*-1;};
  if(invRX == true){ rotX = rotX*-1;};
  if(invRY == true){ rotY = rotY*-1;};
  if(invRZ == true){ rotZ = rotZ*-1;};

// Report translation and rotation values if enabled. Approx -800 to 800 depending on the parameter.
  if(debug == 4){
    Serial.print("TX:");
    Serial.print(transX);
    Serial.print(",");
    Serial.print("TY:");
    Serial.print(transY);
    Serial.print(",");
    Serial.print("TZ:");
    Serial.print(transZ);
    Serial.print(",");
    Serial.print("RX:");
    Serial.print(rotX);
    Serial.print(",");
    Serial.print("RY:");
    Serial.print(rotY);
    Serial.print(",");
    Serial.print("RZ:");
    Serial.print(rotZ);
      // LivingTheDream added printing the key pressed values
    Serial.print(",");
    Serial.print("Key1:");
    Serial.print(keyOut[0]);
    Serial.print(",");
    Serial.print("Key2:");
    Serial.print(keyOut[1]);
    Serial.print(",");
    Serial.print("Key3:");
    Serial.print(keyOut[2]);
    Serial.print(",");
    Serial.print("Key4:");
    Serial.println(keyOut[3]);  
  }
// Report debug 4 and 5 info side by side for direct reference if enabled. Very useful if you need to alter which inputs are used in the arithmatic above.
  if(debug == 5){
    Serial.print("AX:");
    Serial.print(centered[0]);
    Serial.print(",");
    Serial.print("AY:");
    Serial.print(centered[1]);
    Serial.print(",");
    Serial.print("BX:");
    Serial.print(centered[2]);
    Serial.print(",");
    Serial.print("BY:");
    Serial.print(centered[3]);
    Serial.print(",");
    Serial.print("CX:");
    Serial.print(centered[4]);
    Serial.print(",");
    Serial.print("CY:");
    Serial.print(centered[5]);
    Serial.print(",");
    Serial.print("DX:");
    Serial.print(centered[6]);
    Serial.print(",");
    Serial.print("DY:");
    Serial.print(centered[7]);
    Serial.print("||");
    Serial.print("TX:");
    Serial.print(transX);
    Serial.print(",");
    Serial.print("TY:");
    Serial.print(transY);
    Serial.print(",");
    Serial.print("TZ:");
    Serial.print(transZ);
    Serial.print(",");
    Serial.print("RX:");
    Serial.print(rotX);
    Serial.print(",");
    Serial.print("RY:");
    Serial.print(rotY);
    Serial.print(",");
    Serial.print("RZ:");
    Serial.println(rotZ);
  }

// Send data to the 3DConnexion software.
// The correct order for TeachingTech was determined after trial and error
  if (switchYZ==true){
    //Original from TT, but 3DConnextion tutorial will not work: 
    send_command(rotX, rotZ, rotY, transX, transZ, transY,keyOut[0], keyOut[1], keyOut[2], keyOut[3]);
  }else{
    // Daniel_1284580 noticed the 3dconnexion tutorial was not working the right way so they got changed 
    send_command(rotX, rotY, rotZ, transX, transY, transZ,keyOut[0], keyOut[1], keyOut[2], keyOut[3]);
  }
}
