#ifndef _KISSTAG_PNG_H
#define _KISSTAG_PNG_H

#include <stdint.h>
#include <stddef.h>

#include "kisstag_image.h"

#if __BYTE__ORDER__ == __LITTLE_ENDIAN__
    #define PNG_IEND 0x444E4549
    #define PNG_tEXt 0x74584574
#else
    #define PNG_IEND 0x49454E44
    #define PNG_tEXt 0x74455874
#endif

struct KTPNGChunk
{
    /* Byte offset of the chunk data from the beginning of the file */
    uint32_t offset;

    /* Size of the chunk data */
    uint32_t size;

    /* Chunk ID with null-terminator */
    char id[5];

    /* Byte offset of content if the chunk is a tEXt chunk */
    uint32_t contentOffset;

    /* Size of tEXt content */
    uint32_t contentSize;
};

int KT_PNG_SetTextContent(struct KTImage* image, const char* keyword, const char* newContent);
char* KT_PNG_GetTextContent(struct KTImage* image, const char* keyword);

#endif