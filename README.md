# TACTICAL

TACTICAL is a very comprehensive evidence collection tool. The purpose of TACTICAL is to help the researcher gather evidence in the system within seconds without wasting time in the case of DFIR. TACTICAL can collect more than 150 evidence types from host machine. 

Some features of TACTICAL are:
 * The fastest evidence collection tool
 * Easy to install and use
 * Support GUI and terminal
 * Quite practical. With a few clicks or a single command, all system evidence is recorded. 
 * TACTICAL can work on a computer as well as over a USB dongle and network. 
 * Reporting in html format 

Main Headings of Evidence Types:
 * System Info
 * Processes
 * Autoruns
 * RAM Image
 * Custom Content
 * Files
 * Downloads
 * Drivers
 * Event Logs
 * Registery
 * Installed Applications
 * Disk
 * Browser
 * Other Evidence
 * Artifacts
 * Recycle Bin
 * Screenshots
 * Prefetch
 * SRUM
 * Amcache
 * VMI
 * Clipboard
 * Firewall Rules
 * Network
 * Crash Dumps
 * System Restore

# Application Combability Cache - AppCompatCache (Shim Cache)

AppCompatCache is one of the parts designed by Microsoft to avoid compatibility issues in Windows operating systems.
DFIR researchers use AppCompatCache to detect malicious softwares.
AppCompatCache is saved to disk when only system is rebooted or shut down.

AppCompatCache entries can be updated in two scenario:
 * An executable file is ran. (All versions)
 * An executable or library file are displayed on explorer. (Vista and upper) 

Some features of AppCompatCache are: 
 * In Chronological Order
 * Gives Information About Last Startup

AppCompatCache can include:
 * File Full Path
 * Last Modified Time
  ---
    Depend of versions
    * Cached File Size
    * AppCompatCache Entry Last Updated Time (Last Executed Time)
    * Application Execution Flag

AppcompatCache file located at:
 * HKLM\SYSTEM\CurrentControlSet\Control\Session Manager\AppCompatibility\AppCompatCache on XP systems.
 * HKLM\SYSTEM\CurrentControlSet\Control\Session Manager\AppCompatCache\AppCompatCache on upper version from XP systems.

| Operating System | File Path | Last Modified Time | Execution Information | File Size | Entry Last Updated Time |
| :--------------: | :-------: | :----------------: | :-------------------: | :-------: | :---------------------: |
|      WinXP       |     +     |         +          |           +           |     +     |            +            |
|      Win2k3      |     +     |         +          |           +           |     +     |            -            |
|    Vista/2k8     |     +     |         +          |           +           |     -     |            -            |
|    Win7/2k8R2    |     +     |         +          |           +           |     -     |            -            |
|       Win8       |     +     |         +          |           +           |     -     |            -            |
|      Win10       |     +     |         +          |           -           |     -     |            -            |

## Windows XP

```c
//32-bit WinXP AppCompatCache Structure
typedef struct WINXP_32_Entry
{
    WCHAR Path[MAX_PATH+4];
    FILETIME ftLastModTime;
    LARGE_INTEGER qwFileSize;
    FILETIME ftLastUpdateTime;
};
```

## Windows Server 2003

```c
//32-bit Win2k3 AppCompatCache Structure
struct WIN2K3_32_Entry 
{
    USHORT wLength;
    USHORT wMaximumLength;
    DWORD dwPathOffset;
    FILETIME ftLastModTime;
    LARGE_INTEGER qwFileSize;
};

//64-bit Win2k3 AppCompatCache Structure
struct WIN2K3_64_Entry 
{
    USHORT wLength;
    USHORT wMaximumLength;
    DWORD dwPadding;
    QWORD dwPathOffset;
    FILETIME ftLastModTime;
    LARGE_INTEGER qwFileSize;
};
```

## Windows Vista and Windows Server 2008

