#include <time.h>
#include <stdio.h>
#include <sys/stat.h>
#include "ShimCacheParser.h"

// COMMON WIN10
const DWORD WIN10_MAGIC_NUMBER = 0x73743031;
const DWORD WIN10_ENTRY_HEADER_SIZE = 0x0C;
const DWORD WIN10_ENTRY_LENGTH_OFFSET = 0x08;
const DWORD WIN10_PATH_LENGTH_OFFSET = 0x0C;
const DWORD WIN10_FILETIME_OFFSET = 0x0E;

// SPECIAL 
const DWORD WIN10_HEADER_SIZE = 0x30;
const DWORD WIN10_ENTRY_COUNT_OFFSET = 0x18;
const DWORD WIN10C_HEADER_SIZE = 0x34;

// COMMON WIN8
const DWORD WIN8_HEADER_SIZE = 0x80;
const DWORD WIN8_ENTRY_HEADER_SIZE = 0x0C;
const DWORD WIN8_ENTRY_LENGTH_OFFSET = 0x08;
const DWORD WIN8_PATH_LENGTH_OFFSET = 0x0C;
const DWORD WIN8_PACKAGE_OFFSET = 0x0E;
const DWORD WIN8_FILETIME_OFFSET = 0x18;

// SPECIAL 
const DWORD WIN8_0_MAGIC_NUMBER = 0x73743030;
const DWORD WIN8_1_MAGIC_NUMBER = 0x73743031;

// COMMON WIN7
const DWORD WIN7_HEADER_SIZE = 0x80;
const DWORD WIN7_MAGIC_NUMBER = 0xBADC0FEE;
const DWORD WIN7_ENTRY_COUNT_OFFSET = 0x04;
const DWORD WIN7_FILE_OFFSET = 0x08;

// SPECIAL 
const DWORD WIN7_32_ENTRY_SIZE = 0x20;
const DWORD WIN7_64_ENTRY_SIZE = 0x30;

struct WIN7_32_Entry
{
    USHORT pathLength;
	USHORT pathMaxLength;
	DWORD pathOffset;
	FILETIME lastModTime;
	DWORD insertFlags;
	DWORD shimFlags;
	DWORD blobSize;
	DWORD blobOffset;
};

struct WIN7_64_Entry
{
    USHORT pathLength;
	USHORT pathMaxLength;
	DWORD padding;
	QWORD pathOffset;
	FILETIME lastModTime;
	DWORD insertFlags;
	DWORD shimFlags;
	QWORD blobSize;
	QWORD blobOffset;
};

// COMMON WINVISTA
const DWORD WINVISTA_HEADER_SIZE = 0x08;
const DWORD WINVISTA_MAGIC_NUMBER = 0xBADC0FFE;
const DWORD WINVISTA_ENTRY_COUNT_OFFSET = 0x04;
const DWORD WINVISTA_FILE_OFFSET = 0x08;

// SPECIAL 
const DWORD WINVISTA_32_ENTRY_SIZE = 0x18;
const DWORD WINVISTA_64_ENTRY_SIZE = 0x20;

struct WINVISTA_32_Entry
{
    USHORT pathLength;
	USHORT pathMaxLength;
	DWORD pathOffset;
	FILETIME lastModTime;
	DWORD insertFlags;
	DWORD shimFlags;
};

struct WINVISTA_64_Entry
{
    USHORT pathLength;
	USHORT pathMaxLength;
	DWORD padding;
	QWORD pathOffset;
	FILETIME lastModTime;
	DWORD insertFlags;
	DWORD shimFlags;
};

// COMMON WINXP
const DWORD WINXP_HEADER_SIZE = 0x0190;
const DWORD WINXP_MAGIC_NUMBER = 0xDEADBEEF;
const DWORD WINXP_ENTRY_COUNT_OFFSET = 0x04; // will be added to WCHAR otherwise 0x08
const DWORD WINXP_ENTRY_SIZE = 0x0228;
const DWORD WINXP_FILE_OFFSET = 0x04;

struct WINXP_Entry
{   
    WCHAR filePath[MAX_PATH+4];
    FILETIME lastModTime;
    LARGE_INTEGER fileSize;
    FILETIME lastUpdateTime;
};

