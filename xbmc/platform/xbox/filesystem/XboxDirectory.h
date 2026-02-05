/*
 *  Copyright (C) 2014-2018 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#pragma once

#include "filesystem/IDirectory.h"

namespace XFILE
{

  class CXboxDirectory : public IDirectory
  {
  public:
    CXboxDirectory(void);
    virtual ~CXboxDirectory(void);
    virtual bool GetDirectory(const CURL& url, CFileItemList &items) override;
    virtual bool Create(const CURL& url) override;
    virtual bool Exists(const CURL& url) override;
    virtual bool Remove(const CURL& url) override;
    virtual bool RemoveRecursive(const CURL& url) override;

  private:
    bool Create(std::string path) const;
  };
}
