/*
 *  Copyright (C) 2012-2018 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#include "PngIO.h"

#include "utils/log.h"

#include <vector>

CPngIO::CPngIO()
  : m_pngBuffer(nullptr)
  , m_pngSize(0)
{
  m_hasAlpha = false;
  memset(&m_ihdr, 0, sizeof(m_ihdr));
}

CPngIO::~CPngIO() = default;

bool CPngIO::LoadImageFromMemory(unsigned char* buffer, unsigned int bufSize,
                                unsigned int width, unsigned int height)
{
  if (!buffer || bufSize == 0)
    return false;

  m_pngBuffer = buffer;
  m_pngSize   = bufSize;

  spng_ctx* ctx = spng_ctx_new(0);
  if (!ctx)
    return false;

  if (spng_set_png_buffer(ctx, buffer, bufSize) != 0)
  {
    spng_ctx_free(ctx);
    return false;
  }

  if (spng_get_ihdr(ctx, &m_ihdr) != 0)
  {
    spng_ctx_free(ctx);
    return false;
  }

  m_width  = m_ihdr.width;
  m_height = m_ihdr.height;
  m_originalWidth  = m_width;
  m_originalHeight = m_height;
  m_orientation = 0;

  m_hasAlpha = (m_ihdr.color_type == 4 || m_ihdr.color_type == 6);

  spng_ctx_free(ctx);
  return true;
}

bool CPngIO::Decode(unsigned char * const pixels, unsigned int width, unsigned int height,
                    unsigned int pitch, unsigned int format)
{
  spng_ctx* ctx = spng_ctx_new(0);
  if (!ctx)
    return false;

  if (spng_set_png_buffer(ctx, m_pngBuffer, m_pngSize) != 0)
  {
    spng_ctx_free(ctx);
    return false;
  }

  size_t decoded_size = 0;
  if (spng_decoded_image_size(ctx, SPNG_FMT_RGBA8, &decoded_size) != 0)
  {
    spng_ctx_free(ctx);
    return false;
  }

  std::vector<unsigned char> temp(decoded_size);
  int ret = spng_decode_image(ctx, temp.data(), decoded_size, SPNG_FMT_RGBA8, 0);

  spng_ctx_free(ctx);

  if (ret != 0)
  {
    CLog::Log(LOGERROR, "libspng decode failed");
    return false;
  }

  // Convert RGBA → BGRA and respect pitch
  for (unsigned int y = 0; y < m_height; y++)
  {
    unsigned char* dst = pixels + (y * pitch);
    unsigned char* src = temp.data() + (y * m_width * 4);

    for (unsigned int x = 0; x < m_width; x++)
    {
      dst[0] = src[2]; // B
      dst[1] = src[1]; // G
      dst[2] = src[0]; // R
      dst[3] = src[3]; // A

      dst += 4;
      src += 4;
    }
  }

  return true;
}

bool CPngIO::CreateThumbnailFromSurface(unsigned char* bufferin, unsigned int width,
                                        unsigned int height, unsigned int format,
                                        unsigned int pitch,
                                        const std::string& destFile,
                                        unsigned char* &bufferout,
                                        unsigned int &bufferoutSize)
{
  bufferout = nullptr;
  bufferoutSize = 0;
  return false;
}

void CPngIO::ReleaseThumbnailBuffer()
{
}