```c
//32-bit Vista/2k8 AppCompatCache Entry Structure
struct WINVISTA_32_Entry 
{
    USHORT wLength;
    USHORT wMaximumLength;
    DWORD dwPathOffset;
    FILETIME ftLastModTime;
    DWORD dwInsertFlags;
    DWORD dwFlags;
};

//64-bit Vista/2k8 AppCompatCache Entry Structure
struct WINVISTA_64_Entry 
{
    USHORT wLength;
    USHORT wMaximumLength;
    DWORD dwPadding;
    QWORD dwPathOffset;
    FILETIME ftLastModTime;
    DWORD dwInsertFlags;
    DWORD dwFlags;
};
```

## Windows 7 and Windows Server 2008 R2

```c
//32-bit Win7/2k8R2 AppCompatCache Entry Structure
struct WIN7_32_Entry
{
    USHORT wLength;
    USHORT wMaximumLength;
    DWORD dwPathOffset;
    FILETIME ftLastModTime;
    DWORD dwInsertFlags;
    DWORD dwShimFlags;
    DWORD dwBlobSize;
    DWORD dwBlobOffset;
};

//64-bit Win7/2k8R2 AppCompatCache Entry Structure
struct WIN7_64_Entry
{
    USHORT wLength;
    USHORT wMaximumLength;
    DWORD dwPadding;
    QWORD dwPathOffset;
    FILETIME ftLastModTime;
    DWORD dwInsertFlags;
    DWORD dwShimFlags;
    QWORD qwBlobSize;
    QWORD qwBlobOffset;
};
```

## Windows 8

```c
//Win8 AppCompatCache Entry Structure
struct WIN8_Entry
{
    DWORD entryLength;
    USHORT pathLength;
    CHAR filePath[pathLength]; 
    USHORT packageLength;
    CHAR package[packageLength];
    DWORD insertFlags;
    FILETIME lastModTime;
};
```

## Windows 10

```c
//Win10 AppCompatCache Entry Structure
struct WIN10_Entry
{
    DWORD entryLength;
    USHORT pathLength;
    CHAR filePath[pathLength]; 
    FILETIME lastModTime;
};
```

|  Operating System   | Header Size | Magic Word | Entry Size | Max Entry Count |
| :-----------------: | :---------: | :--------: | :--------: | :-------------: |
|   WinXP (32 bit)    |  400 byte   | 0xDEADBEEF |  552 byte  |       96        |
|   Win2k3 (32 bit)   |   8 byte    | 0xBADC0FFE |  24 byte   |       512       |
|   Win2k3 (64 bit)   |   8 byte    | 0xBADC0FFE |  32 byte   |       512       |
| Vista/2k8 (32 bit)  |   8 byte    | 0xBADC0FFE |  24 byte   |      1024       |
| Vista/2k8 (64 bit)  |   8 byte    | 0xBADC0FFE |  32 byte   |      1024       |
| Win7/2k8R2 (32 bit) |  128 byte   | 0xBADC0FFE |  32 byte   |      1024       |
| Win7/2k8R2 (64 bit) |  128 byte   | 0xBADC0FFE |  48 byte   |      1024       |
|    Win8 (64 bit)    |  128 byte   | 0x73743030 |   x byte   |      1024       |
|   Win8.1 (64 bit)   |  128 byte   | 0x73743031 |   x byte   |      1024       |
|   Win10 (64 bit)    |   48 byte   | 0x73743031 |   x byte   |      1024       |

# Parsers

MANDIANT -       https://github.com/mandiant/ShimCacheParser/

Eric Zimmerman - https://www.sans.org/tools/appcompatcacheparser/

opentext -       https://security.opentext.com/appDetails/ShimCache-Parser/

# Performance Comparison 

|   Parser   | Time (ms) |
| :--------: | :-------: |
| Zimmerman  | 862.1151  |
|  mandiant  | 100.8468  |
| My Parser* |  47.2812  |
* This version also check file existence and collect additional information (file size, created time, modified time, accessed time).


# References

https://www.mandiant.com/resources/caching-out-the-val

https://lifars.com/wp-content/uploads/2017/03/Technical_tool_Amcache_Shimcache.pdf

https://www.fireeye.com/content/dam/fireeye-www/services/freeware/shimcache-whitepaper.pdf







