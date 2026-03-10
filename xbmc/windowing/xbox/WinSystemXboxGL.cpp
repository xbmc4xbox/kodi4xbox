/*
 *      Copyright (C) 2005-2013 Team XBMC
 *      http://xbmc.org
 *
 *  This Program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2, or (at your option)
 *  any later version.
 *
 *  This Program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with XBMC; see the file COPYING.  If not, see
 *  <http://www.gnu.org/licenses/>.
 *
 */

#include "WinSystemXboxGL.h"

#include "ServiceBroker.h"
#include "guilib/gui3d.h"
#include "peripherals/Peripherals.h"
#include "settings/DisplaySettings.h"
#include "windowing/WindowSystemFactory.h"

#include <hal/video.h>
#include <pbgl.h>

#include <SDL.h>

void CWinSystemXboxGL::Register()
{
  KODI::WINDOWING::CWindowSystemFactory::RegisterWindowSystem(CreateWinSystem);
}

std::unique_ptr<CWinSystemBase> CWinSystemXboxGL::CreateWinSystem()
{
  return std::make_unique<CWinSystemXboxGL>();
}

CWinSystemXboxGL::CWinSystemXboxGL()
{
}

CWinSystemXboxGL::~CWinSystemXboxGL()
{
}

bool CWinSystemXboxGL::InitWindowSystem()
{
  if(!CWinSystemBase::InitWindowSystem())
    return false;

  return true;
}

bool CWinSystemXboxGL::DestroyWindowSystem()
{
  return true;
}

void CWinSystemXboxGL::PresentRenderImpl(bool rendered)
{
  if (rendered)
    pbgl_swap_buffers();
}

void CWinSystemXboxGL::SetVSyncImpl(bool enable)
{
  // There is not VSync on Xbox
}

bool CWinSystemXboxGL::IsExtSupported(const char* extension)
{
  std::string name;
  name  = " ";
  name += extension;
  name += " ";

  return m_RenderExtensions.find(name) != std::string::npos;
}

bool CWinSystemXboxGL::CreateNewWindow(const std::string& name, bool fullScreen, RESOLUTION_INFO& res)
{
  if (!SetFullScreen(fullScreen, res, false))
    return false;

  m_bWindowCreated = true;
  return true;
}

bool CWinSystemXboxGL::ResizeWindow(int newWidth, int newHeight, int newLeft, int newTop)
{
  // TODO: inspect if this has anything to do when changing resolution or it's only for resizing non-fullscreen Kodi window
  return true;
}

bool CWinSystemXboxGL::SetFullScreen(bool fullScreen, RESOLUTION_INFO& res, bool blankOtherDisplays)
{
  // Xbox is always fullscreen
  m_nWidth      = res.iWidth;
  m_nHeight     = res.iHeight;

  XVideoSetMode(m_nWidth, m_nHeight, 32, REFRESH_DEFAULT);
  CRenderSystemGL::ResetRenderSystem(m_nWidth, m_nHeight, fullScreen, res.fRefreshRate);

  return true;
}

