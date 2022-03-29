#include <stdio.h>
#include <windows.h>
#include "ShimCacheParser.h"

int main(int argc, char *argv[]) {

    DWORD dataSize = 512 * 1024;
    PUCHAR dataBuffer = malloc(dataSize);
#if 0
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
    
    lResult = RegQueryValueEx(hKey, APPCOMPATCACHE_NAME, NULL, NULL, (LPBYTE) dataBuffer, &dataSize);
    if(lResult != ERROR_SUCCESS){
        printf("RegQueryValueEx error. Return Value: %d\n", lResult);
    }

#if 0
    FILE * file = fopen("AppCompatCache.bin","wb");
    fwrite(dataBuffer, dataSize, 1, file);
    fclose(file);
#endif
#endif

    FILE * file = fopen(argv[1], "rb");  // r for read, b for binary
    dataSize = fread(dataBuffer, 1, dataSize, file);

    printf("%d %s\n", dataSize, dataBuffer);
    parse(dataBuffer, dataSize);

    fclose(file);
    free(dataBuffer);
    return 0;
}