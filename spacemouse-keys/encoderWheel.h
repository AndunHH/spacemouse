// Header file for the encoderWheel.cpp

void initEncoderWheel();
void calcEncoderWheel(int16_t* velocity, int debug);
void calcEncoderAsKey(uint8_t keyState[NUMKEYS], int debug);