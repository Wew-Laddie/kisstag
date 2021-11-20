#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "kisstag_util.h"
#include "kisstag_os.h"
#include "kisstag_png.h"

struct KTPNGChunk* KT_PNG_NextChunk(struct KTImage* image, struct KTPNGChunk* current)
{
    if(current->offset == 0)
        current->offset = 4;
    
    fseek(image->filePtr, current->offset + current->size + 4, SEEK_SET);
    
    if(ferror(image->filePtr) || feof(image->filePtr))
        return NULL;
    if(*(uint32_t*)current->id == PNG_IEND)
        return NULL;

    fread(&current->size, 4, 1, image->filePtr);
    fread(&current->id, 1, 4, image->filePtr);
    current->size = NTOHL(current->size);
    current->id[4] = '\0';
    current->offset = ftell(image->filePtr);

    return current;
}

struct KTPNGChunk* KT_PNG_FindTextChunk(struct KTImage* image, struct KTPNGChunk* chunkOut, const char* keyword)
{
    char temp[80];

    while(KT_PNG_NextChunk(image, chunkOut) != NULL)
    {
        if(*(uint32_t*)chunkOut->id == PNG_tEXt)
        {
            fgets(temp, 80, image->filePtr);
            if(strcmp(temp, keyword) == 0)
            {
                chunkOut->contentOffset = chunkOut->offset + strlen(temp) + 1;
                chunkOut->contentSize = chunkOut->size - (chunkOut->contentOffset - chunkOut->offset);
                return chunkOut;
            }
        }
    }

    return NULL;
}

void KT_PNG_WriteTextCRC(struct KTImage* image, const char* keyword, const char* content)
{
    const uint8_t textId[4] = { 0x74, 0x45, 0x58, 0x74 };
    uint32_t crc = KT_Crc32(0, textId, 4);
    crc = KT_Crc32(crc, keyword, strlen(keyword) + 1);
    crc = KT_Crc32(crc, content, strlen(content));
    crc = HTONL(crc);
    fwrite(&crc, 4, 1, image->filePtr);
}

int KT_PNG_AddTextChunk(struct KTImage* image, const char* keyword, const char* content, struct KTPNGChunk* chunkOut)
{
    uint8_t iend[12];
    uint32_t keywordLen = strlen(keyword);
    uint32_t contentLen = strlen(content);
    uint32_t chunkSize = keywordLen + contentLen + 1;
    size_t chunkOffset = 0;
    size_t contentOffset = 0; 

    /* Seek to the IEND chunk */
    fseek(image->filePtr, image->length - 12, SEEK_SET);

    if(ferror(image->filePtr) || feof(image->filePtr))
        return 1;

    /* Save IEND */
    fread(iend, 1, 12, image->filePtr);
    
    /* Seek back to the IEND chunk */
    fseek(image->filePtr, image->length - 12, SEEK_SET);

    /* Write chunk */
    fwrite(&(int){HTONL(chunkSize)}, 4, 1, image->filePtr);
    fwrite(&(int){PNG_tEXt}, 4, 1, image->filePtr);
    chunkOffset = ftell(image->filePtr);
    fwrite(keyword, 1, keywordLen + 1, image->filePtr);
    contentOffset = ftell(image->filePtr);
    fwrite(content, 1, contentLen, image->filePtr);
    KT_PNG_WriteTextCRC(image, keyword, content);

    if(chunkOut != NULL)
    {
        chunkOut->offset = chunkOffset;
        chunkOut->size = chunkSize;
        memcpy(chunkOut->id, &(int){PNG_tEXt}, 4);
        chunkOut->contentOffset = contentOffset;
        chunkOut->contentSize = contentLen;
    }
    
    /* Restore IEND */
    fwrite(iend, 1, 12, image->filePtr);
    fflush(image->filePtr);

    /* Recalculate image size */
    KT_RecalculateImageSize(image);

    return 0;
}

int KT_PNG_SetTextContent(struct KTImage* image, const char* keyword, const char* newContent)
{
    uint32_t contentLen = strlen(newContent);
    int newSize = strlen(keyword) + 1 + contentLen;
    struct KTPNGChunk chunk = { 0 };

    if(KT_PNG_FindTextChunk(image, &chunk, keyword) == NULL)
        return KT_PNG_AddTextChunk(image, keyword, newContent, NULL);
    
    int moveStart = chunk.offset + chunk.size + 4;
    int toMove = image->length - moveStart;
    int movePos = chunk.offset + newSize + 4;

    /* If the old tags are the same length as the new tags, just replace them */
    if(contentLen == chunk.contentSize)
    {
        /* Write content */
        fseek(image->filePtr, chunk.contentOffset, SEEK_SET);
        fwrite(newContent, 1, contentLen, image->filePtr);

        /* Recalculate and write checksum */
        KT_PNG_WriteTextCRC(image, keyword, newContent);

        return 0;
    }

    /* Expand file to accommodate new content */
    char *moveBuffer = malloc(toMove);

    fseek(image->filePtr, moveStart, SEEK_SET);
    fread(moveBuffer, 1, toMove, image->filePtr);

    if(contentLen < chunk.contentSize)
        KT_TruncateFile(image->filePtr, movePos);

    fseek(image->filePtr, chunk.offset - 8, SEEK_SET);
    fwrite(&(int){HTONL(newSize)}, 4, 1, image->filePtr);

    fseek(image->filePtr, chunk.contentOffset, SEEK_SET);
    fwrite(newContent, 1, contentLen, image->filePtr);
    chunk.contentSize = contentLen;
    KT_PNG_WriteTextCRC(image, keyword, newContent);

    fseek(image->filePtr, movePos, SEEK_SET);
    fwrite(moveBuffer, 1, toMove, image->filePtr);
    fflush(image->filePtr);

    free(moveBuffer);

    /* Recalculate image size */
    KT_RecalculateImageSize(image);

    return 0;
}

char* KT_PNG_GetTextContent(struct KTImage* image, const char* keyword)
{
    struct KTPNGChunk chunk = { 0 };
    char* buffer = NULL;

    if(KT_PNG_FindTextChunk(image, &chunk, keyword) == NULL)
        return NULL;

    buffer = calloc(chunk.contentSize + 1, 1);
    fseek(image->filePtr, chunk.contentOffset, SEEK_SET);
    fread(buffer, 1, chunk.contentSize, image->filePtr);

    return buffer;
}