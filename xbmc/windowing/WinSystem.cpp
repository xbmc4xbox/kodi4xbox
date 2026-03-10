/*
 *  Copyright (C) 2005-2018 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#include "WinSystem.h"

#include "settings/DisplaySettings.h"
#include "utils/StringUtils.h"
#include "windowing/GraphicContext.h"

#include <mutex>

CWinSystemBase::CWinSystemBase() : m_gfxContext(std::make_unique<CGraphicContext>())
{
}

CWinSystemBase::~CWinSystemBase() = default;

bool CWinSystemBase::InitWindowSystem()
{
  UpdateResolutions();
  CDisplaySettings::GetInstance().ApplyCalibrations();

  return true;
}

bool CWinSystemBase::DestroyWindowSystem()
{
  return false;
}

void CWinSystemBase::UpdateDesktopResolution(RESOLUTION_INFO& newRes,
                                             const std::string& output,
                                             int width,
                                             int height,
                                             float refreshRate,
                                             uint32_t dwFlags)
{
  newRes.Overscan.left = 0;
  newRes.Overscan.top = 0;
  newRes.Overscan.right = width;
  newRes.Overscan.bottom = height;
  newRes.bFullScreen = true;
  newRes.iSubtitles = (int)(0.965 * height);
  newRes.dwFlags = dwFlags;
  newRes.fRefreshRate = refreshRate;
  newRes.fPixelRatio = 1.0f;
  newRes.iWidth = width;
  newRes.iHeight = height;
  newRes.iScreenWidth = width;
  newRes.iScreenHeight = height;
  newRes.strMode = StringUtils::Format("{}: {}x{}", output, width, height);
  if (refreshRate > 1)
    newRes.strMode += StringUtils::Format(" @ {:.2f}Hz", refreshRate);
  if (dwFlags & D3DPRESENTFLAG_INTERLACED)
    newRes.strMode += "i";
  if (dwFlags & D3DPRESENTFLAG_MODE3DTB)
    newRes.strMode += "tab";
  if (dwFlags & D3DPRESENTFLAG_MODE3DSBS)
    newRes.strMode += "sbs";
  newRes.strOutput = output;
}

void CWinSystemBase::UpdateResolutions()
{
  // Nothing to do here on Xbox - there is no window resolution
}

bool CWinSystemBase::UseLimitedColor()
{
  return true;
}

std::string CWinSystemBase::GetClipboardText(void)
{
  return "";
}

void CWinSystemBase::DriveRenderLoop()
{
  MessagePump();
}

CGraphicContext& CWinSystemBase::GetGfxContext() const
{
  return *m_gfxContext;
}
