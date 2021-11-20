#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "kisstag_os.h"

#if defined (_WIN32) || defined (_WIN64)
    #include <io.h>
    #include <windows.h>
    #include <Shlwapi.h>
    #include <tchar.h>
#else
    #include <unistd.h>
#endif

#if defined (_WIN32) || defined (_WIN64)
int KT_TruncateFile(FILE* file, size_t newSize)
{
    return _chsize_s(_fileno(file), newSize);
}

int KT_RecurseDir(const char* dirPath, kt_file_callback_t fn)
{
    WIN32_FIND_DATAA FindFileData;
    HANDLE hFindFile;
    CHAR szFullPath[MAX_PATH];

    PathCombineA(szFullPath, dirPath, "*");
    hFindFile = FindFirstFileA(szFullPath, &FindFileData);

    if(hFindFile != INVALID_HANDLE_VALUE)
    {
        do
        {
            LPCSTR szFileName = FindFileData.cFileName;
            if(szFileName[0] == '.' || (szFileName[0] == '.' && szFileName[1] == '.'))
                continue;
            
            if(FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
            {
                PathCombineA(szFullPath, dirPath, szFileName);
                KT_RecurseDir(szFullPath, fn);
            }
            else
            {
                if(fn != NULL)
                {
                    LPCSTR ext = PathFindExtensionA(szFileName);
                    if(StrCmpA(ext, ".png") != 0 && StrCmpA(ext, ".jpg") != 0)
                        continue;
                    
                    PathCombineA(szFullPath, dirPath, szFileName);
                    fn(szFullPath);
                }
            }
        } while (FindNextFileA(hFindFile, &FindFileData));
        FindClose(hFindFile);
    }

    return 0;
}
#else
int KT_TruncateFile(FILE* file, size_t newSize)
{
    return ftruncate(fileno(file), newSize);
}
#endif