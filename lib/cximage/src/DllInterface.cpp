#ifdef _DLL
#include "ximage.h"

#include <sys/types.h>
#include <sys/stat.h>

extern "C" struct ImageInfo
{
  unsigned int width;
  unsigned int height;
  unsigned int originalwidth;
  unsigned int originalheight;
  EXIFINFO exifInfo;
  BYTE *texture;
  void *context;
  BYTE *alpha;
};

// helper functions

int ResampleKeepAspect(CxImage &image, unsigned int width, unsigned int height)
{
  bool bResize = false;
  float fAspect = ((float)image.GetWidth()) / ((float)image.GetHeight());
  unsigned int newwidth = image.GetWidth();
  unsigned int newheight = image.GetHeight();
  if (newwidth > width)
  {
    bResize = true;
    newwidth = width;
    newheight = (DWORD)( ( (float)newwidth) / fAspect);
  }
  if (newheight > height)
  {
    bResize = true;
    newheight = height;
    newwidth = (DWORD)( fAspect * ( (float)newheight) );
  }
  if (bResize)
  {
    if (!image.Resample(newwidth, newheight, 0) || !image.IsValid())
    {
      printf("PICTURE::SaveThumb: Unable to resample picture: Error:%s\n", image.GetLastError());
      return -1;
    }
  }
  return bResize ? 1 : 0;
}

bool IsPNG(const char *file)
{
  if ( !file || ( 0 == *file ) )
    return false;

  const char *ext = strrchr(file, '.');
  if (ext == NULL)
    ext = (char*)file;
  else
    ext++;

  if ( 0 == *ext )
    return false;

  return 0 == _strcmpi(ext, "png");
}

extern "C"
{
  __declspec(dllexport) bool ReleaseImage(ImageInfo *info)
  {
    if (info && info->context)
    {
      delete ((CxImage *)info->context);
      return true;
    }

    return false;
  }

  __declspec(dllexport) bool LoadImageFromMemory(const BYTE *buffer, unsigned int size, const char *mime, unsigned int maxwidth, unsigned int maxheight, ImageInfo *info)
  {
    if ( !IsPNG(mime) )
    {
      printf("PICTURE::LoadImageFromMemory: Only PNG images are supported.");
      return false;
    }

    if (!buffer || !size || !mime || !info) return false;

    // load the image
    CxImage *image = new CxImage(CXIMAGE_FORMAT_PNG);
    if (!image)
      return false;

    int actualwidth = maxwidth;
    int actualheight = maxheight;

    try
    {
      bool success = image->Decode((BYTE*)buffer, size, CXIMAGE_FORMAT_PNG, actualwidth, actualheight);
      if (!success || !image->IsValid())
      {
        printf("PICTURE::LoadImageFromMemory: Unable to decode image. Error:%s\n", image->GetLastError());
        delete image;
        return false;
      }
    }
    catch (...)
    {
      printf("PICTURE::LoadImageFromMemory: Unable to decode image.");
      delete image;
      return false;
    }

    // ok, now resample the image down if necessary
    if (ResampleKeepAspect(*image, maxwidth, maxheight) < 0)
    {
      printf("PICTURE::LoadImage: Unable to resample picture\n");
      delete image;
      return false;
    }

    // make sure our image is 24bit minimum
    image->IncreaseBpp(24);

    // fill in our struct
    info->width = image->GetWidth();
    info->height = image->GetHeight();
    info->originalwidth = actualwidth;
    info->originalheight = actualheight;
    memcpy(&info->exifInfo, image->GetExifInfo(), sizeof(EXIFINFO));

    // create our texture
    info->context = image;
    info->texture = image->GetBits();
    info->alpha = image->AlphaGetBits();
    return (info->texture != NULL);
  };

  __declspec(dllexport) bool CreateThumbnailFromSurface2(BYTE * bufferin, unsigned int width, unsigned int height, unsigned int stride, const char *thumb,
                                                         BYTE * &bufferout, unsigned int &bufferoutSize)
  {
    if ( !IsPNG(thumb) )
    {
      printf("PICTURE::CreateThumbnailFromSurface2: Only PNG images are supported.");
      return false;
    }

    if (!bufferin) return false;
    // creates an image, and copies the surface data into it.
    CxImage image(width, height, 24, CXIMAGE_FORMAT_PNG);
    if (!image.IsValid()) return false;
    image.AlphaCreate();
    if (!image.AlphaIsValid()) return false;
    bool fullyTransparent(true);
    bool fullyOpaque(true);
    for (unsigned int y = 0; y < height; y++)
    {
      BYTE *ptr = bufferin + (y * stride);
      for (unsigned int x = 0; x < width; x++)
      {
        BYTE b = *ptr++;
        BYTE g = *ptr++;
        BYTE r = *ptr++;
        BYTE a = *ptr++;  // alpha
        if (a)
          fullyTransparent = false;
        if (a != 0xff)
          fullyOpaque = false;
        image.SetPixelColor(x, height - 1 - y, RGB(r, g, b));
        image.AlphaSet(x, height - 1 - y, a);
      }
    }
    if (fullyTransparent || fullyOpaque)
      image.AlphaDelete();
    image.SetJpegQuality(90);

    long buffout=0;

    if (!image.Encode(bufferout, buffout, CXIMAGE_FORMAT_PNG))
    {
      printf("PICTURE::CreateThumbnailFromSurface: Unable to save thumb to %s", thumb);
      return false;
    }
    bufferoutSize = buffout;
    return true;
  };

  __declspec(dllexport) void	FreeMemory(void* memblock)
  {
    if (memblock)
      free(memblock);
  };
}

#endif
