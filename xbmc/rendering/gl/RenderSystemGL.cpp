/*
 *  Copyright (C) 2005-2018 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#include "RenderSystemGL.h"

#include "guilib/GUITextureGL.h"
#include "rendering/MatrixGL.h"
#include "utils/GLUtils.h"
#include "utils/MathUtils.h"
#include "utils/XTimeUtils.h"
#include "utils/log.h"

#ifdef NXDK
#include <pbgl.h>
#endif

using namespace std::chrono_literals;

CRenderSystemGL::CRenderSystemGL() : CRenderSystemBase()
{
}

CRenderSystemGL::~CRenderSystemGL() = default;

bool CRenderSystemGL::InitRenderSystem()
{
#ifdef _XBOX
  pbgl_init(GL_TRUE);
#endif

  m_maxTextureSize = 2048;
  m_renderCaps = 0;

  // Get the GL version number
  m_RenderVersionMajor = 0;
  m_RenderVersionMinor = 0;
  m_RenderVersion = "<none>";
  const char* ver = reinterpret_cast<const char*>(glGetString(GL_VERSION));
  if (ver)
  {
    sscanf(ver, "%d.%d", &m_RenderVersionMajor, &m_RenderVersionMinor);
    m_RenderVersion = ver;
  }

  CLog::Log(LOGINFO, "CRenderSystemGL::{} - Version: {}, Major: {}, Minor: {}", __FUNCTION__,
            m_RenderVersion, m_RenderVersionMajor, m_RenderVersionMinor);

  m_RenderExtensions = " ";
  const char* extensions = reinterpret_cast<const char*>(glGetString(GL_EXTENSIONS));
  if (extensions)
  {
    m_RenderExtensions += extensions;
    m_RenderExtensions += " ";
  }

  LogGraphicsInfo();

  // Get our driver vendor and renderer
  const char* tmpVendor = reinterpret_cast<const char*>(glGetString(GL_VENDOR));
  m_RenderVendor.clear();
  if (tmpVendor)
    m_RenderVendor = tmpVendor;

  const char* tmpRenderer = reinterpret_cast<const char*>(glGetString(GL_RENDERER));
  m_RenderRenderer.clear();
  if (tmpRenderer)
    m_RenderRenderer = tmpRenderer;

  // grab our capabilities
  if (IsExtSupported("GL_EXT_texture_compression_s3tc"))
    m_renderCaps |= RENDER_CAPS_DXT;

  if (IsExtSupported("GL_ARB_texture_non_power_of_two"))
  {
    m_renderCaps |= RENDER_CAPS_NPOT;
    if (m_renderCaps & RENDER_CAPS_DXT)
      m_renderCaps |= RENDER_CAPS_DXT_NPOT;
  }

  m_bRenderCreated = true;

  CGUITextureGL::Register();

  return true;
}

bool CRenderSystemGL::ResetRenderSystem(int width, int height)
{
  if (!m_bRenderCreated)
    return false;

  m_width = width;
  m_height = height;

  glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );

  CalculateMaxTexturesize();

  CRect rect( 0, 0, width, height );
  SetViewPort( rect );

  glEnable(GL_TEXTURE_2D);
  glEnable(GL_SCISSOR_TEST);

  glMatrixProject.Clear();
  glMatrixProject->LoadIdentity();
  glMatrixProject->Ortho(0.0f, width-1, height-1, 0.0f, -1.0f, 1.0f);
  glMatrixProject.Load();

  glMatrixModview.Clear();
  glMatrixModview->LoadIdentity();
  glMatrixModview.Load();

  glMatrixTexture.Clear();
  glMatrixTexture->LoadIdentity();
  glMatrixTexture.Load();

  if (IsExtSupported("GL_ARB_multitexture"))
  {
    //clear error flags
    ResetGLErrors();

#ifdef NXDK
    GLint maxtex = 4;
#else
    GLint maxtex;
    glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &maxtex);

    //some sanity checks
    GLenum error = glGetError();
    if (error != GL_NO_ERROR)
    {
      CLog::Log(LOGERROR, "ResetRenderSystem() GL_MAX_TEXTURE_IMAGE_UNITS returned error {}",
                (int)error);
      maxtex = 3;
    }
    else if (maxtex < 1 || maxtex > 32)
    {
      CLog::Log(LOGERROR,
                "ResetRenderSystem() GL_MAX_TEXTURE_IMAGE_UNITS returned invalid value {}",
                (int)maxtex);
      maxtex = 3;
    }
#endif

    //reset texture matrix for all textures
    for (GLint i = 0; i < maxtex; i++)
    {
      glActiveTexture(GL_TEXTURE0 + i);
      glMatrixTexture.Load();
    }
    glActiveTexture(GL_TEXTURE0);
  }

  glBlendFunc(GL_SRC_ALPHA, GL_ONE);
  glEnable(GL_BLEND);          // Turn Blending On
  glDisable(GL_DEPTH_TEST);

  return true;
}

bool CRenderSystemGL::DestroyRenderSystem()
{
  m_bRenderCreated = false;

#ifdef _XBOX
  pbgl_shutdown();
#endif
  return true;
}

bool CRenderSystemGL::BeginRender()
{
  if (!m_bRenderCreated)
    return false;

  return true;
}

bool CRenderSystemGL::EndRender()
{
  if (!m_bRenderCreated)
    return false;

  return true;
}

bool CRenderSystemGL::ClearBuffers(UTILS::COLOR::Color color)
{
  if (!m_bRenderCreated)
    return false;

  float r = KODI::UTILS::GL::GetChannelFromARGB(KODI::UTILS::GL::ColorChannel::R, color) / 255.0f;
  float g = KODI::UTILS::GL::GetChannelFromARGB(KODI::UTILS::GL::ColorChannel::G, color) / 255.0f;
  float b = KODI::UTILS::GL::GetChannelFromARGB(KODI::UTILS::GL::ColorChannel::B, color) / 255.0f;
  float a = KODI::UTILS::GL::GetChannelFromARGB(KODI::UTILS::GL::ColorChannel::A, color) / 255.0f;

  glClearColor(r, g, b, a);

  GLbitfield flags = GL_COLOR_BUFFER_BIT;
  glClear(flags);

  return true;
}

bool CRenderSystemGL::IsExtSupported(const char* extension) const
{
  std::string name;
  name  = " ";
  name += extension;
  name += " ";

  return m_RenderExtensions.find(name) != std::string::npos;
}

void CRenderSystemGL::PresentRender(bool rendered, bool videoLayer)
{
  if (!m_bRenderCreated)
    return;

  PresentRenderImpl(rendered);

  if (!rendered)
    KODI::TIME::Sleep(40ms);
}

void CRenderSystemGL::CaptureStateBlock()
{
  if (!m_bRenderCreated)
    return;

  glMatrixProject.Push();
  glMatrixModview.Push();
  glMatrixTexture.Push();

  glDisable(GL_SCISSOR_TEST); // fixes FBO corruption on Macs
  glActiveTexture(GL_TEXTURE0_ARB);
  glDisable(GL_TEXTURE_2D);
  glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
  glColor3f(1.0, 1.0, 1.0);
}

void CRenderSystemGL::ApplyStateBlock()
{
  if (!m_bRenderCreated)
    return;

  glViewport(m_viewPort[0], m_viewPort[1], m_viewPort[2], m_viewPort[3]);

  glMatrixProject.PopLoad();
  glMatrixModview.PopLoad();
  glMatrixTexture.PopLoad();

  glActiveTexture(GL_TEXTURE0_ARB);
  glEnable(GL_TEXTURE_2D);
  glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
  glEnable(GL_BLEND);
  glEnable(GL_SCISSOR_TEST);
}

void CRenderSystemGL::SetCameraPosition(const CPoint &camera, int screenWidth, int screenHeight, float stereoFactor)
{
  if (!m_bRenderCreated)
    return;

  CPoint offset = camera - CPoint(screenWidth*0.5f, screenHeight*0.5f);


  float w = (float)m_viewPort[2]*0.5f;
  float h = (float)m_viewPort[3]*0.5f;

  glMatrixModview->LoadIdentity();
  glMatrixModview->Translatef(-(w + offset.x - stereoFactor), +(h + offset.y), 0);
  glMatrixModview->LookAt(0.0f, 0.0f, -2.0f * h, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f);
  glMatrixModview.Load();

  glMatrixProject->LoadIdentity();
  glMatrixProject->Frustum( (-w - offset.x)*0.5f, (w - offset.x)*0.5f, (-h + offset.y)*0.5f, (h + offset.y)*0.5f, h, 100*h);
  glMatrixProject.Load();
}

void CRenderSystemGL::Project(float &x, float &y, float &z)
{
  GLfloat coordX, coordY, coordZ;
  if (CMatrixGL::Project(x, y, z, glMatrixModview.Get(), glMatrixProject.Get(), m_viewPort, &coordX, &coordY, &coordZ))
  {
    x = coordX;
    y = (float)(m_viewPort[1] + m_viewPort[3] - coordY);
    z = 0;
  }
}

void CRenderSystemGL::CalculateMaxTexturesize()
{
#ifdef _XBOX
  m_maxTextureSize = 2048;
#else
  GLint width = 256;

  // reset any previous GL errors
  ResetGLErrors();

  // max out at 2^(8+8)
  for (int i = 0 ; i<8 ; i++)
  {
    glTexImage2D(GL_PROXY_TEXTURE_2D, 0, GL_RGBA, width, width, 0, GL_BGRA,
                 GL_UNSIGNED_BYTE, NULL);
    glGetTexLevelParameteriv(GL_PROXY_TEXTURE_2D, 0, GL_TEXTURE_WIDTH,
                             &width);

    // GMA950 on OS X sets error instead
    if (width == 0 || (glGetError() != GL_NO_ERROR) )
      break;

    m_maxTextureSize = width;
    width *= 2;
    if (width > 65536) // have an upper limit in case driver acts stupid
    {
      CLog::Log(LOGERROR, "GL: Could not determine maximum texture width, falling back to 2048");
      m_maxTextureSize = 2048;
      break;
    }
  }
#endif

  CLog::Log(LOGINFO, "GL: Maximum texture width: {}", m_maxTextureSize);
}

void CRenderSystemGL::GetViewPort(CRect& viewPort)
{
  if (!m_bRenderCreated)
    return;

  viewPort.x1 = m_viewPort[0];
  viewPort.y1 = m_height - m_viewPort[1] - m_viewPort[3];
  viewPort.x2 = m_viewPort[0] + m_viewPort[2];
  viewPort.y2 = viewPort.y1 + m_viewPort[3];
}

void CRenderSystemGL::SetViewPort(const CRect& viewPort)
{
  if (!m_bRenderCreated)
    return;

  glScissor((GLint) viewPort.x1, (GLint) (m_height - viewPort.y1 - viewPort.Height()), (GLsizei) viewPort.Width(), (GLsizei) viewPort.Height());
  glViewport((GLint) viewPort.x1, (GLint) (m_height - viewPort.y1 - viewPort.Height()), (GLsizei) viewPort.Width(), (GLsizei) viewPort.Height());
  m_viewPort[0] = viewPort.x1;
  m_viewPort[1] = m_height - viewPort.y1 - viewPort.Height();
  m_viewPort[2] = viewPort.Width();
  m_viewPort[3] = viewPort.Height();
}

void CRenderSystemGL::SetScissors(const CRect &rect)
{
  if (!m_bRenderCreated)
    return;
  GLint x1 = MathUtils::round_int(static_cast<double>(rect.x1));
  GLint y1 = MathUtils::round_int(static_cast<double>(rect.y1));
  GLint x2 = MathUtils::round_int(static_cast<double>(rect.x2));
  GLint y2 = MathUtils::round_int(static_cast<double>(rect.y2));
  glScissor(x1, m_height - y2, x2-x1, y2-y1);
}

void CRenderSystemGL::ResetScissors()
{
  SetScissors(CRect(0, 0, (float)m_width, (float)m_height));
}

void CRenderSystemGL::ResetGLErrors()
{
  int count = 0;
  while (glGetError() != GL_NO_ERROR)
  {
    count++;
    if (count >= 100)
    {
      CLog::Log(
          LOGWARNING,
          "CRenderSystemGL::ResetGLErrors glGetError didn't return GL_NO_ERROR after {} iterations",
          count);
      break;
    }
  }
}

