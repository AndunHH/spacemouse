// File for calibration specific functions

#include <Arduino.h>
#include "calibration.h"
#include "kinematics.h"
#include "config.h"

/// @brief Prints an array to the Serial, in order to copy the output again to C-Code. Example output: {-519, -521, -512, -2, -519, -482, -508, -1}
/// @param arr array to print
/// @param size size of the array
void printArray(int arr[], int size) {
  Serial.print("{");
  for (int i = 0; i < size; i++) {
    Serial.print(arr[i]);
    if (i < size - 1) {
      Serial.print(", ");
    }
  }
  Serial.println("}");
}

char const * axisNames[] = { "AX:", "AY:", "BX:", "BY:", "CX:", "CY:", "DX:", "DY:" };  // 8
char const * velNames[] = { "TX:", "TY:", "TZ:", "RX:", "RY:", "RZ:" };                 // 6

void debugOutput1(int* rawReads, int* keyVals) {
  if (isDebugOutputDue()) {
    // Report back 0-1023 raw ADC 10-bit values if enabled
    for (int i = 0; i < 8; i++) {
      Serial.print(axisNames[i]);
      Serial.print(rawReads[i]);
      Serial.print(", ");
    }
    for (int i = 0; i < NUMKEYS; i++) {
      Serial.print("K");
      Serial.print(i);
      Serial.print(":");
      Serial.print(keyVals[i]);
      Serial.print(", ");
    }
    Serial.println("");
  }
}

void debugOutput2(int* centered) {
  if (isDebugOutputDue()) {
    // this routine creates the output for the former debug = 2 and debug = 3
    for (int i = 0; i < 8; i++) {
      Serial.print(axisNames[i]);
      Serial.print(centered[i]);
      Serial.print(", ");
    }
    Serial.println("");
  }
}

/// @brief Report translation and rotation values if enabled. 
/// @param velocity pointer to velocity array
/// @param keyOut pointer to keyOut array
void debugOutput4(int16_t* velocity, uint8_t* keyOut) {
  // 
  if (isDebugOutputDue()) {
    for (int i = 0; i < 6; i++) {
      Serial.print(velNames[i]);
      Serial.print(velocity[i]);
      Serial.print(", ");
    }
    for (int i = 0; i < NUMKEYS; i++) {
      Serial.print("K");
      Serial.print(i);
      Serial.print(":");
      Serial.print(keyOut[i]);
      Serial.print(", ");
    }
    Serial.println("");
  }
}

void debugOutput5(int* centered, int16_t* velocity) {
  // Report debug 4 and 5 info side by side for direct reference if enabled. Very useful if you need to alter which inputs are used in the arithmetic above.
  if (isDebugOutputDue()) {
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
}

// Variables and function to get the min and maximum value of the centered values
int minMaxCalcState = 0;  // little state machine -> setup in 0 -> measure in 1 -> output in 2 ->  end in 3
int minValue[8];          // Array to store the minimum values
int maxValue[8];          // Array to store the maximum values
unsigned long startTime;  // Start time for the measurement

/// @brief This function records the minimum and maximum movement of the joysticks: After initialization, move the mouse for 15s and see the printed output. Replug/reset the mouse, to enable the semi-automatic calibration for a second time.
/// @param centered pointer to the array with the centered joystick values
void calcMinMax(int* centered) {
  if (minMaxCalcState == 0) {
    delay(2000);
    // Initialize the arrays
    for (int i = 0; i < 8; i++) {
      minValue[i] = 1023;  // Set the min value to the maximum possible value
      maxValue[i] = 0;     // Set the max value to the minimum possible value
    }
    startTime = millis();  // Record the current time
    minMaxCalcState = 1;   // next State: measure!
    Serial.println(F("Please start moving the spacemouse around for 15 sec!"));
  } else if (minMaxCalcState == 1) {
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
    } else {
      // 15s are over. go to next state and report via console
      Serial.println(F("\n\nStop moving the spacemouse. These are the result. Copy them in config.h"));
      minMaxCalcState = 2;
    }
  } else if (minMaxCalcState == 2) {
    Serial.print(F("#define MINVALS "));
    printArray(minValue, 8);
    Serial.print(F("#define MAXVALS "));
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
    minMaxCalcState = 3;  // no further reporting
  }
}


/// @brief Check, if a new debug output shall be generated. This is used in order to generate a debug line only every DEBUGDELAY ms, see config.h
/// @return true, if debug message is due
bool isDebugOutputDue() {
  static unsigned long lastDebugOutput = 0;  // time from millis(), when the last debug output was given

  if (millis() - lastDebugOutput > DEBUGDELAY) {
    lastDebugOutput = millis();
    return true;
  } else {
    return false;
  }
}

