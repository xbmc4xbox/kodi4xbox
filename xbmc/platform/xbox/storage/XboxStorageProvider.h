/*
 *  Copyright (C) 2005-2018 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#pragma once

#include "storage/IStorageProvider.h"
#include "utils/Job.h"

#include <vector>

enum Drive_Types
{
  ALL_DRIVES = 0,
  LOCAL_DRIVES,
  REMOVABLE_DRIVES,
  DVD_DRIVES
};

class CXboxStorageProvider : public IStorageProvider
{
public:
  virtual ~CXboxStorageProvider() { }

  virtual void Initialize();
  virtual void Stop() { }

  virtual void GetLocalDrives(VECSOURCES &localDrives);
  virtual void GetRemovableDrives(VECSOURCES &removableDrives);
  virtual std::string GetFirstOpticalDeviceFileName();

  virtual bool Eject(const std::string& mountpath);

  virtual std::vector<std::string> GetDiskUsage();

  virtual bool PumpDriveChangeEvents(IStorageEventsCallback *callback);

  static void SetEvent() { xbevent = true; }
  static bool xbevent;
};

class CDetectDisc : public CJob
{
public:
  CDetectDisc(const std::string &strPath, const bool bautorun);
  bool DoWork();

private:
  std::string  m_strPath;
  bool        m_bautorun;
};

