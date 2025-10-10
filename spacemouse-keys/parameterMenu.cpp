// File for parameter-menu specific functions

#include <Arduino.h>
#include <EEPROM.h>
#include "parameterMenu.h"

/* possible commands in ProgMode:

  Cmd    |function                       |returns (>= 10000 -> NOK)
  -------|-------------------------------|----------------------------------------------------------------------------
  >p...   parameter number                <p...   (PE_OK,PE_INVALID_PARAM)
  >r      read value                      <r...   (<value> or PE_INVALID_PARAM
  >w...   write value                     <w...   (PE_OK,PE_INVALID_PARAM,PE_INVALID_VALUE "not in
  [-10000..+10000]") >l      load params from EEPROM         <l0     (PE_OK) >s      save params to
  EEPROM           <s0     (PE_OK) >c      clear EEPROM                    <c0     (PE_OK) >m get
  magic number                <m...   (<magic number>: all values are valid, no fault-codes!) >n get
  number of parameters        <n...   (<number of params>) >i      invalidate magic number <i0
  (PE_OK) >t      get type of parameter           <t...   (<type>: 0=bool,1=int,2=float or
  PE_INVALID_PARAM) >d      get description of parameter    <d...   (<name of parameter> or
  PE_INVALID_PARAM)
*/

#if ENABLE_PROGMODE > 0
ProgCmd prog;
#endif
long invalidNum = 0xFFFFFFFF;

