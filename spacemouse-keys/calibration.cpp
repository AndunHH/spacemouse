// File for calibration specific functions

#include <Arduino.h>
#include "calibration.h"

void printArray(int arr[], int size) {
  /*This functions prints an array to the Serial and you can copy the output the c code again.
     Example output: int minVals[8] = {-519, -521, -512, -2, -519, -482, -508, -1};

     Before calling this function, you should need to call the following line and change "mValues" with the variable name, you want to be printed
  */
  //Serial.print("int mValues["); //print in calling context
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


void debugOutput1(int* rawReads, int* keyVals) {
  // Report back 0-1023 raw ADC 10-bit values if enabled
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

void debugOutput2(int* centered) {
  // this routine creates the output for the former debug = 2 and debug = 3
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

void debugOutput4(int16_t* velocity, int8_t* keyOut) {
  // Report translation and rotation values if enabled. Approx -350 to +350 depending on the parameter.
  Serial.print("TX:");
  Serial.print(velocity[TRANSX]);
  Serial.print(",");
  Serial.print("TY:");
  Serial.print(velocity[TRANSY]);
  Serial.print(",");
  Serial.print("TZ:");
  Serial.print(velocity[TRANSZ]);
  Serial.print(",");
  Serial.print("RX:");
  Serial.print(velocity[ROTX]);
  Serial.print(",");
  Serial.print("RY:");
  Serial.print(velocity[ROTY]);
  Serial.print(",");
  Serial.print("RZ:");
  Serial.print(velocity[ROTZ]);
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

void debugOutput5(int* centered, int16_t* velocity) {
  // Report debug 4 and 5 info side by side for direct reference if enabled. Very useful if you need to alter which inputs are used in the arithmetic above.
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
  Serial.print(velocity[TRANSX]);
  Serial.print(",");
  Serial.print("TY:");
  Serial.print(velocity[TRANSY]);
  Serial.print(",");
  Serial.print("TZ:");
  Serial.print(velocity[TRANSZ]);
  Serial.print(",");
  Serial.print("RX:");
  Serial.print(velocity[ROTX]);
  Serial.print(",");
  Serial.print("RY:");
  Serial.print(velocity[ROTY]);
  Serial.print(",");
  Serial.print("RZ:");
  Serial.println(velocity[ROTZ]);
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
