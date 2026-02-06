#include "unimplemented.h"

int WideCharToMultiByte(unsigned int codePage, unsigned long flags,
                        const wchar_t* wideStr, int wideLen,
                        char* multiByteStr, int multiByteLen,
                        const char* defaultChar, bool* usedDefaultChar)
{
  if (codePage != CP_UTF8)
    return 0;

  std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
  std::string utf8Str = converter.to_bytes(wideStr, wideStr + wideLen);

  if (multiByteStr && multiByteLen > 0)
  {
    size_t copyLen = (utf8Str.size() < (size_t)multiByteLen) ? utf8Str.size() : (size_t)multiByteLen;
    memcpy(multiByteStr, utf8Str.c_str(), copyLen);
    return (int)copyLen;
  }

  return (int)utf8Str.size();
}

int MultiByteToWideChar(unsigned int codePage, unsigned long flags,
                        const char* multiByteStr, int multiByteLen,
                        wchar_t* wideStr, int wideLen)
{
  if (codePage != CP_UTF8)
    return 0;

  std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
  std::wstring wide = converter.from_bytes(multiByteStr, multiByteStr + multiByteLen);

  if (wideStr && wideLen > 0)
  {
    size_t copyLen = (wide.size() < (size_t)wideLen) ? wide.size() : (size_t)wideLen;
    wcsncpy(wideStr, wide.c_str(), copyLen);
    return (int)copyLen;
  }

  return (int)wide.size();
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
