#include "../../include/utils.h"

uint8_t  sum(uint64_t x, uint64_t y) {
    return mod(x + y);
}
uint8_t  sub(uint64_t x, uint64_t y) {
    return mod(x - y);
}
uint8_t  times(uint64_t x, uint64_t y) {
    return mod(x * y);
}
uint8_t  modDiv(uint64_t x, uint64_t y) {
    return times(x, inverses[mod(y)]);
}
uint8_t mod(int64_t x) {
    while (x < 0) {
        x += P;
    }
    return x % P;
}

void openDirectory(TShadowGenerator* generator, char* directoryPath) {
    DIR* directory = opendir(directoryPath);
    if (directory == NULL) {
        perror("Unable to open the given directory");
        exit(EXIT_FAILURE);
    }
    char** fileNames = malloc(generator->n * sizeof(char*));
    int currentFile = 0;
    struct dirent* entry;
    while ((entry = readdir(directory)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }
        uint64_t directoryLength = strlen(directoryPath);
        size_t fileNameLength = strlen(entry->d_name);
        fileNames[currentFile] = malloc((directoryLength + 1 + fileNameLength + 1) * sizeof(char));
        strcpy(fileNames[currentFile], directoryPath);
        strcpy(fileNames[currentFile] + directoryLength, "/");
        strcpy(fileNames[currentFile] + directoryLength + 1, entry->d_name);
        currentFile++;
    }

    generator->imageFiles = malloc(currentFile * sizeof(char*));

    for (int i = 0; i < currentFile; i++) {
        generator->imageFiles[i] = malloc(strlen(fileNames[i]) * sizeof(char) + 1);
        strcpy(generator->imageFiles[i], fileNames[i]);
        strcpy(generator->imageFiles[i] + strlen(fileNames[i]), "\0");
    }

    for(int i=0 ; i<generator->n; i++){
        free(fileNames[i]);
    }
    free(fileNames);
    free(entry);
}

void freeShadow(TShadow * shadow){
    if(shadow->values != NULL){
        free(shadow->values);
    }
    free(shadow);

}

void freeShadows(TShadow** shadows, uint8_t length){
    for(int i=0; i< length; i++){
        freeShadow(shadows[i]);
    }
    free(shadows);
}