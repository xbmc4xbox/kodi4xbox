/*
 *  Copyright (C) 2005-2018 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#include "SlideShowPictureGL.h"

#include "ServiceBroker.h"
#include "guilib/Texture.h"
#include "rendering/gl/RenderSystemGL.h"
#include "utils/GLUtils.h"
#include "windowing/WinSystem.h"

std::unique_ptr<CSlideShowPic> CSlideShowPic::CreateSlideShowPicture()
{
  return std::make_unique<CSlideShowPicGL>();
}

void CSlideShowPicGL::Render(float* x, float* y, CTexture* pTexture, UTILS::COLOR::Color color)
{
  if (pTexture)
  {
    int unit = 0;
    pTexture->LoadToGPU();
    pTexture->BindToUnit(unit++);

    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);          // Turn Blending On

    // diffuse coloring
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE);
    glTexEnvf(GL_TEXTURE_ENV, GL_COMBINE_RGB, GL_MODULATE);
    glTexEnvf(GL_TEXTURE_ENV, GL_SOURCE0_RGB, GL_TEXTURE0);
    glTexEnvf(GL_TEXTURE_ENV, GL_OPERAND0_RGB, GL_SRC_COLOR);
    glTexEnvf(GL_TEXTURE_ENV, GL_SOURCE1_RGB, GL_PRIMARY_COLOR);
    glTexEnvf(GL_TEXTURE_ENV, GL_OPERAND1_RGB, GL_SRC_COLOR);

    if(CServiceBroker::GetWinSystem()->UseLimitedColor())
    {
      // compress range
      pTexture->BindToUnit(unit++); // dummy bind
      const GLfloat rgba1[4] = {(235.0 - 16.0f) / 255.0f, (235.0 - 16.0f) / 255.0f, (235.0 - 16.0f) / 255.0f, 1.0f};
      glTexEnvi (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE , GL_COMBINE);
      glTexEnvfv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, rgba1);
      glTexEnvi (GL_TEXTURE_ENV, GL_COMBINE_RGB , GL_MODULATE);
      glTexEnvi (GL_TEXTURE_ENV, GL_SOURCE0_RGB , GL_PREVIOUS);
      glTexEnvi (GL_TEXTURE_ENV, GL_SOURCE1_RGB , GL_CONSTANT);
      glTexEnvi (GL_TEXTURE_ENV, GL_OPERAND0_RGB , GL_SRC_COLOR);
      glTexEnvi (GL_TEXTURE_ENV, GL_OPERAND1_RGB , GL_SRC_COLOR);
      glTexEnvi (GL_TEXTURE_ENV, GL_COMBINE_ALPHA , GL_REPLACE);
      glTexEnvi (GL_TEXTURE_ENV, GL_SOURCE0_ALPHA , GL_PREVIOUS);

      // transition
      pTexture->BindToUnit(unit++); // dummy bind
      const GLfloat rgba2[4] = {16.0f / 255.0f, 16.0f / 255.0f, 16.0f / 255.0f, 0.0f};
      glTexEnvi (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE , GL_COMBINE);
      glTexEnvfv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, rgba2);
      glTexEnvi (GL_TEXTURE_ENV, GL_COMBINE_RGB , GL_ADD);
      glTexEnvi (GL_TEXTURE_ENV, GL_SOURCE0_RGB , GL_PREVIOUS);
      glTexEnvi (GL_TEXTURE_ENV, GL_SOURCE1_RGB , GL_CONSTANT);
      glTexEnvi (GL_TEXTURE_ENV, GL_OPERAND0_RGB , GL_SRC_COLOR);
      glTexEnvi (GL_TEXTURE_ENV, GL_OPERAND1_RGB , GL_SRC_COLOR);
      glTexEnvi (GL_TEXTURE_ENV, GL_COMBINE_ALPHA , GL_REPLACE);
      glTexEnvi (GL_TEXTURE_ENV, GL_SOURCE0_ALPHA , GL_PREVIOUS);
    }
  }
}
