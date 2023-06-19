#ifndef ERRORS_H
#define ERRORS_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h> 

//Error codes
#define ERROR_MALLOC 1
#define ERROR_INIT 2

void exitError(uint8_t errorCode, char* errorMsg);

#endif // ERRORS_H