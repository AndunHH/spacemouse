// This file contains all functions to calculate the kinematics

// The user specific settings, like pin mappings or special configuration variables and sensitivities are stored in config.h.
// Please open config_sample.h, adjust your settings and save it as config.h
#include "config.h"
#include <Arduino.h>

// Include math operators for doing better calculation algorithms. Arduino math is a standard library already included.
#include <math.h>
#define sign(x) ((x) < 0 ? -1 : ((x) > 0 ? 1 : 0)) // Define Signum Function

#include "parameterMenu.h"
#include "kinematics.h"
#include "calibration.h"

// Do not change this! Use independent sensitivity multipliers.
#define TOTALSENSITIVITY 350

/**--- Modifier Functions
// SNo: enhanced maths
//      - no deadzones at maximum because of truncated values,
//      - finer trimmable exponential-function
//
// 1.  first x is normalized, so it runs from -1.0 ... 0 ... +1.0
// 2.  to get rid of sign-problems in the "curve-functions", xn=abs(x) is used to get only positive values xn= +1.0 ... 0 ... +1.0
// 3.  to get the negative values back, the sign sx=sign(x) is multiplied at the right places
// 4a. any exponential function y=(xn^a)*sx (with a=1.0 ... 3.0) results in a y of -1.0 ... +1.0
// 4b. a tangent function y=tan(b*(xn*sn)) / tan(b) results in a y of -1.0 ... +1.0 (when x in -1.0 ... +1.0 and b < pi/2)
// 5.  so the tangent() of our exponential() function also results in a y of -1.0 ... +1.0
// 6.  after denormalizing we get a result of -350 ... +350 with the curve always fitting into the used x-/y-range
*/

/// @brief Function to modify the input value according to different mathematic modes. Choose the mathematical function in config.h as MODFUNC (0, 1 or 3)
/// @param x input between -350 and +350
/// @return output between -350 and +350
int modifierFunction(int x, ParamStorage& par) {

  x = constrain(x, -TOTALSENSITIVITY, +TOTALSENSITIVITY); // making sure function input x never exceeds the range of -350 to 350
  double y;
  double xn = abs((double)x / (double)TOTALSENSITIVITY);  // normalize x
  double sx = sign(x);                                    // detect sign of x

  if(par.modFunc == 1){
    // using "squared" function y = abs(x)^a * sign(x)
    // sign putting out -1 or 1 depending on sign of x. (Is needed because x^2 will always be positive)
    y = pow(xn, par.slope_at_zero) * sx;

    // modFunc 2: tan is not supported anymore, because squared tangens serves the same purpose
  }else if(par.modFunc == 3){
    // using "squared" tangens function: y = tan(b * (abs(x)^a * sign(x))) / tan(b)
    y = tan(par.slope_at_end * (pow(xn, par.slope_at_zero) * sx)) / tan(par.slope_at_end);

  }else{
    //MODFUNC == 0 or others...
    // 1:1 linear function
    y = xn * sx;
  }

  //make sure values between -350 and 350 are allowed
  y = constrain(y * (double)TOTALSENSITIVITY, (double)-TOTALSENSITIVITY, (double)+TOTALSENSITIVITY);
  //converting doubles to int again
  return (int)round(y);
}

// define an array for reading the analog pins of the joysticks, see config.h
int pinList[8] = PINLIST;
int invertList[8] = INVERTLIST;

/// @brief Function to read and store analogue voltages for each joystick axis.
/// @param rawReads pointer to 8 analog values
void readAllFromJoystick(int *rawReads){
  for (int i = 0; i < 8; i++) {
    if (invertList[i] == 1) {
      // invert the reading
      rawReads[i] = 1023 - analogRead(pinList[i]);
    } else {
      rawReads[i] = analogRead(pinList[i]);
    }
  }
}

// set the min and maxvals from the config.h into real variables
int minVals[8] = MINVALS;
int maxVals[8] = MAXVALS;

/// @brief Takes the centered joystick values, applies a deadzone and maps the values to +/- 350.
/// @param centered pointer to array with 8 centered analog values
void FilterAnalogReadOuts(int *centered, ParamStorage& par){
    // Filter movement values. Set to zero if movement is below deadzone threshold.
  for(int i = 0; i < 8; i++){
    if (centered[i] < par.deadzone && centered[i] > -par.deadzone){
            centered[i] = 0;
    }else{
      if(centered[i] < 0){ // if the value is smaller 0 ...
        // ... map the value from the [min,-DEADZONE] to [-350,0]
        centered[i] = map(centered[i], minVals[i], -par.deadzone, -TOTALSENSITIVITY, 0);
      }else{ // if the value is > 0 ...
        // ... map the values from the [DEADZONE,max] to [0,+350]
        centered[i] = map(centered[i], par.deadzone, maxVals[i], 0, TOTALSENSITIVITY);
      }
    }
  }
}

