#include "../../include/errors.h"

static char* getErrorMsg(uint8_t errorCode);

void exitError(uint8_t errorCode) {
    perror(getErrorMsg(errorCode));
    exit(errorCode);
}

//---------------------------------------------------
// STATIC FUNCTIONS ---------------------------------
//---------------------------------------------------

static char* getErrorMsg(uint8_t errorCode) {
    switch (errorCode) {
        case ERROR_MALLOC:
            return "Error allocating memory";
        case ERROR_INIT:
            return "Error initializing";
        case ERROR_PARAMS:
            return "Error in params";
        default:
            return "Unknown error";
    }
}