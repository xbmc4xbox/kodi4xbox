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

#include "TextureManager.h"

#include <cassert>

#include "filesystem/Directory.h"
#include "filesystem/File.h"
#include "GraphicContext.h"
#include "system.h"
#include "Texture.h"
#include "threads/SystemClock.h"
#include "URL.h"
#include "utils/log.h"
#include "utils/StringUtils.h"
#include "utils/URIUtils.h"

#include <mutex>

#ifdef _DEBUG_TEXTURES
#include "utils/TimeUtils.h"
#endif
#if defined(TARGET_DARWIN_IOS)
#include "windowing/WindowingFactory.h" // for g_Windowing in CGUITextureManager::FreeUnusedTextures
#endif

#if defined(HAS_GL) || defined(HAS_GLES)
#include "system_gl.h"
#endif

/************************************************************************/
/*                                                                      */
/************************************************************************/
CTextureArray::CTextureArray(int width, int height, int loops,  bool texCoordsArePixels)
{
  m_width = width;
  m_height = height;
  m_loops = loops;
  m_orientation = 0;
  m_texWidth = 0;
  m_texHeight = 0;
#ifdef HAS_XBOX_D3D
  m_texCoordsArePixels = texCoordsArePixels;
#else
  m_texCoordsArePixels = false;
#endif
}

CTextureArray::CTextureArray()
{
  Reset();
}

CTextureArray::~CTextureArray()
{

}

unsigned int CTextureArray::size() const
{
  return m_textures.size();
}


void CTextureArray::Reset()
{
  m_textures.clear();
  m_delays.clear();
  m_width = 0;
  m_height = 0;
  m_loops = 0;
  m_orientation = 0;
  m_texWidth = 0;
  m_texHeight = 0;
  m_texCoordsArePixels = false;
}

void CTextureArray::Add(std::shared_ptr<CTexture> texture, int delay)
{
  if (!texture)
    return;

  m_texWidth = texture->GetTextureWidth();
  m_texHeight = texture->GetTextureHeight();
#ifdef HAS_XBOX_D3D
  m_texCoordsArePixels = texture->GetTexCoordsArePixels();
#else
  m_texCoordsArePixels = false;
#endif

  m_textures.emplace_back(std::move(texture));
  m_delays.push_back(delay);
}

void CTextureArray::Set(std::shared_ptr<CTexture> texture, int width, int height)
{
  assert(!m_textures.size()); // don't try and set a texture if we already have one!
  m_width = width;
  m_height = height;
  m_orientation = texture ? texture->GetOrientation() : 0;
  Add(std::move(texture), 2);
}

void CTextureArray::Free()
{
  std::unique_lock<CCriticalSection> lock(g_graphicsContext);
  for (unsigned int i = 0; i < m_textures.size(); i++)
  {
    m_textures[i].reset();
  }

  m_textures.clear();
  m_delays.clear();

  Reset();
}


/************************************************************************/
/*                                                                      */
/************************************************************************/

CTextureMap::CTextureMap()
{
  m_referenceCount = 0;
  m_memUsage = 0;
}

CTextureMap::CTextureMap(const std::string& textureName, int width, int height, int loops)
: m_texture(width, height, loops)
, m_textureName(textureName)
{
  m_referenceCount = 0;
  m_memUsage = 0;
}

CTextureMap::~CTextureMap()
{
  FreeTexture();
}

bool CTextureMap::Release()
{
  if (!m_texture.m_textures.size())
    return true;
  if (!m_referenceCount)
    return true;

  m_referenceCount--;
  if (!m_referenceCount)
  {
    return true;
  }
  return false;
}

const std::string& CTextureMap::GetName() const
{
  return m_textureName;
}

const CTextureArray& CTextureMap::GetTexture()
{
  m_referenceCount++;
  return m_texture;
}

