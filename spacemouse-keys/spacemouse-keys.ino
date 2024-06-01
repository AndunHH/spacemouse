// This is the arduino source code for the open source space mouse with keys.
// Please read the introduction and history with all contributors here:
// https://github.com/AndunHH/spacemouse

// One good starting point is the work and video by TeachingTech: https://www.printables.com/de/model/864950-open-source-spacemouse-space-mushroom-remix
// Then follow along on github, how we reached this state of the source code.

// Include inbuilt Arduino HID library by NicoHood: https://github.com/NicoHood/HID
#include "HID.h"
//Include math operators for doing better calculation algorithms. Arduino math is a standard library already included.
#include <math.h>
#define sign(x) ((x) < 0 ? -1 : ((x) > 0 ? 1 : 0)) //Define Signum Function

// The user specific settings, like pin mappings or special configuration variables and sensitivities are stored in config.h.
// Please open config_sample.h, adjust your settings and save it as config.h
#include "config.h"

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

// Axes are matched to pin order. Don't change this, this is just for accessing the arrays from 0 to 7.
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
// not used?
//define KEY_LEFT_MENU 14
//define KEY_LEFT_ROTATEVIEW 15
//define KEY_LEFT_UNKNOWN 10
//define KEY_LEFT_FIT 5

//Please do not change this anymore. Use indipendent sensitivity multiplier.
int totalSensitivity = 350;
// Centerpoint variable to be populated during setup routine.
int centerPoints[8];

// Variables to read of the keys
int keyVals[4]; //store raw value of the keys, without debouncing
int keyState[4];
//Needed for key evaluation
int8_t keyOut[4];
int key_waspressed[4];
float timestamp[4];

//Modifier Functions
int modifierFunction(int x) {
  //making sure function input never exedes range of -350 to 350
  x = constrain(x, -350, 350);
  double result;
  if (modFunc == 0) {
    //no modification
    result = x;
  }
  if (modFunc == 1) {
    // using squared function y = x^2*sign(x)
    result = 350 * pow(x / 350.0, 2) * sign(x); //sign putting out -1 or 1 depending on sign of value. (Is needed because x^2 will always be positive)
  }
  if (modFunc == 2) {
    // unsing tan function: tan(x)
    result = 350 * tan(x / 350.0);
  }
  if (modFunc == 3) {
    // unsing squared tan function: tan(x^2*sign(x))
    result = 350 * tan(pow(x / 350.0, 2) * sign(x)); //sign putting out -1 or 1 depending on sign of value. (Is needed because x^2 will always be positive)
  }
  if (modFunc == 4) {
    //unsing cubed tan function: tan(x^3)
    result = 350 * tan(pow(x / 350.0, 3));
  }

  //make sure values between-350 and 350 are allowed
  result = constrain(result, -350, 350);
  //converting doubles to int again
  return (int)round(result);
}

// Function to read and store analogue voltages for each joystick axis.
void readAllFromJoystick(int *rawReads) {
  for (int i = 0; i < 8; i++) {
    rawReads[i] = analogRead(PINLIST[i]);
  }
}