const DWORD WIN2K3_32_FILESIZE_OFFSET = 0x10;
const DWORD WIN2K3_64_FILESIZE_OFFSET = 0x18;

void GetErrorMessage()
{
    CHAR buffer[1024];
    DWORD dwErrorCode = GetLastError();
    FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                                 NULL,  /* (not used with FORMAT_MESSAGE_FROM_SYSTEM) */
                                 dwErrorCode,
                                 MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                                 buffer,
                                 1024,
                                 NULL);
    printf("%s\n", buffer);
}

time_t filetime_to_timet(FILETIME ft)  
{
    ULARGE_INTEGER ull;
    ull.LowPart = ft.dwLowDateTime;
    ull.HighPart = ft.dwHighDateTime;   
    return ull.QuadPart / 10000000ULL - 11644473600ULL;  
}

void insertFileTime(FILE * file, CHAR * filePath, time_t lastModTime)
{
    CHAR buffer[32];
    struct tm * tp;
    struct stat status;
    int exist = stat(filePath, &status);
    
    if(exist != 0)
    {
        fprintf(file, "0 B;\t"); // File Size
        tp = localtime(&lastModTime);
        strftime(buffer, 32, "%Y/%m/%d-%H:%M:%S", tp);
        fprintf(file, "%s;\t", buffer); 
        fprintf(file, ";\t;\t;\t"); // Modified, Accesse, Created
        printf("%s -> ", filePath);
        GetErrorMessage();
    }
    else
    {
        fprintf(file, "%d B;\t", status.st_size); // File Size
        tp = localtime(&lastModTime);
        strftime(buffer, 32, "%Y/%m/%d-%H:%M:%S", tp);
        fprintf(file, "%s;\t", buffer); 
        tp = localtime(&status.st_mtime);
        strftime(buffer, 32, "%Y/%m/%d-%H:%M:%S", tp);
        fprintf(file, "%s;\t", buffer); 
        tp = localtime(&status.st_atime);
        strftime(buffer, 32, "%Y/%m/%d-%H:%M:%S", tp);
        fprintf(file, "%s;\t", buffer); 
        tp = localtime(&status.st_ctime);
        strftime(buffer, 32, "%Y/%m/%d-%H:%M:%S", tp);
        fprintf(file, "%s;\t", buffer); 
    }  
}

BOOL parseWinXP(PUCHAR dataBuffer, DWORD dataSize, const char* fileName)
{
    FILE * file = fopen(fileName, "w");
    fprintf(file, "FilePath;\tCached File Size;\t File Size;\t Cached Modified;\tModified;\tAccessed;\tCreated;\tExecuted;\tKey Path\n");

    CHAR filePath[512];
    UINT entryCount = *(UINT*)(dataBuffer + WINXP_ENTRY_COUNT_OFFSET);
    PUCHAR index = dataBuffer + WINXP_HEADER_SIZE;
    printf("entryCount: %d\n", entryCount);
    UINT counter = 0;
    while( index < dataBuffer + dataSize && counter < entryCount) {
        struct WINXP_Entry entry = *(struct WINXP_Entry*)index;
        if(*(QWORD*)entry.filePath != 0x005C003F003F005C) // "\??\" file prefix 
        {
            break;
        }
        wcstombs(filePath, (const wchar_t *)(entry.filePath + WINXP_FILE_OFFSET), MAX_PATH + 4);
        index += WINXP_ENTRY_SIZE;        
        counter++;
        
        fprintf(file, "%s;\t", filePath);
        fprintf(file, ";\t"); // cached file size
        insertFileTime(file, filePath, filetime_to_timet(entry.lastModTime));
        fprintf(file, ";\t"); // executed flag
        fprintf(file, "%s\n", APPCOMPATCACHE_PATH_XP);
    }
    fclose(file);
    return TRUE;
}

