#ifndef _KISSTAG_OS_H
#define _KISSTAG_OS_H

#include <stdio.h>
#include <stddef.h>

typedef void(*kt_file_callback_t)(const char* path);

int KT_TruncateFile(FILE* file, size_t newSize);
int KT_RecurseDir(const char* dirPath, kt_file_callback_t fn);

#endif