/*
 *  Copyright (C) 2005-2018 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */
#include "XboxStorageProvider.h"

#include "ServiceBroker.h"
#include "filesystem/SpecialProtocol.h"
#include "guilib/LocalizeStrings.h"
#include "storage/MediaManager.h"
#include "utils/StringUtils.h"
#include "utils/log.h"

#include "platform/win32/CharsetConverter.h"

#include <windows.h>
#include <nxdk/mount.h>

const struct HddDrive
{
  char letter;
  const int haddrive;
  const int partition;
} drives[] = {{'C', 0, 2},
              {'E', 0, 1},
              {'X', 0, 3},
              {'Y', 0, 4},
              {'Z', 0, 5},
              {'F', 0, 6},
              {'G', 0, 7},
              {'R', 0, 8},
              {'S', 0, 9},
              {'V', 0, 10},
              {'W', 0, 11},
              {'A', 0, 12},
              {'B', 0, 13},
              {'P', 0, 14}};

bool CXboxStorageProvider::xbevent = false;

std::unique_ptr<IStorageProvider> IStorageProvider::CreateInstance()
{
  return std::make_unique<CXboxStorageProvider>();
}

void CXboxStorageProvider::Initialize()
{
  // check for a DVD drive
  DWORD fileAttrs = GetFileAttributesA("D:\\");
  if (fileAttrs != INVALID_FILE_ATTRIBUTES && (fileAttrs & FILE_ATTRIBUTE_DIRECTORY) != 0)
    CServiceBroker::GetMediaManager().SetHasOpticalDrive(true);
  else
    CLog::Log(LOGDEBUG, "{}: No optical drive found.", __FUNCTION__);

#ifdef HAS_DVD_DRIVE
  // Can be removed once the StorageHandler supports optical media
  for (const auto& it : vShare)
    if (CServiceBroker::GetMediaManager().GetDriveStatus(it.strPath) ==
        DriveState::CLOSED_MEDIA_PRESENT)
      CServiceBroker::GetJobManager()->AddJob(new CDetectDisc(it.strPath, false), nullptr);
      // remove end
#endif

  // first five partition are mounted on app launch
  for (int i = 5; i < sizeof(drives) / sizeof(HddDrive); ++i)
  {
    std::string strPath = StringUtils::Format("\\Device\\Harddisk{}\\Partition{}\\", drives[i].haddrive, drives[i].partition);
    nxMountDrive(drives[i].letter, strPath.c_str());
  }
}

void CXboxStorageProvider::GetLocalDrives(VECSOURCES &localDrives)
{
  CMediaSource share;
  share.strPath = CSpecialProtocol::TranslatePath("special://home");
  share.strName = g_localizeStrings.Get(21440);
  share.m_ignore = true;
  share.m_iDriveType = CMediaSource::SOURCE_TYPE_LOCAL;
  localDrives.push_back(share);

  for (const HddDrive& drive : drives)
  {
    // don't expose cache partitions
    if (drive.letter == 'X' || drive.letter == 'Y' || drive.letter == 'Z')
      continue;

    std::string strPath = StringUtils::Format("{}:\\", drive.letter);
    DWORD fileAttrs = GetFileAttributesA(strPath.c_str());
    if (fileAttrs != INVALID_FILE_ATTRIBUTES && (fileAttrs & FILE_ATTRIBUTE_DIRECTORY) != 0)
    {
      share.strPath = strPath;
      share.strName = StringUtils::Format(g_localizeStrings.Get(21438), drive.letter);
      localDrives.push_back(share);
    }
  }
}

void CXboxStorageProvider::GetRemovableDrives(VECSOURCES &removableDrives)
{
  // TODO: Add support for memory sticks and USB drives
}

std::string CXboxStorageProvider::GetFirstOpticalDeviceFileName()
{
  std::string strdevice = "\\\\.\\";
  DWORD fileAttrs = GetFileAttributesA("D:\\");
  if (fileAttrs != INVALID_FILE_ATTRIBUTES && (fileAttrs & FILE_ATTRIBUTE_DIRECTORY) != 0)
    return strdevice.append("D:\\");
  else
    return "";
}

bool CXboxStorageProvider::Eject(const std::string& mountpath)
{
  // TODO: implement this
  return false;
}

std::vector<std::string> CXboxStorageProvider::GetDiskUsage()
{
  std::vector<std::string> result;
  ULARGE_INTEGER ULTotal = { { 0 } };
  ULARGE_INTEGER ULTotalFree = { { 0 } };

  for (const HddDrive& drive : drives)
  {
    std::string strDrive = StringUtils::Format("{}:\\", drive.letter);
    if (GetDiskFreeSpaceExA( strDrive.c_str(), nullptr, &ULTotal, &ULTotalFree ) )
    {
      std::string strRet = StringUtils::Format("{} {} MB {}", drive.letter, int(ULTotalFree.QuadPart / (1024 * 1024)), g_localizeStrings.Get(160));
      result.push_back(strRet);
    }
  }
  return result;
}

bool CXboxStorageProvider::PumpDriveChangeEvents(IStorageEventsCallback *callback)
{
  bool b = xbevent;
  xbevent = false;
  return b;
}

CDetectDisc::CDetectDisc(const std::string &strPath, const bool bautorun)
  : m_strPath(strPath), m_bautorun(bautorun)
{
}

bool CDetectDisc::DoWork()
{
#ifdef HAS_DVD_DRIVE
  CLog::Log(LOGDEBUG, "{}: Optical media found in drive {}", __FUNCTION__, m_strPath);
  CMediaSource share;
  share.strPath = m_strPath;
  share.strStatus = CServiceBroker::GetMediaManager().GetDiskLabel(share.strPath);
  share.strDiskUniqueId = CServiceBroker::GetMediaManager().GetDiskUniqueId(share.strPath);
  if (CServiceBroker::GetMediaManager().IsAudio(share.strPath))
    share.strStatus = "Audio-CD";
  else if(share.strStatus == "")
    share.strStatus = g_localizeStrings.Get(446);
  share.strName = share.strPath;
  share.m_ignore = true;
  share.m_iDriveType = CMediaSource::SOURCE_TYPE_DVD;
  CServiceBroker::GetMediaManager().AddAutoSource(share, m_bautorun);
#endif
  return true;
}
