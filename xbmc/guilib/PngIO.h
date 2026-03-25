/*
 *  Copyright (C) 2012-2018 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#pragma once

#include "iimage.h"

#include <spng.h>

class CPngIO : public IImage
{
public:
  explicit CPngIO();
  ~CPngIO() override;

  bool LoadImageFromMemory(unsigned char* buffer, unsigned int bufSize,
                           unsigned int width, unsigned int height) override;
  bool Decode(unsigned char * const pixels, unsigned int width, unsigned int height,
              unsigned int pitch, unsigned int format) override;
  bool CreateThumbnailFromSurface(unsigned char* bufferin, unsigned int width,
                                  unsigned int height, unsigned int format,
                                  unsigned int pitch, const std::string& destFile,
                                  unsigned char* &bufferout,
                                  unsigned int &bufferoutSize) override;
  void ReleaseThumbnailBuffer() override;

private:
  unsigned char* m_pngBuffer;
  unsigned int   m_pngSize;

  spng_ihdr      m_ihdr;
};
