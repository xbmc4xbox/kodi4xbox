/*
 *  Copyright (C) 2005-2018 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#include "ZipDirectory.h"

#include "FileItem.h"
#include "URL.h"
#include "utils/CharsetConverter.h"
#include "utils/StringUtils.h"
#include "utils/URIUtils.h"

#include <vector>

namespace XFILE
{
  CZipDirectory::CZipDirectory() = default;

  CZipDirectory::~CZipDirectory() = default;

  bool CZipDirectory::GetDirectory(const CURL& urlOrig, CFileItemList& items)
  {
    return false;
  }

  bool CZipDirectory::ContainsFiles(const CURL& url)
  {
    return false;
  }
}