void CTextureMap::Dump() const
{
  if (!m_referenceCount)
    return;   // nothing to see here

  CLog::Log(LOGDEBUG, "{0}: texture:{1} has {2} frames {3} refcount", __FUNCTION__, m_textureName.c_str(), m_texture.m_textures.size(), m_referenceCount);
}

unsigned int CTextureMap::GetMemoryUsage() const
{
  return m_memUsage;
}

void CTextureMap::Flush()
{
  if (!m_referenceCount)
    FreeTexture();
}


void CTextureMap::FreeTexture()
{
  m_texture.Free();
}

void CTextureMap::SetHeight(int height)
{
  m_texture.m_height = (int) height;
}

void CTextureMap::SetWidth(int height)
{
  m_texture.m_width = height;
}

bool CTextureMap::IsEmpty() const
{
  return m_texture.m_textures.empty();
}

void CTextureMap::Add(std::unique_ptr<CTexture> texture, int delay)
{
  m_texture.Add(std::move(texture), delay);

  if (texture)
#ifdef HAS_XBOX_D3D
    m_memUsage += sizeof(CTexture) + (texture->GetPitch() * texture->GetRows()); // This is equal to D3DSURFACE_DESC.Size
#else
    m_memUsage += sizeof(CTexture) + (texture->GetTextureWidth() * texture->GetTextureHeight() * 4);
#endif
}

/************************************************************************/
/*                                                                      */
/************************************************************************/
CGUITextureManager::CGUITextureManager(void)
{
#if 0
#ifdef HAS_XBOX_D3D
  D3DXSetDXT3DXT5(TRUE);
  for (int bundle = 0; bundle < 2; bundle++)
    m_iNextPreload[bundle] = m_PreLoadNames[bundle].end();
#endif
  // we set the theme bundle to be the first bundle (thus prioritizing it)
  m_TexBundle[0].SetThemeBundle(true);
#endif
}

CGUITextureManager::~CGUITextureManager(void)
{
  Cleanup();
}

#if 0
#ifdef HAS_XBOX_D3D
void CGUITextureManager::StartPreLoad()
{
  for (int bundle = 0; bundle < 2; bundle++)
    m_PreLoadNames[bundle].clear();
}

void CGUITextureManager::PreLoad(const std::string& strTextureName)
{
  if (strTextureName.c_str()[1] == ':' || strTextureName == "-")
    return ;

  for (int i = 0; i < (int)m_vecTextures.size(); ++i)
  {
    CTextureMap *pMap = m_vecTextures[i];
    if (pMap->GetName() == strTextureName)
      return ;
  }

  for (int bundle = 0; bundle < 2; bundle++)
  {
    for (std::list<std::string>::iterator i = m_PreLoadNames[bundle].begin(); i != m_PreLoadNames[bundle].end(); ++i)
    {
      if (*i == strTextureName)
        return ;
    }

    if (m_TexBundle[bundle].HasFile(strTextureName))
    {
      m_PreLoadNames[bundle].push_back(strTextureName);
      return;
    }
  }
}

void CGUITextureManager::EndPreLoad()
{
  for (int i = 0; i < 2; i++)
  {
    m_iNextPreload[i] = m_PreLoadNames[i].begin();
    // preload next file
    if (m_iNextPreload[i] != m_PreLoadNames[i].end())
      m_TexBundle[i].PreloadFile(*m_iNextPreload[i]);
  }
}

void CGUITextureManager::FlushPreLoad()
{
  for (int i = 0; i < 2; i++)
  {
    m_PreLoadNames[i].clear();
    m_iNextPreload[i] = m_PreLoadNames[i].end();
  }
}
#endif
#endif

/************************************************************************/
/*                                                                      */
/************************************************************************/
bool CGUITextureManager::CanLoad(const std::string &texturePath)
{
  if (texturePath == "-")
    return false;

  if (!CURL::IsFullPath(texturePath))
    return true;  // assume we have it

  // we can't (or shouldn't) be loading from remote paths, so check these
  return URIUtils::IsHD(texturePath);
}

