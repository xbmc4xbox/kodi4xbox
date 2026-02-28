/*
 *  Copyright (C) 2005-2018 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#include "FileFactory.h"
#ifdef TARGET_POSIX
#include "platform/posix/filesystem/PosixFile.h"
#elif defined(_XBOX)
#include "platform/xbox/filesystem/XboxFile.h"
#elif defined(TARGET_WINDOWS)
#include "platform/win32/filesystem/Win32File.h"
#ifdef TARGET_WINDOWS_STORE
#include "platform/win10/filesystem/WinLibraryFile.h"
#endif
#endif // TARGET_WINDOWS
#include "CurlFile.h"
#include "ZipFile.h"
#include "MusicDatabaseFile.h"
#include "VideoDatabaseFile.h"
#include "PluginFile.h"
#include "SpecialProtocolFile.h"
#include "MultiPathFile.h"
#include "ImageFile.h"
#include "ResourceFile.h"
#include "URL.h"
#include "utils/log.h"
#include "utils/StringUtils.h"
#include "ServiceBroker.h"

using namespace ADDON;
using namespace XFILE;

CFileFactory::CFileFactory() = default;

CFileFactory::~CFileFactory() = default;

IFile* CFileFactory::CreateLoader(const std::string& strFileName)
{
  CURL url(strFileName);
  return CreateLoader(url);
}

IFile* CFileFactory::CreateLoader(const CURL& url)
{
  if (url.IsProtocol("zip")) return new CZipFile();
  else if (url.IsProtocol("musicdb")) return new CMusicDatabaseFile();
  else if (url.IsProtocol("videodb")) return new CVideoDatabaseFile();
  else if (url.IsProtocol("plugin")) return new CPluginFile();
  else if (url.IsProtocol("library")) return nullptr;
  else if (url.IsProtocol("pvr")) return nullptr;
  else if (url.IsProtocol("special")) return new CSpecialProtocolFile();
  else if (url.IsProtocol("multipath")) return new CMultiPathFile();
  else if (url.IsProtocol("image")) return new CImageFile();
#ifdef TARGET_POSIX
  else if (url.IsProtocol("file") || url.GetProtocol().empty())
  {
    return new CPosixFile();
  }
#elif defined(_XBOX)
  else if (url.IsProtocol("file") || url.GetProtocol().empty())
  {
    return new CXboxFile();
  }
#elif defined(TARGET_WINDOWS)
  else if (url.IsProtocol("file") || url.GetProtocol().empty())
  {
#ifdef TARGET_WINDOWS_STORE
    if (CWinLibraryFile::IsInAccessList(url))
      return new CWinLibraryFile();
#endif
    return new CWin32File();
  }
#endif // TARGET_WINDOWS
  else if (url.IsProtocol("resource")) return new CResourceFile();

  if (url.IsProtocol("ftp")
  ||  url.IsProtocol("ftps")
  ||  url.IsProtocol("rss")
  ||  url.IsProtocol("rsss")
  ||  url.IsProtocol("http")
  ||  url.IsProtocol("https")) return new CCurlFile();

  CLog::Log(LOGWARNING, "{} - unsupported protocol({}) in {}", __FUNCTION__, url.GetProtocol(),
            url.GetRedacted());
  return NULL;
}
