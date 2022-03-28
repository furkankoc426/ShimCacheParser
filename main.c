#include <stdio.h>
#include <windows.h>

const UCHAR * APPCOMPATCACHE_PATH = "SYSTEM\\CurrentControlSet\\Control\\Session Manager\\AppCompatCache";
const UCHAR * APPCOMPATCACHE_NAME = "AppCompatCache";
const DWORD HEADER_SIZE = 0x34;
const DWORD MAGIC_NUMBER = 0x73743031;
const DWORD LENGTH_OFFSET = 0x08;
const DWORD PATH_LENGTH_OFFSET = 0x0C;
const DWORD FILETIME_OFFSET = 0x0E;

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
        /*
        for (int i = 0; i < dataSize; i++)
        {
            if (i > 0) printf(":");
            printf("%02X", dataBuffer[i]);
        }
        printf("\n");
        */
    }
    else {
        printf("RegQueryValueEx error. Return Value: %d\n", lResult);
    }

    /*
    FILE * file = fopen("AppCompatCache.bin","wb");
    fwrite(dataBuffer, dataSize, 1, file);
    fclose(file);
	*/

    PWCHAR filePath = malloc(512);
    PUCHAR index = dataBuffer;
    index += HEADER_SIZE;
    int counter = 0;
    while( index < dataBuffer + dataSize) {
        printf("### Entry count: %d ###\n", counter++);
        
        if( *((UINT32*)index) != MAGIC_NUMBER) {
            printf("Magic Number Error. %X\n", *((UINT32*)index));
        }
        UINT length = *(UINT*)(index + LENGTH_OFFSET);
        USHORT pathLength = *(USHORT*)(index + PATH_LENGTH_OFFSET);
        memcpy(filePath, index + FILETIME_OFFSET, pathLength);
        filePath[pathLength/2] = 0;
        FILETIME filetime = *(FILETIME*)(index + FILETIME_OFFSET + pathLength);

        wprintf(L"Length: %X\n", length);
        wprintf(L"PathLength: %X\n", pathLength);
        wprintf(L"Filepath: %s\n", filePath);
        SYSTEMTIME st;    
        FileTimeToSystemTime(&filetime, &st);
        printf("Filetime: %d/%02d/%02d-%02d:%02d:%02d\n", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);

        index += length + 12;
    }

    free(dataBuffer);
    return 0;
}