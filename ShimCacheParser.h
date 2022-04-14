#ifndef SHIM_CACHE_PARSER_H
#define SHIM_CACHE_PARSER_H

#include <windows.h>
typedef unsigned long long QWORD; 

static LPCSTR APPCOMPATCACHE_PATH = "SYSTEM\\CurrentControlSet\\Control\\Session Manager\\AppCompatCache";
static LPCSTR APPCOMPATCACHE_PATH_XP = "SYSTEM\\CurrentControlSet\\Control\\Session Manager\\AppCompatibility";
static LPCSTR APPCOMPATCACHE_NAME = "AppCompatCache";

enum WINDOWS_VERSION 
{
    UNDEFINED,
    WIN_XP,
    WIN_2K3_32,
    WIN_2K3_64,
    WIN_VISTA_32,
    WIN_VISTA_64,
    WIN_7_32,
    WIN_7_64,
    WIN_8_0,
    WIN_8_1,
    WIN_10,
    WIN_10_C
};

BOOL parse(PUCHAR dataBuffer, DWORD dataSize);

#endif //SHIM_CACHE_PARSER_H