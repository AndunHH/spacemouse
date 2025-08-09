// Header file for the encoderWheel.cpp

#include "parameterMenu.h"

void initEncoderWheel();
void calcEncoderWheel(int16_t* velocity, bool debugOut, ParamData& par);
void calcEncoderAsKey(uint8_t keyState[NUMKEYS], bool debugOut);
