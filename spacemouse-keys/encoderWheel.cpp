/*
 * This code gives the space mouse the ability to include an encoder wheel.
 * The encoder gives positions but the space mouse reports velocities.
 * Therefore, we calculate a filtered derivative from the encoder position and
 * replace the desired velocity from the original space mouse.
 *
 * Based on the idea by JoseLuisGZA, rewritten by AndunHH
 */

#include <Arduino.h>
#include "config.h"

#if ROTARY_AXIS > 0 or ROTARY_KEYS > 0
  #include "encoderWheel.h"

  // Include Encoder library by Paul Stoffregen
  #include <Encoder.h>
  
  Encoder myEncoder(ENCODER_CLK, ENCODER_DT);
  
  int32_t previousEncoderValue = 0;
  int32_t newEncoderValue; // Store encoder readings
  int32_t delta = 0;       // Tracks encoder increments when turned
  
  float simpull;             // calculated velocity of the encoder wheel
  
  void initEncoderWheel(){
    // Read initial value from encoder
    newEncoderValue = myEncoder.read();
  }
  
  /// @brief Calculate the encoder wheel and update the result in the velocity array
  /// @param velocity   Array with the velocity, which gets updated at position ROTARY_AXIS-1
  /// @param debugOut   Generate a debug output if true
  /// @param par        struct of parameters used by the system at runtime
  void calcEncoderWheel(int16_t *velocity, bool debugOut, ParamStorage& par){
    static float factor = 1.0;
    static int zoomIterator = par.rotAxisEchos; // Counter for echoing the delta through a number of loops for a smoother zoom animation
  
    // read encoder
    newEncoderValue = myEncoder.read();
    if (newEncoderValue != previousEncoderValue){
      // position changed, how much?
      delta = newEncoderValue - previousEncoderValue;
      previousEncoderValue = newEncoderValue;
      zoomIterator = 0;
    }
  
    // Distribute encoder delta through the echoes in the loop and based on simulated axis chosen by the user
    // Faded intensity for echoing the encoder reading.
    if (zoomIterator < par.rotAxisEchos){
      factor = 1.0 - (((float)zoomIterator) / (float)par.rotAxisEchos); // factor shall be between 0.0 and 1.0
      simpull = (factor * par.rotAxisSimStrength) * delta;
      zoomIterator++; // iterate
      // add the velocity of the encoder wheel to one of the 6 axis
      // the ROTARY_AXIS definition is one above the array definition used for the velocity array (see calibration.h)
      // Therefore ROTARY_AXIS-1 is used to change the velocity value
      velocity[ROTARY_AXIS - 1] = velocity[ROTARY_AXIS - 1] + simpull;
    }else{
      // fading has ended
      simpull = 0;
    }
  
    if(debugOut){
      // create debug output
      Serial.print(F("Enc Val: "));
      Serial.print(newEncoderValue);
      Serial.print(F(", factor: "));
      Serial.print(factor);
      Serial.print(F(", simpull: "));
      Serial.println(simpull);
    }
  }
  
  /// @brief Read out the encoder and treat as keystroke
  /// @param keyState  overwrite some keys with encoder movement
  /// @param debugOut  Generate a debug output if true
  void calcEncoderAsKey(uint8_t keyState[NUMKEYS], bool debugOut){
    // read encoder
    newEncoderValue = myEncoder.read();
    if (newEncoderValue != previousEncoderValue){
      // If the position changed, add this to delta. As long as delta != 0, report the key as pressed
      delta = (newEncoderValue - previousEncoderValue)*ROTARY_KEY_STRENGTH + delta;
      previousEncoderValue = newEncoderValue;
      
      if(debugOut){
          // create debug output
          Serial.print("Enc Val: ");
          Serial.println(newEncoderValue);
      }
    }
  
    if(delta > 0){
        // press the button for some small time
        keyState[ROTARY_KEY_IDX_A] = 1;
        delta--;
    }else if (delta < 0){
        // press the button for some small time
        keyState[ROTARY_KEY_IDX_B] = 1;
        delta++;
    }else{
        keyState[ROTARY_KEY_IDX_A] = 0;
        keyState[ROTARY_KEY_IDX_B] = 0;
    }
  }
#endif // whole file is only implemented #if ROTARY_AXIS > 0 or ROTARY_KEYS > 0
