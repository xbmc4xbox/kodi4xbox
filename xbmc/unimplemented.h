// contains all functions, variables and preprocessors which are needed by Kodi, but missing in NXDK
// IMPORTANT: Some of them have valid, but most of them have dummy implementation which is not working

#ifndef UNIMPLEMENTED_H
#define UNIMPLEMENTED_H
#include <windows.h>
#include <string>
#include <wchar.h>
#include <locale>
#include <codecvt>

#define CP_UTF8 1
#define WC_ERR_INVALID_CHARS 0x00000080
#define MB_ERR_INVALID_CHARS 0x00000008

int WideCharToMultiByte(unsigned int codePage, unsigned long flags,
                        const wchar_t* wideStr, int wideLen,
                        char* multiByteStr, int multiByteLen,
                        const char* defaultChar, bool* usedDefaultChar);

int MultiByteToWideChar(unsigned int codePage, unsigned long flags,
                        const char* multiByteStr, int multiByteLen,
                        wchar_t* wideStr, int wideLen);

BOOL LocalFileTimeToFileTime(const FILETIME *lpLocalFileTime, LPFILETIME lpFileTime);

#endif // UNIMPLEMENTED_H
