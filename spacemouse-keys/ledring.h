void initLEDring();

void processLED(int16_t *velocity);

void calcLEDstate(int16_t *velocity);

int8_t getMainVelocity(int16_t *velocity);

#define VelocityDeadzoneForLED 15