/**
 * When using Hall Effect sensors the reading of the sensors has to be translated
 * to the joystick values


 *     7   6              Y+
 *       |                .
 *  8    |    3           .
 *    ---+---        X-...Z+...X+
 *  9    |    2           .
 *       |                .
 *     0   1              Y-
 *
 *
 *
 * Movement table (expected)
 *
 * Movement     Plane           AX  AY  BX  BY  CX  CY  DX  DY  ||  H0  H1  H2  H3  H6  H7  H8  H9
 * ------------|---------------|--------------------------------||---------------------------------
 * West         Horizontal      0   +   0   0   0   -   0   0   ||  0   0   +   +   0   0   -   -
 * East         Horizontal      0   -   0   0   0   +   0   0   ||  0   0   -   -   0   0   +   +
 * North        Horizontal      0   0   0   -   0   0   0   +   ||  +   +   0   0   -   -   0   0
 * South        Horizontal      0   0   0   +   0   0   0   -   ||  -   -   0   0   +   +   0   0
 * Top          Vertical        -   0   -   0   -   0   -   0   ||  +   +   +   +   +   +   +   +               (all magnets further away)
 * Bottom       Vertical        +   0   +   0   +   0   +   0   ||  -   -   -   -   -   -   -   -               (all magnets closer by)
 * Rotx-fw      Vertical        -   0   0   0   +   0   0   0   ||  +   +   0   0   -   -   0   0
 * Rotx-bw      Vertical        +   0   0   0   -   0   0   0   ||  -   -   0   0   +   +   0   0
 * Roty-left    Vertical        0   0   +   0   0   0   -   0   ||  0   0   +   +   0   0   -   -
 * Roty-right   Vertical        0   0   -   0   0   0   +   0   ||  0   0   -   -   0   0   +   +
 * Rotz-clock   Horizontal      -   0   -   0   -   0   -   0   ||  +   -   +   -   +   -   +   -
 * Rotz-cclock  Horizontal      +   0   +   0   +   0   +   0   ||  -   +   -   +   -   +   -   +
 *
 */

void _calculateKinematicSensors(int* centered, int16_t* velocity, bool exclusive){
  // resistive joysticks or hall-joysticks
  #ifndef HALLEFFECT
  _// if exclusive-mode is on, rotations are only calculated, if no z-move is detected
    int cntN = 0;
    int cntP = 0;
    if(centered[AX] < 0){cntN += 1;} if(centered[AX] > 0){cntP += 1;}
    if(centered[BX] < 0){cntN += 1;} if(centered[BX] > 0){cntP += 1;}
    if(centered[CX] < 0){cntN += 1;} if(centered[CX] > 0){cntP += 1;}
    if(centered[DX] < 0){cntN += 1;} if(centered[DX] > 0){cntP += 1;}

    bool zMove = exclusive && ((cntP >= 3 && cntN == 0) || (cntN >= 3 && cntP == 0));

    velocity[TRANSX] = (-centered[CY] +centered[AY]);
    velocity[TRANSY] = (-centered[BY] +centered[DY]);
    velocity[TRANSZ] = (-centered[AX] -centered[BX] -centered[CX] -centered[DX]);
    if(!zMove){
      velocity[ROTX] = (-centered[CX] +centered[AX]);
      velocity[ROTY] = (-centered[BX] +centered[DX]);
      velocity[ROTZ] = (+centered[AY] +centered[BY] +centered[CY] +centered[DY]);
    }
  // hall-sensors with magnets
  #else
    velocity[TRANSX] = (centered[HES1] -centered[HES0] +centered[HES6] -centered[HES7]) / 2;
    velocity[TRANSY] = (centered[HES2] -centered[HES3] +centered[HES9] -centered[HES8]) / 2;
    velocity[TRANSZ] = (centered[HES0] +centered[HES1] +centered[HES2] +centered[HES3] +centered[HES6] +centered[HES7] +centered[HES8] +centered[HES9]) / 4;
    velocity[ROTX]   = (centered[HES0] +centered[HES1] -centered[HES6] -centered[HES7]) / 2;
    velocity[ROTY]   = (centered[HES8] +centered[HES9] -centered[HES2] -centered[HES3]) / 2;
    velocity[ROTZ]   = (centered[HES0] +centered[HES2] +centered[HES6] +centered[HES8] -centered[HES1] -centered[HES3] -centered[HES7] -centered[HES9]) / 4;
  #endif
}