BOOL parseWinVista(PUCHAR dataBuffer, DWORD dataSize, const char* fileName, BOOL is32bit)
{
    FILE * file = fopen(fileName, "w");
    fprintf(file, "FilePath;\tCached File Size;\t File Size;\t Cached Modified;\tModified;\tAccessed;\tCreated;\tExecuted;\tKey Path\n");
    
    PUCHAR pathOffset;
    USHORT pathLength; 
    FILETIME lastModTime;
    DWORD entrySize = (is32bit ? WINVISTA_32_ENTRY_SIZE : WINVISTA_64_ENTRY_SIZE );

    CHAR filePath[512];
    UINT entryCount = *(UINT*)(dataBuffer + WINVISTA_ENTRY_COUNT_OFFSET);
    PUCHAR index = dataBuffer + WINVISTA_HEADER_SIZE;
    printf("entryCount: %d\n", entryCount);
    UINT counter = 0;
    while( index < dataBuffer + dataSize && counter < entryCount) {
        if(is32bit)
        {
            struct WINVISTA_32_Entry entry = *(struct WINVISTA_32_Entry*)index;
            lastModTime = entry.lastModTime;
            pathLength = entry.pathLength - WINVISTA_FILE_OFFSET;
            pathOffset = dataBuffer + WINVISTA_FILE_OFFSET + entry.pathOffset;
        }
        else
        {
            struct WINVISTA_64_Entry entry = *(struct WINVISTA_64_Entry*)index;
            lastModTime = entry.lastModTime;
            pathLength = entry.pathLength - WINVISTA_FILE_OFFSET;
            pathOffset = dataBuffer + WINVISTA_FILE_OFFSET + entry.pathOffset;
        }

        wcstombs(filePath, (const wchar_t *)pathOffset, pathLength);
        index += entrySize;        
        counter++;
        
        fprintf(file, "%s;\t", filePath);
        fprintf(file, ";\t"); // cached file size
        insertFileTime(file, filePath, filetime_to_timet(lastModTime));
        fprintf(file, ";\t"); // executed flag
        fprintf(file, "%s\n", APPCOMPATCACHE_PATH);
    }
    fclose(file);
    return TRUE;
}

BOOL parseWin7(PUCHAR dataBuffer, DWORD dataSize, const char* fileName, BOOL is32bit)
{
    FILE * file = fopen(fileName, "w");
    fprintf(file, "FilePath;\tCached File Size;\t File Size;\t Cached Modified;\tModified;\tAccessed;\tCreated;\tExecuted;\tKey Path\n");
    
    PUCHAR pathOffset;
    USHORT pathLength; 
    FILETIME lastModTime;
    DWORD entrySize = (is32bit ? WIN7_32_ENTRY_SIZE : WIN7_64_ENTRY_SIZE );

    CHAR filePath[512];
    UINT entryCount = *(UINT*)(dataBuffer + WIN7_ENTRY_COUNT_OFFSET);
    PUCHAR index = dataBuffer + WIN7_HEADER_SIZE;
    printf("entryCount: %d\n", entryCount);
    UINT counter = 0;
    while( index < dataBuffer + dataSize && counter < entryCount) {
        if(is32bit)
        {
            struct WIN7_32_Entry entry = *(struct WIN7_32_Entry*)index;
            lastModTime = entry.lastModTime;
            pathLength = entry.pathLength - WIN7_FILE_OFFSET;
            pathOffset = dataBuffer + WIN7_FILE_OFFSET + entry.pathOffset;
        }
        else
        {
            struct WIN7_64_Entry entry = *(struct WIN7_64_Entry*)index;
            lastModTime = entry.lastModTime;
            pathLength = entry.pathLength - WIN7_FILE_OFFSET;
            pathOffset = dataBuffer + WIN7_FILE_OFFSET + entry.pathOffset;
        }

        wcstombs(filePath, (const wchar_t *)pathOffset, pathLength);
        index += entrySize;        
        counter++;

        fprintf(file, "%s;\t", filePath);
        fprintf(file, ";\t"); // cached file size
        insertFileTime(file, filePath, filetime_to_timet(lastModTime));
        fprintf(file, ";\t"); // executed flag
        fprintf(file, "%s\n", APPCOMPATCACHE_PATH);
    }
    fclose(file);
    return TRUE;
}

