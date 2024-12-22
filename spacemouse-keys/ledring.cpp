// The user specific settings, like pin mappings or special configuration variables and sensitivities are stored in config.h.
// Please open config_sample.h, adjust your settings and save it as config.h
#include "config.h"
#include <Arduino.h>
#ifdef LEDRING
#include <FastLED.h>

#include "ledring.h"
#include "kinematics.h"

// the following functions are only called from ledring.cpp and are therefore not in the header file
void setLEDsOnClock(uint16_t clock, CRGB color); 
void set4LEDsOnClock(uint16_t clock, CRGB color);
void setAllLEDs(CRGB color);
void rotateColor(boolean clockwise, CRGB color);

CRGB leds[LEDRING];


/// @brief Initialize the LED ring. Call this once during setup()
void initLEDring()
{
    FastLED.addLeds<WS2811, LEDpin, GRB>(leds, LEDRING);
}

/// @brief process the LEDs connected via FastLED. Call this in loop()
/// @param velocity array with velocity informations
/// @param ledCmd transmit if the LED shall be on (as it may be demanded over USB)
void processLED(int16_t *velocity, boolean ledCmd)
{
    unsigned long now = millis();
    static unsigned long lastLEDupdate = now;
    

    if (now - lastLEDupdate >= LEDUPDATERATE_MS)
    {
        setAllLEDs(CRGB::Black);
        if (ledCmd)
        {
            // turn all on
            setAllLEDs(CRGB::DarkOliveGreen);
        }
        else
        {
            // USB doesn't send us commands to turn on LED
            switch (getMainVelocity(velocity))
            {
            case TRANSX:
                setAllLEDs(CRGB::Yellow);
                // TX pos: 3 o'clock neg: 9 o'clock 
                // light up the _free_ positions 
                if ((velocity[TRANSX] > 0) != (INVX == 1))
                {
                    set4LEDsOnClock(9, CRGB::Red);
                }
                else
                {
                    set4LEDsOnClock(3, CRGB::Red);
                }
                break;
            case TRANSY:
                setAllLEDs(CRGB::Yellow);
                // TY pos: 12 o'clock neg, 6 o'clock
                if ((velocity[TRANSY] > 0) != (INVY == 1))
                {
                    set4LEDsOnClock(6, CRGB::Red);
                }
                else
                {
                    set4LEDsOnClock(12, CRGB::Red);
                }
                break;
            case TRANSZ:
                // TZ pos: all white, neg: all dark blue
                if ((velocity[TRANSZ] > 0) != (INVZ == 1))
                {
                    setAllLEDs(CRGB::AntiqueWhite);
                    FastLED.setBrightness(50);
                }
                else
                {
                    setAllLEDs(CRGB::DarkBlue);
                    FastLED.setBrightness(50);
                }
                break;
            case ROTX:
                setAllLEDs(CRGB::SkyBlue);
                // RX pos: red 6 o'clock, neg red 12 o'clock
                if ((velocity[ROTX] > 0) != (INVRX == 1))
                {
                    set4LEDsOnClock(12, CRGB::Green);
                }
                else
                {
                    set4LEDsOnClock(6, CRGB::Green);
                }
                break;
            case ROTY:
                setAllLEDs(CRGB::SkyBlue);
                // RY pos: red 3 o'clock, neg red 9 o'clock
                if ((velocity[ROTY] > 0) != (INVRY == 1))
                {
                    set4LEDsOnClock(9, CRGB::Green);
                }
                else
                {
                    set4LEDsOnClock(3, CRGB::Green);
                }
                break;
            case ROTZ:
                setAllLEDs(CRGB::SkyBlue);
                // RZ pos: red ring wandering around counterclock wise; neg: clockwise
                if ((velocity[ROTZ] > 0) != (INVRZ == 1))
                {
                    rotateColor(false, CRGB::DarkRed);
                }
                else
                {
                    rotateColor(true, CRGB::DarkRed);
                }
                break;
            default: // all very dimm
                setAllLEDs(CRGB::DarkGrey);
                FastLED.setBrightness(5);
                break;
            }
        }
        FastLED.show();
        lastLEDupdate += LEDUPDATERATE_MS;
    }
}

/// @brief rotate a single around the LED ring
/// @param clockwise turns clockwise if true
/// @param color which CRGB color
void rotateColor(boolean clockwise, CRGB color)
{
    static int rotateLEDpos = 0;
    leds[rotateLEDpos] = color;
    if (clockwise)
    {
        rotateLEDpos = (rotateLEDpos + 1) % LEDRING;
    }
    else
    {
        rotateLEDpos = (rotateLEDpos + LEDRING - 1) % LEDRING; // avoid negativ led position
    }
}

/// @brief set all leds to given color
/// @param color
void setAllLEDs(CRGB color)
{
    for (int i = 0; i < LEDRING; i++)
    {
        leds[i] = color;
    }
}

/// @brief set LED on LED ring regarding the ring as a clock
/// @param clock position of the LED to light up
/// @param color color to light
void setLEDsOnClock(uint16_t clock, CRGB color)
{
    uint16_t pos = 0;
    pos = (clock * (LEDRING / 12)) % LEDRING;
    pos = (LEDclockOffset + pos) % LEDRING;
    leds[pos] = color;
}

/// @brief set 4 LEDs on LED ring regarding the ring as a clock
/// @param clock position of the LED to light up
/// @param color color to light
void set4LEDsOnClock(uint16_t clock, CRGB color)
{
    uint16_t pos = 0;
    pos = (clock * (LEDRING / 12)) % LEDRING;
    pos = (LEDclockOffset + pos) % LEDRING;

    leds[pos] = color;
    leds[(pos+1)%LEDRING] = color;
    leds[(LEDRING + pos - 1)%LEDRING] = color;
    leds[(LEDRING + pos - 2)%LEDRING] = color;
}

#endif // #if LEDring

/// @brief Calculate which velocity is the main action. What is the strongest movement?
/// @param velocity array with velocities
/// @return index with the biggest velocity. returns -1 if all in deadzone
int8_t getMainVelocity(int16_t *velocity)
{
    int8_t mainVelocity = -1;
    int16_t velMax = 0;
    for (int i = 0; i < 6; i++)
    {
        // bigger than deadzone and bigger than before?
        if ((abs(velocity[i]) > velMax) && (abs(velocity[i]) > VelocityDeadzoneForLED))
        {
            velMax = abs(velocity[i]);
            mainVelocity = i;
        }
    }
    return mainVelocity;
}