// This is the source code for the open source space mouse with keys.
// Please read the introduction and history with all contributors here:
// https://github.com/AndunHH/spacemouse

// One good starting point is the work and video by TeachingTech: https://www.printables.com/de/model/864950-open-source-spacemouse-space-mushroom-remix
// Then follow along on github, how we reached this state of the source code.

#include <Arduino.h>

// The user specific settings, like pin mappings or special configuration variables and sensitivities are stored in config.h.
// Please open config_sample.h, adjust your settings and save it as config.h
#include "config.h"
#include "parameterMenu.h"

// Include inbuilt Arduino HID library by NicoHood: https://github.com/NicoHood/HID
#include "HID.h"

// header file for calibration output and helper routines
#include "calibration.h"

// header to calculate the kinematics of the mouse
#include "kinematics.h"

// header file for reading the keys
#include "spaceKeys.h"

// header for HID emulation of the spacemouse
#include "SpaceMouseHID.h"

#if ROTARY_AXIS > 0 or ROTARY_KEYS > 0
// if an encoder wheel is used
#include "encoderWheel.h"
#endif

#ifdef LEDRING
#include "ledring.h"
#endif

void setup();
void loop();
#ifdef LEDpin
void lightSimpleLED(boolean light);
#endif
#ifdef HALLEFFECT
void setAnalogReferenceVoltage(int dbg);
#endif

// stores the raw analog values from the joysticks
int rawReads[8];

// stores the values from the joysticks after zeroing and mapping
int centered[8];

// Centerpoints store the zero position of the joysticks
int centerPoints[8];

// Offsets store the drift-compensation values of the joysticks
int offsets[8];

// Resulting calculated velocities / movements
// int16_t to match what the HID protocol expects.
int16_t velocity[6];

// global parameters (also stored in EEPROM)
ParamStorage parStorage;

ParamData par = { .values      = &parStorage,
                  .description = {
                    {PARAM_TYPE_BOOL,  "",                       NULL                              }, // param 0 is unused
                    {PARAM_TYPE_INT,   "DEADZONE",               &parStorage.deadzone              }, //       1
                    {PARAM_TYPE_FLOAT, "SENS_TX",                &parStorage.transX_sensitivity    }, //       2
                    {PARAM_TYPE_FLOAT, "SENS_TY",                &parStorage.transY_sensitivity    }, //       3
                    {PARAM_TYPE_FLOAT, "SENS_PTZ",               &parStorage.pos_transZ_sensitivity}, //       4
                    {PARAM_TYPE_FLOAT, "SENS_NTZ",               &parStorage.neg_transZ_sensitivity}, //       5
                    {PARAM_TYPE_FLOAT, "GATE_NTZ",               &parStorage.gate_neg_transZ       }, //       6
                    {PARAM_TYPE_INT,   "GATE_RX",                &parStorage.gate_rotX             }, //       7
                    {PARAM_TYPE_INT,   "GATE_RY",                &parStorage.gate_rotY             }, //       8
                    {PARAM_TYPE_INT,   "GATE_RZ",                &parStorage.gate_rotZ             }, //       9
                    {PARAM_TYPE_FLOAT, "SENS_RX",                &parStorage.rotX_sensitivity      }, //      10
                    {PARAM_TYPE_FLOAT, "SENS_RY",                &parStorage.rotY_sensitivity      }, //      11
                    {PARAM_TYPE_FLOAT, "SENS_RZ",                &parStorage.rotZ_sensitivity      }, //      12
                    {PARAM_TYPE_INT,   "MODFUNC",                &parStorage.modFunc               }, //      13
                    {PARAM_TYPE_FLOAT, "MOD_A",                  &parStorage.slope_at_zero         }, //      14
                    {PARAM_TYPE_FLOAT, "MOD_B",                  &parStorage.slope_at_end          }, //      15
                    {PARAM_TYPE_BOOL,  "INVX",                   &parStorage.invX                  }, //      16
                    {PARAM_TYPE_BOOL,  "INVY",                   &parStorage.invY                  }, //      17
                    {PARAM_TYPE_BOOL,  "INVZ",                   &parStorage.invZ                  }, //      18
                    {PARAM_TYPE_BOOL,  "INVRX",                  &parStorage.invRX                 }, //      19
                    {PARAM_TYPE_BOOL,  "INVRY",                  &parStorage.invRY                 }, //      20
                    {PARAM_TYPE_BOOL,  "INVRZ",                  &parStorage.invRZ                 }, //      21
                    {PARAM_TYPE_BOOL,  "SWITCHXY",               &parStorage.switchXY              }, //      22
                    {PARAM_TYPE_BOOL,  "SWITCHYZ",               &parStorage.switchYZ              }, //      23
                    {PARAM_TYPE_BOOL,  "EXCLUSIVE",              &parStorage.exclusiveMode         }, //      24
                    {PARAM_TYPE_INT,   "EXCL_HYST",              &parStorage.exclusiveHysteresis   }, //      25
                    {PARAM_TYPE_BOOL,  "EXCL_PRIOZ",             &parStorage.prioZexclusiveMode    }, //      26
                    {PARAM_TYPE_BOOL,  "COMP_EN",                &parStorage.compEnabled           }, //      27
                    {PARAM_TYPE_INT,   "COMP_NR",                &parStorage.compNoOfPoints        }, //      28
                    {PARAM_TYPE_INT,   "COMP_WAIT",              &parStorage.compWaitTime          }, //      29
                    {PARAM_TYPE_INT,   "COMP_MDIFF",             &parStorage.compMinMaxDiff        }, //      30
                    {PARAM_TYPE_INT,   "COMP_CDIFF",             &parStorage.compCenterDiff        }, //      31
                    {PARAM_TYPE_INT,   "RAXIS_ECH",              &parStorage.rotAxisEchos          }, //      32
                    {PARAM_TYPE_INT,   "RAXIS_STR",              &parStorage.rotAxisSimStrength    }  //      33
                  }
                };

