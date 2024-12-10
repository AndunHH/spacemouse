// The user specific settings, like pin mappings or special configuration variables and sensitivities are stored in config.h.
// Please open config_sample.h, adjust your settings and save it as config.h
#include "config.h"
#include <Arduino.h>

#include "ledring.h"
#include <FastLED.h>

#include "calibration.h"

CRGB leds[LEDRING];
; // time from millis(), when the last led was set
#define LEDUPDATERATE_MS 500

/// @brief Initialize the LED ring. Call this once during setup()
void initLEDring() {
    FastLED.addLeds<WS2811, LEDpin, RGB>(leds, LEDRING);
}

/// @brief process the LEDs connected via FastLED. Call this in loop()
/// @param velocity 
void processLED(int16_t* velocity) {
    unsigned long now = millis();
    static unsigned long lastLEDupdate = now;
    //static int whiteLed = 0;
    if (now - lastLEDupdate >= LEDUPDATERATE_MS) {  
      // Turn our current led back to black for the next loop around
      //leds[whiteLed] = CRGB::Black;
      //whiteLed = (whiteLed + 1) % LEDRING;

      calcLEDstate(velocity);
    getMainVelocity(velocity);
      // Turn our current led on to white, then show the leds
      //leds[whiteLed] = CRGB::White;

      // Show the leds (only one of which is set to white, from above)
      FastLED.show();
      lastLEDupdate += LEDUPDATERATE_MS;
    }
}

/// @brief calculate which LEDs shall light up, depending on the velocity informations
/// @param velocity 
void calcLEDstate(int16_t *velocity)
{
    if (velocity[TRANSX] > 10)
    {
        leds[6] = CRGB::White;
        leds[18] = CRGB::Black;
    }
    else
    {
        leds[18] = CRGB::White;
        leds[6] = CRGB::Black;
    }

    if (velocity[TRANSY] > 10)
    {
        leds[12] = CRGB::White;
        leds[0] = CRGB::Black;
    }
    else
    {
        leds[0] = CRGB::White;
        leds[12] = CRGB::Black;
    }
}

/// @brief Calculate which velocity is the main action. What is the strongest movement?
/// @param velocity array with velocities
/// @return index with the biggest velocity. returns -1 if all in deadzone
int8_t getMainVelocity(int16_t *velocity)
{
    int8_t mainVelocity = -1;
    int16_t velMax = 0;
    for (int i = 0; i < 6; i++) {
        // bigger than deadzone and bigger than before?
        if((abs(velocity[i]) > velMax) && (abs(velocity[i]) > VelocityDeadzoneForLED)) {
            velMax = abs(velocity[i]);
            mainVelocity = i;
        }
    }
    return mainVelocity;
}