/*
 *      Copyright (C) 2005-2013 Team XBMC
 *      http://xbmc.org
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
 *  along with XBMC; see the file COPYING.  If not, see
 *  <http://www.gnu.org/licenses/>.
 *
 */

#include "EmuFileWrapper.h"
#include "filesystem/File.h"
#include "threads/SingleLock.h"

CEmuFileWrapper g_emuFileWrapper;

CEmuFileWrapper::CEmuFileWrapper()
{
  // since we always use dlls we might just initialize it directly
  for (int i = 0; i < MAX_EMULATED_FILES; i++)
  {
    memset(&m_files[i], 0, sizeof(EmuFileObject));
    m_files[i].used = false;
#if 0
    m_files[i].file_emu._file = -1;
#endif
  }
}

CEmuFileWrapper::~CEmuFileWrapper()
{
}

void CEmuFileWrapper::CleanUp()
{
  std::unique_lock<CCriticalSection> lock(m_criticalSection);
  for (int i = 0; i < MAX_EMULATED_FILES; i++)
  {
    if (m_files[i].used)
    {
      m_files[i].file_xbmc->Close();
      delete m_files[i].file_xbmc;

      memset(&m_files[i], 0, sizeof(EmuFileObject));
      m_files[i].used = false;
#if 0
      m_files[i].file_emu._file = -1;
#endif
    }
  }
}

EmuFileObject* CEmuFileWrapper::RegisterFileObject(XFILE::CFile* pFile)
{
  EmuFileObject* object = NULL;

  std::unique_lock<CCriticalSection> lock(m_criticalSection);

  for (int i = 0; i < MAX_EMULATED_FILES; i++)
  {
    if (!m_files[i].used)
    {
      // found a free location
      object = &m_files[i];
      object->used = true;
      object->file_xbmc = pFile;
#if 0
      object->file_emu._file = (i + FILE_WRAPPER_OFFSET);
#endif
      break;
    }
  }

  return object;
}

void CEmuFileWrapper::UnRegisterFileObjectByDescriptor(int fd)
{
  int i = fd - FILE_WRAPPER_OFFSET;
  if (i >= 0 && i < MAX_EMULATED_FILES)
  {
    if (m_files[i].used)
    {
      std::unique_lock<CCriticalSection> lock(m_criticalSection);

      // we assume the emulated function alreay deleted the CFile object
      if (m_files[i].used)
      {
        memset(&m_files[i], 0, sizeof(EmuFileObject));
        m_files[i].used = false;
#if 0
        m_files[i].file_emu._file = -1;
#endif
      }
    }
  }
}

void CEmuFileWrapper::UnRegisterFileObjectByStream(FILE* stream)
{
  if (stream != NULL)
  {
#if 0
    return UnRegisterFileObjectByDescriptor(stream->_file);
#endif
  }
}

EmuFileObject* CEmuFileWrapper::GetFileObjectByDescriptor(int fd)
{
  int i = fd - FILE_WRAPPER_OFFSET;
  if (i >= 0 && i < MAX_EMULATED_FILES)
  {
    if (m_files[i].used)
    {
      return &m_files[i];
    }
  }
  return NULL;
}

EmuFileObject* CEmuFileWrapper::GetFileObjectByStream(FILE* stream)
{
  if (stream != NULL)
  {
#if 0
    return GetFileObjectByDescriptor(stream->_file);
#endif
  }

  return NULL;
}

XFILE::CFile* CEmuFileWrapper::GetFileXbmcByDescriptor(int fd)
{
  EmuFileObject* object = GetFileObjectByDescriptor(fd);
  if (object != NULL && object->used)
  {
    return object->file_xbmc;
  }
  return NULL;
}

XFILE::CFile* CEmuFileWrapper::GetFileXbmcByStream(FILE* stream)
{
  if (stream != NULL)
  {
#if 0
    EmuFileObject* object = GetFileObjectByDescriptor(stream->_file);
    if (object != NULL && object->used)
    {
      return object->file_xbmc;
    }
#endif
  }
  return NULL;
}

int CEmuFileWrapper::GetDescriptorByStream(FILE* stream)
{
  if (stream != NULL)
  {
#if 0
    int i = stream->_file - FILE_WRAPPER_OFFSET;
    if (i >= 0 && i < MAX_EMULATED_FILES)
    {
      return stream->_file;
    }
#endif
  }
  return -1;
}

FILE* CEmuFileWrapper::GetStreamByDescriptor(int fd)
{
  EmuFileObject* object = GetFileObjectByDescriptor(fd);
  if (object != NULL && object->used)
  {
    return &object->file_emu;
  }
  return NULL;
}

bool CEmuFileWrapper::DescriptorIsEmulatedFile(int fd)
{
  int i = fd - FILE_WRAPPER_OFFSET;
  if (i >= 0 && i < MAX_EMULATED_FILES)
  {
    return true;
  }
  return false;
}

bool CEmuFileWrapper::StreamIsEmulatedFile(FILE* stream)
{
  if (stream != NULL)
  {
#if 0
    return DescriptorIsEmulatedFile(stream->_file);
#endif
  }
  return false;
}
