#include "../../include/Distribute.h"

static void openDirectory(TShadowGenerator* generator, char* directoryPath);
static TShadow** initializeShadowArray(TShadowGenerator* shadowGenerator, uint32_t shadowPoints);
static uint8_t evaluatePolynomial(TShadowGenerator* shadowGenerator, uint8_t* coefficients, uint8_t value);
static void distributeSecret(TShadowGenerator* shadowGenerator);
static TShadowGenerator* initializeDistributor(TParams* params);
static void hideSecret(TShadowGenerator * shadowGenerator);
static void hideShadow(uint8_t  k , bmpFile * image, TShadow * hidingShadow);
static void insertBits(uint8_t  *imagePixelPointer, uint8_t  *shadowPointer, uint8_t k);

uint8_t fourSignificant[] = {0xC0, 0x30, 0x0C, 0x03};
uint8_t twoSignificant[] = {0xF0, 0x0F};


void distribute(TParams* params) {
    TShadowGenerator* generator = initializeDistributor(params);
    distributeSecret(generator);
    // printf("\n");
    // printf("Shadows generated:\n");
    // for (uint8_t i = 0; i < 1; i++) {
    //     for (uint64_t j = 0; j < generator->generatedShadows[i]->pointNumber; j++) {
    //         printf("%d ", generator->generatedShadows[i]->points[j]);
    //     }
    // }
    hideSecret(generator);
    printf("llegue");
}

//---------------------------------------------------
// STATIC FUNCTIONS ---------------------------------
//---------------------------------------------------

static TShadowGenerator* initializeDistributor(TParams* params) {
    TShadowGenerator* shadowGenerator = malloc(sizeof(TShadowGenerator));
    shadowGenerator->file = openBmpFile(params->file);
    shadowGenerator->k = params->k;
    shadowGenerator->n = params->n;
    openDirectory(shadowGenerator, params->directory);
    return shadowGenerator;


}

static void distributeSecret(TShadowGenerator* shadowGenerator) {
    uint32_t shadowPoints = (shadowGenerator->file->header->size) / (shadowGenerator->k - 1);
    TShadow** shadowArray = initializeShadowArray(shadowGenerator, shadowPoints);

    uint8_t* pixelPoints = shadowGenerator->file->pixels;
    uint8_t k = shadowGenerator->k;
    uint32_t currentBlock = 0;
    uint8_t* aCoefficients = malloc(k * sizeof(uint8_t));
    uint8_t* bCoefficients = malloc(k * sizeof(uint8_t));
    uint8_t a_0, a_1;

    while (currentBlock < shadowPoints) {

        memcpy(aCoefficients, pixelPoints, k);
        memcpy(bCoefficients + 2, pixelPoints + k, k - 2);
        uint8_t random = (rand() % (P - 2)) + 1;
        a_0 = mod(aCoefficients[0]) == 0 ? 1 : aCoefficients[0];
        a_1 = mod(aCoefficients[1]) == 0 ? 1 : aCoefficients[1];
        bCoefficients[0] = mod(mul(mod(-random), a_0));
        bCoefficients[1] = mod(mul(mod(-random), a_1));

        for (int j = 0; j < shadowGenerator->n; j++) {
            shadowArray[j]->points[currentBlock] = evaluatePolynomial(shadowGenerator,
                aCoefficients, shadowArray[j]->shadowNumber);
            shadowArray[j]->points[currentBlock + 1] = evaluatePolynomial(shadowGenerator,
                bCoefficients, shadowArray[j]->shadowNumber);
        }

        pixelPoints += (2 * k) - 2;
        currentBlock += 2;
    }

    shadowGenerator->generatedShadows = shadowArray;
}

