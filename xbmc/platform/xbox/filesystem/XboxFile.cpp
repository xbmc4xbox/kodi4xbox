/*
 *  Copyright (C) 2014-2018 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#include "XboxFile.h"

#include "utils/log.h"
#include "utils/StringUtils.h"
#include "Util.h"
#include "URL.h"

#include <sys/stat.h>

using namespace XFILE;

CXboxFile::CXboxFile()
    : m_hFile(INVALID_HANDLE_VALUE)
{
}

CXboxFile::~CXboxFile()
{
  Close();
}

std::string CXboxFile::GetLocal(const CURL &url)
{
  std::string path( url.GetFileName() );

  if(url.IsProtocol("file"))
  {
    // file://drive[:]/path
    // file:///drive:/path
    std::string host( url.GetHostName() );

    if (!host.empty()) {
      if (host.size() > 0 && host.substr(host.size() - 1) == ":")
        path = host + "/" + path;
      else
        path = host + ":/" + path;
    }
  }

  StringUtils::Replace(path, '/', '\\');
  /* g_charsetConverter.utf8ToStringCharset(path); */
  return path;
}

bool CXboxFile::Open(const CURL& url)
{
  std::string strFile = GetLocal(url);

  m_hFile = CreateFile(strFile.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
  if (m_hFile == INVALID_HANDLE_VALUE) 
    return false;

  m_i64FilePos = 0;
  m_i64FileLen = 0;

  LARGE_INTEGER i64Size;
  GetFileSizeEx(m_hFile, &i64Size);
  m_i64FileLength = i64Size.QuadPart;

  return true;
}

bool CXboxFile::Exists(const CURL& url)
{
  std::string strFile = GetLocal(url);
  const DWORD attrs = GetFileAttributesA(strFile.c_str());
  return attrs != INVALID_FILE_ATTRIBUTES && (attrs & FILE_ATTRIBUTE_DIRECTORY) == 0;
}


int CXboxFile::Stat(const CURL& url, struct __stat64* buffer)
{
  std::string strFile = GetLocal(url);

  HANDLE hSearch;
  WIN32_FIND_DATAA findData;
  hSearch = FindFirstFile(strFile.c_str(), &findData);
  if (hSearch == INVALID_HANDLE_VALUE)
    return -1;
  CloseHandle(hSearch);

  buffer->st_dev = 0;
  buffer->st_ino = 0;
  buffer->st_mode = (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) ? S_IFDIR : S_IFREG;
  buffer->st_nlink = 1;
  buffer->st_uid = 0;
  buffer->st_gid = 0;
  buffer->st_rdev = 0;

  buffer->st_size = static_cast<off_t>(
      (static_cast<__int64>(findData.nFileSizeHigh) << 32) | findData.nFileSizeLow);

  buffer->st_atime = static_cast<__time64_t>(findData.ftLastAccessTime.dwLowDateTime);
  buffer->st_mtime = static_cast<__time64_t>(findData.ftLastWriteTime.dwLowDateTime);
  buffer->st_ctime = static_cast<__time64_t>(findData.ftCreationTime.dwLowDateTime);

  return 0;
}

bool CXboxFile::SetHidden(const CURL &url, bool hidden)
{
  std::string path = GetLocal(url);
  /* g_charsetConverter.utf8ToW(GetLocal(url), path, false); */

  DWORD attributes = hidden ? FILE_ATTRIBUTE_HIDDEN : FILE_ATTRIBUTE_NORMAL;
  if (SetFileAttributesA(path.c_str(), attributes))
    return true;

  return false;
}

bool CXboxFile::OpenForWrite(const CURL& url, bool bOverWrite)
{
  // make sure it's a legal FATX filename (we are writing to the harddisk)
  std::string strPath = GetLocal(url);

#if 0
  if (CSettings::GetInstance().GetBool("services.ftpautofatx"))
  { // allow overriding
    std::string strPathOriginal = strPath;
    CUtil::GetFatXQualifiedPath(strPath);
    if (strPathOriginal != strPath)
      CLog::Log(LOGINFO,"CXboxFile::OpenForWrite: WARNING: Truncated filename {} {}", strPathOriginal, strPath);
  }
#endif

  m_hFile = CreateFile(strPath.c_str(), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL, bOverWrite ? CREATE_ALWAYS : OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
  if (m_hFile == INVALID_HANDLE_VALUE)
    return false;

  m_i64FilePos = 0;
  LARGE_INTEGER i64Size;
  GetFileSizeEx(m_hFile, &i64Size);
  m_i64FileLength = i64Size.QuadPart;
  Seek(0, SEEK_SET);

  return true;
}

ssize_t CXboxFile::Read(void *lpBuf, size_t uiBufSize)
{
  assert(lpBuf != NULL);
  if (m_hFile == INVALID_HANDLE_VALUE)
    return -1;

  if (uiBufSize > SSIZE_MAX)
    uiBufSize = SSIZE_MAX;

  DWORD nBytesRead;
  if ( ReadFile(m_hFile, lpBuf, (DWORD)uiBufSize, &nBytesRead, NULL) )
  {
    m_i64FilePos += nBytesRead;
    return nBytesRead;
  }
  return -1;
}

ssize_t CXboxFile::Write(const void* lpBuf, size_t uiBufSize)
{
  if (m_hFile == INVALID_HANDLE_VALUE)
    return 0;

  DWORD nBytesWriten;
  if ( WriteFile(m_hFile, (void*) lpBuf, (DWORD)uiBufSize, &nBytesWriten, NULL) )
    return nBytesWriten;

  return 0;
}

void CXboxFile::Close()
{
  if (m_hFile != INVALID_HANDLE_VALUE)
    CloseHandle(m_hFile);

  m_hFile = INVALID_HANDLE_VALUE;
}

int64_t CXboxFile::Seek(int64_t iFilePosition, int iWhence)
{
  LARGE_INTEGER lPos, lNewPos;
  lPos.QuadPart = iFilePosition;
  int bSuccess;

  switch (iWhence)
  {
  case SEEK_SET:
    bSuccess = SetFilePointerEx(m_hFile, lPos, &lNewPos, FILE_BEGIN);
    break;

  case SEEK_CUR:
    bSuccess = SetFilePointerEx(m_hFile, lPos, &lNewPos, FILE_CURRENT);
    break;

  case SEEK_END:
    bSuccess = SetFilePointerEx(m_hFile, lPos, &lNewPos, FILE_END);
    break;

  default:
    return -1;
  }
  if (bSuccess)
  {
    m_i64FilePos = lNewPos.QuadPart;
    return m_i64FilePos;
  }
  else
    return -1;
}

int64_t CXboxFile::GetLength()
{
  if(m_i64FileLen <= m_i64FilePos || m_i64FileLen == 0)
  {
    LARGE_INTEGER i64Size;
    if(GetFileSizeEx((HANDLE)m_hFile, &i64Size))
      m_i64FileLen = i64Size.QuadPart;
    else
      CLog::Log(LOGERROR, "CXboxFile::GetLength - GetFileSizeEx failed with error {}", GetLastError());
  }
  return m_i64FileLen;
}

int64_t CXboxFile::GetPosition()
{
  return m_i64FilePos;
}

bool CXboxFile::Delete(const CURL& url)
{
  std::string strFile = GetLocal(url);

  return ::DeleteFile(strFile.c_str()) ? true : false;
}

bool CXboxFile::Rename(const CURL& url, const CURL& urlnew)
{
  std::string strFile = GetLocal(url);
  std::string  strNewFile = GetLocal(urlnew);

  return ::MoveFile(strFile.c_str(), strNewFile.c_str()) ? true : false;
}

void CXboxFile::Flush()
{
  IO_STATUS_BLOCK ioStatus;
  NtFlushBuffersFile(m_hFile, &ioStatus);
}
