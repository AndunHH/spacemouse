// File for parameter-menu specific functions

#include <Arduino.h>
#include <EEPROM.h>
#include "parameterMenu.h"

/// @brief  Test for User-input on serial interface. If something is typed in, the input is checked for a (floating point-)number, 'q' or ESC.
/// @param  value: (output) number entered by user - not valid, if returned state <> 1, so check return first!!!
/// @return state of the user-input: 0=nothing typed in; 1=new value entered, see "value"; 2=aborted by pressing q or ESC; 3=input timed out; 4=undefined input
int userInput(double& value){ // returns: 0=nothing  1=new value  2=aborted  3=input timed out  4=undefined
  int state = 0;  // initial "nothing"

  if(Serial.available()){
    char next = toLowerCase(Serial.peek());
    if(isDigit(next) || next == '-'){                             // eine Ziffer oder Vorzeichen ist eingegeben:
      double v = Serial.parseFloat();                             //   Zahl übernehmen, Funktion ausführen
      if(Serial.available()){                                     //   ein Zeichen hat die Zahl begrenzt:
        char next = toLowerCase(Serial.read());                   //     Zeichen holen und prüfen:
        if(next == 'q' || next == 27)     {state = 2;}            //     'q' oder ESC       -> "aborted"
        else if(next ==  13 || next == 10){state = 1;}            //     CR und LF          -> "new value"
        else                              {state = 4;}            //     alles andere       -> "undefined"
      }else                               {state = 3;}            //   parseFloat()-Timeout -> "timed out"
      if(state == 1){value = v;}                                  //   bei gültiger Zahleneingabe: Zahlen-Wert übernehmen
    }
    else if(next == 'q' || next == 27){state = 2; Serial.read();} // 'q' oder ESC           -> "aborted"
    else if(next ==  13 || next == 10){state = 0; Serial.read();} // CR und LF ausblenden   -> "nothing"
    else                              {state = 4; Serial.read();} // alles andere           -> "undefined"
  }
  return state;
}

/// @brief  StateMachine to display the parameters-menu, do the user-interaction and show/edit/read/write the parameters
/// @param  par: struct of parameters used by the system at runtime
/// @return state of StateMachine: 0=parameter-menu is off; 1=writeMenu-text to serial; 2=getInput via serial from user; 3=do the requested work
int parameterMenu(ParamStorage& par) {
  /* this function builds the config-management menu */
  /* state: 0=off, 1=writeMenu, 2=getInput, 3=doWork */
  static int state      = 0;
  static int menuMode   = -1; // mode requested by user input (-1 = nothing)
  long       invalidNum = 0xFFFFFFFF;

  if(state == 0 || state == 1){ // 0 = "off", 1 = "writeMenu"
    Serial.print  (F("\r\nSpaceMouse FW")); Serial.print(F(FW_RELEASE)); Serial.println(F(" - Parameters"));
    Serial.println(F("ESC leave parameter-menu (ESC, Q)"));
    Serial.println(F("  1  list parameters"));
    Serial.println(F("  2  edit parameters"));
    Serial.println(F("  3  read from EEPROM"));
    Serial.println(F("  4  write to EEPROM"));
    Serial.println(F("  5  clear EEPROM to 0xFF"));
    Serial.println(F("  6  set EEPROM params invalid"));
    Serial.println(F("  7  list parameters as defines"));
    Serial.print  (F("param::"));
    menuMode  = -1; // nothing
    state     = 2;  // getInput
  }

  if(state == 2){ // 2 = "getInput"
    double num;
    int result = userInput(num);
    if(     result == 1               ){menuMode = (int)num;
                                        Serial.println(menuMode);
                                        state = 3;} // new value -> doWork
    else if(result == 2               ){state = 0;} // aborted -> off
    else if(result == 3 || result == 4){state = 1;} // timeout,undefined -> writeMenu
  }

  if(state == 3){ // 3 = "doWork"
    switch(menuMode){
      case 1: printAllParameters(par, true);
              state = 1;  // writeMenu
              break;

      case 2: if(editParameters(par) == 0){
                state = 1;  // writeMenu
              }
              break;

      case 3: Serial.println(F("reading paramerters from EEPROM"));
              getParametersFromEEPROM(par);
              state = 1;  // writeMenu
              break;

      case 4: Serial.println(F("writing paramerters to EEPROM"));
              putParametersToEEPROM(par);
              state = 1;  // writeMenu
              break;

      case 5: Serial.println(F("clearing EEPROM"));
              for(unsigned int i=0; i < EEPROM.length(); i++){
                EEPROM.update(i, 255);
              }
              state = 1;  // writeMenu
              break;

      case 6: Serial.println(F("setting EEPROM params invalid"));
              EEPROM.put(BASE_ADDRESS_MAGIC, invalidNum);
              state = 1;  // writeMenu
              break;

      case 7: for(int i=1; i<=NUM_PARAMS; i++){
                Serial.print("#define ");
                printOneParameter(i, par, true, false);
              }
              state = 1;  // writeMenu
              break;

      default:state = 1;  // writeMenu
    }
  }

  return state;
}

