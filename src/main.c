#include "../include/utils.h"
#include "../include/Distribute.h"
#include "../include/Recover.h"
#include "../include/errors.h"

TParams* validateParams(int argc, char* argv[]);
uint8_t validK[] = { 3,4,5,6,7,8 };

int main(int argc, char* argv[]) {
    struct params* params = validateParams(argc, argv);
    if (params->action == DISTRIBUTE)
        distribute(params);
    else
        recover(params);
    return 0;
}

TParams* validateParams(int argc, char* argv[]) {

    if (argc != EXPECTED_PARAMS) {
        exitError(ERROR_PARAMS);
    }
    struct params* params = malloc(sizeof(struct params));
    if (params == NULL) {
        exitError(ERROR_MALLOC);
    }
    params->action = strcmp(argv[1], "d") == 0 ? DISTRIBUTE : RECOVER;
    params->file = malloc((strlen(argv[2]) + 1));
    if (params->file == NULL) {
            free(params);
            exitError(ERROR_MALLOC);
    }
    strcpy(params->file, argv[2]);
    params->k = atoi(argv[3]);
    int valid = 0;
    for (uint8_t i = 0; i < sizeof(validK) && !valid; i++) {
        if (params->k == validK[i])
            valid = 1;
    }
    if (!valid) {
        free(params->file);
        free(params);
        exitError(ERROR_PARAMS);
    }
    params->directory = malloc(sizeof(argv[4]) + 1);
    if (params->directory == NULL) {
            free(params->file);
            free(params);
            exitError(ERROR_MALLOC);
    }
    strcpy(params->directory, argv[4]);
    params->n = 8; //SHADOWS_NUMBER;
    return params;
}