/*
 *  Copyright (C) 2019 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#include "PlatformXbox.h"

#include "windowing/xbox/WinSystemXboxGL.h"

CPlatform* CPlatform::CreateInstance()
{
  return new CPlatformXbox();
}

bool CPlatformXbox::InitStageOne()
{
  if (!CPlatform::InitStageOne())
    return false;

  CWinSystemXboxGL::Register();

  return true;
}
