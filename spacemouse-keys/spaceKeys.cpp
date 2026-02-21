// The user specific settings, like pin mappings or special configuration variables and
// sensitivities are stored in config.h. Please open config_sample.h, adjust your settings and save
// it as config.h
#include "config.h"
#include <Arduino.h>
// check config.h if this functions and variables are needed
#if NUMKEYS > 0

// array with the pin definition of all keys
int keyList[NUMKEYS] = KEYLIST;

// Function to setup up all keys in keyList
void setupKeys() {
  for (int i = 0; i < NUMKEYS; i++) {
    pinMode(keyList[i], INPUT_PULLUP);
  }
}

// Function to read and store the digital states for each of the keys
void readAllFromKeys(int *keyVals) {
  for (int i = 0; i < NUMKEYS; i++) {
    keyVals[i] = digitalRead(keyList[i]);
  }
}

// Evaluate and debounce all keys from the raw keyVals into the debounced keyOut event or the
// debounced keyState. The keyOut is only 1 for one iteration of the loop.
void evalKeys(int *keyVals, uint8_t *keyOut, uint8_t *keyState) {
  static unsigned long timestamp[NUMKEYS]; // needed for key evaluation

  // Button Evaluation
  for (int i = 0; i < NUMKEYS; i++) {
    // The keys are configured with pull_up, see setupKeys() and are pulled to ground, when pressed.
    // Therefore, the pressed key is false, which is an inverted logic
    if (!keyVals[i]) { // the key is pressed
      // Making sure button cannot trigger multiple times which would result in overloading HID.
      if (keyState[i] == 0) { // if the button has not been pressed lately:
        keyOut[i] = 1;   // this is the variable telling the outside world only one iteration, that
                         // the key was pressed
        keyState[i] = 1; // remember, that we already told the outside world about this key
        timestamp[i] = millis(); // remember the time, the button was pressed
#ifdef DEBUG_KEYS
        Serial.println("");
        Serial.print("Key: "); // this is always sent over the serial console, and not only in debug
        Serial.println(i);
#endif
      } else { // the button was already pressed and is still pressed (and the event sent in the
               // last loop), don't send the keyOut event again.
        keyOut[i] = 0;
      }
    } else {                  // the button is not pressed
      if (keyState[i] == 1) { // has it been pressed lately?
        // debouncing:
        if (millis() - timestamp[i] >
            DEBOUNCE_KEYS_MS) { // check if the last button press is long enough in the past
          keyState[i] = 0;      // reset this marker and allow a new button press
        }
      }
    }
  }
}
#endif