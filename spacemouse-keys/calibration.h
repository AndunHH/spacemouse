// Header for calibration specific functions and variables

void debugOutput1(int* rawReads, int* keyVals);
void debugOutput2(int* centered);
void debugOutput4(int16_t* velocity, uint8_t* keyOut);
void debugOutput5(int* centered, int16_t* velocity);

void printArray(int arr[], int size);
void calcMinMax(int* centered);

// The following constants are here for more readable access to the arrays. You don't need to change this values!
// Axes in centered or rawValues array
#define AX 0
#define AY 1
#define BX 2
#define BY 3
#define CX 4
#define CY 5
#define DX 6
#define DY 7

// Define position in velocity array.
#define TRANSX 0
#define TRANSY 1
#define TRANSZ 2
#define ROTX 3
#define ROTY 4
#define ROTZ 5

// call this function to find out, if a new debug output shall be generated
// this is used in order to generate a debug line only every DEBUGDELAY ms
#define DEBUGDELAY 100
bool isDebugOutputDue();

// update and report the function to learn at what frequency the loop is running
void updateFrequencyReport();

// this function zeros the space mouse
// It's blocking other functions in the meantime
// set debugFlag=true for result on the serial console
boolean busyZeroing(int *centerPoints, boolean debugFlag);

// Function to read and store analogue voltages for each joystick axis.
void readAllFromJoystick(int *rawReads);