/// @brief Calculate the kinematic of the three axis from the eight joysticks
/// @param centered eight values from the four joysticks
/// @param velocity resulting translational and rotational motions
void calculateKinematic(int *centered, int16_t *velocity, ParamStorage& par){
  // Get raw kinematics from sensors
  _calculateKinematicSensors(centered, velocity, par.exclusiveMode);

  // transX
  velocity[TRANSX] = velocity[TRANSX] / par.transX_sensitivity;
  velocity[TRANSX] = modifierFunction(velocity[TRANSX], par);                             // recalculate with modifier function

  // transY
  velocity[TRANSY] = velocity[TRANSY] / par.transY_sensitivity;
  velocity[TRANSY] = modifierFunction(velocity[TRANSY], par);                             // recalculate with modifier function

  // transZ
  if(velocity[TRANSZ] < 0){
    velocity[TRANSZ] = velocity[TRANSZ] / par.neg_transZ_sensitivity;
    velocity[TRANSZ] = modifierFunction(velocity[TRANSZ], par);                           // recalculate with modifier function
    if (abs(velocity[TRANSZ]) < par.gate_neg_transZ){
      velocity[TRANSZ] = 0;
    }
  }else{                                                                                  // pulling the knob upwards is much heavier... smaller factor
    velocity[TRANSZ] = velocity[TRANSZ] / par.pos_transZ_sensitivity;
    velocity[TRANSZ] = constrain(velocity[TRANSZ], (double)-TOTALSENSITIVITY, (double)TOTALSENSITIVITY);  // no modifier function, just constrain linear!
  }

  // rotX
  velocity[ROTX] = velocity[ROTX] / par.rotX_sensitivity;
  velocity[ROTX] = modifierFunction(velocity[ROTX], par);                                 // recalculate with modifier function
  if(abs(velocity[ROTX]) < par.gate_rotX){
    velocity[ROTX] = 0;
  }

  // rotY
  velocity[ROTY] = velocity[ROTY] / par.rotY_sensitivity;
  velocity[ROTY] = modifierFunction(velocity[ROTY], par); // recalculate with modifier function
  if(abs(velocity[ROTY]) < par.gate_rotY){
    velocity[ROTY] = 0;
  }

  // rotZ
  velocity[ROTZ] = velocity[ROTZ] / par.rotZ_sensitivity;
  velocity[ROTZ] = modifierFunction(velocity[ROTZ], par); // recalculate with modifier function
  if(abs(velocity[ROTZ]) < par.gate_rotZ){
    velocity[ROTZ] = 0;
  }

  // Invert directions if needed
  if(par.invX  == 1){velocity[TRANSX] = -velocity[TRANSX];}
  if(par.invY  == 1){velocity[TRANSY] = -velocity[TRANSY];}
  if(par.invZ  == 1){velocity[TRANSZ] = -velocity[TRANSZ];}
  if(par.invRX == 1){velocity[ROTX]   = -velocity[ROTX];}
  if(par.invRY == 1){velocity[ROTY]   = -velocity[ROTY];}
  if(par.invRZ == 1){velocity[ROTZ]   = -velocity[ROTZ];}
} // end calculateKinematic

/// @brief Switch position of X and Y values
/// @param velocity pointer to velocity array
void switchXY(int16_t *velocity){
  int16_t tmp      = velocity[TRANSX];
  velocity[TRANSX] = velocity[TRANSY];
  velocity[TRANSY] = tmp;

  tmp = velocity[ROTX];
  velocity[ROTX] = velocity[ROTY];
  velocity[ROTY] = tmp;
}

/// @brief Switch position of Y and Z values
/// @param velocity pointer to velocity array
void switchYZ(int16_t *velocity){
  int16_t tmp      = velocity[TRANSY];
  velocity[TRANSY] = velocity[TRANSZ];
  velocity[TRANSZ] = tmp;

  tmp = velocity[ROTY];
  velocity[ROTY] = velocity[ROTZ];
  velocity[ROTZ] = tmp;
}

/// @brief Check if translation or rotation is dominant and set the other values to zero to allow exclusively rotation or translation
// to avoid issues with classics joysticks
/// @param velocity pointer to velocity array
void exclusiveMode(int16_t *velocity){
  uint16_t totalRot   = abs(velocity[ROTX]  ) + abs(velocity[ROTY]  ) + abs(velocity[ROTZ]  );
  uint16_t totalTrans = abs(velocity[TRANSX]) + abs(velocity[TRANSY]) + abs(velocity[TRANSZ]);
  if(totalRot > totalTrans){
    velocity[TRANSX] = 0;
    velocity[TRANSY] = 0;
    velocity[TRANSZ] = 0;
  }else{
    velocity[ROTX] = 0;
    velocity[ROTY] = 0;
    velocity[ROTZ] = 0;
  }
}
