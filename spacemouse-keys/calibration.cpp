// File for calibration specific functions

#include <Arduino.h>
#include "calibration.h"
#include "kinematics.h"
#include "config.h"

// hold characters to plot them
char debugOutputBuffer[20];

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

#ifndef HALLEFFECT
char const *axisNames[] = {"AX:", "AY:", "BX:", "BY:", "CX:", "CY:", "DX:", "DY:"}; // 8
#else
char const *axisNames[] = {"H0:", "H1:", "H2:", "H3:", "H6:", "H7:", "H8:", "H9:"}; // 8
#endif
char const *velNames[] = {"TX:", "TY:", "TZ:", "RX:", "RY:", "RZ:"}; // 6

void debugOutput1(int* rawReads, int* keyVals) {
  if (isDebugOutputDue()) {
    // Report back 0-1023 raw ADC 10-bit values if enabled
    for (int i = 0; i < 8; i++) {
      sprintf(debugOutputBuffer,"%2.2s: %4d ", axisNames[i],rawReads[i] );
      Serial.print(debugOutputBuffer);
    }
    for (int i = 0; i < NUMKEYS; i++) {
      Serial.print("K");
      Serial.print(i);
      Serial.print(":");
      Serial.print(keyVals[i]);
      Serial.print(", ");
    }
    Serial.print(DEBUG_LINE_END); 
  }
}

void debugOutput2(int* centered) {
  if (isDebugOutputDue()) {
    // this routine creates the output for the former debug = 2 and debug = 3
    for (int i = 0; i < 8; i++) {
      sprintf(debugOutputBuffer,"%2.2s: %4d ", axisNames[i],centered[i] );
      Serial.print(debugOutputBuffer);
    }
    Serial.print(DEBUG_LINE_END); 
  }
}

/// @brief Report translation and rotation values if enabled. 
/// @param velocity pointer to velocity array
/// @param keyOut pointer to keyOut array
void debugOutput4(int16_t* velocity, uint8_t* keyOut) {
  if (isDebugOutputDue()) {
    for (int i = 0; i < 6; i++) {
      sprintf(debugOutputBuffer,"%2.2s: %4d ", velNames[i],velocity[i] );
      Serial.print(debugOutputBuffer);
    }
    for (int i = 0; i < NUMKEYS; i++) {
      Serial.print("K");
      Serial.print(i);
      Serial.print(":");
      Serial.print(keyOut[i]);
      Serial.print(", ");
    }
    Serial.print(DEBUG_LINE_END); 
  }
}

/// @brief Report single axis and resulting velocities info side by side for direct reference. Very useful if you need to alter which inputs are used in the arithmetic above.
/// @param centered pointer to arrays of 8 axis
/// @param velocity pointer to array of 6 velocities
void debugOutput5(int* centered, int16_t* velocity) {
  if (isDebugOutputDue()) {
    for (int i = 0; i < 8; i++) {
      sprintf(debugOutputBuffer,"%2.2s: %4d ", axisNames[i],centered[i] );
      Serial.print(debugOutputBuffer);
    }
    Serial.print(" || ");
    for (int i = 0; i < 6; i++) {
      sprintf(debugOutputBuffer,"%2.2s: %4d ", velNames[i],velocity[i] );
      Serial.print(debugOutputBuffer);
    }
    Serial.print(DEBUG_LINE_END); 
  }
}


#ifndef HALLEFFECT
#define MINMAX_MINWARNING 250
#define MINMAX_MAXWARNING 250
#else
// The Hall effect sensors aren't centered around zero, due to the nature of the hardware.
// In my version of the Spacemouse, the values vary between -425 and 285, the centerpoint is thus around -70
// The MIN and MAX warning levels have to be shifted accordingly.
#define MINMAX_MINWARNING (100 - centerPoint)
#define MINMAX_MAXWARNING (100 + centerPoint)
#endif

