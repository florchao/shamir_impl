#include "../../include/Recover.h"
static TShadowGenerator * initializeRetriever(TParams * params);
static void initializeShadows(TShadowGenerator* shadowGenerator);
static TShadow * fromImageToShadow(uint8_t k ,bmpFile * imageFile);
static void recoverSecret(TShadowGenerator* generator);
static uint8_t  * interpolate(uint8_t  k , uint8_t * x_c, uint8_t * a_c, uint8_t * b_c);
static uint8_t  * interpolatePolynomial(uint8_t k , uint8_t * points, uint8_t * x_c);
static void checkCoefficients(uint8_t  k ,uint8_t * coefficients);
static void freeRetriever(TShadowGenerator * retriever);

void recover(TParams* params){
    TShadowGenerator * retriever = initializeRetriever(params);
    initializeShadows(retriever);
    recoverSecret(retriever);
    freeShadows(retriever->generatedShadows, retriever->n);
    freeRetriever(retriever);
}

//---------------------------------------------------
// STATIC FUNCTIONS ---------------------------------
//---------------------------------------------------

static TShadowGenerator * initializeRetriever(TParams * params){
    TShadowGenerator * retriever = malloc(sizeof(TShadowGenerator));
    if (retriever == NULL) {
        printf("Memory allocation failed for retriever.\n");
        return NULL;
    }
    retriever->k = params->k;
    retriever->n = params->n;
    openDirectory(retriever, params->directory);
    retriever->retrievedImage = malloc(sizeof(strlen(params->file) + 1));
    if (retriever-> retrievedImage == NULL) {
        free(retriever);
        printf("Memory allocation failed for retriever.\n");
        return NULL;
    }
    strcpy(retriever->retrievedImage, params->file);
    return retriever;
}

static void initializeShadows(TShadowGenerator* shadowGenerator) {

    TShadow** shadows = malloc(shadowGenerator->k * sizeof(TShadow*));
    if (shadows == NULL){
        perror("Unable to allocate memory for shadows");
        return;
    }

    bmpFile  * currentImageFile;
    
    for (int i = 0; i < shadowGenerator->k; i++) {
        currentImageFile = openBmpFile(shadowGenerator->imageFiles[i]);
        shadows[i] = fromImageToShadow(shadowGenerator->k, currentImageFile);
    }

    shadowGenerator->generatedShadows = shadows;

    shadowGenerator->file = malloc(sizeof(bmpFile * ));
    if (shadowGenerator->file  == NULL){
        perror("Unable to allocate memory for bmp file");
        freeShadows(shadows, shadowGenerator->k);
        return;
    }
    int headerSize = currentImageFile->header->size - currentImageFile->header->image_size_bytes;
    shadowGenerator->file->header = malloc(headerSize * sizeof (uint8_t));
    if (shadowGenerator->file->header  == NULL){
        perror("Unable to allocate memory for bmp file header");
        free(shadowGenerator->file);
        freeShadows(shadows, shadowGenerator->k);
        return;
    }
    memcpy(shadowGenerator->file->header, currentImageFile->header, headerSize);
    shadowGenerator->file->pixels = calloc(shadowGenerator->file->header->image_size_bytes,1 );
    if (shadowGenerator->file->pixels  == NULL){
        perror("Unable to allocate memory for bmp file pixels");
        free(shadowGenerator->file->header);
        free(shadowGenerator->file);
        freeShadows(shadows, shadowGenerator->k);
        return;
    }
}

static void freeRetriever(TShadowGenerator * retriever) {
    free(retriever->file);
    for (int i = 0; i < retriever->n; i++) {
        free(retriever->imageFiles[i]);
    }
    free(retriever->imageFiles);
    free(retriever->retrievedImage);
    free(retriever);
}

static TShadow * fromImageToShadow(uint8_t k ,bmpFile * imageFile){
    TShadow * shadow = malloc(sizeof (TShadow));
    shadow->shadowNumber = imageFile->header->reserved1;
    shadow->pointNumber = imageFile->header->image_size_bytes / (k -1);
    shadow->points = calloc(shadow->pointNumber,1);

    int lsb4 = ( k == 3 || k == 4 ) ? 1 : 0;
    int ImageBytesToShadowByte= ( lsb4 ) ? 2 : 4; // if lsb4 you need two uint8_t from image to generate a shadow uint8_t
    int bitOperator = lsb4 ? 0x0f:0x03; // four or two least significant bits.
    uint8_t shifter = lsb4 ? 4 : 2;


    uint64_t currentShadowBlock = 0;
    while(currentShadowBlock < shadow->pointNumber){
        for(uint64_t i = (ImageBytesToShadowByte) * currentShadowBlock ; i < (ImageBytesToShadowByte* ( currentShadowBlock + 1)); i++){
            shadow->points[currentShadowBlock] += imageFile->pixels[i]  & bitOperator;
            if (i + 1 != ((ImageBytesToShadowByte* ( currentShadowBlock + 1))) )
                shadow->points[currentShadowBlock] = shadow->points[currentShadowBlock] << shifter;
        }
        currentShadowBlock++;
    }

    return shadow;
}

