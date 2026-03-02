/*
 *  Copyright (C) 2005-2018 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#include "GUITextureGL.h"

#include "ServiceBroker.h"
#include "Texture.h"
#include "utils/GLUtils.h"
#include "utils/Geometry.h"
#include "windowing/WinSystem.h"

#ifdef NXDK
#define glMultiTexCoord2fARB glMultiTexCoord2f
#endif

void CGUITextureGL::Register()
{
  CGUITexture::Register(CGUITextureGL::CreateTexture, CGUITextureGL::DrawQuad);
}

CGUITexture* CGUITextureGL::CreateTexture(
    float posX, float posY, float width, float height, const CTextureInfo& texture)
{
  return new CGUITextureGL(posX, posY, width, height, texture);
}

CGUITextureGL::CGUITextureGL(
    float posX, float posY, float width, float height, const CTextureInfo& texture)
  : CGUITexture(posX, posY, width, height, texture)
{
  memset(m_col, 0, sizeof(m_col));
}

CGUITextureGL* CGUITextureGL::Clone() const
{
  return new CGUITextureGL(*this);
}

void CGUITextureGL::Begin(UTILS::COLOR::Color color)
{
  int range, unit = 0;
  if(CServiceBroker::GetWinSystem()->UseLimitedColor())
    range = 235 - 16;
  else
    range = 255 -  0;

  m_col[0] = KODI::UTILS::GL::GetChannelFromARGB(KODI::UTILS::GL::ColorChannel::R, color) * range / 255;
  m_col[1] = KODI::UTILS::GL::GetChannelFromARGB(KODI::UTILS::GL::ColorChannel::G, color) * range / 255;
  m_col[2] = KODI::UTILS::GL::GetChannelFromARGB(KODI::UTILS::GL::ColorChannel::B, color) * range / 255;
  m_col[3] = KODI::UTILS::GL::GetChannelFromARGB(KODI::UTILS::GL::ColorChannel::A, color);

  CTexture* texture = m_texture.m_textures[m_currentFrame].get();
  texture->LoadToGPU();
  if (m_diffuse.size())
    m_diffuse.m_textures[0]->LoadToGPU();

  texture->BindToUnit(unit++);

  glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_BLEND);          // Turn Blending On
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

  // diffuse coloring
  glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE);
  glTexEnvf(GL_TEXTURE_ENV, GL_COMBINE_RGB, GL_MODULATE);
  glTexEnvf(GL_TEXTURE_ENV, GL_SOURCE0_RGB, GL_TEXTURE);
  glTexEnvf(GL_TEXTURE_ENV, GL_OPERAND0_RGB, GL_SRC_COLOR);
  glTexEnvf(GL_TEXTURE_ENV, GL_SOURCE1_RGB, GL_PRIMARY_COLOR);
  glTexEnvf(GL_TEXTURE_ENV, GL_OPERAND1_RGB, GL_SRC_COLOR);

  glTexEnvf(GL_TEXTURE_ENV, GL_COMBINE_ALPHA, GL_MODULATE);
  glTexEnvf(GL_TEXTURE_ENV, GL_SOURCE0_ALPHA, GL_TEXTURE);
  glTexEnvf(GL_TEXTURE_ENV, GL_SOURCE1_ALPHA, GL_PRIMARY_COLOR);
  glTexEnvf(GL_TEXTURE_ENV, GL_OPERAND0_ALPHA, GL_SRC_ALPHA);
  glTexEnvf(GL_TEXTURE_ENV, GL_OPERAND1_ALPHA, GL_SRC_ALPHA);
  VerifyGLState();

  if (m_diffuse.size())
  {
    m_diffuse.m_textures[0]->BindToUnit(unit++);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE);
    glTexEnvf(GL_TEXTURE_ENV, GL_COMBINE_RGB, GL_MODULATE);
    glTexEnvf(GL_TEXTURE_ENV, GL_SOURCE0_RGB, GL_TEXTURE);
    glTexEnvf(GL_TEXTURE_ENV, GL_OPERAND0_RGB, GL_SRC_COLOR);
    glTexEnvf(GL_TEXTURE_ENV, GL_SOURCE1_RGB, GL_PREVIOUS);
    glTexEnvf(GL_TEXTURE_ENV, GL_OPERAND1_RGB, GL_SRC_COLOR);

    glTexEnvf(GL_TEXTURE_ENV, GL_COMBINE_ALPHA, GL_MODULATE);
    glTexEnvf(GL_TEXTURE_ENV, GL_SOURCE0_ALPHA, GL_TEXTURE);
    glTexEnvf(GL_TEXTURE_ENV, GL_SOURCE1_ALPHA, GL_PREVIOUS);
    glTexEnvf(GL_TEXTURE_ENV, GL_OPERAND0_ALPHA, GL_SRC_ALPHA);
    glTexEnvf(GL_TEXTURE_ENV, GL_OPERAND1_ALPHA, GL_SRC_ALPHA);
    VerifyGLState();
  }

  if(CServiceBroker::GetWinSystem()->UseLimitedColor())
  {
    texture->BindToUnit(unit++); // dummy bind
    const GLfloat rgba[4] = {16.0f / 255.0f, 16.0f / 255.0f, 16.0f / 255.0f, 0.0f};
    glTexEnvi (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE , GL_COMBINE);
    glTexEnvfv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, rgba);
    glTexEnvi (GL_TEXTURE_ENV, GL_COMBINE_RGB      , GL_ADD);
    glTexEnvi (GL_TEXTURE_ENV, GL_SOURCE0_RGB      , GL_PREVIOUS);
    glTexEnvi (GL_TEXTURE_ENV, GL_SOURCE1_RGB      , GL_CONSTANT);
    glTexEnvi (GL_TEXTURE_ENV, GL_OPERAND0_RGB     , GL_SRC_COLOR);
    glTexEnvi (GL_TEXTURE_ENV, GL_OPERAND1_RGB     , GL_SRC_COLOR);

    glTexEnvi (GL_TEXTURE_ENV, GL_COMBINE_ALPHA    , GL_REPLACE);
    glTexEnvi (GL_TEXTURE_ENV, GL_SOURCE0_ALPHA    , GL_PREVIOUS);
    VerifyGLState();
  }

  //glDisable(GL_TEXTURE_2D); // uncomment these 2 lines to switch to wireframe rendering
  //glBegin(GL_LINE_LOOP);
  glBegin(GL_QUADS);
}

void CGUITextureGL::End()
{
  glEnd();
  glActiveTexture(GL_TEXTURE2_ARB);
  glBindTexture(GL_TEXTURE_2D, 0);
  glDisable(GL_TEXTURE_2D);
  glActiveTexture(GL_TEXTURE1_ARB);
  glBindTexture(GL_TEXTURE_2D, 0);
  glDisable(GL_TEXTURE_2D);
  glActiveTexture(GL_TEXTURE0_ARB);
  glBindTexture(GL_TEXTURE_2D, 0);
  glDisable(GL_TEXTURE_2D);
}

void CGUITextureGL::Draw(float *x, float *y, float *z, const CRect &texture, const CRect &diffuse, int orientation)
{
  // Top-left vertex (corner)
  glColor4ub(m_col[0], m_col[1], m_col[2], m_col[3]);
  glMultiTexCoord2fARB(GL_TEXTURE0_ARB, texture.x1, texture.y1);
  if (m_diffuse.size())
    glMultiTexCoord2fARB(GL_TEXTURE1_ARB, diffuse.x1, diffuse.y1);
  glVertex3f(x[0], y[0], z[0]);

  // Top-right vertex (corner)
  glColor4ub(m_col[0], m_col[1], m_col[2], m_col[3]);
  if (orientation & 4)
    glMultiTexCoord2fARB(GL_TEXTURE0_ARB, texture.x1, texture.y2);
  else
    glMultiTexCoord2fARB(GL_TEXTURE0_ARB, texture.x2, texture.y1);
  if (m_diffuse.size())
  {
    if (m_info.orientation & 4)
      glMultiTexCoord2fARB(GL_TEXTURE1_ARB, diffuse.x1, diffuse.y2);
    else
      glMultiTexCoord2fARB(GL_TEXTURE1_ARB, diffuse.x2, diffuse.y1);
  }
  glVertex3f(x[1], y[1], z[1]);

  // Bottom-right vertex (corner)
  glColor4ub(m_col[0], m_col[1], m_col[2], m_col[3]);
  glMultiTexCoord2fARB(GL_TEXTURE0_ARB, texture.x2, texture.y2);
  if (m_diffuse.size())
    glMultiTexCoord2fARB(GL_TEXTURE1_ARB, diffuse.x2, diffuse.y2);
  glVertex3f(x[2], y[2], z[2]);

  // Bottom-left vertex (corner)
  glColor4ub(m_col[0], m_col[1], m_col[2], m_col[3]);
  if (orientation & 4)
    glMultiTexCoord2fARB(GL_TEXTURE0_ARB, texture.x2, texture.y1);
  else
    glMultiTexCoord2fARB(GL_TEXTURE0_ARB, texture.x1, texture.y2);
  if (m_diffuse.size())
  {
    if (m_info.orientation & 4)
      glMultiTexCoord2fARB(GL_TEXTURE1_ARB, diffuse.x2, diffuse.y1);
    else
      glMultiTexCoord2fARB(GL_TEXTURE1_ARB, diffuse.x1, diffuse.y2);
  }
  glVertex3f(x[3], y[3], z[3]);
}

void CGUITextureGL::DrawQuad(const CRect& rect,
                             UTILS::COLOR::Color color,
                             CTexture* texture,
                             const CRect* texCoords)
{
  if (texture)
  {
    texture->LoadToGPU();
    texture->BindToUnit(0);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE);
    glTexEnvf(GL_TEXTURE_ENV, GL_COMBINE_RGB, GL_MODULATE);
    glTexEnvf(GL_TEXTURE_ENV, GL_SOURCE0_RGB, GL_TEXTURE1);
    glTexEnvf(GL_TEXTURE_ENV, GL_OPERAND0_RGB, GL_SRC_COLOR);
    glTexEnvf(GL_TEXTURE_ENV, GL_SOURCE1_RGB, GL_PREVIOUS);
    glTexEnvf(GL_TEXTURE_ENV, GL_OPERAND1_RGB, GL_SRC_COLOR);
  }
  else
  glDisable(GL_TEXTURE_2D);

  glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_BLEND);          // Turn Blending On
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

  // diffuse coloring
  glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE);
  glTexEnvf(GL_TEXTURE_ENV, GL_COMBINE_RGB, GL_MODULATE);
  glTexEnvf(GL_TEXTURE_ENV, GL_SOURCE0_RGB, GL_TEXTURE0);
  glTexEnvf(GL_TEXTURE_ENV, GL_OPERAND0_RGB, GL_SRC_COLOR);
  glTexEnvf(GL_TEXTURE_ENV, GL_SOURCE1_RGB, GL_PRIMARY_COLOR);
  glTexEnvf(GL_TEXTURE_ENV, GL_OPERAND1_RGB, GL_SRC_COLOR);
  VerifyGLState();

  glBegin(GL_QUADS);

  glColor4ub((GLubyte)KODI::UTILS::GL::GetChannelFromARGB(KODI::UTILS::GL::ColorChannel::R, color),
             (GLubyte)KODI::UTILS::GL::GetChannelFromARGB(KODI::UTILS::GL::ColorChannel::G, color),
             (GLubyte)KODI::UTILS::GL::GetChannelFromARGB(KODI::UTILS::GL::ColorChannel::B, color),
             (GLubyte)KODI::UTILS::GL::GetChannelFromARGB(KODI::UTILS::GL::ColorChannel::A, color));

  CRect coords = texCoords ? *texCoords : CRect(0.0f, 0.0f, 1.0f, 1.0f);
  glTexCoord2f(coords.x1, coords.y1);
  glVertex3f(rect.x1, rect.y1, 0);
  glTexCoord2f(coords.x2, coords.y1);
  glVertex3f(rect.x2, rect.y1, 0);
  glTexCoord2f(coords.x2, coords.y2);
  glVertex3f(rect.x2, rect.y2, 0);
  glTexCoord2f(coords.x1, coords.y2);
  glVertex3f(rect.x1, rect.y2, 0);

  glEnd();
  if (texture)
    glDisable(GL_TEXTURE_2D);
}