/// @brief  StateMachine to edit parameters: list all, let the user select one, let the user input a value, write to selected parameter
/// @param  par: struct of parameters used by the system at runtime
/// @return state of StateMachine: 0=edit is off; 1=show list on serial; 2=user-input index; 3=show old value; 4=user-input new value; 5=write new value to parameter
int editParameters(ParamStorage& par){
  static int    state    = 0;
  static bool   isFloat;
  static int    parIndex = 0;
  static double parValue = 0.0;
  int           result   = 0;

  if(state == 0){   // OFF
    parIndex = 0;
    parValue = 0.0;
    state    = 1;                                   // coming from OFF -> start over
  }

  if(state == 1){   // show list
    Serial.println();
    printAllParameters(par, true);
    Serial.println();
    Serial.println(F("enter number of parameter to edit (ESC, Q to leave)"));
    Serial.print  (F("edit::"));
    state = 2;
  }

  if(state == 2){   // user input: parameter index
    double num;
    result = userInput(num);
    if(      result == 1){parIndex = (int)num;
                          Serial.println(parIndex);
                          state = 3;  // new value -> edit selected
    }else if(result == 2){state = 0; // aborted -> end this menu
    }else if(result != 0){state = 1; // others  -> show menu
    }
  }

  if(state == 3){   // show actual parameter value
    if(parIndex >= 1 && parIndex <= NUM_PARAMS){
      isFloat = printOneParameter(parIndex, par, false, true);
      Serial.print(F(" -> "));
      state = 4;                                    // input parameter value
    }else{
      state = 1;                                    // invalid number -> show menu
    }
  }

  if(state == 4){   // user input: new parameter value
    result = userInput(parValue);
    if(      result == 1){state = 5;                      // new value -> edit selected
    }else if(result != 0){Serial.println(F("unchanged")); // others    -> abort input
                          state = 1;
    }
  }

  if(state == 5){   // write new parameter
    writeParameter(parIndex, parValue, par);
    if(isFloat){Serial.println(parValue);
    }else      {Serial.println((int)trunc(parValue));
    }
    state = 1;
  }

  return state;
}

/// @brief  gets all parameters from EEPROM, if the magic number in EEPROM is correct
/// @param  par: struct of parameters used by the system at runtime, read from EEPROM
void getParametersFromEEPROM(ParamStorage& par){
  long magicNumber = 0L;
  EEPROM.get(BASE_ADDRESS_MAGIC, magicNumber);
  if(magicNumber == MAGIC_NUMBER){
    EEPROM.get(BASE_ADDRESS_PAR, par);
  }else{
    Serial.println(F("wrong magic number, no parameters in EEPROM"));
  }
}

/// @brief  puts all parameters to EEPROM, sets the magic number in EEPROM
/// @param  par: struct of parameters used by the system at runtime, written to EEPROM
void putParametersToEEPROM(ParamStorage& par){
  long magicNumber = MAGIC_NUMBER;
  EEPROM.put(BASE_ADDRESS_PAR, par);
  EEPROM.put(BASE_ADDRESS_MAGIC, magicNumber);
}

