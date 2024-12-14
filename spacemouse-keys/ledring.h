// This is the public header for the ledring.cpp file
// It contains all functions which can be called from the main application

void initLEDring();

void processLED(int16_t *velocity, boolean ledCmd);

int8_t getMainVelocity(int16_t *velocity);


