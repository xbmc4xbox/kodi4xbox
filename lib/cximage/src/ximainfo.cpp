// ximainfo.cpp : main attributes
/* 03/10/2004 v1.00 - Davide Pizzolato - www.xdp.it
 * CxImage version 6.0.0 02/Feb/2008
 */

#include "ximage.h"
#if defined(_LINUX) || defined(__APPLE__)
#define _tcsnicmp(a,b,c) strcasecmp(a, b)
#endif

////////////////////////////////////////////////////////////////////////////////
/**
 * \return the color used for transparency, and/or for background color
 */
RGBQUAD	CxImage::GetTransColor()
{
	if (head.biBitCount<24 && info.nBkgndIndex>=0) return GetPaletteColor((BYTE)info.nBkgndIndex);
	return info.nBkgndColor;
}
////////////////////////////////////////////////////////////////////////////////
/**
 * Gets the index used for transparency. Returns -1 for no transparancy.
 */
long CxImage::GetTransIndex() const
{
	return info.nBkgndIndex;
}
////////////////////////////////////////////////////////////////////////////////
/**
 * Sets the index used for transparency with 1, 4 and 8 bpp images. Set to -1 to remove the effect.
 */
void CxImage::SetTransIndex(long idx)
{
	if (idx<(long)head.biClrUsed)
		info.nBkgndIndex = idx;
	else 
		info.nBkgndIndex = 0;
}
////////////////////////////////////////////////////////////////////////////////
/**
 * Sets the color used for transparency with 24 bpp images.
 * You must call SetTransIndex(0) to enable the effect, SetTransIndex(-1) to disable it.
 */
void CxImage::SetTransColor(RGBQUAD rgb)
{
	rgb.rgbReserved=0;
	info.nBkgndColor = rgb;
}
////////////////////////////////////////////////////////////////////////////////
bool CxImage::IsTransparent() const
{
	return info.nBkgndIndex>=0; // <vho>
}
////////////////////////////////////////////////////////////////////////////////
/**
 * Returns true if the image has 256 colors or less.
 */
bool CxImage::IsIndexed() const
{
	return head.biClrUsed!=0;
}
////////////////////////////////////////////////////////////////////////////////
/**
 * \return 1 = indexed, 2 = RGB, 4 = RGBA
 */
BYTE CxImage::GetColorType()
{
	BYTE b = (BYTE)((head.biBitCount>8) ? 2 /*COLORTYPE_COLOR*/ : 1 /*COLORTYPE_PALETTE*/);
#if CXIMAGE_SUPPORT_ALPHA
	if (AlphaIsValid()) b = 4 /*COLORTYPE_ALPHA*/;
#endif //CXIMAGE_SUPPORT_ALPHA
	return b;
}
////////////////////////////////////////////////////////////////////////////////
/**
 * \return Resolution for TIFF, JPEG, PNG and BMP formats.
 */
long CxImage::GetXDPI() const
{
	return info.xDPI;
}
////////////////////////////////////////////////////////////////////////////////
/**
 * \return Resolution for TIFF, JPEG, PNG and BMP formats.
 */
long CxImage::GetYDPI() const
{
	return info.yDPI;
}
////////////////////////////////////////////////////////////////////////////////
/**
 * Set resolution for TIFF, JPEG, PNG and BMP formats.
 */
void CxImage::SetXDPI(long dpi)
{
	if (dpi<=0) dpi = CXIMAGE_DEFAULT_DPI;
	info.xDPI = dpi;
	head.biXPelsPerMeter = (long) floor(dpi * 10000.0 / 254.0 + 0.5);
	if (pDib) ((BITMAPINFOHEADER*)pDib)->biXPelsPerMeter = head.biXPelsPerMeter;
}
////////////////////////////////////////////////////////////////////////////////
/**
 * Set resolution for TIFF, JPEG, PNG and BMP formats.
 */
void CxImage::SetYDPI(long dpi)
{
	if (dpi<=0) dpi = CXIMAGE_DEFAULT_DPI;
	info.yDPI = dpi;
	head.biYPelsPerMeter = (long) floor(dpi * 10000.0 / 254.0 + 0.5);
	if (pDib) ((BITMAPINFOHEADER*)pDib)->biYPelsPerMeter = head.biYPelsPerMeter;
}
////////////////////////////////////////////////////////////////////////////////
/**
 * \sa SetFlags
 */
