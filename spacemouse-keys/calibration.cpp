// File for calibration specific functions

#include <Arduino.h>
#include "calibration.h"
#include "config.h"

void printArray(int arr[], int size) {
  /*This functions prints an array to the Serial and you can copy the output the c code again.
     Example output: int minVals[8] = {-519, -521, -512, -2, -519, -482, -508, -1};

     Before calling this function, you should need to call the following line and change "mValues" with the variable name, you want to be printed
  */
  Serial.print("{");
  for (int i = 0; i < size; i++) {
    Serial.print(arr[i]);
    if (i < size - 1) {
      Serial.print(", ");
    }
  }
  Serial.println("}");
}

char *axisNames[] = {"AX:", "AY:", "BX:", "BY:", "CX:", "CY:", "DX:", "DY:"}; // 8
char *keyNames[] = {"K0:", "K1:", "K2:", "K3:"}; // 4
char *velNames[] = {"TX:", "TY:", "TZ:", "RX:", "RY:", "RZ:"}; // 6

void debugOutput1(int* rawReads, int* keyVals) {
  // Report back 0-1023 raw ADC 10-bit values if enabled
  for (int i = 0; i < 8; i++) {
    Serial.print(axisNames[i]);
    Serial.print(rawReads[i]);
    Serial.print(", ");
  }
  for (int i = 0; i < 4; i++) {
    Serial.print(keyNames[i]);
    Serial.print(keyVals[i]);
    Serial.print(", ");
  }
  Serial.println("");
}

void debugOutput2(int* centered) {
  // this routine creates the output for the former debug = 2 and debug = 3
  for (int i = 0; i < 8; i++) {
    Serial.print(axisNames[i]);
    Serial.print(centered[i]);
    Serial.print(", ");
  }
  Serial.println("");
}

void debugOutput4(int16_t* velocity, uint8_t* keyOut) {
  // Report translation and rotation values if enabled. Approx -350 to +350 depending on the parameter.
  for (int i = 0; i < 6; i++) {
    Serial.print(velNames[i]);
    Serial.print(velocity[i]);
    Serial.print(", ");
  }
  for (int i = 0; i < 4; i++) {
    Serial.print(keyNames[i]);
    Serial.print(keyOut[i]);
    Serial.print(", ");
  }
  Serial.println("");
}

void debugOutput5(int* centered, int16_t* velocity) {
  // Report debug 4 and 5 info side by side for direct reference if enabled. Very useful if you need to alter which inputs are used in the arithmetic above.
  for (int i = 0; i < 8; i++) {
    Serial.print(axisNames[i]);
    Serial.print(centered[i]);
    Serial.print(", ");
  }
  Serial.print("||");
  for (int i = 0; i < 6; i++) {
    Serial.print(velNames[i]);
    Serial.print(velocity[i]);
    Serial.print(", ");
  }
  Serial.println("");
}

// Variables and function to get the min and maximum value of the centered values
int minMaxCalcState = 0; // little state machine -> setup in 0 -> measure in 1 -> output in 2 ->  end in 3
int minValue[8]; // Array to store the minimum values
int maxValue[8]; // Array to store the maximum values
unsigned long startTime; // Start time for the measurement

void calcMinMax(int* centered) {
  // compile the sketch, upload it and wait for confirmation in the serial console.
  // Move the spacemouse around for 15s to get a min and max value.
  // copy the output from the console into your config.h

  if (minMaxCalcState == 0) {
    delay(2000);
    // Initialize the arrays
    for (int i = 0; i < 8; i++) {
      minValue[i] = 1023; // Set the min value to the maximum possible value
      maxValue[i] = 0; // Set the max value to the minimum possible value
    }
    startTime = millis(); // Record the current time
    minMaxCalcState = 1; // next State: measure!
    Serial.println(F("Please start moving the spacemouse around!"));
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
      Serial.println(F("Stop moving the spacemouse. These are the result:"));
      minMaxCalcState = 2;
    }
  }
  else if (minMaxCalcState == 2) {
    Serial.print(F("MINVALS "));
    printArray(minValue, 8);
    Serial.print(F("MAXVALS "));
    printArray(maxValue, 8);
    for (int i = 0; i < 8; i++) {
      if (abs(minValue[i]) < 250) {
        Serial.print(F("Warning: minValue["));
        Serial.print(i);
        Serial.print("] ");
        Serial.print(axisNames[i]);
        Serial.print(F(" is small: "));
        Serial.println(minValue[i]);
      }
      if (abs(maxValue[i]) < 250) {
        Serial.print(F("Warning: maxValue["));
        Serial.print(i);
        Serial.print("] ");
        Serial.print(axisNames[i]);
        Serial.print(F(" is small: "));
        Serial.println(maxValue[i]);
      }
    }
    minMaxCalcState = 3; // no further reporting
  }
}
