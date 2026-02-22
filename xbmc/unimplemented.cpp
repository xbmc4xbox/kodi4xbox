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

static const uint64_t EPOCH_DIFFERENCE = 116444736000000000ULL; // 1601 to 1970 in 100ns units
static const uint64_t HUNDRED_NS_PER_SECOND = 10000000ULL;

BOOL LocalFileTimeToFileTime(const FILETIME *lpLocalFileTime, LPFILETIME lpFileTime)
{
  uint64_t localTimeValue = ((uint64_t)lpLocalFileTime->dwHighDateTime << 32) | lpLocalFileTime->dwLowDateTime;

  TIME_ZONE_INFORMATION tzInfo;
  DWORD result = GetTimeZoneInformation(&tzInfo);
  if (result == TIME_ZONE_ID_INVALID)
    return 0;

  int64_t bias = (int64_t)tzInfo.Bias * 600000000LL;
  if (result == TIME_ZONE_ID_STANDARD)
    bias += (int64_t)tzInfo.StandardBias * 600000000LL;
  else if (result == TIME_ZONE_ID_DAYLIGHT)
    bias += (int64_t)tzInfo.DaylightBias * 600000000LL;

  uint64_t fileTimeValue = localTimeValue + bias;

  lpFileTime->dwLowDateTime = (uint32_t)fileTimeValue;
  lpFileTime->dwHighDateTime = (uint32_t)(fileTimeValue >> 32);
  return 1;
}