uint16_t iterationsPerSecond = 0;       // count the iterations within one second
unsigned long lastFrequencyUpdate = 0;  // time from millis(), when the last frequency was calculated

/// @brief update and report the function to learn at what frequency the loop is running
void updateFrequencyReport() {
  // increase iterations counter
  iterationsPerSecond++;
  if (millis() - lastFrequencyUpdate > 1000) {  // if one second has past: report frequency
    Serial.print("Frequency: ");
    Serial.print(iterationsPerSecond);
    Serial.println(" Hz");
    lastFrequencyUpdate = millis(); // reset timer
    iterationsPerSecond = 0;        // reset iteration counter
  }
}

/// @brief Calibrate (=zero) the space mouse. The function is blocking other functions of the spacemouse during zeroing.
/// @param centerPoints 
/// @param numIterations How many readings are taken to calculate the mean. Suggestion: 500 iterations, they take approx. 480ms.
/// @param debugFlag With debugFlag = true, a suggestion for the dead zone is given on the serial interface to save to the config.h
/// @return returns true, if no warnings occured. Warnings are given if the zero positions are very unlikely
bool busyZeroing(int *centerPoints, uint16_t numIterations, boolean debugFlag)
{
  bool noWarningsOccured = true;
  if (debugFlag == true)
  {
    Serial.println(F("Zeroing Joysticks..."));
  }
  int act[8];                                  // actual value
  uint32_t mean[8] = {0, 0, 0, 0, 0, 0, 0, 0}; // Array to count all values during the averaging
  int minValue[8];                             // Array to store the minimum values
  int maxValue[8];                             // Array to store the maximum values
  for (int i = 0; i < 8; i++)
  {
    minValue[i] = 1023; // Set the min value to the maximum possible value
    maxValue[i] = 0;    // Set the max value to the minimum possible value
  }

  // measure duration
  unsigned int long start, end;
  start = millis();

  uint16_t count;

  for (count = 0; count < numIterations; count++)
  {
    readAllFromJoystick(act);
    for (uint8_t i = 0; i < 8; i++)
    {
      // Add to mean
      mean[i] = mean[i] + act[i];
      // Update the minimum and maximum values for dead zone evaluation
      if (act[i] < minValue[i])
      {
        minValue[i] = act[i];
      }
      if (act[i] > maxValue[i])
      {
        maxValue[i] = act[i];
      }
    }
  }

  int16_t deadZone[8];
  int16_t maxDeadZone = 0;
  // calculating average by dividing the mean by the number of iterations
  for (uint8_t i = 0; i < 8; i++)
  {
    centerPoints[i] = mean[i] / count;
    deadZone[i] = maxValue[i] - minValue[i];
    if (deadZone[i] > maxDeadZone)
    {
      // get maximum deadzone independet of axis
      maxDeadZone = deadZone[i];
    }
    
    // a dead zone above the following value will be warned
    #define DEADZONEWARNING 10
    // a centerpoint below or above those values will be warned (512 +/- 128)
    #define CENTERPOINTWARNINGMIN 384
    #define CENTERPOINTWARNINGMAX 640

    if (deadZone[i] > DEADZONEWARNING || centerPoints[i] < CENTERPOINTWARNINGMIN || centerPoints[i] > CENTERPOINTWARNINGMAX)
      {
        noWarningsOccured = false;
      }
  }

  // report everything, if with debugFlag
  if (debugFlag)
  {
    Serial.println(F("##  Min- Mean - Max -> Dead Zone"));
    for (int i = 0; i < 8; i++)
    {
      Serial.print(axisNames[i]);
      Serial.print(" ");
      Serial.print(minValue[i]);
      Serial.print(" - ");
      Serial.print(centerPoints[i]);
      Serial.print(" - ");
      Serial.print(maxValue[i]);
      Serial.print(" -> ");
      Serial.print(deadZone[i]);
      Serial.print(" ");
      if (deadZone[i] > DEADZONEWARNING)
      {
        Serial.print(F(" Attention! Moved axis?"));
      }
      if (centerPoints[i] < CENTERPOINTWARNINGMIN || centerPoints[i] > CENTERPOINTWARNINGMAX)
      {
        Serial.print(F(" Attention! Axis in idle?"));
      }
      Serial.println("");
    }
    end = millis();
    Serial.println(F("Using mean as zero position..."));
    Serial.print(F("Suggestion for config.h: "));
    Serial.print(F("#define DEADZONE "));
    Serial.println(maxDeadZone);
    Serial.print(F("This took "));
    Serial.print(end - start);
    Serial.print(F(" ms for "));
    Serial.print(count);
    Serial.println(F(" iterations."));
  }
  return noWarningsOccured;
}
