#include "../../include/Recover.h"
static TShadowGenerator * initializeRetriever(TParams * params);
static void initializeShadows(TShadowGenerator* shadowGenerator);
static TShadow * fromImageToShadow(uint8_t k ,bmpFile * imageFile);

void recover(TParams* params){

    TShadowGenerator * generator = initializeRetriever(params);

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
    retriever->retrievedImage = malloc(sizeof(strlen(params->file)));
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
        free(shadows);
        return;
    }
    int headerSize = currentImageFile->header->size - currentImageFile->header->image_size_bytes;
    shadowGenerator->file->header = malloc(headerSize * sizeof (uint8_t));
    if (shadowGenerator->file->header  == NULL){
        perror("Unable to allocate memory for bmp file header");
        free(shadowGenerator->file);
        free(shadows);
        return;
    }
    memcpy(shadowGenerator->file->header, currentImageFile->header, headerSize);
    shadowGenerator->file->pixels = calloc(shadowGenerator->file->header->image_size_bytes,1 );
    if (shadowGenerator->file->pixels  == NULL){
        perror("Unable to allocate memory for bmp file pixels");
        free(shadowGenerator->file->header);
        free(shadowGenerator->file);
        free(shadows);
        return;
    }
}

void freeRetriever(TShadowGenerator * retriever){
    // TODO
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

