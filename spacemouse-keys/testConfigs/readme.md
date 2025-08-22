# TestConfigs

This folder contains config files, that can be used to test different combinations of firmware configurations. 

This is became important because especially the LED library is using much flash. Therefore before releasing a new version, the content of the config files here shall be copied into the config.h and shall be tested if they compile.

- [testConfig_hallEffect-allFeauteres.h](testConfig_hallEffect-allFeauteres.h): Worst case with all features enabled, Hall-Effect Magnets 
- [testConfig_joyStick-keys-LEDRing.h](testConfig_joyStick-keys-LEDRing.h): Joystick with keys and all features enabled.