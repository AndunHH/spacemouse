// This file contains all functions to calculate the kinematics

// The user specific settings, like pin mappings or special configuration variables and sensitivities are stored in config.h.
// Please open config_sample.h, adjust your settings and save it as config.h
#include "config.h"
#include <Arduino.h>

// Include math operators for doing better calculation algorithms. Arduino math is a standard library already included.
#include <math.h>
#define sign(x) ((x) < 0 ? -1 : ((x) > 0 ? 1 : 0)) // Define Signum Function

#include "kinematics.h"
#include "calibration.h"

/// @brief Function to modify the input value according to different mathematic modes. Choose the mathematical function in config.h as modFunc
/// @param x input between -350 and +350
/// @return output between -350 and +350
int modifierFunction(int x)
{
    // making sure function input never exedes range of -350 to 350
    x = constrain(x, -350, 350);
    double result;
#if (MODFUNC == 1)
    // using squared function y = x^2*sign(x)
    result = 350 * pow(x / 350.0, 2) * sign(x); // sign putting out -1 or 1 depending on sign of value. (Is needed because x^2 will always be positive)
#elif (MODFUNC == 2)
    // using tan function: tan(x)
    result = 350 * tan(x / 350.0);
#elif (MODFUNC == 3)
    // using squared tan function: tan(x^2*sign(x))
    result = 350 * tan(pow(x / 350.0, 2) * sign(x)); // sign putting out -1 or 1 depending on sign of value. (Is needed because x^2 will always be positive)
#elif (MODFUNC == 4)
    // using cubed tan function: tan(x^3)
    result = 350 * tan(pow(x / 350.0, 3));
#else
    // MODFUNC == 0 or others...
    // no modification
    result = x;
#endif

    // make sure values between-350 and 350 are allowed
    result = constrain(result, -350, 350);
    // converting doubles to int again
    return (int)round(result);
}

// define an array for reading the analog pins of the joysticks, see config.h
int pinList[8] = PINLIST;
int invertList[8] = INVERTLIST;

/// @brief Function to read and store analogue voltages for each joystick axis.
/// @param rawReads pointer to 8 analog values
void readAllFromJoystick(int *rawReads)
{
    for (int i = 0; i < 8; i++)
    {
        if (invertList[i] == 1)
        {
            // invert the reading
            rawReads[i] = 1023 - analogRead(pinList[i]);
        }
        else
        {
            rawReads[i] = analogRead(pinList[i]);
        }
    }
}

// set the min and maxvals from the config.h into real variables
int minVals[8] = MINVALS;
int maxVals[8] = MAXVALS;
// Please do not change this anymore. Use indipendent sensitivity multiplier.
#define TOTALSENSITIVITY 350

/// @brief Takes the centered joystick values, applies a deadzone and maps the values to +/- 350.
/// @param centered pointer to array with 8 centered analog values
void FilterAnalogReadOuts(int *centered)
{
    // Filter movement values. Set to zero if movement is below deadzone threshold.
    for (int i = 0; i < 8; i++)
    {
        if (centered[i] < DEADZONE && centered[i] > -DEADZONE)
        {
            centered[i] = 0;
        }
        else
        {
            if (centered[i] < 0)
            { // if the value is smaller 0 ...
                // ... map the value from the [min,-DEADZONE] to [-350,0]
                centered[i] = map(centered[i], minVals[i], -DEADZONE, -TOTALSENSITIVITY, 0);
            }
            else
            { // if the value is > 0 ...
                // ... map the values from the [DEADZONE,max] to [0,+350]
                centered[i] = map(centered[i], DEADZONE, maxVals[i], 0, TOTALSENSITIVITY);
            }
        }
    }
}

