#ifndef UTILS_H
#define UTILS_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h> 
#include <dirent.h>
#include "bmp.h"


#define EXPECTED_PARAMS 5
#define P 251

typedef enum {
    DISTRIBUTE,
    RETRIEVE
} PROGRAM_ACTION;

typedef struct params {
    PROGRAM_ACTION action;
    char* file;
    uint8_t  k;
    char* directory;
    uint8_t n;
} TParams;

typedef struct shadow {
    uint8_t shadowNumber;
    uint64_t pointNumber;
    uint8_t* points;
} TShadow;

// [[s1], [s2], .. [sn]]

// s1 = {
//       1 (1 a n), j(q de bloques), [v1, .., vj]
//                                  v0m, v0d, v2m, v2d, 
//       }

typedef struct shadowGenerator {
    bmpFile* file; // name of the bmp file
    uint8_t  k; // polynomial grade
    uint8_t  n; // number of shadows
    TShadow** generatedShadows;  
    char** imageFiles;
    char* retrievedImage;
} TShadowGenerator;


uint8_t  sum(uint64_t x, uint64_t y);
uint8_t  sub(uint64_t x, uint64_t y);
uint8_t  mul(uint64_t x, uint64_t y);
uint8_t  modDiv(uint64_t x, uint64_t y);
uint8_t mod(int64_t x);

void openDirectory(TShadowGenerator* generator, char* directoryPath);

void freeShadows(TShadow** shadows, uint8_t length);
void freeShadow(TShadow * shadow);

static const uint8_t inverses[P] = {
        0, 1, 126, 84, 63, 201, 42, 36, 157, 28, 226, 137, 21, 58, 18, 67, 204,
        192, 14, 185, 113, 12, 194, 131, 136, 241, 29, 93, 9, 26, 159, 81, 102,
        213, 96, 208, 7, 95, 218, 103, 182, 49, 6, 216, 97, 106, 191, 235, 68,
        41, 246, 64, 140, 90, 172, 178, 130, 229, 13, 234, 205, 107, 166, 4, 51,
        112, 232, 15, 48, 211, 104, 99, 129, 196, 173, 164, 109, 163, 177, 197,
        91, 31, 150, 124, 3, 189, 108, 176, 174, 110, 53, 80, 221, 27, 243, 37,
        34, 44, 146, 71, 123, 169, 32, 39, 70, 153, 45, 61, 86, 76, 89, 199, 65,
        20, 240, 227, 132, 118, 117, 135, 228, 195, 179, 100, 83, 249, 2, 168,
        151, 72, 56, 23, 116, 134, 133, 119, 24, 11, 231, 186, 52, 162, 175, 165,
        190, 206, 98, 181, 212, 219, 82, 128, 180, 105, 207, 217, 214, 8, 224,
        30, 171, 198, 141, 77, 75, 143, 62, 248, 127, 101, 220, 160, 54, 74, 88,
        142, 87, 78, 55, 122, 152, 147, 40, 203, 236, 19, 139, 200, 247, 85, 144,
        46, 17, 238, 22, 121, 73, 79, 161, 111, 187, 5, 210, 183, 16, 60, 145,
        154, 35, 245, 202, 69, 148, 33, 156, 244, 43, 155, 38, 149, 170, 92, 225,
        242, 158, 222, 10, 115, 120, 57, 239, 138, 66, 237, 59, 47, 184, 233,
        193, 230, 114, 25, 223, 94, 215, 209, 50, 188, 167, 125, 250
};

#endif //UTILS_H