/*
 *      Copyright (C) 2005-2014 Team XBMC
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

#pragma once

#if defined(_XBOX) && defined(HAS_GL)

#include "WinSystemXboxGL.h"
#include "windowing/WinSystem.h"
#include "rendering/gl/RenderSystemGL.h"
#include "utils/GlobalsHandling.h"

class CWinSystemXboxGL : public CWinSystemBase, public CRenderSystemGL
{
public:
  CWinSystemXboxGL();
  virtual ~CWinSystemXboxGL();

  // CWinSystemBase
  bool InitWindowSystem() override;
  bool DestroyWindowSystem() override;
  bool CreateNewWindow(const std::string& name, bool fullScreen, RESOLUTION_INFO& res) override;
  bool ResizeWindow(int newWidth, int newHeight, int newLeft, int newTop) override;
  bool SetFullScreen(bool fullScreen, RESOLUTION_INFO& res, bool blankOtherDisplays) override;
  void UpdateResolutions() override;

  bool IsExtSupported(const char* extension) override;

protected:
  void PresentRenderImpl(bool rendered) override;
  void SetVSyncImpl(bool enable) override;

private:
  RESOLUTION_INFO GetResolutionInfo(RESOLUTION res);
};

XBMC_GLOBAL_REF(CWinSystemXboxGL,g_Windowing);
#define g_Windowing XBMC_GLOBAL_USE(CWinSystemXboxGL)

#endif
