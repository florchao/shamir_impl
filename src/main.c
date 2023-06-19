#include "../include/utils.h"
#include "../include/Distribute.h"
#include "../include/Recover.h"

TParams* validateParams(int argc, char* argv[]);
uint8_t validK[] = { 3,4,5,6,7,8 };

int main(int argc, char* argv[]) {
    struct params* params = validateParams(argc, argv);
    printf("Params:\n");
    printf("Action: %s\n", params->action == DISTRIBUTE ? "Distribute" : "Retrieve");
    printf("File: %s\n", params->file);
    printf("K: %d\n", params->k);
    printf("N: %d\n", params->n);
    printf("Directory: %s\n", params->directory);
    if (params->action == DISTRIBUTE)
        distribute(params);
    else
        recover(params);
    return 0;
}

TParams* validateParams(int argc, char* argv[]) {

    if (argc != EXPECTED_PARAMS) {
        printf("Params must be 5\n");
        exit(1);
    }
    struct params* params = malloc(sizeof(struct params));
    if (params == NULL) {
            printf("Memory allocation failed for params.\n");
            exit(1);
    }
    params->action = strcmp(argv[1], "d") == 0 ? DISTRIBUTE : RETRIEVE;
    params->file = malloc((strlen(argv[2]) + 1));
    if (params->file == NULL) {
            printf("Memory allocation failed for file.\n");
            free(params);
            exit(1);
    }
    strcpy(params->file, argv[2]);
    params->k = atoi(argv[3]);
    int valid = 0;
    for (uint8_t i = 0; i < sizeof(validK) && !valid; i++) {
        if (params->k == validK[i])
            valid = 1;
    }
    if (!valid) {
        printf("Invalid k: %d\n", params->k);
        free(params->file);
        free(params);
        exit(1);
    }
    params->directory = malloc(sizeof(argv[4]) + 1);
    if (params->directory == NULL) {
            printf("Memory allocation failed for directory.\n");
            free(params->file);
            free(params);
            exit(1);
    }
    strcpy(params->directory, argv[4]);
    params->n = 8; //SHADOWS_NUMBER;
    return params;
}