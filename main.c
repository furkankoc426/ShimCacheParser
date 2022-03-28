#include <stdio.h>
#include <windows.h>

const UCHAR * APPCOMPATCACHE_PATH = "SYSTEM\\CurrentControlSet\\Control\\Session Manager\\AppCompatCache";
const UCHAR * APPCOMPATCACHE_NAME = "AppCompatCache";

int main() {
    
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

    DWORD dataSize = 512 * 1024;
    PUCHAR dataBuffer = malloc(dataSize);
    
    lResult = RegQueryValueEx(hKey, APPCOMPATCACHE_NAME, NULL, NULL, (LPBYTE) dataBuffer, &dataSize);
    if(lResult == ERROR_SUCCESS){
        for (int i = 0; i < dataSize; i++)
        {
            if (i > 0) printf(":");
            printf("%02X", dataBuffer[i]);
        }
        printf("\n");
    }
    else {
        printf("%d\n", lResult);
    }

    free(dataBuffer);
	


    return 0;
}