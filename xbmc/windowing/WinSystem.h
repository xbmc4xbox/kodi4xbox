/*
 *  Copyright (C) 2005-2018 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#pragma once

#include "Resolution.h"

class CGraphicContext;
class CRenderSystemBase;

class CWinSystemBase
{
public:
  CWinSystemBase();
  virtual ~CWinSystemBase();

  static std::unique_ptr<CWinSystemBase> CreateWinSystem();

  // Access render system interface
  virtual CRenderSystemBase *GetRenderSystem() { return nullptr; }

  virtual const std::string GetName() { return "platform default"; }

  // windowing interfaces
  virtual bool InitWindowSystem();
  virtual bool DestroyWindowSystem();
  virtual bool CreateNewWindow(const std::string& name, bool fullScreen, RESOLUTION_INFO& res) = 0;
  virtual bool DestroyWindow(){ return false; }
  virtual bool SetFullScreen(bool fullScreen, RESOLUTION_INFO& res, bool blankOtherDisplays) = 0;
  //does the output expect limited color range (ie 16-235)
  virtual bool UseLimitedColor();

  // resolution interfaces
  unsigned int GetWidth() { return m_nWidth; }
  unsigned int GetHeight() { return m_nHeight; }
  bool IsFullScreen() { return true; }

  virtual void UpdateResolutions();

  // text input interface
  virtual std::string GetClipboardText(void);

  // render loop
  void DriveRenderLoop();

  // winsystem events
  virtual bool MessagePump() { return false; }

  // Access render system interface
  virtual CGraphicContext& GetGfxContext() const;

protected:
  void UpdateDesktopResolution(RESOLUTION_INFO& newRes, const std::string &output, int width, int height, float refreshRate, uint32_t dwFlags);

  int m_nWidth = 0;
  int m_nHeight = 0;
  float m_fRefreshRate = 0.0f;

  std::unique_ptr<CGraphicContext> m_gfxContext;
};
