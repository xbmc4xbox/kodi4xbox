/*
 *  Copyright (C) 2005-2018 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#pragma once

#if defined(_XBOX) && defined(HAS_GL)

#include "rendering/gl/RenderSystemGL.h"
#include "windowing/WinSystem.h"

class CWinSystemXboxGL : public CWinSystemBase, public CRenderSystemGL
{
public:
  CWinSystemXboxGL() = default;
  virtual ~CWinSystemXboxGL() = default;

  static void Register();
  static std::unique_ptr<CWinSystemBase> CreateWinSystem();

  // Implementation of CWinSystemBase
  CRenderSystemBase *GetRenderSystem() override { return this; }
  bool CreateNewWindow(const std::string& name, bool fullScreen, RESOLUTION_INFO& res) override;
  bool SetFullScreen(bool fullScreen, RESOLUTION_INFO& res, bool blankOtherDisplays) override;
  void UpdateResolutions() override;

  bool MessagePump() override;

protected:
  // Implementation of CRenderSystemGL
  void PresentRenderImpl(bool rendered) override;

private:
  RESOLUTION_INFO GetResolutionInfo(RESOLUTION res);
};

#endif
