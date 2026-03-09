/*
 *  Copyright (C) 2022 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#include "XboxDiscDriveHandler.h"

#include "utils/log.h"

#include <memory>
#include <windows.h>

std::shared_ptr<IDiscDriveHandler> IDiscDriveHandler::CreateInstance()
{
  return std::make_shared<CXboxDiscDriveHandler>();
}

DriveState CXboxDiscDriveHandler::GetDriveState(const std::string& devicePath)
{
  DriveState driveState = DriveState::NOT_READY;

  ULONG trayState = 0x70;
  NTSTATUS status = HalReadSMCTrayState(&trayState, NULL);
  if (NT_SUCCESS(status) == 0)
  {
    switch (status & 0x70)
    {
      case 0x20:
      case 0x30:
      case 0x50:
        driveState = DriveState::NOT_READY;
        break;
      case 0x00:
      case 0x40:
        driveState = DriveState::CLOSED_NO_MEDIA;
        break;
      case 0x10:
        driveState = DriveState::OPEN;
        break;
      case 0x60:
        driveState = DriveState::CLOSED_MEDIA_PRESENT;
        break;
      default:
        CLog::LogF(LOGWARNING, "Unknown/unhandled drive state interpreted as NOT_READY");
        break;
    }
  }
  return driveState;
}

TrayState CXboxDiscDriveHandler::GetTrayState(const std::string& devicePath)
{
  TrayState trayState = TrayState::UNDEFINED;
  DriveState driveState = GetDriveState(devicePath);
  switch (driveState)
  {
    case DriveState::OPEN:
      trayState = TrayState::OPEN;
      break;
    case DriveState::CLOSED_NO_MEDIA:
      trayState = TrayState::CLOSED_NO_MEDIA;
      break;
    case DriveState::CLOSED_MEDIA_PRESENT:
      trayState = TrayState::CLOSED_MEDIA_PRESENT;
      break;
    default:
      CLog::LogF(LOGWARNING, "Unknown/unhandled tray state interpreted as TrayState::UNDEFINED");
      break;
  }
  return trayState;
}

void CXboxDiscDriveHandler::EjectDriveTray(const std::string& devicePath)
{
  HalWriteSMBusValue(0x20, 0x0C, FALSE, 0);
}

void CXboxDiscDriveHandler::CloseDriveTray(const std::string& devicePath)
{
  HalWriteSMBusValue(0x20, 0x0C, FALSE, 1);
}

void CXboxDiscDriveHandler::ToggleDriveTray(const std::string& devicePath)
{
  if (GetDriveState(devicePath) == DriveState::OPEN)
    CloseDriveTray(devicePath);
  else
    EjectDriveTray(devicePath);
}
