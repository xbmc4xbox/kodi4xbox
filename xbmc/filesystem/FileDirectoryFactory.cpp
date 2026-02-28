/*
 *  Copyright (C) 2005-2018 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#include "FileDirectoryFactory.h"

#include "utils/URIUtils.h"
#include "Directory.h"
#include "FileItem.h"
#include "PlaylistFileDirectory.h"
#include "ServiceBroker.h"
#include "SmartPlaylistDirectory.h"
#include "URL.h"
#include "ZipDirectory.h"
#include "addons/ExtsMimeSupportList.h"
#include "addons/addoninfo/AddonInfo.h"
#include "playlists/PlayListFactory.h"
#include "playlists/SmartPlayList.h"
#include "utils/StringUtils.h"
#include "utils/log.h"

using namespace ADDON;
using namespace KODI::ADDONS;
using namespace XFILE;
using namespace PLAYLIST;

CFileDirectoryFactory::CFileDirectoryFactory(void) = default;

CFileDirectoryFactory::~CFileDirectoryFactory(void) = default;

// return NULL + set pItem->m_bIsFolder to remove it completely from list.
IFileDirectory* CFileDirectoryFactory::Create(const CURL& url, CFileItem* pItem, const std::string& strMask)
{
  if (url.IsProtocol("stack")) // disqualify stack as we need to work with each of the parts instead
    return NULL;

  /**
   * Check available binary addons which can contain files with underlaid
   * folders / files.
   * Currently in vfs and audiodecoder addons.
   *
   * @note The file extensions are absolutely necessary for these in order to
   * identify the associated add-on.
   */
  /**@{*/

  // Get file extensions to find addon related to it.
  std::string strExtension = URIUtils::GetExtension(url);
  StringUtils::ToLower(strExtension);

  /**@}*/

  if (pItem->IsDiscImage())
  {
    return nullptr;
  }

  if (url.IsFileType("zip"))
  {
    CURL zipURL = URIUtils::CreateArchivePath("zip", url);

    CFileItemList items;
    CDirectory::GetDirectory(zipURL, items, strMask, DIR_FLAG_DEFAULTS);
    if (items.Size() == 0) // no files
      pItem->m_bIsFolder = true;
    else if (items.Size() == 1 && items[0]->m_idepth == 0 && !items[0]->m_bIsFolder)
    {
      // one STORED file - collapse it down
      *pItem = *items[0];
    }
    else
    { // compressed or more than one file -> create a zip dir
      pItem->SetURL(zipURL);
      return new CZipDirectory;
    }
    return NULL;
  }
  if (url.IsFileType("xsp"))
  { // XBMC Smart playlist - just XML renamed to XSP
    // read the name of the playlist in
    CSmartPlaylist playlist;
    if (playlist.OpenAndReadName(url))
    {
      pItem->SetLabel(playlist.GetName());
      pItem->SetLabelPreformatted(true);
    }
    IFileDirectory* pDir=new CSmartPlaylistDirectory;
    return pDir; // treat as directory
  }
  if (CPlayListFactory::IsPlaylist(url))
  { // Playlist file
    // currently we only return the directory if it contains
    // more than one file.  Reason is that .pls and .m3u may be used
    // for links to http streams etc.
    IFileDirectory *pDir = new CPlaylistFileDirectory();
    CFileItemList items;
    if (pDir->GetDirectory(url, items))
    {
      if (items.Size() > 1)
        return pDir;
    }
    delete pDir;
    return NULL;
  }

  return NULL;
}

