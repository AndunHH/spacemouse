// The user specific settings, like pin mappings or special configuration variables and sensitivities are stored in config.h.
// Please open config_sample.h, adjust your settings and save it as config.h
#include "config.h"
#include <Arduino.h>

#include "ledring.h"
#include <FastLED.h>

CRGB leds[LEDRING];
; // time from millis(), when the last led was set
#define LEDUPDATERATE_MS 500

void initLEDring() {
    FastLED.addLeds<WS2811, LEDpin, RGB>(leds, LEDRING);
}

void processLED() {
    unsigned long now = millis();
    static unsigned long lastLEDupdate = now;
    static int whiteLed = 0;
    if (now - lastLEDupdate >= LEDUPDATERATE_MS) {  
      // Turn our current led back to black for the next loop around
      leds[whiteLed] = CRGB::Black;
      whiteLed = (whiteLed + 1) % LEDRING;

      // Turn our current led on to white, then show the leds
      leds[whiteLed] = CRGB::White;

      // Show the leds (only one of which is set to white, from above)
      FastLED.show();
      lastLEDupdate += LEDUPDATERATE_MS;
    }
}