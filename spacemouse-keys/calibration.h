// Header for calibration specific functions and variables

void debugOutput1(int* rawReads, int* keyVals);
void debugOutput2(int* centered);
void debugOutput4(int16_t* velocity, uint8_t* keyOut);
void debugOutput5(int* centered, int16_t* velocity);

void printArray(int arr[], int size);
void calcMinMax(int* centered);



// call this function to find out, if a new debug output shall be generated
// this is used in order to generate a debug line only every DEBUGDELAY ms
#define DEBUGDELAY 100
bool isDebugOutputDue();

// update and report the function to learn at what frequency the loop is running
void updateFrequencyReport();

// Calibrate (=zero) the space mouse
bool busyZeroing(int *centerPoints, uint16_t numIterations, boolean debugFlag);

