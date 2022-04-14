#include <stdio.h>
#include <windows.h>
#include "ShimCacheParser.h"

int main(int argc, char *argv[]) 
{
    DWORD dataSize = 0;
    PUCHAR dataBuffer = NULL;
    
    if(argc < 2)
    {
        HKEY hKey = NULL;
        LONG lResult = RegOpenKeyEx (HKEY_LOCAL_MACHINE, APPCOMPATCACHE_PATH, 0, KEY_READ, &hKey);
        if (lResult != ERROR_SUCCESS) 
        {
            if (lResult == ERROR_FILE_NOT_FOUND) {
                printf("Key not found.\n");
                return TRUE;
            } 
            else {
                printf("Error opening key.\n");
                return FALSE;
            }
        }
        else {
            printf("Key opened.\n");
        }
        
        lResult = RegQueryValueEx(hKey, APPCOMPATCACHE_NAME, NULL, NULL, NULL, &dataSize);
        if(lResult != ERROR_SUCCESS){
            printf("RegQueryValueEx error. Return Value: %ld\n", lResult);
            return FALSE;
        }

        printf("Allocating buffer: %ld\n", dataSize);
            
        dataBuffer = malloc(dataSize);
        if(dataBuffer == NULL)
        {
            printf("Error allocating buffer\n");    
            return FALSE;
        }

        lResult = RegQueryValueEx(hKey, APPCOMPATCACHE_NAME, NULL, NULL, (LPBYTE) dataBuffer, &dataSize);
        if(lResult != ERROR_SUCCESS){
            printf("RegQueryValueEx error. Return Value: %ld\n", lResult);
            return FALSE;
        }

        lResult = RegCloseKey(hKey);
        if(lResult != ERROR_SUCCESS){
            printf("Key close error. Return Value: %ld\n", lResult);
            return FALSE;
        }
    }
    else
    {
        dataSize = GetFileSize(argv[1], NULL);
        dataBuffer = malloc(dataSize);
        if(dataBuffer == NULL)
        {
            printf("Error allocating buffer\n");
            return FALSE;
        }
        
        FILE * file = fopen(argv[1], "rb");  // r for read, b for binary
        if (file == NULL) {
            printf("Error opening file: %s\n", argv[1]);
            return FALSE;
        }

        dataSize = fread(dataBuffer, 1, dataSize, file);
        fclose(file);
    }

    parse(dataBuffer, dataSize);

    free(dataBuffer);
    return 0;
}