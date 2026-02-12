/*
 *  Copyright (C) 2016-2018 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#pragma once

#include "iimage.h"

#include <png.h>

struct MemBuffer
{
  const unsigned char* data = nullptr;
  size_t size = 0;
  size_t pos = 0;
};

class CPngIO : public IImage
{
public:
  CPngIO() = default;
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

protected:
	static void mem_read_data(png_structp png_ptr, png_bytep outBuffer, png_size_t length)
	{
    MemBuffer* buffer = (MemBuffer*)(png_get_io_ptr(png_ptr));
    if (!buffer || buffer->pos + length > buffer->size)
      png_error(png_ptr, "Read Error");

    memcpy(outBuffer, buffer->data + buffer->pos, length);
    buffer->pos += length;
	}

private:
  MemBuffer m_buf;

  unsigned char* m_texture = nullptr;
  unsigned char* m_alpha = nullptr;
};
