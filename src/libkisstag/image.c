#include <stdlib.h>
#include <string.h>
#include "kisstag_image.h"
#include "kisstag_png.h"
#include "kisstag_util.h"

static const unsigned char png_signature[] = {
    0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A
};

struct KTImage* KT_OpenImage(const char* path)
{
    unsigned char sig[8];
    struct KTImage* image = calloc(1, sizeof(struct KTImage));
    
    if(image == NULL)
        return NULL;
    
    image->path = path;
    image->filePtr = fopen(path, "r+b");

    if(image->filePtr == NULL)
    {
        free(image);
        return NULL;
    }

    fread(sig, 1, 8, image->filePtr);

    if(memcmp(sig, png_signature, 8) == 0)
        image->imageType = KT_IMAGE_PNG;
    else
    {
        fclose(image->filePtr);
        free(image);
        return NULL;
    }

    fseek(image->filePtr, 0, SEEK_END);
    image->length = ftell(image->filePtr);
    rewind(image->filePtr);

    return image;
}

size_t KT_RecalculateImageSize(struct KTImage* image)
{
    if(image == NULL || image->filePtr == NULL)
        return 0;
    
    fseek(image->filePtr, 0, SEEK_END);
    image->length = ftell(image->filePtr);
    rewind(image->filePtr);
    return image->length;
}

int KT_SetImageTags(struct KTImage* image, char* tags)
{
    if(image == NULL || tags == NULL)
        return 1;

    if(image->imageType == KT_IMAGE_PNG)
        return KT_PNG_SetTextContent(image, "KTags", tags);
    
    return 1;
}

char* KT_GetImageTags(struct KTImage* image)
{
    if(image == NULL)
        return NULL;
    
    if(image->imageType == KT_IMAGE_PNG)
        return KT_PNG_GetTextContent(image, "KTags");
    
    return NULL;
}

int KT_ImageHasAny(struct KTImage* image, const char* tags)
{
    char* searchTags = strdup(tags);
    char* imageTags = KT_GetImageTags(image);
    char** searchList;
    int searchCount;
    char** imageList;
    int imageCount;

    searchList = KT_SplitTagList(searchTags, &searchCount);
    imageList = KT_SplitTagList(imageTags, &imageCount);

    for(int i = 0; i < imageCount; i++)
    {
        for(int j = 0; j < searchCount; j++)
        {
            if(strcmp(imageList[i], searchList[j]) == 0)
            {
                free(imageList);
                free(imageTags);
                free(searchList);
                free(searchTags);
                return 1;
            }
        }
    }

    free(imageList);
    free(imageTags);
    free(searchList);
    free(searchTags);
    return 0;
}

void KT_CloseImage(struct KTImage* image)
{
    if(image == NULL)
        return;

    if(image->filePtr != NULL)
        fclose(image->filePtr);

    free(image);
}