//store raw value of the keys, without debouncing
int keyVals[NUMKEYS];

// key event, after debouncing. It is 1 only for a single sample
uint8_t keyOut[NUMKEYS];

// state of the key, which stays 1 as long as the key is pressed
uint8_t keyState[NUMKEYS];

void setup() {
  // Get parameters from EEPROM
  #if PARAM_IN_EEPROM > 0
  getParametersFromEEPROM(par);
  #endif

  // setup the keys e.g. to internal pull-ups
  #if NUMKEYS > 0
  setupKeys();
  #endif

  #ifdef HALLEFFECT
  // Set the ADC reference voltage to 2,56V if HALLEFFECT is defined, 5V otherwise.
  // It is important the reference Voltage is set before the Zeroing of the sensors is executed.
  setAnalogReferenceVoltage(0);
  #endif

  // Begin Serial for debugging
  Serial.begin(115200);
  Serial.setTimeout(30000);  // the serial interface will wait for new menu number for max.30s
  while(!Serial){delay(500);}

  // Read idle/centre positions for joysticks.
  // zero the joystick position 500 times (takes approx. 480 ms)
  // during setup() we are not interested in the debug output: debugFlag = false
  busyZeroing(centerPoints, 750, false);
  for(int i=0; i<8; i++){offsets[i] = 0;}

  #if ROTARY_AXIS > 0 or ROTARY_KEYS > 0
  initEncoderWheel();
  #endif

  #ifdef LEDpin
  #ifdef LEDRING
  initLEDring();
  #else
  // configure LED output for simple LED
  pinMode(LEDpin, OUTPUT);
  #endif
  #endif
}