RESOLUTION_INFO CWinSystemXboxGL::GetResolutionInfo(RESOLUTION res)
{
  RESOLUTION_INFO info;
  switch (res)
  {
  case RES_HDTV_1080i:
    UpdateDesktopResolution(info, 0, 1920, 1080, REFRESH_60HZ, D3DPRESENTFLAG_INTERLACED | D3DPRESENTFLAG_WIDESCREEN);
    break;
  case RES_HDTV_720p:
    UpdateDesktopResolution(info, 0, 1280, 720, REFRESH_60HZ, D3DPRESENTFLAG_PROGRESSIVE | D3DPRESENTFLAG_WIDESCREEN);
    break;
  case RES_HDTV_480p_4x3:
    UpdateDesktopResolution(info, 0, 720, 480, REFRESH_60HZ, D3DPRESENTFLAG_PROGRESSIVE);
    info.iSubtitles = (int)(0.9 * 480);
    info.fPixelRatio = 4320.0f / 4739.0f;
    break;
  case RES_HDTV_480p_16x9:
    UpdateDesktopResolution(info, 0, 720, 480, REFRESH_60HZ, D3DPRESENTFLAG_PROGRESSIVE | D3DPRESENTFLAG_WIDESCREEN);
    info.fPixelRatio = 4320.0f / 4739.0f*4.0f / 3.0f;
    break;
  case RES_NTSC_4x3:
    UpdateDesktopResolution(info, 0, 720, 480, REFRESH_60HZ, D3DPRESENTFLAG_INTERLACED);
    info.iSubtitles = (int)(0.9 * 480);
    info.fPixelRatio = 4320.0f / 4739.0f;
    break;
  case RES_NTSC_16x9:
    UpdateDesktopResolution(info, 0, 720, 480, REFRESH_60HZ, D3DPRESENTFLAG_INTERLACED | D3DPRESENTFLAG_WIDESCREEN);
    info.fPixelRatio = 4320.0f / 4739.0f*4.0f / 3.0f;
    break;
  case RES_PAL_4x3:
    UpdateDesktopResolution(info, 0, 720, 576, REFRESH_50HZ, D3DPRESENTFLAG_INTERLACED);
    info.iSubtitles = (int)(0.9 * 576);
    info.fPixelRatio = 128.0f / 117.0f;
    break;
  case RES_PAL_16x9:
    UpdateDesktopResolution(info, 0, 720, 576, REFRESH_50HZ, D3DPRESENTFLAG_INTERLACED | D3DPRESENTFLAG_WIDESCREEN);
    info.fPixelRatio = 128.0f / 117.0f*4.0f / 3.0f;
    break;
  case RES_PAL60_4x3:
    UpdateDesktopResolution(info, 0, 720, 480, REFRESH_60HZ, D3DPRESENTFLAG_INTERLACED);
    info.iSubtitles = (int)(0.9 * 480);
    info.fPixelRatio = 4320.0f / 4739.0f;
    break;
  case RES_PAL60_16x9:
    UpdateDesktopResolution(info, 0, 720, 480, REFRESH_60HZ, D3DPRESENTFLAG_INTERLACED | D3DPRESENTFLAG_WIDESCREEN);
    info.fPixelRatio = 4320.0f / 4739.0f*4.0f / 3.0f;
    break;
  default:
    break;
  }
  info.iScreenWidth  = info.iWidth;
  info.iScreenHeight = info.iHeight;
  info.Overscan.right = info.iWidth;
  info.Overscan.bottom = info.iHeight;

  return info;
}

void CWinSystemXboxGL::UpdateResolutions()
{
  CWinSystemBase::UpdateResolutions();

  // erase previous stored modes
  CDisplaySettings::GetInstance().ClearCustomResolutions();

  VIDEO_MODE xmode;
  void *p = NULL;
  while(XVideoListModes(&xmode, 0, 0, &p))
  {
    // ignore 640 wide modes
    if (xmode.width < 720)
      continue;

    RESOLUTION_INFO info;
    if (xmode.width == 1080)
      info = GetResolutionInfo(RES_HDTV_1080i);
    if (xmode.width == 1280)
      info = GetResolutionInfo(RES_HDTV_720p);
    if (xmode.width == 720)
      info = GetResolutionInfo(RES_HDTV_480p_16x9);
    else
      continue;

    CDisplaySettings::GetInstance().AddResolutionInfo(info);
  }

  // Update desktop resolution to best available resolution
  RESOLUTION_INFO& info = CDisplaySettings::GetInstance().GetResolutionInfo(RES_DESKTOP);
  UpdateDesktopResolution(info, 0, xmode.width, xmode.height, xmode.refresh, 0);

  CDisplaySettings::GetInstance().ApplyCalibrations();
}

bool CWinSystemXboxGL::MessagePump()
{
  SDL_Event event;
  while (SDL_PollEvent(&event))
  {
    CServiceBroker::GetPeripherals().UpdateJoystick(event);
  }
  return true;
}
