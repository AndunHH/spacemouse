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
  // spacemouse-keys.ino
  // 7. insert a line into the initialization of par (at the right position!!!)
  //    example:
  //    {PARAM_TYPE_FLOAT, "TEST",          &parStorage.test      }, //      34
  //     ^type of param     ^name of param   ^pointer to the variable        ^number as comment
  //
  // because all user-interface handles numbers and the type for the variables is forced now:
  //   use int8_T  for PARAM_TYPE_BOOL  [0 , 1]
  //   use int16_t for PARAM_TYPE_INT   [-9999 .. 9999]
  //   use double  for PARAM_TYPE_FLOAT [-xx.yyy .. 0.000 .. xx.yyy]
  //
  // 8. consider putting the values from 3. as initial values into config.h
  // 9. compile/download the new program
  // 10. check the parameters with "list parameters"
  // 11. modify the parameters as needed with "edit parameters"
  // 12. store the parameters to the EEPROM with "write to EEPROM"
  //---------------------------------------------------------

  #define NUM_PARAMS         33   // total number of parameters in struct ParamStorage

  #define MAX_PARAM_NAME_LEN 24   // maximum length of any parameter name

  #define MAGIC_NUMBER       1209196405L
  #define BASE_ADDRESS_MAGIC 0
  #define BASE_ADDRESS_PAR   4

  #define PARAM_TYPE_BOOL    1
  #define PARAM_TYPE_INT     2
  #define PARAM_TYPE_FLOAT   3

  typedef struct _ParamStorage {
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

    int16_t modFunc                = MODFUNC;         //SNo: changed to int16_t because all INT-values should be int16
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
    int16_t exclusiveHysteresis    = EXCLUSIVEHYSTERESIS;
    int8_t  prioZexclusiveMode     = PRIO_Z_EXCLUSIVEMODE;

    int8_t  compEnabled            = COMP_ENABLED;
    int16_t compNoOfPoints         = COMP_NO_OF_POINTS;
    int16_t compWaitTime           = COMP_WAIT_TIME;
    int16_t compMinMaxDiff         = COMP_MIN_MAX_DIFF;
    int16_t compCenterDiff         = COMP_CENTER_DIFF;

    int16_t rotAxisEchos           = ECHOES;
    int16_t rotAxisSimStrength     = SIMSTRENGTH;    
  } ParamStorage;

  typedef struct _ParamDescription {
    int   type;
    char  name[MAX_PARAM_NAME_LEN+1];
    void* storage;
  } ParamDescription;

  typedef struct _ParamData {
    ParamStorage*     values;
    ParamDescription  description[NUM_PARAMS+1];
  } ParamData;

  #if ENABLE_PROGMODE > 0
    typedef struct _ProgCmd {
      char    cmd;
      double  value;
      double  retval;
      int16_t paramNo;
    } ProgCmd;

    #define PE_OK            10000
    #define PE_INVALID_PARAM 10001
    #define PE_INVALID_VALUE 10002
    #define PE_VALUE_FAULT   10003
    #define PE_CMD_FAULT     10004
  #endif

  int    userInput(double& value);
  double readParameter(int i, ParamData& par);
  void   writeParameter(int i, double value, ParamData& par);
  void   getParametersFromEEPROM(ParamData& par);
  void   putParametersToEEPROM(ParamData& par);
  void   printParameterName(int i, ParamData& par, bool formatted);
  bool   printOneParameter(int i, ParamData& par, bool line, bool num);
  void   printAllParameters(ParamData& par, bool num);
  int    editParameters(ParamData& par);
  int    parameterMenu(ParamData& par);
  #if ENABLE_PROGMODE > 0
    void executeProgCommand(ParamData& par);
  #endif
#endif
