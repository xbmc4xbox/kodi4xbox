/*
 *  Copyright (C) 2022 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#pragma once

#include "storage/discs/IDiscDriveHandler.h"

#include <string>

// for proper DVD DRIVE support we need https://github.com/rkalz/xbmc4xbox/tree/master/xbmc/lib/libcdio
class CXboxDiscDriveHandler : public IDiscDriveHandler
{
public:
  /*! \brief Xbox DiscDriveHandler constructor
  */
  CXboxDiscDriveHandler() = default;

  /*! \brief Xbox DiscDriveHandler destructor
  */
  ~CXboxDiscDriveHandler() override = default;

  /*! \brief Get the optical drive state provided its device path
  * \param devicePath the path for the device drive (e.g. D\://)
  * \return The drive state
  */
  DriveState GetDriveState(const std::string& devicePath) override;

  /*! \brief Get the optical drive tray state provided the drive device path
  * \param devicePath the path for the device drive (e.g. D\://)
  * \return The drive state
  */
  TrayState GetTrayState(const std::string& devicePath) override;

  /*! \brief Eject the provided drive device
  * \param devicePath the path for the device drive (e.g. D\://)
  */
  void EjectDriveTray(const std::string& devicePath) override;

  /*! \brief Close the provided drive device
  * \note Some drives support closing appart from opening/eject
  * \param devicePath the path for the device drive (e.g. D\://)
  */
  void CloseDriveTray(const std::string& devicePath) override;

  /*! \brief Toggle the state of a given drive device
  *
  * \param devicePath the path for the device drive (e.g. D\://)
  */
  void ToggleDriveTray(const std::string& devicePath) override;
};
