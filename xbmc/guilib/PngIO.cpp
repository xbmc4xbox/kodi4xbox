/*
 *  Copyright (C) 2016-2018 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#include "PngIO.h"

#include <vector>

CPngIO::~CPngIO()
{
  m_buf.data = nullptr;
  m_buf.pos = 0;
  m_buf.size = 0;
}

bool CPngIO::LoadImageFromMemory(unsigned char* buffer, unsigned int bufSize,
                                unsigned int width, unsigned int height)
{
  png_structp png_ptr;
  png_infop info_ptr;

  /* Create and initialize the png_struct with the default error handler functions. */
  png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  if (png_ptr == nullptr)
  {
    return false;
  }

  /* Allocate/initialize the memory for image information.  REQUIRED. */
  info_ptr = png_create_info_struct(png_ptr);
  if (info_ptr == NULL)
  {
    png_destroy_read_struct(&png_ptr, NULL, NULL);
    return false;
  }

  /* Set error handling if you are using the setjmp/longjmp method */
  if (setjmp(png_jmpbuf(png_ptr)))
  {
    png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
    return false;
  }

  /* initialize our buffer */
  m_buf.data = buffer;
  m_buf.size = bufSize;
  m_buf.pos = 0;

  /* use custom I/O functions */
  png_set_read_fn(png_ptr, &m_buf, mem_read_data);

  /* read the file information */
  png_read_info(png_ptr, info_ptr);

  m_width  = png_get_image_width(png_ptr, info_ptr);
  m_height = png_get_image_height(png_ptr, info_ptr);
  m_originalWidth  = width;
  m_originalHeight = height;
  m_orientation = 0; // PNG has no EXIF orientation

  if (m_width > width || m_height > height)
  {
    png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
    return false;
  }

  /* Normalize PNG */
  int bitDepth = png_get_bit_depth(png_ptr, info_ptr);
  if (bitDepth == 16)
    png_set_strip_16(png_ptr);

  int colorType = png_get_color_type(png_ptr, info_ptr);
  if (colorType == PNG_COLOR_TYPE_PALETTE)
    png_set_palette_to_rgb(png_ptr);

  if (colorType == PNG_COLOR_TYPE_GRAY && bitDepth < 8)
    png_set_expand_gray_1_2_4_to_8(png_ptr);

  if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS))
    png_set_tRNS_to_alpha(png_ptr);

  /* Our renderer expects BGR order */
  png_set_bgr(png_ptr);

  png_read_update_info(png_ptr, info_ptr);

  colorType = png_get_color_type(png_ptr, info_ptr);
  m_hasAlpha = (colorType & PNG_COLOR_MASK_ALPHA) != 0;

  /* Read full image as RGBA or RGB */
  png_size_t rowBytes = png_get_rowbytes(png_ptr, info_ptr);

  std::vector<unsigned char> imageData(rowBytes * m_height);
  std::vector<png_bytep> rowPointers(m_height);

  for (unsigned int y = 0; y < m_height; y++)
    rowPointers[y] = imageData.data() + y * rowBytes;

  png_read_image(png_ptr, rowPointers.data());

  png_destroy_read_struct(&png_ptr, &info_ptr, NULL);

  /* Allocate engine buffers */
  unsigned int rgbPitch = ((m_width + 1) * 3 / 4) * 4;
  m_texture = new unsigned char[rgbPitch * m_height];
  m_alpha   = m_hasAlpha ? new unsigned char[m_width * m_height] : nullptr;

  /* Separate RGB and Alpha */
  for (unsigned int y = 0; y < m_height; y++)
  {
    unsigned int srcRow = m_height - 1 - y;
    unsigned char* src = imageData.data() + srcRow * rowBytes;
    unsigned char* dstRGB = m_texture + y * rgbPitch;
    unsigned char* dstA   = m_hasAlpha ? m_alpha + y * m_width : nullptr;

    for (unsigned int x = 0; x < m_width; x++)
    {
      dstRGB[x * 3 + 0] = src[x * (m_hasAlpha ? 4 : 3) + 0];
      dstRGB[x * 3 + 1] = src[x * (m_hasAlpha ? 4 : 3) + 1];
      dstRGB[x * 3 + 2] = src[x * (m_hasAlpha ? 4 : 3) + 2];

      if (m_hasAlpha)
        dstA[x] = src[x * 4 + 3];
    }
  }

  return true;
}

bool CPngIO::Decode(unsigned char * const pixels, unsigned int width, unsigned int height,
                    unsigned int pitch, unsigned int format)
{
  if (m_width == 0 || m_height == 0)
    return false;

  unsigned int dstPitch = pitch;
  unsigned int srcPitch = ((m_width + 1)* 3 / 4) * 4; // bitmap row length is aligned to 4 bytes

  unsigned char *dst = (unsigned char*)pixels;
  unsigned char *src = m_texture + (m_height - 1) * srcPitch;

  for (unsigned int y = 0; y < m_height; y++)
  {
    unsigned char *dst2 = dst;
    unsigned char *src2 = src;
    for (unsigned int x = 0; x < m_width; x++, dst2 += 4, src2 += 3)
    {
      dst2[0] = src2[0];
      dst2[1] = src2[1];
      dst2[2] = src2[2];
      dst2[3] = 0xff;
    }
    src -= srcPitch;
    dst += dstPitch;
  }

  if(m_hasAlpha)
  {
    dst = (unsigned char*)pixels + 3;
    src = m_alpha + (m_height - 1) * m_width;

    for (unsigned int y = 0; y < m_height; y++)
    {
      unsigned char *dst2 = dst;
      unsigned char *src2 = src;

      for (unsigned int x = 0; x < m_width; x++,  dst2+=4, src2++)
        *dst2 = *src2;
      src -= m_width;
      dst += dstPitch;
    }
  }

  delete[] m_texture;
  m_texture = nullptr;
  delete[] m_alpha;
  m_alpha = nullptr;
  return true;
}

bool CPngIO::CreateThumbnailFromSurface(unsigned char* bufferin, unsigned int width,
                                        unsigned int height, unsigned int format,
                                        unsigned int pitch,
                                        const std::string& destFile,
                                        unsigned char* &bufferout,
                                        unsigned int &bufferoutSize)
{
  return false;
}

void CPngIO::ReleaseThumbnailBuffer()
{

}