/// @brief  prints all parameters as a list to Serial
/// @param  par: struct of parameters used by the system at runtime
/// @param  num: true=numbers the parameter-lines; false=no numbering
void printAllParameters(ParamStorage& par, bool num){
  for(int i=1; i<=NUM_PARAMS; i++){
    printOneParameter(i, par, true, num);
  }
}

/// @brief  prints one parameter with its name to Serial  >>when defining a new parameter, edit this function<<
/// @param  i:    index of the parameter to print
/// @param  par:  struct of parameters used by the system at runtime
/// @param  line: true=outputs lineend-chars at last; false=no lineend
/// @param  numbering:  true=numbers the lines using the index; false=no numbering
/// @return isFloat: true=selected parameter is a double; false=selected parameter is an integer
bool printOneParameter(int i, ParamStorage& par, bool line, bool numbering){
  bool isInt = true;

  if(i >= 1 && i <= NUM_PARAMS){
    if(numbering){if(i <= 9){Serial.print(" ");} Serial.print(i); Serial.print(" ");}
    switch(i){
      case  1: Serial.print(F("DEADZONE               ")); break;
      case  2: Serial.print(F("TRANSX_SENSITIVITY     ")); isInt = false; break;
      case  3: Serial.print(F("TRANSY_SENSITIVITY     ")); isInt = false; break;
      case  4: Serial.print(F("POS_TRANSZ_SENSITIVITY ")); isInt = false; break;
      case  5: Serial.print(F("NEG_TRANSZ_SENSITIVITY ")); isInt = false; break;
      case  6: Serial.print(F("GATE_NEG_TRANSZ        ")); isInt = false; break;
      case  7: Serial.print(F("GATE_ROTX              ")); break;
      case  8: Serial.print(F("GATE_ROTY              ")); break;
      case  9: Serial.print(F("GATE_ROTZ              ")); break;
      case 10: Serial.print(F("ROTX_SENSITIVITY       ")); isInt = false; break;
      case 11: Serial.print(F("ROTY_SENSITIVITY       ")); isInt = false; break;
      case 12: Serial.print(F("ROTZ_SENSITIVITY       ")); isInt = false; break;
      case 13: Serial.print(F("MODFUNC                ")); break;
      case 14: Serial.print(F("SLOPE_AT_ZERO          ")); isInt = false; break;
      case 15: Serial.print(F("SLOPE_AT_END           ")); isInt = false; break;
      case 16: Serial.print(F("INVX                   ")); break;
      case 17: Serial.print(F("INVY                   ")); break;
      case 18: Serial.print(F("INVZ                   ")); break;
      case 19: Serial.print(F("INVRX                  ")); break;
      case 20: Serial.print(F("INVRY                  ")); break;
      case 21: Serial.print(F("INVRZ                  ")); break;
      case 22: Serial.print(F("SWITCHXY               ")); break;
      case 23: Serial.print(F("SWITCHYZ               ")); break;
      case 24: Serial.print(F("EXCLUSIVEMODE          ")); break;
      case 25: Serial.print(F("PRIO_Z_EXCLUSIVEMODE   ")); break;
      case 26: Serial.print(F("COMP_ENABLED           ")); break;
      case 27: Serial.print(F("COMP_NO_OF_POINTS      ")); break;
      case 28: Serial.print(F("COMP_WAIT_TIME         ")); break;
      case 29: Serial.print(F("COMP_MIN_MAX_DIFF      ")); break;
      case 30: Serial.print(F("COMP_CENTER_DIFF       ")); break;
      case 31: Serial.print(F("ECHOES                 ")); break; // for ROTARY_AXIS 
      case 32: Serial.print(F("SIMSTRENGTH            ")); break; // for ROTARY_AXIS 
    }
    double value = readParameter(i, par);
    if(isInt){Serial.print((int)trunc(value));
    }else    {Serial.print(value);
    }
    if(line) {Serial.println();}
  }
  return isInt;
}

