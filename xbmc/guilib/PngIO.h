#include "iimage.h"

class CPngIO : public IImage
{
public:
  CPngIO();
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
  std::string m_texturePath;

  unsigned char* m_inputBuff;
  unsigned int m_inputBuffSize;
};
