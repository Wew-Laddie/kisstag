#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "kisstag_util.h"

char** KT_SplitTagList(char* tagList, int* tagCount)
{
    char** tagArray = NULL;
    char* token = NULL;
    char* s = tagList;
    int i = 0;
    int tc = 0;

    if(tagList == NULL || tagCount == NULL)
        return NULL;

    while(*s != '\0')
    {
        if(*s++ == ' ')
            tc++;
    }
    tc++;

    tagArray = calloc(tc, sizeof(char*));
    if(tagArray == NULL)
        return NULL;

    token = strtok(tagList, " ");
    while(token != NULL)
    {
        tagArray[i++] = token;
        token = strtok(NULL, " ");
    }

    *tagCount = tc;
    return tagArray;
}