DWORD CxImage::GetFlags() const
{
	return info.dwFlags;
}
////////////////////////////////////////////////////////////////////////////////
/**
 * Image flags, for future use
 * \param flags
 *  - 0x??00000 = reserved for 16 bit, CMYK, multilayer
 *  - 0x00??0000 = blend modes
 *  - 0x0000???? = layer id or user flags
 *
 * \param bLockReservedFlags protects the "reserved" and "blend modes" flags 
 */
void CxImage::SetFlags(DWORD flags, bool bLockReservedFlags)
{
	if (bLockReservedFlags) info.dwFlags = flags & 0x0000ffff;
	else info.dwFlags = flags;
}
////////////////////////////////////////////////////////////////////////////////
/**
 * \sa SetCodecOption
 */
DWORD CxImage::GetCodecOption(DWORD imagetype)
{
	imagetype = GetTypeIndexFromId(imagetype);
	if (imagetype==0){
		imagetype = GetTypeIndexFromId(GetType());
	}
	return info.dwCodecOpt[imagetype];
}
////////////////////////////////////////////////////////////////////////////////
/**
 * Encode option for GIF, TIF and JPG.
 * - GIF : 0 = LZW (default), 1 = none, 2 = RLE.
 * - TIF : 0 = automatic (default), or a valid compression code as defined in "tiff.h" (COMPRESSION_NONE = 1, COMPRESSION_CCITTRLE = 2, ...)
 * - JPG : valid values stored in enum CODEC_OPTION ( ENCODE_BASELINE = 0x01, ENCODE_PROGRESSIVE = 0x10, ...)
 * - RAW : valid values stored in enum CODEC_OPTION ( DECODE_QUALITY_LIN = 0x00, DECODE_QUALITY_VNG = 0x01, ...)
 *
 * \return true if everything is ok
 */
bool CxImage::SetCodecOption(DWORD opt, DWORD imagetype)
{
	imagetype = GetTypeIndexFromId(imagetype);
	if (imagetype==0){
		imagetype = GetTypeIndexFromId(GetType());
	}
	info.dwCodecOpt[imagetype] = opt;
	return true;
}
////////////////////////////////////////////////////////////////////////////////
/**
 * \return internal hDib object..
 */
void* CxImage::GetDIB() const
{
	return pDib;
}
////////////////////////////////////////////////////////////////////////////////
DWORD CxImage::GetHeight() const
{
	return head.biHeight;
}
////////////////////////////////////////////////////////////////////////////////
DWORD CxImage::GetWidth() const
{
	return head.biWidth;
}
////////////////////////////////////////////////////////////////////////////////
/**
 * \return DWORD aligned width of the image.
 */
DWORD CxImage::GetEffWidth() const
{
	return info.dwEffWidth;
}
////////////////////////////////////////////////////////////////////////////////
/**
 * \return 2, 16, 256; 0 for RGB images.
 */
DWORD CxImage::GetNumColors() const
{
	return head.biClrUsed;
}
////////////////////////////////////////////////////////////////////////////////
/**
 * \return: 1, 4, 8, 24.
 */
WORD CxImage::GetBpp() const
{
	return head.biBitCount;
}
////////////////////////////////////////////////////////////////////////////////
/**
 * \return original image format
 * \sa ENUM_CXIMAGE_FORMATS.
 */
DWORD CxImage::GetType() const
{
	return info.dwType;
}
////////////////////////////////////////////////////////////////////////////////
/**
 * change image format identifier
 * \sa ENUM_CXIMAGE_FORMATS.
 */