void loop() {
  // the debug mode can be set during runtime via the serial interface. See config.h for a description of the different debug modes.
  static int  debug     = STARTDEBUG;
  static bool showMenu  = false;

  //--- check if the user entered a debug mode via serial interface
  if((debug != 20) && (debug != 30)){       //SNo: don't change debug-mode/menu when calcMinMax() or parameterMenu() are running
    double num;

    int state = userInput(num);
    #if ENABLE_PROGMODE > 0
    if(state == 10){executeProgCommand(par); state = 0;}
    #endif
    if(state == 1){
      debug = (int)num;
      Serial.println(debug);
      #ifdef HALLEFFECT
      // Debug is updated: check if the ADC referencevoltage has to be changed.
      setAnalogReferenceVoltage(debug);
      #endif
    }
    if(state == 2 || state == 3 || state == 4){debug = 99;}
    if((state != 0) && (debug == 0 || debug == 99)){showMenu = true;}

    if(showMenu){
      Serial.print(F("\r\n\r\nSpaceMouse FW"));Serial.print(F(FW_RELEASE));Serial.println(F(" - Debug Modes"));
      Serial.println(F("ESC stop running mode, leave menu (ESC, Q)"));
      Serial.println(F("  1 raw sensors ADC values full range, max. 0..1023"));
      #ifdef HALLEFFECT
      Serial.println(F(" 10 raw sensors ADC values used range, max. 0..1023"));
      #endif
      Serial.println(F("  2 centered values -500..+500"));
      Serial.println(F(" 11 auto calibrate centers, show deadzones"));
      Serial.println(F(" 20 find min/max-values over 20s (move stick)"));
      Serial.println(F("  3 centered values w.deadzones -350..+350"));
      Serial.println(F(" 31 drift compensation offsets"));
      Serial.println(F("  4 velocity- (trans-/rot-)values -350..+350"));
      Serial.println(F("  5 centered- & velocity-values, (3) and (4)"));
      Serial.println(F("  6 velocity after kill-keys and keys"));
      Serial.println(F(" 61 velocity after axis-switch, exclusive"));
      Serial.println(F("  7 loop-frequency-test"));
      Serial.println(F("  8 key-test, button-codes to send"));
      Serial.println(F("  9 encoder wheel-test"));
      #if PARAM_IN_EEPROM > 0
      Serial.println(F(" 30 parameters (load, save, edit, view)"));
      #endif
      Serial.print(F("mode::"));
      showMenu = false;
    }
  }

  //--- run parameter-menu
  if(debug == 30){
    #if PARAM_IN_EEPROM > 0
      if(parameterMenu(par) == 0){
        showMenu = true;
        debug = 0;
      }
    #else
      showMenu = true;
      debug = 0;
    #endif
  }

  //--- Read joystick values. 0-1023
  readAllFromJoystick(rawReads);

  //--- Reading of key presses
  #if NUMKEYS > 0
  readAllFromKeys(keyVals);
  #endif

  // Report back 0-1023 raw ADC 10-bit values if enabled
  #ifdef HALLEFFECT
  if ((debug == 1) || (debug == 10)) {
    debugOutput1(rawReads, keyVals);
  }
  #else
  if (debug == 1) {
    debugOutput1(rawReads, keyVals);
  }
  #endif

  //--- calibrate the joystick
  if (debug == 11) {
    // As this is called in the debug=11, we do more iterations.
    busyZeroing(centerPoints, 2000, true);
    debug = -1; // after function is done, leave this debug mode to "off" (-1)
  }

  //--- Calculate drift compensation offsets
  if((par.values->compEnabled == 1) && (debug != 20)){  // only when not in debug 20 = find min/max vlaues
    compensateDrifts(rawReads, centerPoints, offsets, par);
  }else{
    for(int i = 0; i < 8; i++){offsets[i] = 0;}
  }

  // Report compensation-offset values
  if (debug == 31) {
    debugOutput2(offsets);
  }

  //--- Subtract centre position and drift-offsets from measured position to determine movement.
  for (int i = 0; i < 8; i++) {
    centered[i] = rawReads[i] - centerPoints[i] + offsets[i];
  }

  //--- calibrate MinMax values
  if (debug == 20) {
    // has to be (re-)called, as long as it doesn't signal "done"
    if(calcMinMax(centered) == 0){  // when calcMinMax() signals 0="done/idle":
      debug = -1;                   // leave this debug-mode 20 to "off" (-1)
    }
  }

  // Report centered joystick values if enabled. Values should be approx -350 to +350, jitter around 0 at idle
  if (debug == 2) {
    debugOutput2(centered);
  }

  //--- Set movement values to zero if movement is below deadzone threshold, scale to +/-350
  FilterAnalogReadOuts(centered, par);

  // Report centered joystick values. Filtered for deadzone. Approx -350 to +350, locked to zero at idle
  if (debug == 3) {
    debugOutput2(centered);
  }

  //--- Calculate the kinematic (centered->velocity)
  calculateKinematic(centered, velocity, par);

  //--- if an encoder wheel is used, calculate the velocity of the wheel
  //    and replace one of the former calculated velocities
  #if (ROTARY_AXIS > 0) && (ROTARY_AXIS < 7)
  calcEncoderWheel(velocity, (debug == 9), par);
  #endif

  //--- if defined, evaluate keys
  #if NUMKEYS > 0
  evalKeys(keyVals, keyOut, keyState);
  #endif

  // The encoder wheel shall be treated as a key
  #if ROTARY_KEYS > 0 
  // The encoder wheel shall be treated as a key
  calcEncoderAsKey(keyState, (debug == 9));
  #endif

  // report translation and rotation values if enabled
  if (debug == 4) {
    debugOutput4(velocity, keyOut);
  }

  if (debug == 5) {
    debugOutput5(centered, velocity);
  }

  // if the kill-key feature is enabled, rotations or translations are killed=set to zero
  #if (NUMKILLKEYS == 2)
  if (keyVals[KILLROT] == LOW) {  
    // check for the raw keyVal and not keyOut, because keyOut is only 1 for a single iteration. keyVals has inverse Logic due to pull-ups
    // kill rotation
    velocity[ROTX] = 0;
    velocity[ROTY] = 0;
    velocity[ROTZ] = 0;
  }
  if (keyVals[KILLTRANS] == LOW) {
    // kill translation
    velocity[TRANSX] = 0;
    velocity[TRANSY] = 0;
    velocity[TRANSZ] = 0;
  }
  #endif

  // report velocity and keys after possible kill-key feature
  if (debug == 6) {
    debugOutput4(velocity, keyOut);
  }

  //--- exchange axis if desired
  if(par.values->switchYZ == 1) {switchYZ(velocity);}
  if(par.values->switchXY == 1) {switchXY(velocity);}

  // exclusive mode: rotation OR translation, but never both at the same time to avoid issues with classics joysticks
  if(par.values->exclusiveMode == 1){
    exclusiveMode(velocity, par.values->exclusiveHysteresis);
  }

  // report velocity and keys after Switch or ExclusiveMode
  if(debug == 61){
    debugOutput4(velocity, keyOut);
  }

  SpaceMouseHID.send_command(velocity[ROTX], velocity[ROTY], velocity[ROTZ], velocity[TRANSX], velocity[TRANSY], velocity[TRANSZ], keyState, debug);

  // update and report at what frequency the loop is running
  if(debug == 7){
    updateFrequencyReport();
  }

  // Check for the LED state by calling updateLEDState.
  // This empties the USB input buffer and checks for the corresponding report.
  SpaceMouseHID.updateLEDState();

  #ifdef LEDpin
  #ifdef LEDRING
  processLED(velocity, SpaceMouseHID.getLEDState());
  #else
  lightSimpleLED(SpaceMouseHID.getLEDState());
  #endif
  #endif

} //end loop()

