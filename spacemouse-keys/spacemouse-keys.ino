// This is the source code for the open source space mouse with keys.
// Please read the introduction and history with all contributors here:
// https://github.com/AndunHH/spacemouse

// One good starting point is the work and video by TeachingTech: https://www.printables.com/de/model/864950-open-source-spacemouse-space-mushroom-remix
// Then follow along on github, how we reached this state of the source code.

// The user specific settings, like pin mappings or special configuration variables and sensitivities are stored in config.h.
// Please open config_sample.h, adjust your settings and save it as config.h
#include "config.h"

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

#ifdef LEDpin
void lightSimpleLED(boolean light);
#endif
#ifdef LEDRING
#include "ledring.h"
#include "Arduino.h"
#endif

// the debug mode can be set during runtime via the serial interface. See config.h for a description of the different debug modes.
int debug = STARTDEBUG;

// stores the raw analog values from the joysticks
int rawReads[8];

// Centerpoints store the zero position of the joysticks
int centerPoints[8];

// stores the values from the joysticks after zeroing and mapping
int centered[8];

// store raw value of the keys, without debouncing
int keyVals[NUMKEYS];

// key event, after debouncing. It is 1 only for a single sample
uint8_t keyOut[NUMKEYS];
// state of the key, which stays 1 as long as the key is pressed
uint8_t keyState[NUMKEYS];

// Resulting calculated velocities / movements
// int16_t to match what the HID protocol expects.
int16_t velocity[6];

int tmpInput; // store the value, the user might input over the serial

void setup()
{
// setup the keys e.g. to internal pull-ups
#if NUMKEYS > 0
  setupKeys();
#endif

  // Begin Seral for debugging
  Serial.begin(250000);
  delay(100);
  Serial.setTimeout(2); // the serial interface will look for new debug values and it will only wait 2ms
  // Read idle/centre positions for joysticks.

  // zero the joystick position 500 times (takes approx. 480 ms)
  // during setup() we are not interested in the debug output: debugFlag = false
  busyZeroing(centerPoints, 500, false);

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

void loop()
{
  // check if the user entered a debug mode via serial interface
  if (Serial.available())
  {
    tmpInput = Serial.parseInt(); // Read from serial interface, if a new debug value has been sent. Serial timeout has been set in setup()
    if (tmpInput != 0)
    {
      debug = tmpInput;
      if (tmpInput == -1)
      {
        Serial.println(F("Please enter the debug mode now or while the script is reporting."));
      }
    }
  }

  // Joystick values are read. 0-1023
  readAllFromJoystick(rawReads);

#if NUMKEYS > 0
  // LivingTheDream added reading of key presses
  readAllFromKeys(keyVals);
#endif
  // Report back 0-1023 raw ADC 10-bit values if enabled
  if (debug == 1)
  {
    debugOutput1(rawReads, keyVals);
  }

  if (debug == 11)
  {
    // calibrate the joystick
    // As this is called in the debug=11, we do more iterations.
    busyZeroing(centerPoints, 2000, true);
    debug = -1; // this only done once
  }

  // Subtract centre position from measured position to determine movement.
  for (int i = 0; i < 8; i++)
  {
    centered[i] = rawReads[i] - centerPoints[i];
  }

  if (debug == 20)
  {
    calcMinMax(centered); // debug=20 to calibrate MinMax values
  }

  // Report centered joystick values if enabled. Values should be approx -500 to +500, jitter around 0 at idle
  if (debug == 2)
  {
    debugOutput2(centered);
  }

  FilterAnalogReadOuts(centered);

  // Report centered joystick values. Filtered for deadzone. Approx -350 to +350, locked to zero at idle
  if (debug == 3)
  {
    debugOutput2(centered);
  }

  calculateKinematic(centered, velocity);

#if ROTARY_AXIS > 0
  // If an encoder wheel is used, calculate the velocity of the wheel and replace one of the former calculated velocities
  calcEncoderWheel(velocity, debug);
#endif

#if NUMKEYS > 0
  evalKeys(keyVals, keyOut, keyState);
#endif

#if ROTARY_KEYS > 0 
 // The encoder wheel shall be treated as a key
  calcEncoderAsKey(keyState, debug);
#endif

  if (debug == 4)
  {
    debugOutput4(velocity, keyOut);
    // Report translation and rotation values if enabled.
  }
  if (debug == 5)
  {
    debugOutput5(centered, velocity);
  }

  // if the kill-key feature is enabled, rotations or translations are killed=set to zero
#if (NUMKILLKEYS == 2)
  if (keyVals[KILLROT] == LOW)
  {
    // check for the raw keyVal and not keyOut, because keyOut is only 1 for a single iteration. keyVals has inverse Logic due to pull-ups
    // kill rotation
    velocity[ROTX] = 0;
    velocity[ROTY] = 0;
    velocity[ROTZ] = 0;
  }
  if (keyVals[KILLTRANS] == LOW)
  {
    // kill translation
    velocity[TRANSX] = 0;
    velocity[TRANSY] = 0;
    velocity[TRANSZ] = 0;
  }
#endif

  // report velocity and keys after possible kill-key feature
  if (debug == 6)
  {
    debugOutput4(velocity, keyOut);
  }

#if SWITCHYZ > 0
  switchYZ(velocity);
#endif

#ifdef EXCLUSIVEMODE
  // exclusive mode
  // rotation OR translation, but never both at the same time
  // to avoid issues with classics joysticks
  exclusiveMode(velocity);
#endif

  // report velocity and keys after Switch or ExclusiveMode
  if (debug == 61)
  {
    debugOutput4(velocity, keyOut);
  }

  // get the values to the USB HID driver to send if necessary
  SpaceMouseHID.send_command(velocity[ROTX], velocity[ROTY], velocity[ROTZ], velocity[TRANSX], velocity[TRANSY], velocity[TRANSZ], keyState, debug);

  if (debug == 7)
  {
    // update and report the at what frequency the loop is running
    updateFrequencyReport();
  }

#ifdef LEDpin
#ifdef LEDRING
  processLED(velocity, SpaceMouseHID.updateLEDState());
#else
  lightSimpleLED(SpaceMouseHID.updateLEDState());
  // Check for the LED state by calling updateLEDState.
  // This empties the USB input buffer and checks for the corresponding report.
#endif
#endif

} // end loop()

#ifdef LEDpin
  /// @brief Turn on or off a simple led. The pin is defined by LEDpin in config.h. If the LED needs to be inverted, define LEDinvert in config.h
/// @param light turn on or off
void lightSimpleLED(boolean light)
{
// Check for the LED state by calling updateLEDState.
// This empties the USB input buffer and checks for the corresponding report.
#ifdef LEDinvert
  // Swap the LED logic, if necessary
  if (light)
#else
  if (!light)
#endif
  {
    // true -> LED on -> pull kathode down
    digitalWrite(LEDpin, LOW); // turn the LED o
  }
  else
  {
    // false -> LED off -> pull kathode up
    digitalWrite(LEDpin, HIGH); // turn the LED
  }
}
#endif
