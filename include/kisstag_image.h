#ifndef _KISSTAG_IMAGE_H
#define _KISSTAG_IMAGE_H

#include <stdio.h>
#include <stddef.h>

#define KT_IMAGE_NULL   0
#define KT_IMAGE_PNG    1
#define KT_IMAGE_JPG    2

struct KTImage
{
    /* Path to image */
    const char* path;

    /* Type of image */
    int imageType;

    /* Handle to open file */
    FILE* filePtr;

    /* Length of file */
    size_t length;
};

struct KTImage* KT_OpenImage(const char* path);
size_t KT_RecalculateImageSize(struct KTImage* image);
int KT_SetImageTags(struct KTImage* image, char* tags);
char* KT_GetImageTags(struct KTImage* image);
int KT_ImageHasAny(struct KTImage* image, const char* tags);
void KT_CloseImage(struct KTImage* image);

#endif