bool CGUITextureManager::HasTexture(const std::string &textureName, std::string *path, int *bundle, int *size)
{
  std::unique_lock<CCriticalSection> lock(m_section);

  // default values
  if (bundle) *bundle = -1;
  if (size) *size = 0;
  if (path) *path = textureName;

  if (textureName.empty())
    return false;

  if (!CanLoad(textureName))
    return false;

  // Check our loaded and bundled textures - we store in bundles using \\.
  for (int i = 0; i < (int)m_vecTextures.size(); ++i)
  {
    CTextureMap *pMap = m_vecTextures[i];
    if (pMap->GetName() == textureName)
    {
#if 0
#ifdef HAS_XBOX_D3D
      for (int i = 0; i < 2; i++)
      {
        if (m_iNextPreload[i] != m_PreLoadNames[i].end() && (*m_iNextPreload[i] == bundledName))
        {
          ++m_iNextPreload[i];
          // preload next file
          if (m_iNextPreload[i] != m_PreLoadNames[i].end())
            m_TexBundle[i].PreloadFile(*m_iNextPreload[i]);
        }
      }
#endif
#endif
      if (size) *size = 1;
      return true;
    }
  }

#if 0
  std::string bundledName = CTextureBundle::Normalize(textureName);
  for (int i = 0; i < 2; i++)
  {
#ifdef HAS_XBOX_D3D
    if (m_iNextPreload[i] != m_PreLoadNames[i].end() && (*m_iNextPreload[i] == bundledName))
    {
      if (bundle) *bundle = i;
      ++m_iNextPreload[i];
      // preload next file
      if (m_iNextPreload[i] != m_PreLoadNames[i].end())
        m_TexBundle[i].PreloadFile(*m_iNextPreload[i]);
      return true;
    }
    else
#endif
    if (m_TexBundle[i].HasFile(bundledName))
    {
      if (bundle) *bundle = i;
      return true;
    }
  }
#endif

  std::string fullPath = GetTexturePath(textureName);
  if (path)
    *path = fullPath;

  return !fullPath.empty();
}