bool CxImage::SetType(DWORD type)
{
	switch (type){
#if CXIMAGE_SUPPORT_PNG
	case CXIMAGE_FORMAT_PNG:
#endif
		info.dwType = type;
		return true;
	}
	info.dwType = CXIMAGE_FORMAT_UNKNOWN;
	return false;
}
////////////////////////////////////////////////////////////////////////////////
DWORD CxImage::GetNumTypes()
{
	return CMAX_IMAGE_FORMATS-1;
}
////////////////////////////////////////////////////////////////////////////////
DWORD CxImage::GetTypeIdFromName(const TCHAR* ext)
{
#if CXIMAGE_SUPPORT_PNG
	if (_tcsnicmp(ext,_T("png"),3)==0 )		return CXIMAGE_FORMAT_PNG;
#endif

	return CXIMAGE_FORMAT_UNKNOWN;
}
////////////////////////////////////////////////////////////////////////////////
DWORD CxImage::GetTypeIdFromIndex(const DWORD index)
{
	DWORD n;

	n=0; if (index == n) return CXIMAGE_FORMAT_UNKNOWN;
#if CXIMAGE_SUPPORT_PNG
	n++; if (index == n) return CXIMAGE_FORMAT_PNG;
#endif

	return CXIMAGE_FORMAT_UNKNOWN;
}
////////////////////////////////////////////////////////////////////////////////
DWORD CxImage::GetTypeIndexFromId(const DWORD id)
{
	DWORD n;

	n=0; if (id == CXIMAGE_FORMAT_UNKNOWN) return n;
#if CXIMAGE_SUPPORT_PNG
	n++; if (id == CXIMAGE_FORMAT_PNG) return n;
#endif

	return 0;
}
////////////////////////////////////////////////////////////////////////////////
/**
 * \return current frame delay in milliseconds. Only for GIF and MNG formats.
 */
DWORD CxImage::GetFrameDelay() const
{
	return info.dwFrameDelay;
}
////////////////////////////////////////////////////////////////////////////////
/**
 * Sets current frame delay. Only for GIF format.
 * \param d = delay in milliseconds
 */
void CxImage::SetFrameDelay(DWORD d)
{
	info.dwFrameDelay=d;
}
////////////////////////////////////////////////////////////////////////////////
void CxImage::GetOffset(long *x,long *y)
{
	*x=info.xOffset;
	*y=info.yOffset;
}
////////////////////////////////////////////////////////////////////////////////
void CxImage::SetOffset(long x,long y)
{
	info.xOffset=x;
	info.yOffset=y;
}
////////////////////////////////////////////////////////////////////////////////
/**
 * \sa SetJpegQuality, GetJpegQualityF
 * \author [DP]; changes [Stefan Sch�rmans]
 */
BYTE CxImage::GetJpegQuality() const
{
	return (BYTE)(info.fQuality + 0.5f);
}
////////////////////////////////////////////////////////////////////////////////
/**
 * \sa SetJpegQuality, GetJpegQuality
 * \author [Stefan Sch�rmans]
 */
float CxImage::GetJpegQualityF() const
{
	return info.fQuality;
}
////////////////////////////////////////////////////////////////////////////////
/**
 * quality level for JPEG and JPEG2000
 * \param q: can be from 0 to 100
 * \author [DP]; changes [Stefan Sch�rmans]
 */
void CxImage::SetJpegQuality(BYTE q){
	info.fQuality = (float)q;
}
////////////////////////////////////////////////////////////////////////////////
/**
 * quality level for JPEG and JPEG2000
 * necessary for JPEG2000 when quality is between 0.0 and 1.0
 * \param q: can be from 0.0 to 100.0
 * \author [Stefan Sch�rmans]
 */
void CxImage::SetJpegQualityF(float q){
	if (q>0) info.fQuality = q;
	else  info.fQuality = 0.0f;
}
////////////////////////////////////////////////////////////////////////////////
/**
 * \sa SetJpegScale
 */
BYTE CxImage::GetJpegScale() const
{
	return info.nJpegScale;
}
////////////////////////////////////////////////////////////////////////////////
/**
 * scaling down during JPEG decoding valid numbers are 1, 2, 4, 8
 * \author [ignacio]
 */
void CxImage::SetJpegScale(BYTE q){
	info.nJpegScale = q;
}
////////////////////////////////////////////////////////////////////////////////
/**
 * Used to monitor the slow loops.
 * \return value is from 0 to 100.
 * \sa SetProgress
 */
long CxImage::GetProgress() const
{
	return info.nProgress;
}
////////////////////////////////////////////////////////////////////////////////
/**
 * \return the escape code.
 * \sa SetEscape
 */
long CxImage::GetEscape() const
{
	return info.nEscape;
}
////////////////////////////////////////////////////////////////////////////////
/**
 * Forces the value of the internal progress variable.
 * \param p should be from 0 to 100.
 * \sa GetProgress
 */
void CxImage::SetProgress(long p)
{
	info.nProgress = p;
}
////////////////////////////////////////////////////////////////////////////////
/**
 * Used to quit the slow loops or the codecs.
 * - SetEscape(-1) before Decode forces the function to exit, right after  
 *   the image width and height are available ( for bmp, jpg, gif, tif )
 */
