// Header for parameter-menu specific functions and variables
#ifndef PARAMETERMENU_H
  #define PARAMETERMENU_H

  #include "config.h"

  //---------------------------------------------------------
  // to define a new parameter:
  // ---------------------------
  // 1. run the "old" system on the controller, login via Serial
  // 2. "list parameters as defines" and get them from terminal via COPY to clipboard
  // 3. PASTE the parameters in an editor to a text-file or into your new config.h (to use them as initial values)
  //
  // parameterMenu.h
  // 4. change the magicNumber to something new to invalidate the actual content of the EEPROM
  // 5. insert the new parameter into the struct ParamStorage
  // 6. increment the number of parameters in NUM_PARAMS
  // parameterMenu.cpp
  // 7. edit printOneParameter() to get an user-readable output on Serial
  // 8. edit readParameter() to read the parameter-value out of ParamStorage
  // 9. edit writeParameter() to write a value to the parameter in ParamStorage
  //
  // because all user-interface handles numbers, use int8_T for boolean
  // for numbers double, int, int8_t, int16_t, int32_t are OK
  //
  // 10. consider putting the values from 3. as initial values into config.h
  // 11. compile/download the new program
  // 12. check the parameters with "list parameters"
  // 13. modify the parameters as needed with "edit parameters"
  // 14. store the parameters to the EEPROM with "write to EEPROM"
  //---------------------------------------------------------

  #define MAGIC_NUMBER       1209196403L
  #define BASE_ADDRESS_MAGIC 0
  #define BASE_ADDRESS_PAR   4

  #if (ROTARY_AXIS > 0) && (ROTARY_AXIS < 7)
  #define NUM_PARAMS 31   // total number of parameters in struct ParamStorage
  #else
  #define NUM_PARAMS 29   // total number of parameters in struct ParamStorage
  #endif

  struct ParamStorage {
    int16_t deadzone               = DEADZONE;

    double  transX_sensitivity     = TRANSX_SENSITIVITY;
    double  transY_sensitivity     = TRANSY_SENSITIVITY;
    double  pos_transZ_sensitivity = POS_TRANSZ_SENSITIVITY;
    double  neg_transZ_sensitivity = NEG_TRANSZ_SENSITIVITY;
    double  gate_neg_transZ        = GATE_NEG_TRANSZ;
    int16_t gate_rotX              = GATE_ROTX;
    int16_t gate_rotY              = GATE_ROTY;
    int16_t gate_rotZ              = GATE_ROTZ;

    double  rotX_sensitivity       = ROTX_SENSITIVITY;
    double  rotY_sensitivity       = ROTY_SENSITIVITY;
    double  rotZ_sensitivity       = ROTZ_SENSITIVITY;

    int8_t  modFunc                = MODFUNC;
    double  slope_at_zero          = SLOPE_AT_ZERO;
    double  slope_at_end           = SLOPE_AT_END;

    int8_t  invX                   = INVX;
    int8_t  invY                   = INVY;
    int8_t  invZ                   = INVZ;
    int8_t  invRX                  = INVRX;
    int8_t  invRY                  = INVRY;
    int8_t  invRZ                  = INVRZ;

    int8_t  switchXY               = SWITCHXY;
    int8_t  switchYZ               = SWITCHYZ;
    int8_t  exclusiveMode          = EXCLUSIVEMODE;

    int8_t  compEnabled            = COMP_ENABLED;
    int16_t compNoOfPoints         = COMP_NO_OF_POINTS;
    int16_t compWaitTime           = COMP_WAIT_TIME;
    int16_t compMinMaxDiff         = COMP_MIN_MAX_DIFF;
    int16_t compCenterDiff         = COMP_CENTER_DIFF;
  };

  int    userInput(double& value);
  double readParameter(int i, ParamStorage& par);
  void   writeParameter(int i, double value, ParamStorage& par);
  void   getParametersFromEEPROM(ParamStorage& par);
  void   putParametersToEEPROM(ParamStorage& par);
  bool   printOneParameter(int i, ParamStorage& par, bool line, bool num);
  void   printAllParameters(ParamStorage& par, bool num);
  int    editParameters(ParamStorage& par);
  int    parameterMenu(ParamStorage& par);
#endif
