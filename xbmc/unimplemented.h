// contains all functions, variables and preprocessors which are needed by Kodi, but missing in NXDK
// IMPORTANT: Some of them have valid, but most of them have dummy implementation which is not working

#ifndef UNIMPLEMENTED_H
#define UNIMPLEMENTED_H
#include <windows.h>

#define CP_UTF8 1
#define WC_ERR_INVALID_CHARS 0x00000080
#define MB_ERR_INVALID_CHARS 0x00000008

int WideCharToMultiByte(unsigned int page, unsigned long flags, const wchar_t* src, int srclen, char* dst, int dstlen, const char* defchar, bool *used );
int MultiByteToWideChar(unsigned int page, unsigned long flags, const char* src, int srclen, wchar_t* dst, int dstlen );

INT _WideCharToMultiByte(UINT page, DWORD flags, LPCWSTR src, INT srclen, LPSTR dst, INT dstlen, LPCSTR defchar, BOOL *used );
INT _MultiByteToWideChar(UINT page, DWORD flags, LPCSTR src, INT srclen, LPWSTR dst, INT dstlen );

BOOL LocalFileTimeToFileTime(const FILETIME *lpLocalFileTime, LPFILETIME lpFileTime);

#endif // UNIMPLEMENTED_H