/// @brief  reads one parameter (selected by index i) out of parameter-set  >>when defining a new parameter, edit this function<<
/// @param  i:    index of the parameter to print
/// @param  par:  struct of parameters used by the system at runtime
/// @return value read from the selected parameter
double readParameter(int i, ParamStorage& par){
  double value = NAN;

  switch(i){
    case  1: value = par.deadzone;                break;
    case  2: value = par.transX_sensitivity;      break;
    case  3: value = par.transY_sensitivity;      break;
    case  4: value = par.pos_transZ_sensitivity;  break;
    case  5: value = par.neg_transZ_sensitivity;  break;
    case  6: value = par.gate_neg_transZ;         break;
    case  7: value = par.gate_rotX;               break;
    case  8: value = par.gate_rotY;               break;
    case  9: value = par.gate_rotZ;               break;
    case 10: value = par.rotX_sensitivity;        break;
    case 11: value = par.rotY_sensitivity;        break;
    case 12: value = par.rotZ_sensitivity;        break;
    case 13: value = par.modFunc;                 break;
    case 14: value = par.slope_at_zero;           break;
    case 15: value = par.slope_at_end;            break;
    case 16: value = par.invX;                    break;
    case 17: value = par.invY;                    break;
    case 18: value = par.invZ;                    break;
    case 19: value = par.invRX;                   break;
    case 20: value = par.invRY;                   break;
    case 21: value = par.invRZ;                   break;
    case 22: value = par.switchXY;                break;
    case 23: value = par.switchYZ;                break;
    case 24: value = par.exclusiveMode;           break;
    case 25: value = par.prioZexclusiveMode;      break;
    case 26: value = par.compEnabled;             break;
    case 27: value = par.compNoOfPoints;          break;
    case 28: value = par.compWaitTime;            break;
    case 29: value = par.compMinMaxDiff;          break;
    case 30: value = par.compCenterDiff;          break;
    case 31: value = par.rotAxisEchos;            break; // for ROTARY_AXIS 
    case 32: value = par.rotAxisSimStrength;      break; // for ROTARY_AXIS 
  }
  return value;
}

/// @brief  writes one parameter (selected by index i) to the parameter-set  >>when defining a new parameter, edit this function<<
/// @param  i:     index of the parameter to print
/// @param  value: value to write into the selected parameter
/// @param  par:   struct of parameters used by the system at runtime
void writeParameter(int i, double value, ParamStorage& par){
  switch(i){
    case  1: par.deadzone                = value; break;
    case  2: par.transX_sensitivity      = value; break;
    case  3: par.transY_sensitivity      = value; break;
    case  4: par.pos_transZ_sensitivity  = value; break;
    case  5: par.neg_transZ_sensitivity  = value; break;
    case  6: par.gate_neg_transZ         = value; break;
    case  7: par.gate_rotX               = value; break;
    case  8: par.gate_rotY               = value; break;
    case  9: par.gate_rotZ               = value; break;
    case 10: par.rotX_sensitivity        = value; break;
    case 11: par.rotY_sensitivity        = value; break;
    case 12: par.rotZ_sensitivity        = value; break;
    case 13: par.modFunc                 = value; break;
    case 14: par.slope_at_zero           = value; break;
    case 15: par.slope_at_end            = value; break;
    case 16: par.invX                    = value; break;
    case 17: par.invY                    = value; break;
    case 18: par.invZ                    = value; break;
    case 19: par.invRX                   = value; break;
    case 20: par.invRY                   = value; break;
    case 21: par.invRZ                   = value; break;
    case 22: par.switchXY                = value; break;
    case 23: par.switchYZ                = value; break;
    case 24: par.exclusiveMode           = value; break;
    case 25: par.prioZexclusiveMode      = value; break;
    case 26: par.compEnabled             = value; break;
    case 27: par.compNoOfPoints          = value; break;
    case 28: par.compWaitTime            = value; break;
    case 29: par.compMinMaxDiff          = value; break;
    case 30: par.compCenterDiff          = value; break;
    case 31: par.rotAxisEchos            = value; break;
    case 32: par.rotAxisSimStrength      = value; break;
  }
}
