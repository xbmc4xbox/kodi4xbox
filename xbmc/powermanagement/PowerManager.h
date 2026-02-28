/*
 *  Copyright (C) 2005-2018 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#pragma once

#include <memory>
#include <string>
#include <utility>
#include <vector>

class CSetting;
class CSettings;

struct IntegerSettingOption;

// This class will wrap and handle PowerSyscalls.
// It will handle and decide if syscalls are needed.
class CPowerManager
{
public:
  CPowerManager();
  ~CPowerManager();

  void Initialize();
  void SetDefaults();

  bool Powerdown();
  bool Suspend();
  bool Hibernate();
  bool Reboot();

  bool CanPowerdown();
  bool CanSuspend();
  bool CanHibernate();
  bool CanReboot();

  int  BatteryLevel();

  void ProcessEvents();

  static void SettingOptionsShutdownStatesFiller(const std::shared_ptr<const CSetting>& setting,
                                                 std::vector<IntegerSettingOption>& list,
                                                 int& current,
                                                 void* data);

private:
  // Construction parameters
  std::shared_ptr<CSettings> m_settings;
};
