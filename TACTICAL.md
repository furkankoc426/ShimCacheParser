# TACTICAL

TACTICAL is a very comprehensive evidence collection tool. The purpose of TACTICAL is to help the researcher gather evidence in the system within seconds without wasting time in the case of DFIR. 

Some features of TACTICAL are:
 * The fastest evidence colllect tools
 * Easy to install and use
 * Support GUI and terminal
 * Quite practical. With a few clicks or a single command, all system evidence is recorded. 
 * TACTICAL can work on a computer as well as over a USB dongle and network. 
 * Reporting in html format


# Application Combability Cache - AppCompatCache (Shim Cache)

AppCompatCache is one of the parts designed by Microsoft to avoid compatibility issues in Windows operating systems. DFIR researchers use AppCompatCache to detect malicious softwares. AppCompatCache is written when only system reboot or shutdown.

AppCompatCache entries can be updated in two scenario:
 * An executable file is ran.
 * An executable or library file are displayed on explorer. 

Some features of AppCompatCache are: 
 * In Chronological Order
 * Gives Information About Last Startup
 * 

AppCompatCache can include:
 * File Full Path
 * Last Modified Time
 * File Size
 * AppCompatCache Last Updated Time
 * Process Execution Flag

AppcompatCache file located at:
 * HKLM\SYSTEM\CurrentControlSet\Control\Session Manager\AppCompatibility\AppCompatCache on XP systems.
 * HKLM\SYSTEM\CurrentControlSet\Control\Session Manager\AppCompatCache\AppCompatCache on upper version from XP systems.

## Windows XP

```c
//32-bit WinXP AppCompatCache Structure
typedef struct AppCompatCacheEntry_XP{
    WCHAR Path[MAX_PATH+4];
    FILETIME ftLastModTime;
    LARGE_INTEGER qwFileSize;
    FILETIME ftLastUpdateTime;
} APPCOMPATCACHE_ENTRY32_XP;
```

## Windows Server 2003

```c
//32-bit Win2k3 AppCompatCache Structure
typedef struct AppCompatCacheEntry32_2k3 {
    USHORT wLength;
    USHORT wMaximumLength;
    DWORD dwPathOffset;
    FILETIME ftLastModTime;
    LARGE_INTEGER qwFileSize;
} APPCOMPATCACHE_ENTRY32_2k3;
//64-bit Win2k3 AppCompatCache Structure
typedef struct AppCompatCacheEntry64_2k3 {
    USHORT wLength;
    USHORT wMaximumLength;
    DWORD dwPadding;
    QWORD dwPathOffset;
    FILETIME ftLastModTime;
    LARGE_INTEGER qwFileSize;
} APPCOMPATCACHE_ENTRY64_2k3;
```

## Windows Vista and Windows Server 2008

```c
//32-bit Vista/2k8 AppCompatCache Entry Structure
typedef struct AppCompatCacheEntry32_Vista {
    USHORT wLength;
    USHORT wMaximumLength;
    DWORD dwPathOffset;
    FILETIME ftLastModTime;
    DWORD dwInsertFlags;
    DWORD dwFlags;
} APPCOMPATCACHE_ENTRY32_VISTA;
//64-bit Vista/2k8 AppCompatCache Entry Structure
typedef struct AppCompatCacheEntry64_Vista {
    USHORT wLength;
    USHORT wMaximumLength;
    DWORD dwPadding;
    QWORD dwPathOffset;
    FILETIME ftLastModTime;
    DWORD dwInsertFlags;
    DWORD dwFlags;
} APPCOMPATCACHE_ENTRY64_VISTA;
```

## Windows 7 and Windows Server 2008 R2

```c
//32-bit Win7/2k8R2 AppCompatCache Entry Structure
typedef struct AppCompatCacheEntry32_Win7{
    USHORT wLength;
    USHORT wMaximumLength;
    DWORD dwPathOffset;
    FILETIME ftLastModTime;
    DWORD dwInsertFlags;
    DWORD dwShimFlags;
    DWORD dwBlobSize;
    DWORD dwBlobOffset;
} APPCOMPATCACHE_ENTRY32_WIN7;

//64-bit Win7/2k8R2 AppCompatCache Entry Structure
typedef struct AppCompatCacheEntry64_Win7{
    USHORT wLength;
    USHORT wMaximumLength;
    DWORD dwPadding;
    QWORD dwPathOffset;
    FILETIME ftLastModTime;
    DWORD dwInsertFlags;
    DWORD dwShimFlags;
    QWORD qwBlobSize;
    QWORD qwBlobOffset;
} APPCOMPATCACHE_ENTRY64_WIN7;
```

| Operating System    | Header Size | Magic Word |  Entry Size |  Entry Count |
| :-----------------: | :---------: | :--------: |  :--------: | :----------: |
| WinXP (32 bit)      | 400 byte    | 0xDEADBEEF |  552 byte   | 96           |
| Win2k3 (32 bit)     | 8 byte      | 0xBADC0FFE |  24 byte    | 512          |
| Win2k3 (64 bit)     | 8 byte      | 0xBADC0FFE |  32 byte    | 512          |
| Vista/2k8 (32 bit)  | 8 byte      | 0xBADC0FFE |  24 byte    | 1024         |
| Vista/2k8 (64 bit)  | 8 byte      | 0xBADC0FFE |  32 byte    | 1024         |
| Win7/2k8R2 (32 bit) | 128 byte    | 0xBADC0FFE |  32 byte    | 1024         |
| Win7/2k8R2 (64 bit) | 128 byte    | 0xBADC0FFE |  48 byte    | 1024         |
| Win8 (64 bit)       | 128 byte    | 0x73743030 |  x byte     | 1024         |
| Win8.1 (64 bit)     | 128 byte    | 0x73743031 |  x byte     | 1024         |
| Win10 (64 bit)      | 48 byte     | 0x73743031 |  x byte     | 1024         |

Parsers
Eric Zimmerman - https://www.sans.org/tools/appcompatcacheparser/
MANDIANT -       https://github.com/mandiant/ShimCacheParser
opentext -       https://security.opentext.com/appDetails/ShimCache-Parser

References:
https://lifars.com/wp-content/uploads/2017/03/Technical_tool_Amcache_Shimcache.pdf
https://www.mandiant.com/resources/caching-out-the-val







