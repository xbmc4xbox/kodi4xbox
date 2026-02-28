/*
 *  Copyright (C) 2005-2018 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#include "ZipFile.h"

#include "URL.h"
#include "utils/URIUtils.h"
#include "utils/log.h"

#include <sys/stat.h>

#define ZIP_CACHE_LIMIT 4*1024*1024

using namespace XFILE;

CZipFile::CZipFile()
{
}

CZipFile::~CZipFile()
{
  Close();
}

bool CZipFile::Open(const CURL&url)
{
  return false;
}

bool CZipFile::InitDecompress()
{
  return false;
}

int64_t CZipFile::GetLength()
{
  return -1;
}

int64_t CZipFile::GetPosition()
{
  return -1;
}

int64_t CZipFile::Seek(int64_t iFilePosition, int iWhence)
{
  return -1;
}

bool CZipFile::Exists(const CURL& url)
{
  return false;
}

int CZipFile::Stat(struct __stat64 *buffer)
{
  return -1;
}

int CZipFile::Stat(const CURL& url, struct __stat64* buffer)
{
  return 0;
}

ssize_t CZipFile::Read(void* lpBuf, size_t uiBufSize)
{
  return -1;
}

void CZipFile::Close()
{
}

bool CZipFile::FillBuffer()
{
  return false;
}

void CZipFile::DestroyBuffer(void* lpBuffer, int iBufSize)
{
}

int CZipFile::UnpackFromMemory(std::string& strDest, const std::string& strInput, bool isGZ)
{
  return 0;
}

bool CZipFile::DecompressGzip(const std::string& in, std::string& out)
{
  return false;
}
