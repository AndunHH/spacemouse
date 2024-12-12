// The user specific settings, like pin mappings or special configuration variables and sensitivities are stored in config.h.
// Please open config_sample.h, adjust your settings and save it as config.h
#include "config.h"
#include <Arduino.h>
#include <FastLED.h>

#include "ledring.h"

void setLEDsOnClock(uint16_t clock, CRGB color); // should stay private - move  to better place TODO
void setAllLEDs(CRGB color);
void rotateColor(boolean clockwise, CRGB color);


#include "calibration.h"

CRGB leds[LEDRING];
; // time from millis(), when the last led was set
#define LEDUPDATERATE_MS 200

/// @brief Initialize the LED ring. Call this once during setup()
void initLEDring() {
    FastLED.addLeds<WS2811, LEDpin, GRB>(leds, LEDRING);
}

/// @brief process the LEDs connected via FastLED. Call this in loop()
/// @param velocity 
void processLED(int16_t* velocity) {
    unsigned long now = millis();
    static unsigned long lastLEDupdate = now;
    
    if (now - lastLEDupdate >= LEDUPDATERATE_MS) {  
        setAllLEDs(CRGB::Black);

       switch (getMainVelocity(velocity)) {
            case TRANSX:
            // TX pos: 3 o'clock neg: 9 o'clock
                if(velocity[TRANSX] > 0) {
                    setLEDsOnClock(3, CRGB::Red);
                } 
                else {
                    setLEDsOnClock(9, CRGB::Red);
                }
            break;
            case TRANSY:
            // TY pos: 12 o'clock neg, 6 o'clock
                if(velocity[TRANSY] > 0) {
                    setLEDsOnClock(12, CRGB::Red);
                } 
                else {
                    setLEDsOnClock(6, CRGB::Red);
                }
            break;
            case TRANSZ:
            // TZ pos: all white, neg: all dark blue
                if(velocity[TRANSZ] > 0) {
                    setAllLEDs(CRGB::AntiqueWhite);
                    FastLED.setBrightness(50);
                } 
                else {
                    setAllLEDs(CRGB::DarkBlue);
                    FastLED.setBrightness(50);
                }
            break;
            case ROTX:
            // RX pos: red 6 o'clock, neg red 12 o'clock
                if(velocity[ROTX] > 0) {
                    setLEDsOnClock(6, CRGB::Green);
                } 
                else {
                    setLEDsOnClock(12, CRGB::Green);
                }
            break;
            case ROTY:
            // RY pos: red 3 o'clock, neg red 9 o'clock
                if(velocity[ROTY] > 0) {
                    setLEDsOnClock(3, CRGB::Green);
                } 
                else {
                    setLEDsOnClock(9, CRGB::Green);
                }
            break;
            case ROTZ:
            // RZ pos: red ring wandering around counterclock wise; neg: clockwise
                if(velocity[ROTZ] > 0) {
                    rotateColor(false, CRGB::Green);
                }
                else {
                    rotateColor(true, CRGB::Red);
                }
            break;
            default: // all very dimm
                    setAllLEDs(CRGB::DarkGrey);
                    FastLED.setBrightness(10);
            break;
        }
        
      FastLED.show();
      lastLEDupdate += LEDUPDATERATE_MS;
    }
}

/// @brief rotate a single around the LED ring
/// @param clockwise turns clockwise if true
/// @param color which CRGB color
void rotateColor(boolean clockwise, CRGB color) {
    static int rotateLEDpos = 0;
    leds[rotateLEDpos] = color;
    if (clockwise) {
        rotateLEDpos = (rotateLEDpos + 1) % LEDRING;
    }
    else{
        rotateLEDpos = (rotateLEDpos + LEDRING - 1) % LEDRING; // avoid negativ led position 
    }
    
}

/// @brief set all leds to given color
/// @param color 
void setAllLEDs(CRGB color) {
    for (int i = 0; i<LEDRING; i++) {
    leds[i] = color;
    }
}

/// @brief set LED on LED ring regarding the ring as a clock
/// @param clock position of the LED to light up
/// @param color color to light
void setLEDsOnClock(uint16_t clock, CRGB color) {
    uint16_t pos = 0;
    pos = (clock * (LEDRING/12))%LEDRING;
    pos = (LEDclockOffset + pos)%LEDRING;
    leds[pos] = color;
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