BOOL parseWin8(PUCHAR dataBuffer, DWORD dataSize, const char* fileName, const DWORD magicNumber)
{
    FILE * file = fopen(fileName, "w");
    fprintf(file, "FilePath;\tCached File Size;\t File Size;\t Cached Modified;\tModified;\tAccessed;\tCreated;\tExecuted;\tKey Path\n");
    
    CHAR filePath[512];
    PUCHAR index = dataBuffer + WIN8_HEADER_SIZE;
    while( index < dataBuffer + dataSize) {        
        if( *((UINT32*)index) != magicNumber) {
            break;
        }
        
        UINT entryLength = *(UINT*)(index + WIN8_ENTRY_LENGTH_OFFSET);
        USHORT pathLength = *(USHORT*)(index + WIN8_PATH_LENGTH_OFFSET);
        wcstombs(filePath, (const wchar_t *) (index + WIN8_PACKAGE_OFFSET), pathLength);
        USHORT packageLength = *(USHORT*)(index + WIN8_PACKAGE_OFFSET + pathLength);
        FILETIME lastModTime = *(FILETIME*)(index + WIN8_FILETIME_OFFSET + pathLength + packageLength);
        index += WIN8_ENTRY_HEADER_SIZE + entryLength;
        
        fprintf(file, "%s;\t", filePath);
        fprintf(file, ";\t"); // cached file size
        insertFileTime(file, filePath, filetime_to_timet(lastModTime));
        fprintf(file, ";\t"); // executed flag
        fprintf(file, "%s\n", APPCOMPATCACHE_PATH);
    }
    fclose(file);
    return TRUE;
}

// WIN10_ENTRY_COUNT_OFFSET ile toplam entry sayısı kontrol edilebilir.
BOOL parseWin10(PUCHAR dataBuffer, DWORD dataSize, const char* fileName, const DWORD headerSize)
{
    FILE * file = fopen(fileName, "w");
    fprintf(file, "FilePath;\tCached File Size;\t File Size;\t Cached Modified;\tModified;\tAccessed;\tCreated;\tExecuted;\tKey Path\n");
    
    CHAR filePath[512];
    PUCHAR index = dataBuffer + headerSize;
    while( index < dataBuffer + dataSize) {        
        if( *((UINT32*)index) != WIN10_MAGIC_NUMBER) {
            break;
        }
        
        UINT entryLength = *(UINT*)(index + WIN10_ENTRY_LENGTH_OFFSET);
        USHORT pathLength = *(USHORT*)(index + WIN10_PATH_LENGTH_OFFSET);
        wcstombs(filePath, (const wchar_t *) (index + WIN10_FILETIME_OFFSET), pathLength);
        filePath[pathLength/2] = 0;
        FILETIME lastModTime = *(FILETIME*)(index + WIN10_FILETIME_OFFSET + pathLength);
        index += WIN10_ENTRY_HEADER_SIZE + entryLength;

        fprintf(file, "%s;\t", filePath);
        fprintf(file, ";\t"); // cached file size
        insertFileTime(file, filePath, filetime_to_timet(lastModTime));
        fprintf(file, ";\t"); // executed flag
        fprintf(file, "%s\n", APPCOMPATCACHE_PATH);
    }
    fclose(file);
    return TRUE;
}

enum WINDOWS_VERSION checkVersion(PUCHAR dataBuffer, DWORD dataSize)
{
    if(WINXP_MAGIC_NUMBER == *(UINT32*)dataBuffer)
    {
        return WIN_XP;    
    }
    
    if(WINVISTA_MAGIC_NUMBER == *(UINT32*)dataBuffer)
    {
        UINT32 fileSizeLow;
        UINT16 length = *(UINT16*)(dataBuffer + WINVISTA_HEADER_SIZE);
        UINT16 maxlength = *(UINT16*)(dataBuffer + WINVISTA_HEADER_SIZE + 2);
        UINT32 padding = *(UINT32*)(dataBuffer + WINVISTA_HEADER_SIZE + 4);
        if (maxlength - length == 0x02) {
            if (padding == 0x00000000)
            {
                fileSizeLow = *(UINT32*)(dataBuffer + WINVISTA_HEADER_SIZE + WIN2K3_64_FILESIZE_OFFSET);
                if(fileSizeLow > 3)
                {
                    return WIN_2K3_64;
                }
                else
                {
                    return WIN_VISTA_64;
                }
            }
            else
            {
                fileSizeLow = *(UINT32*)(dataBuffer + WINVISTA_HEADER_SIZE + WIN2K3_32_FILESIZE_OFFSET);
                if(fileSizeLow > 3)
                {
                    return WIN_2K3_32;
                }
                else
                {
                    return WIN_VISTA_32;
                }
            }
        }    
    }
    
