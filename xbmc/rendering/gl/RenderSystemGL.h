/*
 *  Copyright (C) 2005-2018 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#pragma once

#include "rendering/RenderSystem.h"

#include "system_gl.h"

class CRenderSystemGL : public CRenderSystemBase
{
public:
  CRenderSystemGL();
  ~CRenderSystemGL() override;
  bool InitRenderSystem() override;
  bool DestroyRenderSystem() override;
  bool ResetRenderSystem(int width, int height) override;

  bool BeginRender() override;
  bool EndRender() override;
  void PresentRender(bool rendered, bool videoLayer) override;
  bool ClearBuffers(UTILS::COLOR::Color color) override;
  bool IsExtSupported(const char* extension) const override;

  void SetViewPort(const CRect& viewPort) override;
  void GetViewPort(CRect& viewPort) override;

  void SetScissors(const CRect &rect) override;
  void ResetScissors() override;

  void CaptureStateBlock() override;
  void ApplyStateBlock() override;

  void SetCameraPosition(const CPoint &camera, int screenWidth, int screenHeight, float stereoFactor = 0.0f) override;

  void Project(float &x, float &y, float &z) override;

  void ResetGLErrors();

protected:
  virtual void PresentRenderImpl(bool rendered) = 0;
  void CalculateMaxTexturesize();

  int m_width;
  int m_height;

  std::string m_RenderExtensions;

  GLint m_viewPort[4];
};