static void recoverSecret(TShadowGenerator* generator){
    uint8_t  k = generator->k ;
    uint8_t  * imagePointer = generator->file->pixels;
    uint64_t currentBlock = 0;

    uint8_t  * x_c = malloc(k);
    uint8_t  * a_c = malloc(k);
    uint8_t  * b_c = malloc(k);

    if (x_c == NULL || a_c == NULL || b_c == NULL){
        perror("Unable to allocate memory for points");
        return;
    }

    while( currentBlock < ( (generator->file->header->image_size_bytes) / (k - 1)) ){

        for (int i = 0; i < k ; i ++){
            x_c[i] = generator->generatedShadows[i]->shadowNumber;
            a_c[i] = generator->generatedShadows[i]->points[currentBlock];
            b_c[i] = generator->generatedShadows[i]->points[currentBlock + 1];
        }
        uint8_t * coefficients = interpolate(k, x_c,  a_c, b_c);
        checkCoefficients(k, coefficients);
        memcpy(imagePointer, coefficients, k); // saving a_0 .... a_k-1 coeff
        memcpy(imagePointer + k, coefficients + k + 2, k - 2); //save b_2 .. b_k-1 coeff

        imagePointer += (2*k) - 2;
        currentBlock += 2 ;
    }


    int fd = open(generator->retrievedImage, O_WRONLY | O_CREAT);
    if (fd == -1) {
        free(a_c);
        free(b_c);
        free(x_c);
        perror("open");
        return ;
    }

    //save the retrieved image.
    int headerSize = generator->file->header->size - generator->file->header->image_size_bytes;
    lseek(fd, 0, SEEK_SET);
    write(fd, generator->file->header, headerSize);
    write(fd, generator->file->pixels, generator->file->header->image_size_bytes);
    close(fd);

    free(a_c);
    free(b_c);
    free(x_c);
}

static uint8_t  * interpolate(uint8_t  k , uint8_t * x_c, uint8_t * a_c, uint8_t * b_c){
    uint8_t  * coefficient = malloc( 2* k* sizeof(uint8_t));
    memcpy(coefficient, interpolatePolynomial(k, a_c, x_c), k);
    memcpy(coefficient + k , interpolatePolynomial(k , b_c, x_c) , k);
    return  coefficient;
}

static uint8_t  * interpolatePolynomial(uint8_t k , uint8_t * points, uint8_t * x_c){

    uint8_t  * coefficients = calloc(k , 1);
    uint8_t yPrimes[k];

    for( uint8_t S_i = 0; S_i < k ; S_i ++ ) {
        uint8_t currentCoefficient = 0;
        uint8_t neededPoints = k - S_i;

        for (uint8_t i = 0; i < neededPoints; i++) {
            yPrimes[i] = (S_i == 0) ? mod(points[i]) : modDiv(sub(yPrimes[i], coefficients[S_i - 1]), x_c[i]);
            uint8_t li = 1;
            for (int j = 0; j < neededPoints; j++)
                if (j != i)
                    li = mul(li, mul(-1, modDiv(x_c[j], sub(x_c[i], x_c[j]))));
            currentCoefficient = sum(currentCoefficient, mul(yPrimes[i], li));
        }
        coefficients[S_i] = currentCoefficient;
    }

    return coefficients;
}

static void checkCoefficients(uint8_t  k ,uint8_t * coefficients){
    int valid = 0;
    uint8_t  a_0 = mod(coefficients[0]) == 0 ? 1 : coefficients[0];
    uint8_t  a_1 = mod(coefficients[1]) == 0  ?  1 :  coefficients[1];

    for (int i = 0; i < 251; i++){
        if ( (coefficients[k] == mul(mod(-i) , a_0 )) &&
                (coefficients[k+1] == mul(mod(-i),  a_1) )
        )
            valid = 1;
    }
    if (! valid){
        printf("One invalid shadow was provided. ");
        exit(EXIT_FAILURE);
    }

    return ;
}
