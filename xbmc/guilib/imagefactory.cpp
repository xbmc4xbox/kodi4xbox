/*
 *  Copyright (C) 2012-2018 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#include "imagefactory.h"

#include "ServiceBroker.h"
#include "addons/ExtsMimeSupportList.h"
#include "addons/addoninfo/AddonType.h"
#include "guilib/JpegIO.h"
#include "guilib/cximage.h"
#include "utils/Mime.h"
#include "utils/log.h"

#include <mutex>

CCriticalSection ImageFactory::m_createSec;

using namespace KODI::ADDONS;

IImage* ImageFactory::CreateLoader(const std::string& strFileName)
{
  CURL url(strFileName);
  return CreateLoader(url);
}

IImage* ImageFactory::CreateLoader(const CURL& url)
{
  if(!url.GetFileType().empty())
    return CreateLoaderFromMimeType("image/"+url.GetFileType());

  return CreateLoaderFromMimeType(CMime::GetMimeType(url));
}

IImage* ImageFactory::CreateLoaderFromMimeType(const std::string& strMimeType)
{
  if (strMimeType == "image/jpg" || strMimeType == "image/jpeg" || strMimeType == "image/tbn")
  {
    return new CJpegIO();
  }
  else if (strMimeType == "image/png")
  {
    return new CXImage(strMimeType);
  }

  CLog::Log(LOGWARNING, "{} - image '{}' is not supported. Use JPG or PNG!", __FUNCTION__, strMimeType);
  return nullptr;
}