const CTextureArray& CGUITextureManager::Load(const std::string& strTextureName, bool checkBundleOnly /*= false */)
{
  std::string strPath;
  static CTextureArray emptyTexture;
  int bundle = -1;
  int size = 0;

  if (strTextureName.empty())
    return emptyTexture;

  if (!HasTexture(strTextureName, &strPath, &bundle, &size))
    return emptyTexture;

  if (size) // we found the texture
  {
    for (int i = 0; i < (int)m_vecTextures.size(); ++i)
    {
      CTextureMap *pMap = m_vecTextures[i];
      if (pMap->GetName() == strTextureName)
      {
        //CLog::Log(LOGDEBUG, "Total memusage {}", GetMemoryUsage());
        return pMap->GetTexture();
      }
    }
    // Whoops, not there.
    return emptyTexture;
  }

  for (ilistUnused i = m_unusedTextures.begin(); i != m_unusedTextures.end(); ++i)
  {
    CTextureMap* pMap = i->first;

    auto timestamp = i->second.time_since_epoch();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(timestamp);

    if (pMap->GetName() == strTextureName && duration.count() > 0)
    {
      m_vecTextures.push_back(pMap);
      m_unusedTextures.erase(i);
      return pMap->GetTexture();
    }
  }

  if (checkBundleOnly && bundle == -1)
    return emptyTexture;

  //Lock here, we will do stuff that could break rendering
  std::unique_lock<CCriticalSection> lock(g_graphicsContext);

#ifdef _DEBUG_TEXTURES
  int64_t start;
  start = CurrentHostCounter();
#endif

  if (bundle >= 0 && StringUtils::EndsWithNoCase(strPath, ".gif"))
  {
#if 0
    CTextureMap* pMap = nullptr;
    CTexture **pTextures = nullptr;
    int nLoops = 0, width = 0, height = 0;
    int* Delay = nullptr;
    int nImages = m_TexBundle[bundle].LoadAnim(strTextureName, &pTextures, width, height, nLoops, &Delay);
    if (!nImages)
    {
      CLog::Log(LOGERROR, "Texture manager unable to load bundled file: {}", strTextureName.c_str());
      delete[] pTextures;
      delete[] Delay;
      return emptyTexture;
    }

    unsigned int maxWidth = 0;
    unsigned int maxHeight = 0;
    pMap = new CTextureMap(strTextureName, width, height, nLoops);
    for (int iImage = 0; iImage < nImages; ++iImage)
    {
      pMap->Add(pTextures[iImage], Delay[iImage]);
      maxWidth = std::max(maxWidth, pTextures[iImage]->GetWidth());
      maxHeight = std::max(maxHeight, pTextures[iImage]->GetHeight());
    }

    pMap->SetWidth((int)maxWidth);
    pMap->SetHeight((int)maxHeight);

    delete[] pTextures;
    delete[] Delay;

    m_vecTextures.push_back(pMap);
    return pMap->GetTexture();
#endif
    CLog::Log(LOGDEBUG, "{} - GIFs from bundle are not supported: {}", __FUNCTION__, strPath);
    return emptyTexture;
  }
  else if (StringUtils::EndsWithNoCase(strPath, ".gif") ||
           StringUtils::EndsWithNoCase(strPath, ".apng"))
  {
#if 0
    std::string mimeType;
    if (StringUtils::EndsWithNoCase(strPath, ".gif"))
      mimeType = "image/gif";
    else if (StringUtils::EndsWithNoCase(strPath, ".apng"))
      mimeType = "image/apng";

    XFILE::CFile file;
    XFILE::auto_buffer buf;
    CFFmpegImage anim(mimeType);

    if (file.LoadFile(strPath, buf) <= 0 ||
       !anim.Initialize((uint8_t*)buf.get(), buf.size()))
    {
      CLog::Log(LOGERROR, "Texture manager unable to load file: {}", CURL::GetRedacted(strPath).c_str());
      file.Close();
      return emptyTexture;
    }

    CTextureMap* pMap = new CTextureMap(strTextureName, 0, 0, 0);
    unsigned int maxWidth = 0;
    unsigned int maxHeight = 0;
    uint64_t maxMemoryUsage = 91238400;// 1920*1080*4*11 bytes, i.e, a total of approx. 12 full hd frames

    auto frame = anim.ReadFrame();
    while (frame)
    {
      std::unique_ptr<CTexture> glTexture = CTexture::CreateTexture();
      if (glTexture)
      {
        glTexture->LoadFromMemory(anim.Width(), anim.Height(), frame->GetPitch(), XB_FMT_A8R8G8B8, true, frame->m_pImage);
        pMap->Add(glTexture, frame->m_delay);
        maxWidth = std::max(maxWidth, glTexture->GetWidth());
        maxHeight = std::max(maxHeight, glTexture->GetHeight());
      }

      if (pMap->GetMemoryUsage() <= maxMemoryUsage)
      {
        frame = anim.ReadFrame();
      }
      else
      {
        CLog::Log(LOGDEBUG, "Memory limit (%" PRIu64 " bytes) exceeded, {} frames extracted from file : {}", (maxMemoryUsage/11)*12,pMap->GetTexture().size(), CURL::GetRedacted(strPath).c_str());
        break;
      }
    }

    pMap->SetWidth((int)maxWidth);
    pMap->SetHeight((int)maxHeight);

    file.Close();

    m_vecTextures.push_back(pMap);
    return pMap->GetTexture();
#endif
    CLog::Log(LOGDEBUG, "{} - GIFs/APNGs are not supported: {}", __FUNCTION__, strPath);
    return emptyTexture;
  }

  std::unique_ptr<CTexture> pTexture;
  int width = 0, height = 0;
  if (bundle >= 0)
  {
#if 0
    if (!m_TexBundle[bundle].LoadTexture(strTextureName, &pTexture, width, height))
    {
      CLog::Log(LOGERROR, "Texture manager unable to load bundled file: {}", strTextureName.c_str());
      return emptyTexture;
    }
#endif
  }
  else
  {
#ifdef HAS_XBOX_D3D
    bool isThumbnail = URIUtils::GetExtension(strPath).Equals(".tbn");
    pTexture = CTexture::LoadFromFile(strPath, isThumbnail ? CServiceBroker::GetSettingsComponent()->GetAdvancedSettings()->GetThumbSize() : 0, isThumbnail ? CServiceBroker::GetSettingsComponent()->GetAdvancedSettings()->GetThumbSize() : 0);
#else
    pTexture = CTexture::LoadFromFile(strPath);
#endif
    if (!pTexture)
      return emptyTexture;
    width = pTexture->GetWidth();
    height = pTexture->GetHeight();
  }

  if (!pTexture) return emptyTexture;

  CTextureMap* pMap = new CTextureMap(strTextureName, width, height, 0);
  pMap->Add(std::move(pTexture), 100);
  m_vecTextures.push_back(pMap);

#ifdef _DEBUG_TEXTURES
  int64_t end, freq;
  end = CurrentHostCounter();
  freq = CurrentHostFrequency();
  char temp[200];
  sprintf(temp, "Load {}: %.1fms{}\n", strPath.c_str(), 1000.f * (end - start) / freq, (bundle >= 0) ? " (bundled)" : "");
  OutputDebugString(temp);
#endif

  return pMap->GetTexture();
}


