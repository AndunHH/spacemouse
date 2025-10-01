# Build Report

This document summarizes the build results for all tested configurations.  
Each configuration corresponds to a different `config.h` variant given in folder `testConfig`.

The first line of each configuration file is used as its description.

This file is created manually be calling `testConfigCompileSize.py`.

| Config | Description | Flash (bytes) | Flash (%) | RAM (bytes) | RAM (%) | Build Success |
|--------|-------------|---------------|-----------|-------------|---------|---------------|
| a_test_minimal.h | test file for resistive joystick with no added features | 17424 | 60.8 | 1191 | 46.5 | Yes |
| b_test_resistiveJoystick.h | test file for resistive joystick with modification function enabled | 17424 | 60.8 | 1191 | 46.5 | Yes |
| c1_test_LED.h | test file for resistive joystick with LED support | 17830 | 62.2 | 1191 | 46.5 | Yes |
| c2_test_LEDring.h | test file for resistive joystick with led ring support | 20958 | 73.1 | 1399 | 54.6 | Yes |
| d2_test_encoder_key.h | test file for resistive joystick with encoder and key support (one key is replaced by encoder) | 21050 | 73.4 | 1354 | 52.9 | Yes |
| d_test_encoder.h | test file for resistive joystick with encoder added. | 19532 | 68.1 | 1265 | 49.4 | Yes |
| e2_ergoMouse_progmode.h | test file for ergonomouse: resitive joystick. params in eeprom. modifier function, led ring | 28486 | 99.4 | 1644 | 64.2 | Yes |
| e_test_ergoMouse.h | test file for ergonomouse: resitive joystick. params in eeprom. modifier function, led ring support, keys and encoder | 27346 | 95.4 | 1633 | 63.8 | Yes |
| f_test_hall_effect.h | test file for hall effect joy sticks with nothing else | 17644 | 61.5 | 1193 | 46.6 | Yes |
| g_paramEeprom.h | Test PARAM_IN_EEPROM + PROGMODE | 21660 | 75.5 | 1241 | 48.5 | Yes |
| Summary | [OK] All builds successful. |  | Max: 99.4 |  | Max: 64.2 | [OK] |

**Report generated on:** 2025-10-01 21:28:06
