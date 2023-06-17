#ifndef DISTRIBUTE_H
#define DISTRIBUTE_H

#include "utils.h"
#include "bmp.h"
#include <dirent.h>
#include <time.h>
#include <sys/types.h>



typedef struct shadow {
    uint8_t shadowNumber;
    uint64_t pointNumber;
    uint8_t* points;
} TShadow;

// [[s1], [s2], .. [sn]]

// s1 = {
//       1 (1 a n), j(q de bloques), [v1, .., vj]
//       }


typedef struct shadowGenerator {
    bmpFile* file;
    uint8_t  k;
    uint8_t  n;
    TShadow** generatedShadows;
    char** imageFiles;
    char* creatingFileName;
} TShadowGenerator;

void distribute(TParams* params);

#endif // DISTRIBUTE_H