// LivingTheDream added Daniel_1284580 modified into a forloop
// Function to read and store the digital states for each of the keys
void readAllFromKeys(int *keyVals) {
  for (int i = 0; i < numKeys; i++) {
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
  Serial.setTimeout(2); // the serial interface will look for new debug values and it will only wait 2ms
  // Read idle/centre positions for joysticks.
  readAllFromJoystick(centerPoints);
  readAllFromJoystick(centerPoints);
  delay(100);
  Serial.println("Please enter the debug mode now or while the script is reporting. -1 to shut off.");
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

int rawReads[8], centered[8];

// Integer has been changed to 16 bit int16_t to match what the HID protocol expects.
int16_t transX, transY, transZ, rotX, rotY, rotZ; // Declare movement variables at 16 bit integers
int tmpInput; // store the value, the user might input over the serial


void loop() {
  //check if the user entered a debug mode via serial interface
  if (Serial.available()) {
    tmpInput = Serial.parseInt(); // Read from serial interface, if a new debug value has been sent. Serial timeout has been set in setup()
    if (tmpInput != 0) {
      debug = tmpInput;
      if (tmpInput == -1) {
        Serial.println("Please enter the debug mode now or while the script is reporting.");
      }
    }

  }

  // Joystick values are read. 0-1023
  readAllFromJoystick(rawReads);

  // LivingTheDream added reading of key presses
  readAllFromKeys(keyVals);

  // Report back 0-1023 raw ADC 10-bit values if enabled
  debugOutput1();

  // Subtract centre position from measured position to determine movement.
  for (int i = 0; i < 8; i++) {
    centered[i] = rawReads[i] - centerPoints[i];
  }

  calcMinMax(); // debug=20 to calibrate MinMax values

  // Report centered joystick values if enabled. Values should be approx -500 to +500, jitter around 0 at idle
  debugOutput2();

  // Filter movement values. Set to zero if movement is below deadzone threshold.
  for (int i = 0; i < 8; i++) {
    if (centered[i] < DEADZONE && centered[i] > -DEADZONE) {
      centered[i] = 0;
    }
    else {
      if (centered[i] < 0) { //if the value is smaller 0 ...
        // ... map the value from the [min,0] to [-350,0]
        centered[i] = map(centered[i], minVals[i], 0, -totalSensitivity, 0);
      }
      else { // if the value is > 0 ...
        // ... map the values from the [0,max] to [0,+350]
        centered[i] = map(centered[i], 0, maxVals[i], 0, totalSensitivity);
      }
    }
  }

  // Report centered joystick values. Filtered for deadzone. Approx -350 to +350, locked to zero at idle
  debugOutput3();

  // transX
  transX = (-centered[CY] + centered[AY]) / transX_sensitivity;
  transX = modifierFunction(transX); //recalculate with modifier function

  // transY
  transY = (-centered[BY] + centered[DY]) / transY_sensitivity;
  transY = modifierFunction(transY); //recalculate with modifier function

  transZ = -centered[AX] - centered[BX] - centered[CX] - centered[DX];
  if (transZ < 0) {
    transZ = modifierFunction(transZ / neg_transZ_sensitivity); //recalculate with modifier function
    if (abs(transZ) < gate_neg_transZ) {
      transZ = 0;
    }
  } else { // pulling the knob upwards is much heavier... smaller factor
    transZ = constrain(transZ / pos_transZ_sensitivity, -350, 350); // no modifier function, just constrain linear!
  }

  // rotX
  rotX = (-centered[CX] + centered[AX]) / rotX_sensitivity;
  rotX = modifierFunction(rotX); //recalculate with modifier function
  if (abs(rotX) < 15) {
    rotX = 0;
  }
  //positive rotation in x

  // rotY
  rotY = (-centered[BX] + centered[DX]) / rotY_sensitivity;
  rotY = modifierFunction(rotY); //recalculate with modifier function
  if (abs(rotY) < 15) {
    rotY = 0;
  }

  // rotZ
  rotZ = (centered[AY] + centered[BY] + centered[CY] + centered[DY]) / rotZ_sensitivity;
  rotZ = modifierFunction(rotZ); //recalculate with modifier function
  if (abs(rotZ) < 15) {
    rotZ = 0;
  }
  //positive rotation in z

  //LivingTheDream added
  //Button Evaluation
  for (int i = 0; i < numKeys; i++) {
    if (keyVals[i] != keyState[i]) {
      // Making sure button cannot trigger multiple times which would result in overloading HID.
      if (key_waspressed[i] == 0) {
        keyOut[i] = 1;
        key_waspressed[i] = 1;
        timestamp[i] = millis();
        Serial.print("Key: "); // this is always sent, and not only in debug
        Serial.println(i);
      } else {
        keyOut[i] = 0;
      }
    } else {
      if (key_waspressed[i] == 1) {
        //Debouncing
        if (millis() - timestamp[i] > 200) {
          key_waspressed[i] = 0;
        }
      }

    }
  }
  // Invert directions if needed
  if (invX == true) {
    transX = transX * -1;
  };
  if (invY == true) {
    transY = transY * -1;
  };
  if (invZ == true) {
    transZ = transZ * -1;
  };
  if (invRX == true) {
    rotX = rotX * -1;
  };
  if (invRY == true) {
    rotY = rotY * -1;
  };
  if (invRZ == true) {
    rotZ = rotZ * -1;
  };

  debugOutput4();
  // Report translation and rotation values if enabled. Approx -800 to 800 depending on the parameter.

  debugOutput5();
  // Report debug 4 and 5 info side by side for direct reference if enabled. Very useful if you need to alter which inputs are used in the arithmatic above.

  // Send data to the 3DConnexion software.
  // The correct order for TeachingTech was determined after trial and error
  if (switchYZ == true) {
    //Original from TT, but 3DConnextion tutorial will not work:
    send_command(rotX, rotZ, rotY, transX, transZ, transY, keyOut[0], keyOut[1], keyOut[2], keyOut[3]);
  } else {
    // Daniel_1284580 noticed the 3dconnexion tutorial was not working the right way so they got changed
    send_command(rotX, rotY, rotZ, transX, transY, transZ, keyOut[0], keyOut[1], keyOut[2], keyOut[3]);
  }
}


void debugOutput1() {
  // Report back 0-1023 raw ADC 10-bit values if enabled
  if (debug == 1) {
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
    Serial.print(rawReads[7]);
    Serial.print(",");
    Serial.print("Key1:");
    Serial.print(keyVals[0]);
    Serial.print(",");
    Serial.print("Key2:");
    Serial.print(keyVals[1]);
    Serial.print(",");
    Serial.print("Key3:");
    Serial.print(keyVals[2]);
    Serial.print(",");
    Serial.print("Key4:");
    Serial.println(keyVals[3]);
  }
}

void debugOutput2() {
  // Report centered joystick values if enabled. Values should be approx -500 to +500, jitter around 0 at idle.
  if (debug == 2) {
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
}

void debugOutput3() {
  // Report centered joystick values. Filtered for deadzone. Approx -350 to +350, locked to zero at idle
  if (debug == 3) {
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
}

void debugOutput4() {
  // Report translation and rotation values if enabled. Approx -350 to +350 depending on the parameter.
  if (debug == 4) {
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
}

void debugOutput5() {
  // Report debug 4 and 5 info side by side for direct reference if enabled. Very useful if you need to alter which inputs are used in the arithmetic above.
  if (debug == 5) {
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
}

// Variables and function to get the min and maximum value of the centered values
int minMaxCalcState = 0; // little state machine -> setup in 0 -> measure in 1 -> output in 2 ->  end in 3
int minValue[8]; // Array to store the minimum values
int maxValue[8]; // Array to store the maximum values
unsigned long startTime; // Start time for the measurement

void calcMinMax() {
  // Set debug = 20
  // compile the sketch, upload it and wait for confirmation in the serial console.
  // Move the spacemouse around for 15s to get a min and max value.
  // copy the output from the console into your config.h
  if (debug == 20) {
    if (minMaxCalcState == 0) {
      delay(2000);
      // Initialize the arrays
      for (int i = 0; i < 8; i++) {
        minValue[i] = 1023; // Set the min value to the maximum possible value
        maxValue[i] = 0; // Set the max value to the minimum possible value
      }
      startTime = millis(); // Record the current time
      minMaxCalcState = 1; // next State: measure!
      Serial.println("Please start moving the spacemouse around!");
    }
    else if (minMaxCalcState == 1) {
      if (millis() - startTime < 15000) {
        for (int i = 0; i < 8; i++) {
          // Update the minimum and maximum values
          if (centered[i] < minValue[i]) {
            minValue[i] = centered[i];
          }
          if (centered[i] > maxValue[i]) {
            maxValue[i] = centered[i];
          }
        }
      }
      else {
        // 15s are over. go to next state and report via console
        Serial.println("Stop moving the spacemouse. These are the result:");
        minMaxCalcState = 2;
      }
    }
    else if (minMaxCalcState == 2) {
      Serial.print("int minVals[");
      printArray(minValue, 8);
      Serial.print("int maxVals[");
      printArray(maxValue, 8);
      for (int i = 0; i < 8; i++) {
        if (abs(minValue[i]) < 250) {
          Serial.print("Warning: minValue[");
          Serial.print(i);
          Serial.print("] is small: ");
          Serial.println(minValue[i]);
        }
        if (abs(maxValue[i]) < 250) {
          Serial.print("Warning: maxValue[");
          Serial.print(i);
          Serial.print("] is small: ");
          Serial.println(maxValue[i]);
        }
      }
      minMaxCalcState = 3; // no further reporting
    }
  }
}

void printArray(int arr[], int size) {
  //Serial.print("int m..Values[");
  Serial.print(size);
  Serial.print("] = {");
  for (int i = 0; i < size; i++) {
    Serial.print(arr[i]);
    if (i < size - 1) {
      Serial.print(", ");
    }
  }
  Serial.println("};");
}