#ifdef LEDpin
/// @brief Turn on or off a simple led. The pin is defined by LEDpin in config.h. If the LED needs to be inverted, define LEDinvert in config.h
/// @param light turn on or off
void lightSimpleLED(boolean light){
  // Check for the LED state by calling updateLEDState.
  // This empties the USB input buffer and checks for the corresponding report.
  #ifdef LEDinvert
  // Swap the LED logic, if necessary
  if (light){
  #else
  if (!light){
  #endif
    digitalWrite(LEDpin, LOW);  // LED on -> pull kathode down
  }else{
    digitalWrite(LEDpin, HIGH); // LED off -> pull kathode up
  }
}
#endif

#ifdef HALLEFFECT
/**
 * @brief Set the analog reference to 5V for debug 1 and to 2.56V otherwise
 */
void setAnalogReferenceVoltage(int dbg){
  if (dbg == 1){  // Set the reference voltage for the AD Convertor to 5V only for the first calibration step (pinout/inversion calibration).
    analogReference(DEFAULT);
    #ifdef DEBUG_ADC
      Serial.println(F("Setting analog reference to 5V."));
    #endif
  }else{          // Set the reference voltage for the AD Convertor to 2.56V in order to get larger sensitivity.
    analogReference(INTERNAL);
    #ifdef DEBUG_ADC
      Serial.println(F("Setting analog reference to 2.56V."));
    #endif
  }

  // The first measurements after changing the reference voltage can be wrong. So take 100ms to let the voltage stabilize and
  // take some measurements afterwards just to be sure. Performancewise this isn't a problem due to the debug/setup
  // nature of this function.
  delay(100);
  int tempReads[8];
  for (int i = 0; i <= 8; i++){
    readAllFromJoystick(tempReads);
  }
}
#endif