static void openDirectory(TShadowGenerator* generator, char* directoryPath) {
    DIR* directory = opendir(directoryPath);
    if (directory == NULL) {
        perror("Unable to open the given directory");
        return;
    }
    char** fileNames = malloc(generator->n * sizeof(char*));

    int currentFile = 0;
    struct dirent* entry;
    while ((entry = readdir(directory)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }
        uint64_t directoryLength = strlen(directoryPath);
        fileNames[currentFile] = malloc(directoryLength + 1 + strlen(entry->d_name));
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

    // for (int i = 0; i < generator->n; i++) {
    //     printf("%s\n", generator->imageFiles[i]);
    // }
}

static TShadow** initializeShadowArray(TShadowGenerator* shadowGenerator, uint32_t shadowPoints) {

    TShadow** shadowArray = malloc(shadowGenerator->n * sizeof(TShadow*));
    //initialize all TShadow structures.
    for (int i = 0; i < shadowGenerator->n; i++) {
        shadowArray[i] = malloc(sizeof(TShadow));
        shadowArray[i]->shadowNumber = i + 1;
        shadowArray[i]->pointNumber = shadowPoints;
        shadowArray[i]->points = malloc(shadowPoints * sizeof(uint8_t));
    }
    return  shadowArray;
}

static uint8_t evaluatePolynomial(TShadowGenerator* shadowGenerator, uint8_t* coefficients, uint8_t value) {
    uint8_t result = 0;
    uint8_t power = 1;

    uint8_t x2 = mod(value);

    for (uint8_t i = 0; i < shadowGenerator->k; i++) {
        result = sum(result, mul(coefficients[i], power));
        power = mul(power, x2);
    }

    return result;
}

static void hideSecret(TShadowGenerator * shadowGenerator){
    for (int i = 0 ; i < shadowGenerator -> n ; i ++){
        printf("%s \n",shadowGenerator->imageFiles[i]);
        bmpFile  * currentImageFile = openBmpFile(shadowGenerator->imageFiles[i]);
        TShadow * currentShadow = shadowGenerator->generatedShadows[i];
        hideShadow(shadowGenerator->k,currentImageFile, currentShadow);

        //save the generated image.
        int headerSize = currentImageFile->header->size - currentImageFile->header->image_size_bytes;
        //re-write the entire file.
         lseek(currentImageFile->fd, 0, SEEK_SET);
        write(currentImageFile->fd , currentImageFile->header, headerSize);
        write(currentImageFile->fd , currentImageFile->pixels, currentImageFile->header->image_size_bytes);
        close(currentImageFile->fd);
    }

}


static void hideShadow(uint8_t  k , bmpFile * image, TShadow * hidingShadow){
    image->header->reserved1 = hidingShadow->shadowNumber; //save the shadow number.
    uint8_t * imagePixelPointer = image->pixels;
    uint8_t * shadowPointer = hidingShadow->points;
    for(uint32_t  i = 0; i < hidingShadow->pointNumber; i++ ){
            insertBits(imagePixelPointer, shadowPointer, k);
            shadowPointer += 1 ; //go to the next point;
            imagePixelPointer += (k == 3 || k == 4) ? 2 : 4;
    }
}

static void insertBits(uint8_t  * imagePixelPointer, uint8_t  *shadowPointer, uint8_t  k){
    int lsb4 = ( k == 3 || k == 4 ) ? 1 : 0;
    int bytesUsedFromImage = ( lsb4 ) ? 2 : 4;

    uint8_t  lsb4Shifter[] = {4, 0};
    uint8_t  lsb2Shifter[] = {6,4,2, 0};
    int currentShifterIndex = 0 ;

    uint8_t  bits[bytesUsedFromImage];

    for(int i = 0; i < bytesUsedFromImage ; i++){
        bits[i] = lsb4 ? *shadowPointer & twoSignificant[i] : *shadowPointer & fourSignificant[i] ;
        bits[i] = lsb4 ? bits[i] >> lsb4Shifter[currentShifterIndex] : bits[i] >>lsb2Shifter[currentShifterIndex];
        currentShifterIndex++;
    }

    int and = lsb4 ? 0xF0 : 0xFC;  // 4 o 6 MSB.
    for (int i = 0 ; i < bytesUsedFromImage ; i++)
        imagePixelPointer[i] = (imagePixelPointer[i] & and) + bits[i];

}
