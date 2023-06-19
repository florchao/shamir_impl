#include "../../include/errors.h"

static char* getErrorMsg(uint8_t errorCode);

void exitError(uint8_t errorCode, char* errorMsg) {
    perror("%s: %s", getErrorMsg(errorCode), errorMsg);
    exit(errorMsg);
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
        default:
            return "Unknown error";
    }
}