/// @brief  Test for User-input on serial interface. If something is typed in, the input is checked
/// for a (floating point-)number, 'q' or ESC.
/// @param  value    (output) number entered by user - not valid, if returned state <> 1, so check
/// return first!!!
/// @return state of the user-input: 0=nothing typed in; 1=new value entered, see "value"; 2=aborted
/// by pressing q or ESC; 3=input timed out; 4=undefined input; 10=received prog-command
int userInput(
    double &value) { // returns: 0=nothing  1=new value  2=aborted  3=input timed out  4=undefined
  int state = 0;     // initial "nothing"

  bool progRuns = false; // progMode not running
#if ENABLE_PROGMODE > 0
  bool progMode = false; // not in prog-mode
  bool cmdDone = false;  // cmd not read
  bool valDone = false;  // val not to be read next
  bool crlfDone = false; // CR/LF not read

  prog.cmd = '?';
  prog.value = 0;
  prog.retval = PE_OK;
#endif

  do {
    if (Serial.available()) {
      state = 0;
      char next = toLowerCase(Serial.peek());
      if (isDigit(next) || next == '-') {         // A number or sign is entered:
        double v = Serial.parseFloat();           //   take that float
        if (Serial.available()) {                 //   check what the next letter is
          char next = toLowerCase(Serial.read()); //     get it and check:
          if (next == 'q' || next == 27) {
            state = 2;
          } //     'q' or ESC       -> "aborted"
          else if (next == 13 || next == 10) {
            state = 1;
          } //     CR + LF          -> "new value"
          else {
            state = 4;
          } //     everything else  -> "undefined"
        } else {
          state = 3;
        } //   parseFloat()-Timeout -> "timed out"
        if (state == 1) {
          value = v;
        } //   if valid float: use new value
      }
#if ENABLE_PROGMODE > 0
      else if (next == '>') {
        progMode = true;
        progRuns = true;
        Serial.read();
      } // '>' begins progmode (inits value to 0)
      else if (progMode && !cmdDone && next == 'p') {
        cmdDone = true;
        prog.cmd = next;
        Serial.read();
      } //   'p' set parameter-number
      else if (progMode && !cmdDone && next == 't') {
        cmdDone = true;
        valDone = true;
        prog.cmd = next;
        Serial.read();
      } //   't' get parameter-type
      else if (progMode && !cmdDone && next == 'd') {
        cmdDone = true;
        valDone = true;
        prog.cmd = next;
        Serial.read();
      } //   'd' get parameter-description
      else if (progMode && !cmdDone && next == 'r') {
        cmdDone = true;
        valDone = true;
        prog.cmd = next;
        Serial.read();
      } //   'r' read parameter-value
      else if (progMode && !cmdDone && next == 'w') {
        cmdDone = true;
        prog.cmd = next;
        Serial.read();
      } //   'w' write parameter-value
      else if (progMode && !cmdDone && next == 'l') {
        cmdDone = true;
        valDone = true;
        prog.cmd = next;
        Serial.read();
      } //   'l' load params from EEPRROM
      else if (progMode && !cmdDone && next == 's') {
        cmdDone = true;
        valDone = true;
        prog.cmd = next;
        Serial.read();
      } //   's' save params to EEPROM
      else if (progMode && !cmdDone && next == 'c') {
        cmdDone = true;
        valDone = true;
        prog.cmd = next;
        Serial.read();
      } //   'c' clear EEPROM
      else if (progMode && !cmdDone && next == 'm') {
        cmdDone = true;
        valDone = true;
        prog.cmd = next;
        Serial.read();
      } //   'm' get magic number
      else if (progMode && !cmdDone && next == 'n') {
        cmdDone = true;
        valDone = true;
        prog.cmd = next;
        Serial.read();
      } //   'n' get number of parameters
      else if (progMode && !cmdDone && next == 'i') {
        cmdDone = true;
        valDone = true;
        prog.cmd = next;
        Serial.read();
      } //   'i' invalidate magic-number
#endif
      else if (next == 'q' || next == 27) {
        state = 2;
        Serial.read();
      } // 'q' oder ESC           -> "aborted"
      else if (next == 13 || next == 10) {
        state = 5;
        Serial.read();
      } // CR und LF ausblenden   -> "nothing"
      else {
        state = 4;
        Serial.read();
      } // alles andere           -> "undefined"

#if ENABLE_PROGMODE > 0
      //--- progMode: handle values and CR/LF, test for complete telegram
      if (progMode) {
        if ((state == 1) && valDone) {
          valDone = false;
          crlfDone = true;
        } // value+CR/LF received but not expected
        else if ((state == 1) && !valDone) {
          prog.value = value;
          valDone = true;
          crlfDone = true;
        } // value+CR/LF received as expected
        else if (state == 5) {
          crlfDone = true;
        } // CR/LF received
        else if (state == 4) {
          cmdDone = false;
        } // invalidate cmd-signal to get cmd-fault

        // test received telegram and end if done or telegram-format is wrong
        if (cmdDone && valDone && crlfDone) {
          progRuns = false;
        } // telegram OK (cmd+value+CR/LF or cmd+CR/LF)
        else if (cmdDone && !valDone && crlfDone) {
          prog.retval = PE_VALUE_FAULT;
          progRuns = false;
        } // no value / superflux value
        else if (!cmdDone && crlfDone) {
          prog.retval = PE_CMD_FAULT;
          progRuns = false;
        } // no cmd

        state = 10; // received prog-command
      }
#endif
      if (state == 5) {
        state = 0;
      } // for the parameter-menu a CR/LF is interpreted as "nothing"
    }
  } while (progRuns);

  return state;
}

