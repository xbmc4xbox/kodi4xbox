#include "unimplemented.h"

#include <wchar.h>

int WideCharToMultiByte(unsigned int page, unsigned long flags, const wchar_t* src, int srclen, char* dst, int dstlen, const char* defchar, bool *used )
{
  return _WideCharToMultiByte(page, flags, reinterpret_cast<LPCWSTR>(src), srclen, dst, dstlen, defchar, reinterpret_cast<BOOL*>(used));
}

int MultiByteToWideChar(unsigned int page, unsigned long flags, const char* src, int srclen, wchar_t* dst, int dstlen )
{
  return _MultiByteToWideChar(page, flags, src, srclen, reinterpret_cast<LPWSTR>(dst), dstlen);
}

INT _WideCharToMultiByte(UINT page, DWORD flags, LPCWSTR src, INT srclen, LPSTR dst, INT dstlen, LPCSTR defchar, BOOL *used )
{
  if (page != CP_UTF8)
    return 0;

  int i;

  if (!src || !srclen || (!dst && dstlen))
  {
    SetLastError( ERROR_INVALID_PARAMETER );
    return 0;
  }

  if (srclen < 0) srclen = wcslen((wchar_t*)src) + 1;

  if(!dstlen)
    return srclen;

  for(i=0; i<srclen && i<dstlen; i++)
    dst[i] = src[i] & 0xFF;

  if (used) *used = FALSE;

  return i;
}

INT _MultiByteToWideChar(UINT page, DWORD flags, LPCSTR src, INT srclen, LPWSTR dst, INT dstlen )
{
  if (page != CP_UTF8)
    return 0;

  int i;

  if (!src || !srclen || (!dst && dstlen))
  {
    SetLastError( ERROR_INVALID_PARAMETER );
    return 0;
  }

  if (srclen < 0) srclen = strlen(src) + 1;

  if(!dstlen)
    return srclen;

  for(i=0; i<srclen && i<dstlen; i++)
    dst[i] = src[i];

  return i;
}
