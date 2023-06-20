#include "../../include/bmp.h"

bmpFile* openBmpFile(const char* path) {
    int fd = open(path, O_RDWR);
    if (fd == -1) {
        exitError(ERROR_OPEN_FILE);
    }

    struct stat fileStats;
    if (fstat(fd, &fileStats) != 0) {
        close(fd);
        exitError(ERROR_OPEN_FILE);
    }

    int fileSize = fileStats.st_size;


    void* filePointer = malloc(fileSize);
    if (filePointer == NULL) {
        exitError(ERROR_MALLOC);
    }

    if (read(fd, filePointer, fileSize) != fileSize) {
        free(filePointer);
        exitError(ERROR_READ_FILE);
    }

    bmpHeader* headerPointer = (bmpHeader*) filePointer;
    uint8_t* pixelsPointer = ((uint8_t*) filePointer) + headerPointer->offset;

    bmpFile* bitMapFile = malloc(sizeof(bmpFile));
    if (bitMapFile == NULL) {
        free(filePointer);
        exitError(ERROR_MALLOC);
    }
    bitMapFile->header = headerPointer;
    bitMapFile->pixels = pixelsPointer;
    bitMapFile->fd = fd;

    return bitMapFile;
}