#if ENABLE_PROGMODE > 0
/// @brief  executes a program-command which is stored in the global variable "prog" by userInput()
/// @param  nothing
void executeProgCommand(ParamData &par) {
  long m = 0L;
  bool intVal = true;

  if (prog.retval == PE_OK) {
    if (prog.cmd == 'p') {
      if (prog.value < 1 || prog.value > NUM_PARAMS) {
        prog.retval = PE_INVALID_PARAM;
      } else {
        prog.paramNo = prog.value;
      }
    }

    else if (prog.cmd == 't') {
      prog.retval = par.description[prog.paramNo].type;
    }

    else if (prog.cmd == 'd') {
      if (prog.paramNo < 1 || prog.paramNo > NUM_PARAMS) {
        prog.retval = PE_INVALID_PARAM;
      } else {
        Serial.print(F("<d"));
        printParameterName(prog.paramNo, par, false);
        Serial.println();
        return;
      }
    }

    else if (prog.cmd == 'r') {
      if (prog.paramNo < 1 || prog.paramNo > NUM_PARAMS) {
        prog.retval = PE_INVALID_PARAM;
      } else {
        prog.retval = readParameter(prog.paramNo, par);
        intVal = (par.description[prog.paramNo].type != PARAM_TYPE_FLOAT);
      }
    }

    else if (prog.cmd == 'w') {
      if (prog.paramNo < 1 || prog.paramNo > NUM_PARAMS) {
        prog.retval = PE_INVALID_VALUE;
      } else if (prog.value < -10000.0 || prog.value > +10000.0) {
        prog.retval = PE_INVALID_PARAM;
      } else {
        writeParameter(prog.paramNo, prog.value, par);
      }
    }

    else if (prog.cmd == 'l') {
      getParametersFromEEPROM(par);
    }

    else if (prog.cmd == 's') {
      putParametersToEEPROM(par);
    }

    else if (prog.cmd == 'c') {
      for (unsigned int i = 0; i < EEPROM.length(); i++) {
        EEPROM.update(i, 255);
      }
    }

    else if (prog.cmd == 'm') {
      EEPROM.get(BASE_ADDRESS_MAGIC, m);
      Serial.print(F("<m"));
      Serial.println(m);
      return;
    }

    else if (prog.cmd == 'n') {
      prog.retval = NUM_PARAMS;
    }

    else if (prog.cmd == 'i') {
      EEPROM.put(BASE_ADDRESS_MAGIC, invalidNum);
    }
  }

  Serial.print(F("<"));
  Serial.print(prog.cmd);
  if (intVal) {
    Serial.println((int)prog.retval);
  } else {
    Serial.println(prog.retval, 3);
  }
}
#endif

/// @brief  StateMachine to display the parameters-menu, do the user-interaction and
/// show/edit/read/write the parameters
/// @param  par        struct of parameters used by the system at runtime
/// @return state      of StateMachine: 0=parameter-menu is off; 1=writeMenu-text to serial;
/// 2=getInput via serial from user; 3=do the requested work
int parameterMenu(ParamData &par) {
  /* this function builds the config-management menu */
  /* state: 0=off, 1=writeMenu, 2=getInput, 3=doWork */
  static int state = 0;
  static int menuMode = -1; // mode requested by user input (-1 = nothing)

  if (state == 0 || state == 1) { // 0 = "off", 1 = "writeMenu"
    Serial.print(F("\r\nSpaceMouse FW"));
    Serial.print(F(FW_RELEASE));
    Serial.println(F(" - Parameters"));
    Serial.println(F("ESC leave parameter-menu (ESC, Q)"));
    Serial.println(F("  1  list parameters"));
    Serial.println(F("  2  edit parameters"));
    Serial.println(F("  3  load from EEPROM"));
    Serial.println(F("  4  save to EEPROM"));
    Serial.println(F("  5  clear EEPROM to 0xFF"));
    Serial.println(F("  6  set EEPROM params invalid"));
    Serial.println(F("  7  list parameters as defines"));
    Serial.print(F("param::"));
    menuMode = -1; // nothing
    state = 2;     // getInput
  }

  if (state == 2) { // 2 = "getInput"
    double num;
    int result = userInput(num);
    if (result == 1) {
      menuMode = (int)num;
      Serial.println(menuMode);
      state = 3;
    } // new value -> doWork
    else if (result == 2) {
      state = 0;
    } // aborted -> off
    else if (result == 3 || result == 4) {
      state = 1;
    } // timeout,undefined -> writeMenu
  }

  if (state == 3) { // 3 = "doWork"
    switch (menuMode) {
    case 1:
      printAllParameters(par, true);
      state = 1; // writeMenu
      break;

    case 2:
      if (editParameters(par) == 0) {
        state = 1; // writeMenu
      }
      break;

    case 3:
      Serial.println(F("loading parameters from EEPROM"));
      getParametersFromEEPROM(par);
      state = 1; // writeMenu
      break;

    case 4:
      Serial.println(F("saving parameters to EEPROM"));
      putParametersToEEPROM(par);
      state = 1; // writeMenu
      break;

    case 5:
      Serial.println(F("clearing EEPROM"));
      for (unsigned int i = 0; i < EEPROM.length(); i++) {
        EEPROM.update(i, 255);
      }
      state = 1; // writeMenu
      break;

    case 6:
      Serial.println(F("setting EEPROM params invalid"));
      EEPROM.put(BASE_ADDRESS_MAGIC, invalidNum);
      state = 1; // writeMenu
      break;

    case 7:
      for (int i = 1; i <= NUM_PARAMS; i++) {
        Serial.print("#define ");
        printOneParameter(i, par, true, false);
      }
      state = 1; // writeMenu
      break;

    default:
      state = 1; // writeMenu
    }
  }

  return state;
}

