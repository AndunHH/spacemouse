// test file for ergonomouse: resitive joystick. params in eeprom. modifier function, led ring
// support, keys and encoder

#ifndef CONFIG_h
#define CONFIG_h

#include "release.h"

#define PARAM_IN_EEPROM 1

#define STARTDEBUG 0
#undef HALLEFFECT

#define PINLIST {A0, A1, A2, A3, A6, A7, A8, A9}

#define INVERTLIST {0, 0, 0, 0, 0, 0, 0, 0}

#define DEADZONE 15

#define MINVALS {-400, -400, -400, -400, -400, -400, -400, -400}
#define MAXVALS {+175, +175, +175, +175, +175, +175, +175, +175}

#define SENS_TX 0.80
#define SENS_TY 0.99
#define SENS_PTZ 2.5
#define SENS_NTZ 1.5
#define GATE_NTZ 15
#define GATE_RX 15
#define GATE_RY 15
#define GATE_RZ 15
#define SENS_RX 1.2
#define SENS_RY 1.2
#define SENS_RZ 0.90

#define MODFUNC 3
#define MOD_A 1.15
#define MOD_B 1.15

#define INVX 0
#define INVY 1
#define INVZ 1
#define INVRX 0
#define INVRY 1
#define INVRZ 1

#define SWITCHYZ 0
#define SWITCHXY 0

#define COMP_EN 1
#define COMP_NR 50
#define COMP_WAIT 200
#define COMP_MDIFF 4
#define COMP_CDIFF 50

#define EXCLUSIVE 1
#define EXCL_HYST 5
#define EXCL_PRIOZ 1

#define NUMKEYS 7
#define KEYLIST {16, 14, 15, 2, 10, 7, 5}

#define NUMHIDKEYS 5

#define SM_MENU 0
#define SM_FIT 1
#define SM_T 2
#define SM_R 4
#define SM_F 5
#define SM_RCW 8
#define SM_1 12
#define SM_2 13
#define SM_3 14
#define SM_4 15
#define SM_ESC 22
#define SM_ALT 23
#define SM_SHFT 24
#define SM_CTRL 25
#define SM_ROT 26

#define BUTTONLIST {SM_T, SM_R, SM_F, SM_T, SM_R}

#define NUMKILLKEYS 2
#define KILLROT 2
#define KILLTRANS 3

#define DEBOUNCE_KEYS_MS 200

#define ENCODER_CLK 0
#define ENCODER_DT 1

#define ROTARY_AXIS 5
#define RAXIS_ECH 200
#define RAXIS_STR 200

#define ROTARY_KEYS 0
#define ROTARY_KEY_IDX_A 2
#define ROTARY_KEY_IDX_B 3
#define ROTARY_KEY_STRENGTH 19

#define DEBUGDELAY 100
#define DEBUG_LINE_END "\r"

#define VelocityDeadzoneForLED 15
#define LEDpin 3
#define LEDRING 24

#define LEDclockOffset 2
#define LEDUPDATERATE_MS 150

#define HIDMAXBUTTONS 32

#endif // CONFIG_h
