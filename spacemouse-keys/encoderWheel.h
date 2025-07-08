// Header file for the encoderWheel.cpp

#include "parameterMenu.h"

void initEncoderWheel();
void calcEncoderWheel(int16_t* velocity, int debug, ParamStorage& par);
void calcEncoderAsKey(uint8_t keyState[NUMKEYS], int debug);