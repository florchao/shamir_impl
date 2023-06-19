#include "./bmp.h"

bmpFile* openBmpFile(const char* path) {
    int fd = open(path, O_RDWR);
    if (fd == -1) {
        perror("open");
        return NULL;
    }

    struct stat fileStats;
    if (fstat(fd, &fileStats) != 0) {
        perror("fstat");
        close(fd);
        return NULL;
    }

    int fileSize = fileStats.st_size;


    void* filePointer = malloc(fileSize);
    if (filePointer == NULL) {
            perror("Memory allocation failed for file pointer.\n");
            return NULL;
    }

    if (read(fd, filePointer, fileSize) != fileSize) {
        perror("Unable to read file");
        return NULL;
    }

    bmpHeader* headerPointer = (bmpHeader*) filePointer;
    uint8_t* pixelsPointer = ((uint8_t*) filePointer) + headerPointer->offset;

    bmpFile* bitMapFile = malloc(sizeof(bmpFile));
    if (bitMapFile == NULL) {
            perror("Memory allocation failed for file.\n");
            return NULL;
    }
    bitMapFile->header = headerPointer;
    bitMapFile->pixels = pixelsPointer;
    bitMapFile->fd = fd;

    return bitMapFile;
}

