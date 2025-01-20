// This is the public header for the kinematics.cpp file
// It contains all functions which can be called from the main application

int modifierFunction(int x);

void readAllFromJoystick(int *rawReads);

void FilterAnalogReadOuts(int* centered);

void calculateKinematic(int* centered, int16_t* velocity);

void switchXY(int16_t *velocity);
void switchYZ(int16_t *velocity);

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