void CGUITextureManager::ReleaseTexture(const std::string& strTextureName, bool immediately /*= false */)
{
  std::unique_lock<CCriticalSection> lock(g_graphicsContext);

  ivecTextures i;
  i = m_vecTextures.begin();
  while (i != m_vecTextures.end())
  {
    CTextureMap* pMap = *i;
    if (pMap->GetName() == strTextureName)
    {
      if (pMap->Release())
      {
        //CLog::Log(LOGINFO, "  cleanup:{}", strTextureName.c_str());
        // add to our textures to free
        std::chrono::time_point<std::chrono::steady_clock> timestamp;

        if (!immediately)
          timestamp = std::chrono::steady_clock::now();

        m_unusedTextures.push_back(std::make_pair(pMap, timestamp));
        i = m_vecTextures.erase(i);
      }
      return;
    }
    ++i;
  }
  CLog::Log(LOGWARNING, "{}: Unable to release texture {}", __FUNCTION__, strTextureName.c_str());
}

void CGUITextureManager::FreeUnusedTextures(unsigned int timeDelay)
{
  std::unique_lock<CCriticalSection> lock(g_graphicsContext);
  for (ilistUnused i = m_unusedTextures.begin(); i != m_unusedTextures.end();)
  {
    auto now = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - i->second);

    if (duration.count() >= timeDelay)
    {
      delete i->first;
      i = m_unusedTextures.erase(i);
    }
    else
      ++i;
  }

#if defined(HAS_GL) || defined(HAS_GLES)
  for (unsigned int i = 0; i < m_unusedHwTextures.size(); ++i)
  {
  // on ios the hw textures might be deleted from the os
  // when XBMC is backgrounded (e.x. for backgrounded music playback)
  // sanity check before delete in that case.
#if defined(TARGET_DARWIN_IOS)
    if (!g_Windowing.IsBackgrounded() || glIsTexture(m_unusedHwTextures[i]))
#endif
      glDeleteTextures(1, (GLuint*) &m_unusedHwTextures[i]);
  }
#endif
  m_unusedHwTextures.clear();
}

void CGUITextureManager::ReleaseHwTexture(unsigned int texture)
{
  std::unique_lock<CCriticalSection> lock(g_graphicsContext);
  m_unusedHwTextures.push_back(texture);
}

