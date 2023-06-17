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

    if (read(fd, filePointer, fileSize) != fileSize) {
        perror("Unable to read file");
        return NULL;
    }

    bmpHeader* headerPointer = (bmpHeader*) filePointer;
    uint32_t pixelsOffset = headerPointer->offset;

    uint8_t* pixelsPointer = ((uint8_t*) filePointer) + pixelsOffset;

    bmpFile* bitMapFile = malloc(sizeof(bmpFile));
    bitMapFile->header = headerPointer;
    bitMapFile->pixels = pixelsPointer;
    bitMapFile->fd = fd;
    return bitMapFile;
}

