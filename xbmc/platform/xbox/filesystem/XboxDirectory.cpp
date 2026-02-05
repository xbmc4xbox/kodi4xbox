/*
 *  Copyright (C) 2014-2018 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#include "XboxDirectory.h"

#include "FileItem.h"
#include "URL.h"
#include "utils/CharsetConverter.h"
#include "utils/StringUtils.h"
#include "utils/URIUtils.h"
#include "utils/log.h"

#include "platform/xbox/XBOXUtil.h"

#include <nxdk/mount.h>

#include <windows.h>

#ifndef INVALID_FILE_ATTRIBUTES
#define INVALID_FILE_ATTRIBUTES ((DWORD) -1)
#endif

using namespace XFILE;

// check for empty string, remove trailing slash if any, convert to xbox form
inline static std::string prepareXboxDirectoryName(const std::string& strPath)
{
  if (strPath.empty())
    return std::string(); // empty string

  std::string name(CXBOXUtil::GetFatXQualifiedPath(strPath));
  if (!name.empty())
  {
    if (name.back() == '\\')
      name.pop_back(); // remove slash at the end if any
    if (name.length() == 6 && name.back() == ':') // 6 is the length of "\\?\x:"
      name.push_back('\\'); // always add backslash for root folders
  }
  return name;
}

CXboxDirectory::CXboxDirectory(void)
{}

CXboxDirectory::~CXboxDirectory(void)
{}

bool CXboxDirectory::GetDirectory(const CURL& url, CFileItemList &items)
{
  std::string pathWithSlash(url.Get());
  if (!pathWithSlash.empty() && pathWithSlash.back() != '\\')
    pathWithSlash.push_back('\\');

  std::string searchMask(CXBOXUtil::GetFatXQualifiedPath(pathWithSlash));
  if (searchMask.empty())
    return false;

  //! @todo support m_strFileMask, require rewrite of internal caching
  searchMask += '*';

  WIN32_FIND_DATA findData = {};
  HANDLE hSearch = FindFirstFileA(searchMask.c_str(), &findData);

  if (hSearch == INVALID_HANDLE_VALUE)
    return GetLastError() == ERROR_FILE_NOT_FOUND ? Exists(url) : false; // return true if directory exist and empty

  do
  {
    std::string itemName(findData.cFileName);
    if (itemName == "." || itemName == "..")
      continue;

    CFileItemPtr pItem(new CFileItem(itemName));

    pItem->m_bIsFolder = ((findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0);
    if (pItem->m_bIsFolder)
      pItem->SetPath(pathWithSlash + itemName + '\\');
    else
      pItem->SetPath(pathWithSlash + itemName);

    if ((findData.dwFileAttributes & (FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM)) != 0
          || itemName.front() == '.') // mark files starting from dot as hidden
      pItem->SetProperty("file:hidden", true);

    // calculation of size and date costs a little on win32
    // so DIR_FLAG_NO_FILE_INFO flag is ignored
    KODI::TIME::FileTime fileTime;
    fileTime.lowDateTime = findData.ftLastWriteTime.dwLowDateTime;
    fileTime.highDateTime = findData.ftLastWriteTime.dwHighDateTime;
    KODI::TIME::FileTime localTime;
    if (KODI::TIME::FileTimeToLocalFileTime(&fileTime, &localTime) == TRUE)
      pItem->m_dateTime = localTime;
    else
      pItem->m_dateTime = 0;

    if (!pItem->m_bIsFolder)
        pItem->m_dwSize = (__int64(findData.nFileSizeHigh) << 32) + findData.nFileSizeLow;

    items.Add(pItem);
  } while (FindNextFileA(hSearch, &findData));

  FindClose(hSearch);

  return true;
}

bool CXboxDirectory::Create(const CURL& url)
{
  auto name(prepareXboxDirectoryName(url.Get()));
  if (name.empty())
    return false;

  if (!Create(name))
    return Exists(url);

  return true;
}

bool CXboxDirectory::Remove(const CURL& url)
{
  std::string name(prepareXboxDirectoryName(url.Get()));
  if (name.empty())
    return false;

  if (RemoveDirectoryA(name.c_str()))
    return true;

  return !Exists(url);
}

bool CXboxDirectory::Exists(const CURL& url)
{
  std::string name(prepareXboxDirectoryName(url.Get()));
  if (name.empty())
    return false;

  DWORD fileAttrs = GetFileAttributesA(name.c_str());
  if (fileAttrs == INVALID_FILE_ATTRIBUTES || (fileAttrs & FILE_ATTRIBUTE_DIRECTORY) == 0)
    return false;

  return true;
}

bool CXboxDirectory::RemoveRecursive(const CURL& url)
{
  std::string pathWithSlash(url.Get());
  if (!pathWithSlash.empty() && pathWithSlash.back() != '\\')
    pathWithSlash.push_back('\\');

  auto basePath = CXBOXUtil::GetFatXQualifiedPath(pathWithSlash);
  if (basePath.empty())
    return false;

  auto searchMask = basePath + '*';

  WIN32_FIND_DATA findData = {};
  HANDLE hSearch = FindFirstFileA(searchMask.c_str(), &findData);

  if (hSearch == INVALID_HANDLE_VALUE)
    return GetLastError() == ERROR_FILE_NOT_FOUND ? Exists(url) : false; // return true if directory exist and empty

  bool success = true;
  do
  {
    std::string itemName(findData.cFileName);
    if (itemName == "." || itemName == "..")
      continue;

    auto path = basePath + itemName;
    if (0 != (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
    {
      if (!RemoveRecursive(CURL{ path }))
      {
        success = false;
        break;
      }
    }
    else
    {
      if (FALSE == DeleteFileA(path.c_str()))
      {
        success = false;
        break;
      }
    }
  } while (FindNextFileA(hSearch, &findData));

  FindClose(hSearch);

  if (success)
  {
    if (FALSE == RemoveDirectoryA(basePath.c_str()))
      success = false;
  }

  return success;
}

bool CXboxDirectory::Create(std::string path) const
{
  if (!CreateDirectoryA(path.c_str(), nullptr))
  {
    if (GetLastError() == ERROR_ALREADY_EXISTS)
      return true;

    if (GetLastError() != ERROR_PATH_NOT_FOUND)
      return false;

    auto sep = path.rfind('\\');
    if (sep == std::string::npos)
      return false;

    if (Create(path.substr(0, sep)))
      return Create(path);

    return false;
  }

  return true;
}
