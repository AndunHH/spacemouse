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