    if(WIN7_MAGIC_NUMBER == *(UINT32*)dataBuffer)
    {
        UINT16 length = *(UINT16*)(dataBuffer + WIN7_HEADER_SIZE);
        UINT16 maxlength = *(UINT16*)(dataBuffer + WIN7_HEADER_SIZE + 2);
        UINT32 padding = *(UINT32*)(dataBuffer + WIN7_HEADER_SIZE + 4);
        
        if (maxlength - length == 0x02) {
            if (padding == 0x00000000)
            {
                return WIN_7_64;
            }
            else
            {
                return WIN_7_32;
            }
        }
    }
    
    if(WIN8_0_MAGIC_NUMBER == *(UINT32*)(dataBuffer + WIN8_HEADER_SIZE))
    {
        return WIN_8_0;    
    }
    
    if(WIN8_1_MAGIC_NUMBER == *(UINT32*)(dataBuffer + WIN8_HEADER_SIZE))
    {
        return WIN_8_1;    
    }
    
    if(WIN10_MAGIC_NUMBER == *(UINT32*)(dataBuffer + WIN10_HEADER_SIZE))
    {
        return WIN_10;    
    }
    
    if(WIN10_MAGIC_NUMBER == *(UINT32*)(dataBuffer + WIN10C_HEADER_SIZE))
    {
        return WIN_10_C;
    }
    
    return UNDEFINED;
}

BOOL parse(PUCHAR dataBuffer, DWORD dataSize)
{
    BOOL ret = FALSE;
    switch (checkVersion(dataBuffer, dataSize))
    {
    case WIN_XP:
        printf("WIN_XP parser\n");
        ret = parseWinXP(dataBuffer, dataSize, "AppCompatCache_Win_XP.txt");
        break;
    case WIN_2K3_32:
        printf("WIN_2K3_32 parser\n");
        ret = parseWinVista(dataBuffer, dataSize, "AppCompatCache_Win_2k3_32.txt", TRUE);
        break;
    case WIN_2K3_64:
        printf("WIN_2K3_64 parser\n");
        ret = parseWinVista(dataBuffer, dataSize, "AppCompatCache_Win_2k3_XP_64.txt", FALSE);
        break;
    case WIN_VISTA_32:
        printf("WIN_VISTA_32 parser\n");
        ret = parseWinVista(dataBuffer, dataSize, "AppCompatCache_Win_Vista_2k8_32.txt", TRUE);
        break;
    case WIN_VISTA_64:
        printf("WIN_VISTA_64 parser\n");
        ret = parseWinVista(dataBuffer, dataSize, "AppCompatCache_Win_Vista_2k8_64.txt", FALSE);
        break;
    case WIN_7_32:
        printf("WIN_7_32 parser\n");
        ret = parseWin7(dataBuffer, dataSize, "AppCompatCache_Win_7_32.txt", TRUE);
        break;
    case WIN_7_64:
        printf("WIN_7_64 parser\n");
        ret = parseWin7(dataBuffer, dataSize, "AppCompatCache_Win_7_64.txt", FALSE);
        break;
    case WIN_8_0:
        printf("WIN_8_0 parser\n");
        ret = parseWin8(dataBuffer, dataSize, "AppCompatCache_Win_8_0.txt", WIN8_0_MAGIC_NUMBER);
        break;
    case WIN_8_1:
        printf("WIN_8_1 parser\n");
        ret = parseWin8(dataBuffer, dataSize, "AppCompatCache_Win_8_1.txt", WIN8_1_MAGIC_NUMBER);
        break;
    case WIN_10:
        printf("WIN_10 parser\n");
        ret = parseWin10(dataBuffer, dataSize, "AppCompatCache_Win_10.txt", WIN10_HEADER_SIZE);
        break;
    case WIN_10_C:
        printf("WIN_10_C parser\n");
        ret = parseWin10(dataBuffer, dataSize, "AppCompatCache_Win_10_C.txt", WIN10C_HEADER_SIZE);
        break;
    default:
        break;
    }
    return ret;
};
