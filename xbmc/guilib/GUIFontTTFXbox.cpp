/*
 *      Copyright (C) 2005-2015 Team Kodi
 *      http://kodi.tv
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
 *  along with Kodi; see the file COPYING.  If not, see
 *  <http://www.gnu.org/licenses/>.
 *
 */

#include "GUIFont.h"
#include "GUIFontTTFXbox.h"
#include "GUIFontManager.h"
#include "Texture.h"
#include "TextureManager.h"
#include "utils/log.h"
#include "utils/GLUtils.h"

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H
#include FT_OUTLINE_H

#if defined(_XBOX) && defined(HAS_GL)

CGUIFontTTFXbox::CGUIFontTTFXbox(const std::string& strFileName)
: CGUIFontTTFBase(strFileName)
{
  m_updateY1 = 0;
  m_updateY2 = 0;
  m_textureStatus = TEXTURE_VOID;
}

CGUIFontTTFXbox::~CGUIFontTTFXbox(void)
{
  // It's important that all the CGUIFontCacheEntry objects are
  // destructed before the CGUIFontTTFXbox goes out of scope, because
  // our virtual methods won't be accessible after this point
  m_dynamicCache.Flush();
  DeleteHardwareTexture();
}

bool CGUIFontTTFXbox::FirstBegin()
{
  if (m_textureStatus == TEXTURE_REALLOCATED)
  {
    if (glIsTexture(m_nTexture))
      CServiceBroker::GetGUI()->GetTextureManager().ReleaseHwTexture(m_nTexture);
    m_textureStatus = TEXTURE_VOID;
  }

  if (m_textureStatus == TEXTURE_VOID)
  {
    glGenTextures(1, &m_nTexture);
    glBindTexture(GL_TEXTURE_2D, m_nTexture);

    glEnable(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int padW = CTexture::PadPow2(m_texture->GetWidth());
    int padH = CTexture::PadPow2(m_texture->GetHeight());

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, padW, padH, 0, GL_ALPHA, GL_UNSIGNED_BYTE, nullptr);

    m_textureStatus = TEXTURE_UPDATED;
  }

  if (m_textureStatus == TEXTURE_UPDATED)
  {
    glBindTexture(GL_TEXTURE_2D, m_nTexture);

    if (m_updateY2 >= m_updateY1) 
    {
      glTexSubImage2D(GL_TEXTURE_2D, 0,
                      0, m_updateY1,
                      m_texture->GetWidth(),
                      m_updateY2 - m_updateY1,
                      GL_ALPHA, GL_UNSIGNED_BYTE,
                      m_texture->GetPixels() + m_updateY1 * m_texture->GetPitch());
    } 
    else 
    {
      CLog::Log(LOGWARNING, "CGUIFontTTFXbox::FirstBegin: update range invalid Y1={}, Y2={}", m_updateY1, m_updateY2);
    }

    m_updateY1 = m_updateY2 = 0;
    m_textureStatus = TEXTURE_READY;
  }

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, m_nTexture);

  return true;
}

void CGUIFontTTFXbox::LastEnd()
{
  if (m_vertex.empty())
    return;

  if (m_vertex.size() % 4 != 0)
  {
    CLog::Log(LOGERROR, "CGUIFontTTFXbox::LastEnd: m_vertex.size() not multiple of 4 ({})", m_vertex.size());
    return;
  }

  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, m_nTexture);

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  glBegin(GL_QUADS);
  for (size_t i = 0; i < m_vertex.size(); i += 4)
  {
    glColor4ub(m_vertex[i].r, m_vertex[i].g, m_vertex[i].b, m_vertex[i].a);
    glTexCoord2f(m_vertex[i].u, m_vertex[i].v); glVertex3f(m_vertex[i].x, m_vertex[i].y, m_vertex[i].z);

    glColor4ub(m_vertex[i+1].r, m_vertex[i+1].g, m_vertex[i+1].b, m_vertex[i+1].a);
    glTexCoord2f(m_vertex[i+1].u, m_vertex[i+1].v); glVertex3f(m_vertex[i+1].x, m_vertex[i+1].y, m_vertex[i+1].z);

    glColor4ub(m_vertex[i+2].r, m_vertex[i+2].g, m_vertex[i+2].b, m_vertex[i+2].a);
    glTexCoord2f(m_vertex[i+2].u, m_vertex[i+2].v); glVertex3f(m_vertex[i+2].x, m_vertex[i+2].y, m_vertex[i+2].z);

    glColor4ub(m_vertex[i+3].r, m_vertex[i+3].g, m_vertex[i+3].b, m_vertex[i+3].a);
    glTexCoord2f(m_vertex[i+3].u, m_vertex[i+3].v); glVertex3f(m_vertex[i+3].x, m_vertex[i+3].y, m_vertex[i+3].z);
  }
  glEnd();
  m_vertex.clear();

  glDisable(GL_BLEND);
}