/// @brief This function records the minimum and maximum movement of the joysticks: After initialization, move the mouse for 15s and see the printed output.
/// @param centered pointer to the array with the centered joystick values
/// @return returns 0 if calculations are done, else 1 while collecting data and 2 while calculating
int calcMinMax(int* centered) { //SNo: report internal state as function-result to inform calling loop()
  // Variables and function to get the min and maximum value of the centered values
  static int minMaxCalcState = 0;  // little state machine -> setup in 0 -> measure in 1 -> output in 2 ->  ends with 0
  static int minValue[8];          // Array to store the minimum values
  static int maxValue[8];          // Array to store the maximum values
  static unsigned long startTime;  // Start time for the measurement

  if (minMaxCalcState == 0) {
    delay(2000);
    // Initialize the arrays
    for (int i = 0; i < 8; i++) {
            minValue[i] = 1023; // Set the min value to the maximum possible value
      maxValue[i] = -1023;  // Set the max value to the minimum possible value
    }
        startTime = millis(); // Record the current time
        minMaxCalcState = 1;  // next State: measure!
    Serial.println(F("Please start moving the SpaceMouse around for 15 sec!"));

  } else if (minMaxCalcState == 1) {
    if (millis() - startTime < 15000) {
      for (int i = 0; i < 8; i++) {
        // Update the minimum and maximum values
        if (centered[i] < minValue[i]) {minValue[i] = centered[i];}
        if (centered[i] > maxValue[i]) {maxValue[i] = centered[i];}
      }
    } else {
      // 15s are over. go to next state and report via console
      Serial.println(F("\r\n\nStop moving the SpaceMouse. These are the results, copy them in config.h"));
      minMaxCalcState = 2;
    }

  } else if (minMaxCalcState == 2) {
    Serial.print(F("#define MINVALS "));
    printArray(minValue, 8);
    Serial.print(F("#define MAXVALS "));
    printArray(maxValue, 8);
    #ifdef HALLEFFECT
        // Calculate and print the ranges for each HALL sensor
        int minmaxRanges[8];
        int max = 0;
        int min = 0;
        for (uint8_t i = 0; i < 8; i++) {
            minmaxRanges[i] = abs(minValue[i]) + abs(maxValue[i]);
            max = (abs(maxValue[i]) > max) ? abs(maxValue[i]) : max;
            min = (abs(minValue[i]) > min) ? abs(minValue[i]) : min;
        }
        Serial.print(F("Ranges are: "));
        printArray(minmaxRanges, 8);
        int centerPoint = (max + (min * -1)) / 2;
        Serial.print(F("Centerpoint: "));
        Serial.println(centerPoint);
    #endif
    for(int i = 0; i < 8; i++){
      if(abs(minValue[i]) < MINMAX_MINWARNING){
        Serial.print(F("Warning: minValue["));
        Serial.print(i);
        Serial.print("] ");
        Serial.print(axisNames[i]);
        Serial.print(F(" is small: "));
        Serial.println(minValue[i]);
      }
      if(abs(maxValue[i]) < MINMAX_MAXWARNING){
        Serial.print(F("Warning: maxValue["));
        Serial.print(i);
        Serial.print("] ");
        Serial.print(axisNames[i]);
        Serial.print(F(" is small: "));
        Serial.println(maxValue[i]);
      }
    }
    minMaxCalcState = 0;  //SNo: signal end of run and prepare state-machine for next use

  }else{
    minMaxCalcState = 0;  //SNo: on undefined state: set back to 0 (end of run)
  }

  return minMaxCalcState;
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

/// @brief update and report the function to learn at what frequency the loop is running
void updateFrequencyReport() {
  static uint16_t      iterationsPerSecond = 0;  // count the iterations within one second
  static unsigned long lastFrequencyUpdate = 0;  // time from millis(), when the last frequency was calculated
  // increase iterations counter
  iterationsPerSecond++;
  if (millis() - lastFrequencyUpdate > 1000) {  // if one second has past: report frequency
    Serial.print(F("Frequency: "));
    Serial.print(iterationsPerSecond);
    Serial.println(F(" Hz"));
    lastFrequencyUpdate = millis(); // reset timer
    iterationsPerSecond = 0;        // reset iteration counter
  }
}

/// @brief Calibrate (=zero) the space mouse. The function is blocking other functions of the spacemouse during zeroing.
/// @param centerPoints 
/// @param numIterations How many readings are taken to calculate the mean. Suggestion: 500 iterations, they take approx. 480ms.
/// @param debugFlag With debugFlag = true, a suggestion for the dead zone is given on the serial interface to save to the config.h
/// @return returns true, if no warnings occured. Warnings are given if the zero positions are very unlikely
bool busyZeroing(int *centerPoints, uint16_t numIterations, boolean debugFlag){
  bool noWarningsOccured = true;

  if (debugFlag == true){
    #ifndef HALLEFFECT
    Serial.println(F("Zeroing Joysticks..."));
    #else
        Serial.println(F("Zeroing HALL Sensors..."));
    #endif
  }

  int act[8];                                  // actual value
  uint32_t mean[8] = {0, 0, 0, 0, 0, 0, 0, 0}; // Array to count all values during the averaging
  int16_t  minValue[8];                         // Array to store the minimum values
  int16_t  maxValue[8];                         // Array to store the maximum values
  for (int i = 0; i < 8; i++){
    minValue[i] = 1023; // Set the min value to the maximum possible value
    maxValue[i] = 0;    // Set the max value to the minimum possible value
  }

  // measure duration
  unsigned int long start, end;
  start = millis();

  uint16_t count;

  for (count = 0; count < numIterations; count++){
    readAllFromJoystick(act);
    for (uint8_t i = 0; i < 8; i++){
      // Add to mean
      mean[i] = mean[i] + act[i];
      // Update the minimum and maximum values for dead zone evaluation
      if (act[i] < minValue[i]){minValue[i] = act[i];}
      if (act[i] > maxValue[i]){maxValue[i] = act[i];}
    }
  }

  int16_t deadZone[8];
  int16_t maxDeadZone = 0;
    // a dead zone above the following value will be warned
    #define DEADZONEWARNING 10
  #ifndef HALLEFFECT
    // a centerpoint below or above those values will be warned (384..640)
    #define CENTERPOINTWARNINGMIN (512 - 128)
    #define CENTERPOINTWARNINGMAX (512 + 128)
  #else
        // The centerpoint of the Hall effect mouse is not in the center of the ADC range, due to the hardware nature.
        // According to the height of the base plate, the centerpoint is shifted up or downwards.
    // a centerpoint below or above those values will be warned (592..848)
    #define CENTERPOINTWARNINGMIN (720 - 128)
    #define CENTERPOINTWARNINGMAX (720 + 128)
  #endif

  // calculating average by dividing the mean by the number of iterations
  for (uint8_t i = 0; i < 8; i++){
    centerPoints[i] = mean[i] / count;
    deadZone[i]     = maxValue[i] - minValue[i];
    // get maximum deadzone independet of axis
    if (deadZone[i] > maxDeadZone){maxDeadZone = deadZone[i];}
    
    if (deadZone[i] > DEADZONEWARNING || centerPoints[i] < CENTERPOINTWARNINGMIN || centerPoints[i] > CENTERPOINTWARNINGMAX){
        noWarningsOccured = false;
      }
  }

  // report everything, if with debugFlag
  if (debugFlag){
    Serial.println(F("##  Min - Mean- Max -> Dead Zone"));
    for (int i = 0; i < 8; i++){
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
      if (deadZone[i] > DEADZONEWARNING){
        Serial.print(F(" Attention! Moved axis?"));
      }
      if (centerPoints[i] < CENTERPOINTWARNINGMIN || centerPoints[i] > CENTERPOINTWARNINGMAX){
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

/// @brief  Compensate drifts of the joysticks / hall-sensors (SNo)
/// @param  raw    raw[]-array of joystick-values (input)
/// @param  center centerPoints[]-array to determine drift (input)
/// @param  par    storage of parameters
/// @return offset offset[]-array to compensate raw-values (output)
void compensateDrifts(int *raw, int *center, int *offset, ParamStorage& par) {
  // function is non-blocking, so it has to store some values static to survive:
  static int32_t        cmpMean[8];
  static int            cmpMin[8];
  static int            cmpMax[8];
  static int            cmpNo = 0;
  static bool           cmpRestart = true;
  static unsigned long  cmpStart, cmpEnd;

  bool drifting = true;                     // assume that we have drift

  if(cmpRestart){                           // on restart calculation:
    for(int i=0; i<8; i++){
      cmpMin[i]  = 1023;                    //   init min/max fields
      cmpMax[i]  =    0;
      cmpMean[i] =    0;
    }
    cmpNo      = 0;
    cmpStart   = millis();
    cmpRestart = false;
  }

  for(int i=0; i<8; i++){                   // collect data:
    int r = raw[i];                         //   get raw-value
    if(r < cmpMin[i]){cmpMin[i] = r;}       //   latch if minimum
    if(r > cmpMax[i]){cmpMax[i] = r;}       //   latch if maximum
  }

  for(int i=0; i<8; i++){                   // test new data:
    if(abs(raw[i] - center[i]) > par.compCenterDiff){drifting = false;} // too far away from original center -> not drifting
    if((cmpMax[i] - cmpMin[i]) > par.compMinMaxDiff){drifting = false;} // too much bandwidth -> not drifting 
  }

  if(!drifting){                            // if not only drift:
    cmpRestart = true;                      //   clear data and restart
    //Serial.println(">ret_no_drift<");
    return;                                 //   and end here
  }

  cmpEnd = millis();                        // calculate wait-time
  if(cmpEnd - cmpStart < unsigned(par.compWaitTime)){// end here, if min.duration not yet reached
    return;
  }

  // if wait-time is over and joysticks are not moved:
  for(int i=0; i<8; i++){                   // collect data:
    int r = raw[i];                         //   get raw-value
    cmpMean[i] += r;                        //   store to mean-array
  }
  cmpNo++;                                  //   increment number of points

  if(cmpNo < par.compNoOfPoints){          // if data not completely filled:
    return;                                 //   and end here
  }

  // mean-values are complete:
  for(int i=0; i<8; i++){                   // calculate offsets
    offset[i] = center[i] - trunc(cmpMean[i] / par.compNoOfPoints);
  }

  cmpRestart = true;                        // restart from beginning
  return;
}
