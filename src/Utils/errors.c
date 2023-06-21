#include "../include/errors.h"

static char* getErrorMsg(uint8_t errorCode);

void exitError(uint8_t errorCode) {
    perror(getErrorMsg(errorCode));
    exit(EXIT_FAILURE);
}

//---------------------------------------------------
// STATIC FUNCTIONS ---------------------------------
//---------------------------------------------------

static char* getErrorMsg(uint8_t errorCode) {
    switch (errorCode) {
        case ERROR_MALLOC:
            return "Error allocating memory";
        case ERROR_PARAMS:
            return "Invalid parameters";
        case ERROR_OPEN_IMAGE:
            return "Error opening image";
        case ERROR_OPEN_FILE:
            return "Error opening file";
        case ERROR_READ_FILE:
            return "Error reading file";
        default:
            return "Unknown error";
    }
}