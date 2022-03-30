#include <stdio.h>
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

const DWORD WIN7_HEADER_SIZE = 0x80;
const DWORD WIN7_MAGIC_NUMBER = 0xBADC0FEE;
const DWORD WIN7_ENTRY_COUNT_OFFSET = 0x04;
const DWORD WIN7_FILE_OFFSET = 0x08;
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

const DWORD WINVISTA_HEADER_SIZE = 0x08;
const DWORD WINVISTA_MAGIC_NUMBER = 0xBADC0FFE;

const DWORD WIN2K3_32_FILESIZE_OFFSET = 0x10;
const DWORD WIN2K3_64_FILESIZE_OFFSET = 0x18;

const DWORD WINXP_MAGIC_NUMBER = 0xDEADBEEF;

BOOL parseWinXP(PUCHAR dataBuffer, DWORD dataSize)
{

}

BOOL parseWin2K3_32(PUCHAR dataBuffer, DWORD dataSize)
{

}

BOOL parseWin2K3_64(PUCHAR dataBuffer, DWORD dataSize)
{

}

BOOL parseWinVista_32(PUCHAR dataBuffer, DWORD dataSize)
{

}

BOOL parseWinVista_64(PUCHAR dataBuffer, DWORD dataSize)
{

}

BOOL parseWin7(PUCHAR dataBuffer, DWORD dataSize, const char* fileName, BOOL is32bit)
{
    FILE * file = fopen(fileName, "w");
    fprintf(file, "FilePath;LastModifiedTime\n");
    
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
        SYSTEMTIME st;    
        FileTimeToSystemTime(&lastModTime, &st);
        fwrite(filePath, pathLength/2, 1, file);
        fprintf(file, ";%d/%02d/%02d-%02d:%02d:%02d\n", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);

        index += entrySize;        
        counter++;
    }
}

BOOL parseWin8(PUCHAR dataBuffer, DWORD dataSize, const char* fileName, const DWORD magicNumber)
{
    FILE * file = fopen(fileName, "w");
    fprintf(file, "FilePath;LastModifiedTime\n");
    
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
        SYSTEMTIME st;    
        FileTimeToSystemTime(&lastModTime, &st);
        fwrite(filePath, pathLength/2, 1, file);
        fprintf(file, ";%d/%02d/%02d-%02d:%02d:%02d\n", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);

        index += WIN8_ENTRY_HEADER_SIZE + entryLength;
    }
    fclose(file);
    return TRUE;
}

// WIN10_ENTRY_COUNT_OFFSET ile toplam entry sayısı kontrol edilebilir.
BOOL parseWin10(PUCHAR dataBuffer, DWORD dataSize, const char* fileName, const DWORD headerSize)
{
    FILE * file = fopen(fileName, "w");
    fprintf(file, "FilePath;LastModifiedTime\n");
    
    CHAR filePath[512];
    PUCHAR index = dataBuffer + headerSize;
    while( index < dataBuffer + dataSize) {        
        if( *((UINT32*)index) != WIN10_MAGIC_NUMBER) {
            break;
        }
        
        UINT entryLength = *(UINT*)(index + WIN10_ENTRY_LENGTH_OFFSET);
        USHORT pathLength = *(USHORT*)(index + WIN10_PATH_LENGTH_OFFSET);
        wcstombs(filePath, (const wchar_t *) (index + WIN10_FILETIME_OFFSET), pathLength);
        FILETIME lastModTime = *(FILETIME*)(index + WIN10_FILETIME_OFFSET + pathLength);
        SYSTEMTIME st;    
        FileTimeToSystemTime(&lastModTime, &st);
        fwrite(filePath, pathLength/2, 1, file);
        fprintf(file, ";%d/%02d/%02d-%02d:%02d:%02d\n", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);

        index += WIN10_ENTRY_HEADER_SIZE + entryLength;
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
        fileSizeLow = *(UINT32*)(dataBuffer + WINVISTA_HEADER_SIZE + WIN2K3_32_FILESIZE_OFFSET);
        if(fileSizeLow > 3)
        {
            return WIN_2K3_32;
        }

        fileSizeLow = *(UINT32*)(dataBuffer + WINVISTA_HEADER_SIZE + WIN2K3_64_FILESIZE_OFFSET);
        if(fileSizeLow > 3)
        {
            return WIN_2K3_64;
        }

        UINT16 length = *(UINT16*)(dataBuffer + WINVISTA_HEADER_SIZE);
        UINT16 maxlength = *(UINT16*)(dataBuffer + WINVISTA_HEADER_SIZE + 2);
        UINT32 padding = *(UINT32*)(dataBuffer + WINVISTA_HEADER_SIZE + 4);
        if (maxlength - length == 0x02) {
            if (padding == 0x00000000)
            {
                return WIN_VISTA_64;
            }
            else
            {
                return WIN_VISTA_32;
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
        ret = parseWinXP(dataBuffer, dataSize);
        break;
    case WIN_2K3_32:
        printf("WIN_2K3_32 parser\n");
        ret = parseWin2K3_32(dataBuffer, dataSize);
        break;
    case WIN_2K3_64:
        printf("WIN_2K3_64 parser\n");
        ret = parseWin2K3_64(dataBuffer, dataSize);
        break;
    case WIN_VISTA_32:
        printf("WIN_VISTA_32 parser\n");
        ret = parseWinVista_32(dataBuffer, dataSize);
        break;
    case WIN_VISTA_64:
        printf("WIN_VISTA_64 parser\n");
        ret = parseWinVista_64(dataBuffer, dataSize);
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
