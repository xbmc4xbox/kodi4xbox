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

#include "settings/DisplaySettings.h"

#include <hal/video.h>
#include <pbgl.h>

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

  XVideoSetMode(720, 480, 32, REFRESH_DEFAULT);
  pbgl_init(GL_TRUE);

  return true;
}

bool CWinSystemXboxGL::DestroyWindowSystem()
{
  pbgl_shutdown();
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
  if(!SetFullScreen(fullScreen, res, false))
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

  return true;
}

RESOLUTION_INFO CWinSystemXboxGL::GetResolutionInfo(RESOLUTION res)
{
  RESOLUTION_INFO info;
  switch (res)
  {
  case RES_HDTV_1080i:
    info.iSubtitles = (int)(0.965 * 1080);
    info.iWidth = 1920;
    info.iHeight = 1080;
    info.dwFlags = D3DPRESENTFLAG_INTERLACED | D3DPRESENTFLAG_WIDESCREEN;
    info.fPixelRatio = 1.0f;
    info.strMode ="1080i 16:9";
    break;
  case RES_HDTV_720p:
    info.iSubtitles = (int)(0.965 * 720);
    info.iWidth = 1280;
    info.iHeight = 720;
    info.dwFlags = D3DPRESENTFLAG_PROGRESSIVE | D3DPRESENTFLAG_WIDESCREEN;
    info.fPixelRatio = 1.0f;
    info.strMode = "720p 16:9";
    break;
  case RES_HDTV_480p_4x3:
    info.iSubtitles = (int)(0.9 * 480);
    info.iWidth = 720;
    info.iHeight = 480;
    info.dwFlags = D3DPRESENTFLAG_PROGRESSIVE;
    info.fPixelRatio = 4320.0f / 4739.0f;
    info.strMode = "480p 4:3";
    break;
  case RES_HDTV_480p_16x9:
    info.iSubtitles = (int)(0.965 * 480);
    info.iWidth = 720;
    info.iHeight = 480;
    info.dwFlags = D3DPRESENTFLAG_PROGRESSIVE | D3DPRESENTFLAG_WIDESCREEN;
    info.fPixelRatio = 4320.0f / 4739.0f*4.0f / 3.0f;
    info.strMode = "480p 16:9";
    break;
  case RES_NTSC_4x3:
    info.iSubtitles = (int)(0.9 * 480);
    info.iWidth = 720;
    info.iHeight = 480;
    info.dwFlags = D3DPRESENTFLAG_INTERLACED;
    info.fPixelRatio = 4320.0f / 4739.0f;
    info.strMode = "NTSC 4:3";
    break;
  case RES_NTSC_16x9:
    info.iSubtitles = (int)(0.965 * 480);
    info.iWidth = 720;
    info.iHeight = 480;
    info.dwFlags = D3DPRESENTFLAG_INTERLACED | D3DPRESENTFLAG_WIDESCREEN;
    info.fPixelRatio = 4320.0f / 4739.0f*4.0f / 3.0f;
    info.strMode = "NTSC 16:9";
    break;
  case RES_PAL_4x3:
    info.iSubtitles = (int)(0.9 * 576);
    info.iWidth = 720;
    info.iHeight = 576;
    info.dwFlags = D3DPRESENTFLAG_INTERLACED;
    info.fPixelRatio = 128.0f / 117.0f;
    info.strMode = "PAL 4:3";
    break;
  case RES_PAL_16x9:
    info.iSubtitles = (int)(0.965 * 576);
    info.iWidth = 720;
    info.iHeight = 576;
    info.dwFlags = D3DPRESENTFLAG_INTERLACED | D3DPRESENTFLAG_WIDESCREEN;
    info.fPixelRatio = 128.0f / 117.0f*4.0f / 3.0f;
    info.strMode = "PAL 16:9";
    break;
  case RES_PAL60_4x3:
    info.iSubtitles = (int)(0.9 * 480);
    info.iWidth = 720;
    info.iHeight = 480;
    info.dwFlags = D3DPRESENTFLAG_INTERLACED;
    info.fPixelRatio = 4320.0f / 4739.0f;
    info.strMode = "PAL60 4:3";
    break;
  case RES_PAL60_16x9:
    info.iSubtitles = (int)(0.965 * 480);
    info.iWidth = 720;
    info.iHeight = 480;
    info.dwFlags = D3DPRESENTFLAG_INTERLACED | D3DPRESENTFLAG_WIDESCREEN;
    info.fPixelRatio = 4320.0f / 4739.0f*4.0f / 3.0f;
    info.strMode = "PAL60 16:9";
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

  RESOLUTION_INFO& info = CDisplaySettings::GetInstance().GetResolutionInfo(RES_DESKTOP);
  UpdateDesktopResolution(info, 0, 720, 480, 60.0f, 0);

  // erase previous stored modes
  CDisplaySettings::GetInstance().ClearCustomResolutions();

  for (int res = RES_HDTV_1080i; res <= RES_PAL60_16x9; res++)
  {
    RESOLUTION_INFO info = GetResolutionInfo(static_cast<RESOLUTION>(res));
    CDisplaySettings::GetInstance().AddResolutionInfo(info);
  }

  CDisplaySettings::GetInstance().ApplyCalibrations();
}