void CGUITextureManager::Cleanup()
{
  std::unique_lock<CCriticalSection> lock(g_graphicsContext);

  ivecTextures i;
  i = m_vecTextures.begin();
  while (i != m_vecTextures.end())
  {
    CTextureMap* pMap = *i;
    CLog::Log(LOGWARNING, "{}: Having to cleanup texture {}", __FUNCTION__, pMap->GetName().c_str());
    delete pMap;
    i = m_vecTextures.erase(i);
  }
#if 0
#if 0
  // Kodi Krypton XBT bundle
  m_TexBundle[0].Close();
  m_TexBundle[1].Close();
  m_TexBundle[0] = CTextureBundle(true);
  m_TexBundle[1] = CTextureBundle();
#else
  // XBMC4Xbox XPR bundle
  m_TexBundle[0].Cleanup();
  m_TexBundle[1].Cleanup();
#endif
#endif
  FreeUnusedTextures();
}

void CGUITextureManager::Dump() const
{
  CLog::Log(LOGDEBUG, "{0}: total texturemaps size: {1}", __FUNCTION__, m_vecTextures.size());

  for (int i = 0; i < (int)m_vecTextures.size(); ++i)
  {
    const CTextureMap* pMap = m_vecTextures[i];
    if (!pMap->IsEmpty())
      pMap->Dump();
  }
}

void CGUITextureManager::Flush()
{
  std::unique_lock<CCriticalSection> lock(g_graphicsContext);

  ivecTextures i;
  i = m_vecTextures.begin();
  while (i != m_vecTextures.end())
  {
    CTextureMap* pMap = *i;
    pMap->Flush();
    if (pMap->IsEmpty() )
    {
      delete pMap;
      i = m_vecTextures.erase(i);
    }
    else
    {
      ++i;
    }
  }
}

unsigned int CGUITextureManager::GetMemoryUsage() const
{
  unsigned int memUsage = 0;
  for (int i = 0; i < (int)m_vecTextures.size(); ++i)
  {
    memUsage += m_vecTextures[i]->GetMemoryUsage();
  }
  return memUsage;
}

void CGUITextureManager::SetTexturePath(const std::string &texturePath)
{
  std::unique_lock<CCriticalSection> lock(m_section);
  m_texturePaths.clear();
  AddTexturePath(texturePath);
}

void CGUITextureManager::AddTexturePath(const std::string &texturePath)
{
  std::unique_lock<CCriticalSection> lock(m_section);
  if (!texturePath.empty())
    m_texturePaths.push_back(texturePath);
}

void CGUITextureManager::RemoveTexturePath(const std::string &texturePath)
{
  std::unique_lock<CCriticalSection> lock(m_section);
  for (std::vector<std::string>::iterator it = m_texturePaths.begin(); it != m_texturePaths.end(); ++it)
  {
    if (*it == texturePath)
    {
      m_texturePaths.erase(it);
      return;
    }
  }
}

std::string CGUITextureManager::GetTexturePath(const std::string &textureName, bool directory /* = false */)
{
  if (CURL::IsFullPath(textureName))
    return textureName;
  else
  { // texture doesn't include the full path, so check all fallbacks
    std::unique_lock<CCriticalSection> lock(m_section);
    for (std::vector<std::string>::iterator it = m_texturePaths.begin(); it != m_texturePaths.end(); ++it)
    {
      std::string path = URIUtils::AddFileToFolder(it->c_str(), "media", textureName);
      if (directory)
      {
        if (XFILE::CDirectory::Exists(path))
          return path;
      }
      else
      {
        if (XFILE::CFile::Exists(path))
          return path;
      }
    }
  }

  CLog::Log(LOGDEBUG, "[Warning] CGUITextureManager::GetTexturePath: could not find texture '{}'", textureName.c_str());
  return "";
}

void CGUITextureManager::GetBundledTexturesFromPath(const std::string& texturePath, std::vector<std::string> &items)
{
#if 0
  m_TexBundle[0].GetTexturesFromPath(texturePath, items);
  if (items.empty())
    m_TexBundle[1].GetTexturesFromPath(texturePath, items);
#endif
}
