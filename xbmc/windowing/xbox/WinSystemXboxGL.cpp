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