void CxImage::SetEscape(long i)
{
	info.nEscape = i;
}
////////////////////////////////////////////////////////////////////////////////
/**
 * Checks if the image is correctly initializated.
 */
bool CxImage::IsValid() const
{
	return pDib!=0;
}
////////////////////////////////////////////////////////////////////////////////
/**
 * True if the image is enabled for painting.
 */
bool CxImage::IsEnabled() const
{
	return info.bEnabled;
}
////////////////////////////////////////////////////////////////////////////////
/**
 * Enables/disables the image.
 */
void CxImage::Enable(bool enable)
{
	info.bEnabled=enable;
}
////////////////////////////////////////////////////////////////////////////////
/**
 * This function must be used after a Decode() / Load() call.
 * Use the sequence SetFrame(-1); Load(...); GetNumFrames();
 * to get the number of images without loading the first image.
 * \return the number of images in the file.
 */
long CxImage::GetNumFrames() const
{
	return info.nNumFrames;
}
////////////////////////////////////////////////////////////////////////////////
/**
 * \return the current selected image (zero-based index).
 */
long CxImage::GetFrame() const
{
	return info.nFrame;
}
////////////////////////////////////////////////////////////////////////////////
/**
 * Sets the image number that the next Decode() / Load() call will load
 */
void CxImage::SetFrame(long nFrame){
	info.nFrame=nFrame;
}
////////////////////////////////////////////////////////////////////////////////
/**
 * Sets the method for drawing the frame related to others
 * \sa GetDisposalMethod
 */
void CxImage::SetDisposalMethod(BYTE dm)
{	info.dispmeth=dm; }
////////////////////////////////////////////////////////////////////////////////
/**
 * Gets the method for drawing the frame related to others
 * Values :    0 -   No disposal specified. The decoder is
 *                   not required to take any action.
 *             1 -   Do not dispose. The graphic is to be left
 *                   in place.
 *             2 -   Restore to background color. The area used by the
 *                   graphic must be restored to the background color.
 *             3 -   Restore to previous. The decoder is required to
 *                   restore the area overwritten by the graphic with
 *                   what was there prior to rendering the graphic.
 *             4-7 -    To be defined.
 */
BYTE CxImage::GetDisposalMethod() const
{	return info.dispmeth; }
////////////////////////////////////////////////////////////////////////////////
bool CxImage::GetRetreiveAllFrames() const
{	return info.bGetAllFrames; }
////////////////////////////////////////////////////////////////////////////////
void CxImage::SetRetreiveAllFrames(bool flag)
{	info.bGetAllFrames = flag; }
////////////////////////////////////////////////////////////////////////////////
CxImage * CxImage::GetFrame(long nFrame) const
{
	if ( ppFrames == NULL) return NULL;
	if ( info.nNumFrames == 0) return NULL;
	if ( nFrame >= info.nNumFrames ) return NULL;
	if ( nFrame < 0) nFrame = info.nNumFrames - 1;
	return ppFrames[nFrame];
}
////////////////////////////////////////////////////////////////////////////////
short CxImage::my_ntohs(const short word)
{
	if (info.bLittleEndianHost) return word;
	return ( (word & 0xff) << 8 ) | ( (word >> 8) & 0xff );
}
////////////////////////////////////////////////////////////////////////////////
long CxImage::my_ntohl(const long dword)
{
	if (info.bLittleEndianHost) return dword;
	return  ((dword & 0xff) << 24 ) | ((dword & 0xff00) << 8 ) |
			((dword >> 8) & 0xff00) | ((dword >> 24) & 0xff);
}
////////////////////////////////////////////////////////////////////////////////
void CxImage::bihtoh(BITMAPINFOHEADER* bih)
{
	bih->biSize = my_ntohl(bih->biSize);
	bih->biWidth = my_ntohl(bih->biWidth);
	bih->biHeight = my_ntohl(bih->biHeight);
	bih->biPlanes = my_ntohs(bih->biPlanes);
	bih->biBitCount = my_ntohs(bih->biBitCount);
	bih->biCompression = my_ntohl(bih->biCompression);
	bih->biSizeImage = my_ntohl(bih->biSizeImage);
	bih->biXPelsPerMeter = my_ntohl(bih->biXPelsPerMeter);
	bih->biYPelsPerMeter = my_ntohl(bih->biYPelsPerMeter);
	bih->biClrUsed = my_ntohl(bih->biClrUsed);
	bih->biClrImportant = my_ntohl(bih->biClrImportant);
}
////////////////////////////////////////////////////////////////////////////////
/**
 * Returns the last reported error.
 */