/// @brief Calculate the kinematic of the three axis from the eight joysticks
/// @param centered eight values from the four joysticks
/// @param velocity resulting translational and rotational motions
void calculateKinematic(int *centered, int16_t *velocity)
{
    // transX
    velocity[TRANSX] = (-centered[CY] + centered[AY]) / ((float)TRANSX_SENSITIVITY);
    velocity[TRANSX] = modifierFunction(velocity[TRANSX]); // recalculate with modifier function

    // transY
    velocity[TRANSY] = (-centered[BY] + centered[DY]) / ((float)TRANSY_SENSITIVITY);
    velocity[TRANSY] = modifierFunction(velocity[TRANSY]); // recalculate with modifier function

    velocity[TRANSZ] = -centered[AX] - centered[BX] - centered[CX] - centered[DX];
    if (velocity[TRANSZ] < 0)
    {
        velocity[TRANSZ] = modifierFunction(velocity[TRANSZ] / ((float)NEG_TRANSZ_SENSITIVITY)); // recalculate with modifier function
        if (abs(velocity[TRANSZ]) < GATE_NEG_TRANSZ)
        {
            velocity[TRANSZ] = 0;
        }
    }
    else
    {                                                                                                // pulling the knob upwards is much heavier... smaller factor
        velocity[TRANSZ] = constrain(velocity[TRANSZ] / ((float)POS_TRANSZ_SENSITIVITY), -350, 350); // no modifier function, just constrain linear!
    }

    // rotX
    velocity[ROTX] = (-centered[CX] + centered[AX]) / ((float)ROTX_SENSITIVITY);
    velocity[ROTX] = modifierFunction(velocity[ROTX]); // recalculate with modifier function
    if (abs(velocity[ROTX]) < GATE_ROTX)
    {
        velocity[ROTX] = 0;
    }

    // rotY
    velocity[ROTY] = (-centered[BX] + centered[DX]) / ((float)ROTY_SENSITIVITY);
    velocity[ROTY] = modifierFunction(velocity[ROTY]); // recalculate with modifier function
    if (abs(velocity[ROTY]) < GATE_ROTY)
    {
        velocity[ROTY] = 0;
    }

    // rotZ
    velocity[ROTZ] = (centered[AY] + centered[BY] + centered[CY] + centered[DY]) / ((float)ROTZ_SENSITIVITY);
    velocity[ROTZ] = modifierFunction(velocity[ROTZ]); // recalculate with modifier function
    if (abs(velocity[ROTZ]) < GATE_ROTZ)
    {
        velocity[ROTZ] = 0;
    }

// Invert directions if needed
#if INVX > 0
    velocity[TRANSX] = velocity[TRANSX] * -1;
#endif
#if INVY > 0
    velocity[TRANSY] = velocity[TRANSY] * -1;
#endif
#if INVZ > 0
    velocity[TRANSZ] = velocity[TRANSZ] * -1;
#endif
#if INVRX > 0
    velocity[ROTX] = velocity[ROTX] * -1;
#endif
#if INVRY > 0
    velocity[ROTY] = velocity[ROTY] * -1;
#endif
#if INVRZ > 0
    velocity[ROTZ] = velocity[ROTZ] * -1;
#endif
} // end calculateKinematic

/// @brief Switch position of X and Y values
/// @param velocity pointer to velocity array
void switchXY(int16_t *velocity)
{
    int16_t tmp = 0;
    tmp = velocity[TRANSX];
    velocity[TRANSX] = velocity[TRANSY];
    velocity[TRANSY] = tmp;

    tmp = velocity[ROTX];
    velocity[ROTX] = velocity[ROTY];
    velocity[ROTY] = tmp;
}

/// @brief Switch position of Y and Z values
/// @param velocity pointer to velocity array
void switchYZ(int16_t *velocity)
{
    int16_t tmp = 0;
    tmp = velocity[TRANSY];
    velocity[TRANSY] = velocity[TRANSZ];
    velocity[TRANSZ] = tmp;

    tmp = velocity[ROTY];
    velocity[ROTY] = velocity[ROTZ];
    velocity[ROTZ] = tmp;
}

/// @brief Check if translation or rotation is dominant and set the other values to zero to allow exclusively rotation or translation
// to avoid issues with classics joysticks
/// @param velocity pointer to velocity array
void exclusiveMode(int16_t *velocity)
{
    uint16_t totalRot = abs(velocity[ROTX]) + abs(velocity[ROTY]) + abs(velocity[ROTZ]);
    uint16_t totalTrans = abs(velocity[TRANSX]) + abs(velocity[TRANSY]) + abs(velocity[TRANSZ]);
    if (totalRot > totalTrans)
    {
        velocity[TRANSX] = 0;
        velocity[TRANSY] = 0;
        velocity[TRANSZ] = 0;
    }
    else
    {
        velocity[ROTX] = 0;
        velocity[ROTY] = 0;
        velocity[ROTZ] = 0;
    }
}
