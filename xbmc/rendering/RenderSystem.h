/*
 *  Copyright (C) 2005-2018 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#pragma once

#include "RenderSystemTypes.h"
#include "utils/ColorUtils.h"
#include "utils/Geometry.h"

#include <memory>
#include <string>

/*
 *   CRenderSystemBase interface allows us to create the rendering engine we use.
 *   We currently have two engines: OpenGL and DirectX
 *   This interface is very basic since a lot of the actual details will go in to the derived classes
 */

class CGUIImage;
class CGUITextLayout;

class CRenderSystemBase
{
public:
  CRenderSystemBase();
  virtual ~CRenderSystemBase();

  virtual bool InitRenderSystem() = 0;
  virtual bool DestroyRenderSystem() = 0;
  virtual bool ResetRenderSystem(int width, int height) = 0;

  virtual bool BeginRender() = 0;
  virtual bool EndRender() = 0;
  virtual void PresentRender(bool rendered, bool videoLayer) = 0;
  virtual bool ClearBuffers(UTILS::COLOR::Color color) = 0;
  virtual bool IsExtSupported(const char* extension) const = 0;

  virtual void SetViewPort(const CRect& viewPort) = 0;
  virtual void GetViewPort(CRect& viewPort) = 0;
  virtual void RestoreViewPort() {}

  virtual bool ScissorsCanEffectClipping() { return false; }
  virtual CRect ClipRectToScissorRect(const CRect &rect) { return CRect(); }
  virtual void SetScissors(const CRect &rect) = 0;
  virtual void ResetScissors() = 0;

  virtual void CaptureStateBlock() = 0;
  virtual void ApplyStateBlock() = 0;

  virtual void SetCameraPosition(const CPoint &camera, int screenWidth, int screenHeight, float stereoFactor = 0.f) = 0;

  /**
   * Project (x,y,z) 3d scene coordinates to (x,y) 2d screen coordinates
   */
  virtual void Project(float &x, float &y, float &z) { }

  void GetRenderVersion(unsigned int& major, unsigned int& minor) const;
  const std::string& GetRenderVendor() const { return m_RenderVendor; }
  const std::string& GetRenderRenderer() const { return m_RenderRenderer; }
  const std::string& GetRenderVersionString() const { return m_RenderVersion; }
  bool SupportsDXT() const;
  bool SupportsBGRA() const;
  bool SupportsBGRAApple() const;
  bool SupportsNPOT(bool dxt) const;
  unsigned int GetMaxTextureSize() const { return m_maxTextureSize; }
  unsigned int GetMinDXTPitch() const { return m_minDXTPitch; }

  virtual void ShowSplash(const std::string& message);

protected:
  bool                m_bRenderCreated;
  unsigned int        m_maxTextureSize;
  unsigned int        m_minDXTPitch;

  std::string   m_RenderRenderer;
  std::string   m_RenderVendor;
  std::string   m_RenderVersion;
  int          m_RenderVersionMinor;
  int          m_RenderVersionMajor;
  unsigned int m_renderCaps;

  std::unique_ptr<CGUIImage> m_splashImage;
  std::unique_ptr<CGUITextLayout> m_splashMessageLayout;
};

