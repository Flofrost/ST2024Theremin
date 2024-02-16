#ifndef BUTTON_DEBOUNCER_H
#define BUTTON_DEBOUNCER_H

// Structure that allows easy control of softwar debouncing and edge detection
// The debouncing can use any arbitrary time units: seconds, milliseconds, ticks, etc. Time will be reffered as TU (Time Units).
// This is why a timestamp needs to be provided for each operation.
// Mixing time units is possible, but strongly discouraged
typedef struct BinaryInputController{
    // Delay between each successful operations. (doing something before the delay allows it won't do anything.)
    unsigned int debounceDelay;
    unsigned int _timestamp;
    // Last recorded state of the input.
    unsigned char currentState;
    unsigned char _previousState;
}BinaryInputController;

// Innitiates the structure
//
// controller - Pointer to the controller structure
// timestamp - The "now" in TU
// debounceDelay - Delay in TU
void initBinaryInputController(BinaryInputController* controller, unsigned int timestamp, unsigned int debounceDelay);

// Returns state if the delay allows it, return the previously recorded state otherwise.
// Updates the logical state of the controller.
//
// controller - Pointer to the controller structure
// state - Reading from the physical binary event (eg: button, sensor, etc)
// timestamp - The "now" in TU
unsigned char readState(BinaryInputController* controller, unsigned char state, unsigned int timestamp);

// Returns true if there's a rising or falling edge, fasle otherwise.
// Updates the logical state of the controller.
//
// controller - Pointer to the controller structure
// state - Reading from the physical binary event (eg: button, sensor, etc)
// timestamp - The "now" in TU
unsigned char onEdge(BinaryInputController* controller, unsigned char state, unsigned int timestamp);

// Returns true if there's a rising edge.
// Updates the logical state of the controller.
//
// controller - Pointer to the controller structure
// state - Reading from the physical binary event (eg: button, sensor, etc)
// timestamp - The "now" in TU
unsigned char onRisingEdge(BinaryInputController* controller, unsigned char state, unsigned int timestamp);

// Returns true if there's a falling edge.
// Updates the logical state of the controller.
//
// controller - Pointer to the controller structure
// state - Reading from the physical binary event (eg: button, sensor, etc)
// timestamp - The "now" in TU
unsigned char onFallingEdge(BinaryInputController* controller, unsigned char state, unsigned int timestamp);

#endif