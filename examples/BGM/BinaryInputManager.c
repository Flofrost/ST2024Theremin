#include "BinaryInputManager.h"

void initBinaryInputController(BinaryInputController* controller, unsigned int timestamp, unsigned int debounceDelay){
    controller->currentState = 0;
    controller->_previousState = 0;
    controller->debounceDelay = debounceDelay;
    controller->_timestamp = timestamp;
}

unsigned char readState(BinaryInputController* controller, unsigned char state, unsigned int timestamp){
    if(timestamp - controller->_timestamp > controller->debounceDelay){
        controller->_timestamp = timestamp;
        controller->currentState = state;
        return state;
    }else return controller->currentState;
}

unsigned char onEdge(BinaryInputController* controller, unsigned char state, unsigned int timestamp){
    if((timestamp - controller->_timestamp > controller->debounceDelay) && (state != controller->_previousState)){
        controller->_previousState = state;
        controller->currentState = state;
        controller->_timestamp = timestamp;
        return 1;
    }
    return 0;
}

unsigned char onRisingEdge(BinaryInputController* controller, unsigned char state, unsigned int timestamp){
    if((timestamp - controller->_timestamp > controller->debounceDelay) && (state != controller->_previousState)){
        controller->_previousState = state;
        controller->currentState = state;
        controller->_timestamp = timestamp;
        if(state) return 1;
    }
    return 0;
}

unsigned char onFallingEdge(BinaryInputController* controller, unsigned char state, unsigned int timestamp){
    if((timestamp - controller->_timestamp > controller->debounceDelay) && (state != controller->_previousState)){
        controller->_previousState = state;
        controller->currentState = state;
        controller->_timestamp = timestamp;
        if(!state) return 1;
    }
    return 0;
}