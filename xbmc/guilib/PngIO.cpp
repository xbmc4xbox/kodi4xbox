#include "PngIO.h"

CPngIO::CPngIO()
{
  m_inputBuff = nullptr;
  m_inputBuffSize = 0; 
}

CPngIO::~CPngIO()
{

}

bool CPngIO::LoadImageFromMemory(unsigned char* buffer, unsigned int bufSize,
                                unsigned int width, unsigned int height)
{
  return false;
}

bool CPngIO::Decode(unsigned char * const pixels, unsigned int width, unsigned int height,
                    unsigned int pitch, unsigned int format)
{
  return false;
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