const char* CxImage::GetLastError()
{
	return info.szLastError;
}
////////////////////////////////////////////////////////////////////////////////
DWORD CxImage::DumpSize()
{
	DWORD n;
	n = sizeof(BITMAPINFOHEADER) + sizeof(CXIMAGEINFO) + GetSize();

	if (pAlpha){
		n += 1 + head.biWidth * head.biHeight;
	} else n++;

	if (pSelection){
		n += 1 + head.biWidth * head.biHeight;
	} else n++;

	if (ppFrames){
		for (long m=0; m<GetNumFrames(); m++){
			if (GetFrame(m)){
				n += 1 + GetFrame(m)->DumpSize();
			}
		}
	} else n++;

	return n;
}
////////////////////////////////////////////////////////////////////////////////
DWORD CxImage::Dump(BYTE * dst)
{
	if (!dst) return 0;

	memcpy(dst,&head,sizeof(BITMAPINFOHEADER));
	dst += sizeof(BITMAPINFOHEADER);

	memcpy(dst,&info,sizeof(CXIMAGEINFO));
	dst += sizeof(CXIMAGEINFO);

	memcpy(dst,pDib,GetSize());
	dst += GetSize();

	if (pAlpha){
		memset(dst++, 1, 1);
		memcpy(dst,pAlpha,head.biWidth * head.biHeight);
		dst += head.biWidth * head.biHeight;
	} else {
		memset(dst++, 0, 1);
	}

	if (pSelection){
		memset(dst++, 1, 1);
		memcpy(dst,pSelection,head.biWidth * head.biHeight);
		dst += head.biWidth * head.biHeight;
	} else {
		memset(dst++, 0, 1);
	}

	if (ppFrames){
		memset(dst++, 1, 1);
		for (long m=0; m<GetNumFrames(); m++){
			if (GetFrame(m)){
				dst += GetFrame(m)->Dump(dst);
			}
		}
	} else {
		memset(dst++, 0, 1);
	}

	return DumpSize();
}
////////////////////////////////////////////////////////////////////////////////
DWORD CxImage::UnDump(const BYTE * src)
{
	if (!src)
		return 0;
	if (!Destroy())
		return 0;
	if (!DestroyFrames())
		return 0;

	DWORD n = 0;

	memcpy(&head,src,sizeof(BITMAPINFOHEADER));
	n += sizeof(BITMAPINFOHEADER);

	memcpy(&info,&src[n],sizeof(CXIMAGEINFO));
	n += sizeof(CXIMAGEINFO);

	if (!Create(head.biWidth, head.biHeight, head.biBitCount, info.dwType))
		return 0;

	memcpy(pDib,&src[n],GetSize());
	n += GetSize();

	if (src[n++]){
		if (AlphaCreate()){
			memcpy(pAlpha, &src[n], head.biWidth * head.biHeight);
		}
		n += head.biWidth * head.biHeight;
	}

	if (src[n++]){
		RECT box = info.rSelectionBox;
		if (SelectionCreate()){
			info.rSelectionBox = box;
			memcpy(pSelection, &src[n], head.biWidth * head.biHeight);
		}
		n += head.biWidth * head.biHeight;
	}

	if (src[n++]){
		ppFrames = new CxImage*[info.nNumFrames];
		for (long m=0; m<GetNumFrames(); m++){
			ppFrames[m] = new CxImage();
			n += ppFrames[m]->UnDump(&src[n]);
		}
	}

	return n;
}
////////////////////////////////////////////////////////////////////////////////
/**
 * \return A.BBCCCDDDD
 *  - A = main version
 *  - BB = main revision
 *  - CCC = minor revision (letter)
 *  - DDDD = experimental revision
 */
const float CxImage::GetVersionNumber()
{
	return 6.000000015f;
}
////////////////////////////////////////////////////////////////////////////////
const TCHAR* CxImage::GetVersion()
{
	static const TCHAR CxImageVersion[] = _T("CxImage 6.0.0");
	return (CxImageVersion);
}
////////////////////////////////////////////////////////////////////////////////
