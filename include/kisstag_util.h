#ifndef _KISSTAG_UTIL_H
#define _KISSTAG_UTIL_H

#include <stdint.h>
#include <stddef.h>

#if __BYTE__ORDER__ == __LITTLE_ENDIAN__
    #define NTOHL(n) __builtin_bswap32(n)
    #define HTONL(n) __builtin_bswap32(n)
#else
    #define NTOHL(n) (n)
    #define HTONL(n) (n)
#endif

uint32_t KT_Crc32(uint32_t crc, const void *buf, size_t size);
char** KT_SplitTagList(char* tagList, int* tagCount);

#endif