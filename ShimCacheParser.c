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

BOOL parseWin7_32(PUCHAR dataBuffer, DWORD dataSize)
{

}

BOOL parseWin7_64(PUCHAR dataBuffer, DWORD dataSize)
{

}

BOOL parseWin8Base(PUCHAR dataBuffer, DWORD dataSize, const DWORD magicNumber, const char* fileName)
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
        FILETIME filetime = *(FILETIME*)(index + WIN8_FILETIME_OFFSET + pathLength + packageLength);
        SYSTEMTIME st;    
        FileTimeToSystemTime(&filetime, &st);
        fwrite(filePath, pathLength/2, 1, file);
        fprintf(file, ";%d/%02d/%02d-%02d:%02d:%02d\n", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);

        index += WIN8_ENTRY_HEADER_SIZE + entryLength;
    }
    fclose(file);
    return TRUE;
}

BOOL parseWin8_0(PUCHAR dataBuffer, DWORD dataSize)
{
    return parseWin8Base(dataBuffer, dataSize, WIN8_0_MAGIC_NUMBER, "AppCompatCache_Win_8_0.txt");
}

BOOL parseWin8_1(PUCHAR dataBuffer, DWORD dataSize)
{
    return parseWin8Base(dataBuffer, dataSize, WIN8_1_MAGIC_NUMBER, "AppCompatCache_Win_8_1.txt");
}

BOOL parseWin10Base(PUCHAR dataBuffer, DWORD dataSize, const DWORD headerSize, const char* fileName)
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
        FILETIME filetime = *(FILETIME*)(index + WIN10_FILETIME_OFFSET + pathLength);
        SYSTEMTIME st;    
        FileTimeToSystemTime(&filetime, &st);
        fwrite(filePath, pathLength/2, 1, file);
        fprintf(file, ";%d/%02d/%02d-%02d:%02d:%02d\n", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);

        index += WIN10_ENTRY_HEADER_SIZE + entryLength;
    }
    fclose(file);
    return TRUE;
}

// WIN10_ENTRY_COUNT_OFFSET ile toplam entry sayısı kontrol edilebilir.
BOOL parseWin10(PUCHAR dataBuffer, DWORD dataSize)
{
    return parseWin10Base(dataBuffer, dataSize, WIN10_HEADER_SIZE, "AppCompatCache_Win_10.txt");
}

BOOL parseWin10_C(PUCHAR dataBuffer, DWORD dataSize)
{
    return parseWin10Base(dataBuffer, dataSize, WIN10C_HEADER_SIZE, "AppCompatCache_Win_10_C.txt");;
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
        ret = parseWin7_32(dataBuffer, dataSize);
        break;
    case WIN_7_64:
        printf("WIN_7_64 parser\n");
        ret = parseWin7_64(dataBuffer, dataSize);
        break;
    case WIN_8_0:
        printf("WIN_8_0 parser\n");
        ret = parseWin8_0(dataBuffer, dataSize);
        break;
    case WIN_8_1:
        printf("WIN_8_1 parser\n");
        ret = parseWin8_1(dataBuffer, dataSize);
        break;
    case WIN_10:
        printf("WIN_10 parser\n");
        ret = parseWin10(dataBuffer, dataSize);
        break;
    case WIN_10_C:
        printf("WIN_10_C parser\n");
        ret = parseWin10_C(dataBuffer, dataSize);
        break;
    default:
        break;
    }
    return ret;
};