/// @brief  StateMachine to edit parameters: list all, let the user select one, let the user input a
/// value, write to selected parameter
/// @param  par        struct of parameters used by the system at runtime
/// @return state      of StateMachine: 0=edit is off; 1=show list on serial; 2=user-input index;
/// 3=show old value; 4=user-input new value; 5=write new value to parameter
int editParameters(ParamData &par) {
  static int state = 0;
  static bool isFloat;
  static int parIndex = 0;
  static double parValue = 0.0;
  int result = 0;

  if (state == 0) { // OFF
    parIndex = 0;
    parValue = 0.0;
    state = 1; // coming from OFF -> start over
  }

  if (state == 1) { // show list
    Serial.println();
    printAllParameters(par, true);
    Serial.println();
    Serial.println(F("enter number of parameter to edit (ESC, Q to leave)"));
    Serial.print(F("edit::"));
    state = 2;
  }

  if (state == 2) { // user input: parameter index
    double num;
    result = userInput(num);
    if (result == 1) {
      parIndex = (int)num;
      Serial.println(parIndex);
      state = 3; // new value -> edit selected
    } else if (result == 2) {
      state = 0; // aborted -> end this menu
    } else if (result != 0) {
      state = 1; // others  -> show menu
    }
  }

  if (state == 3) { // show actual parameter value
    if (parIndex >= 1 && parIndex <= NUM_PARAMS) {
      isFloat = printOneParameter(parIndex, par, false, true);
      Serial.print(F(" -> "));
      state = 4; // input parameter value
    } else {
      state = 1; // invalid number -> show menu
    }
  }

  if (state == 4) { // user input: new parameter value
    result = userInput(parValue);
    if (result == 1) {
      state = 5; // new value -> edit selected
    } else if (result != 0) {
      Serial.println(F("unchanged")); // others    -> abort input
      state = 1;
    }
  }

  if (state == 5) { // write new parameter
    writeParameter(parIndex, parValue, par);
    if (isFloat) {
      Serial.println(parValue);
    } else {
      Serial.println((int)trunc(parValue));
    }
    state = 1;
  }

  return state;
}

/// @brief  gets all parameters from EEPROM, if the magic number in EEPROM is correct
/// @param  par        struct of parameters used by the system at runtime, read from EEPROM
void getParametersFromEEPROM(ParamData &par) {
  long magicNumber = 0L;
  EEPROM.get(BASE_ADDRESS_MAGIC, magicNumber);
  if (magicNumber == MAGIC_NUMBER) {
    EEPROM.get(BASE_ADDRESS_PAR, *par.values);
  } else {
    Serial.println(F("Wrong magic!")); // No params in EEPROM are assumed
  }
}