std::unique_ptr<CTexture> CGUIFontTTFXbox::ReallocTexture(unsigned int& newHeight)
{
  newHeight = CTexture::PadPow2(newHeight);

  std::unique_ptr<CTexture> newTexture = CTexture::CreateTexture(m_textureWidth, newHeight, XB_FMT_A8);
  if (!newTexture || newTexture->GetPixels() == NULL)
  {
    CLog::Log(LOGERROR, "CGUIFontTTFGL::ReallocTexture: Error creating new cache texture for size {}", m_height);
    newTexture.reset(nullptr);
    return nullptr;
  }

  m_textureHeight = newTexture->GetHeight();
  m_textureScaleY = 1.0f / m_textureHeight;
  m_textureWidth = newTexture->GetWidth();
  m_textureScaleX = 1.0f / m_textureWidth;

  if (m_textureHeight < newHeight)
    CLog::Log(LOGWARNING, "CGUIFontTTFXbox::ReallocTexture: Allocated new texture with height of {}, requested {}", m_textureHeight, newHeight);

  m_staticCache.Flush();
  m_dynamicCache.Flush();

  memset(newTexture->GetPixels(), 0, m_textureHeight * newTexture->GetPitch());

  if (m_texture)
  {
    m_updateY1 = 0;
    m_updateY2 = m_texture->GetHeight();

    unsigned char* src = (unsigned char*) m_texture->GetPixels();
    unsigned char* dst = (unsigned char*) newTexture->GetPixels();
    for (unsigned int y = 0; y < m_texture->GetHeight(); y++)
    {
      memcpy(dst, src, m_texture->GetPitch());
      src += m_texture->GetPitch();
      dst += newTexture->GetPitch();
    }
    m_texture.reset();
  }

  m_textureStatus = TEXTURE_REALLOCATED;
  return newTexture;
}

bool CGUIFontTTFXbox::CopyCharToTexture(FT_BitmapGlyph bitGlyph,
                                      unsigned int x1, unsigned int y1,
                                      unsigned int x2, unsigned int y2)
{
  if (!m_texture)
  {
    CLog::Log(LOGERROR, "CGUIFontTTFXbox::CopyCharToTexture: texture is null!");
    return false;
  }

  FT_Bitmap& bitmap = bitGlyph->bitmap;

  if (!bitmap.buffer || bitmap.width == 0 || bitmap.rows == 0)
  {
    CLog::Log(LOGWARNING, "CGUIFontTTFXbox::CopyCharToTexture: empty glyph bitmap");
    return false;
  }

  unsigned char* source = bitmap.buffer;
  unsigned char* target = m_texture->GetPixels() + y1 * m_texture->GetPitch() + x1;

  unsigned int copyWidth  = std::min<unsigned int>(x2 - x1, bitmap.width);
  unsigned int copyHeight = std::min<unsigned int>(y2 - y1, bitmap.rows);

  for (unsigned int row = 0; row < copyHeight; row++)
  {
    unsigned int srcRow = row;
    memcpy(target, source + srcRow * bitmap.width, copyWidth);
    target += m_texture->GetPitch();
  }

  switch (m_textureStatus)
  {
    case TEXTURE_UPDATED:
      if (m_updateY1 > y1) m_updateY1 = y1;
      if (m_updateY2 < y2) m_updateY2 = y2;
      break;

    case TEXTURE_READY:
      m_updateY1 = y1;
      m_updateY2 = y2;
      m_textureStatus = TEXTURE_UPDATED;
      break;

    case TEXTURE_REALLOCATED:
      m_updateY2 = std::max(m_updateY2, y2);
      break;

    case TEXTURE_VOID:
    default:
      break;
  }

  return true;
}

void CGUIFontTTFXbox::DeleteHardwareTexture()
{
  if (m_textureStatus != TEXTURE_VOID)
  {
    if (glIsTexture(m_nTexture))
      CServiceBroker::GetGUI()->GetTextureManager().ReleaseHwTexture(m_nTexture);

    m_textureStatus = TEXTURE_VOID;
    m_updateY1 = m_updateY2 = 0;
  }
}

#endif
