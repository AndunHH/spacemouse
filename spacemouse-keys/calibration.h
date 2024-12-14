// Header for calibration specific functions and variables

void debugOutput1(int* rawReads, int* keyVals);
void debugOutput2(int* centered);
void debugOutput4(int16_t* velocity, uint8_t* keyOut);
void debugOutput5(int* centered, int16_t* velocity);

void printArray(int arr[], int size);
void calcMinMax(int* centered);

bool isDebugOutputDue();

void updateFrequencyReport();

bool busyZeroing(int *centerPoints, uint16_t numIterations, boolean debugFlag);