/// @brief  puts all parameters to EEPROM, sets the magic number in EEPROM
/// @param  par        struct of parameters used by the system at runtime, written to EEPROM
void putParametersToEEPROM(ParamData &par) {
  long magicNumber = MAGIC_NUMBER;
  EEPROM.put(BASE_ADDRESS_PAR, *par.values);
  EEPROM.put(BASE_ADDRESS_MAGIC, magicNumber);
}

/// @brief  prints parameter name of parameter requested by index i. Prints unformatted or
/// left-aligned  >>when defining a new parameter, edit this function<<
/// @param  i          index of the parameter to print
/// @param  formatted  true=print name left aligned, false=print only name
/// @return nothing
void printParameterName(int i, ParamData &par, bool formatted) {

  Serial.print(par.description[i].name);

  if (formatted) {
    int c = MAX_PARAM_NAME_LEN - strlen(par.description[i].name);
    char spc[MAX_PARAM_NAME_LEN + 1];

    for (int n = 0; n < c; n++) {
      spc[n] = ' ';
    }
    spc[c] = '\0';
    Serial.print(spc);
  }
}

/// @brief  prints all parameters as a list to Serial
/// @param  par        struct of parameters used by the system at runtime
/// @param  num        true=numbers the parameter-lines; false=no numbering
void printAllParameters(ParamData &par, bool num) {
  for (int i = 1; i <= NUM_PARAMS; i++) {
    printOneParameter(i, par, true, num);
  }
}

/// @brief  prints one parameter with its name to Serial
/// @param  i          index of the parameter to print
/// @param  par        struct of parameters used by the system at runtime
/// @param  line       true=outputs lineend-chars at last; false=no lineend
/// @param  numbering  true=numbers the lines using the index; false=no numbering
/// @return isFloat    true=selected parameter is a double; false=selected parameter is an integer
bool printOneParameter(int i, ParamData &par, bool line, bool numbering) {
  bool isFloat = false;

  if (i >= 1 && i <= NUM_PARAMS) {
    isFloat = (par.description[i].type == PARAM_TYPE_FLOAT);

    if (numbering) {
      if (i <= 9) {
        Serial.print(" ");
      }
      Serial.print(i);
      Serial.print(" ");
    }
    printParameterName(i, par, true);
    Serial.print(" ");
    double value = readParameter(i, par);
    if (isFloat) {
      Serial.print(value);
    } else {
      Serial.print((int)trunc(value));
    }
    if (line) {
      Serial.println();
    }
  }
  return isFloat;
}

/// @brief  reads one parameter (selected by index i) out of parameter-set  >>when defining a new
/// parameter, edit this function<<
/// @param  i         index of the parameter to print
/// @param  par       struct of parameters used by the system at runtime
/// @return value     read from the selected parameter
double readParameter(int i, ParamData &par) {
  double value = NAN;

  if (i >= 1 && i <= NUM_PARAMS) {
    switch (par.description[i].type) {
    case PARAM_TYPE_BOOL:
      value = *(int8_t *)par.description[i].storage;
      break;
    case PARAM_TYPE_INT:
      value = *(int16_t *)par.description[i].storage;
      break;
    case PARAM_TYPE_FLOAT:
      value = *(double *)par.description[i].storage;
      break;
    }
  }
  return value;
}

/// @brief  writes one parameter (selected by index i) to the parameter-set  >>when defining a new
/// parameter, edit this function<<
/// @param  i         index of the parameter to print
/// @param  value     value to write into the selected parameter
/// @param  par       struct of parameters used by the system at runtime
void writeParameter(int i, double value, ParamData &par) {
  if (i >= 1 && i <= NUM_PARAMS) {
    switch (par.description[i].type) {
    case PARAM_TYPE_BOOL:
      *((int8_t *)par.description[i].storage) = (int8_t)trunc(value);
      break;
    case PARAM_TYPE_INT:
      *((int16_t *)par.description[i].storage) = (int16_t)trunc(value);
      break;
    case PARAM_TYPE_FLOAT:
      *((double *)par.description[i].storage) = value;
      break;
    }
  }
}
