/*
 *  Copyright (C) 2005-2018 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#include "PowerManager.h"

#include "PowerTypes.h"
#include "ServiceBroker.h"
#include "guilib/LocalizeStrings.h"
#include "settings/Settings.h"
#include "settings/SettingsComponent.h"
#include "settings/lib/Setting.h"
#include "settings/lib/SettingDefinitions.h"
#include "settings/lib/SettingsManager.h"
#include "utils/log.h"

#include <list>
#include <memory>

CPowerManager::CPowerManager() : m_settings(CServiceBroker::GetSettingsComponent()->GetSettings())
{
  m_settings->GetSettingsManager()->RegisterSettingOptionsFiller("shutdownstates", SettingOptionsShutdownStatesFiller);
}

CPowerManager::~CPowerManager() = default;

void CPowerManager::Initialize()
{
}

void CPowerManager::SetDefaults()
{
  auto setting = m_settings->GetSetting(CSettings::SETTING_POWERMANAGEMENT_SHUTDOWNSTATE);
  if (!setting)
  {
    CLog::Log(LOGERROR, "Failed to load setting for: {}",
              CSettings::SETTING_POWERMANAGEMENT_SHUTDOWNSTATE);
    return;
  }

  std::static_pointer_cast<CSettingInt>(setting)->SetDefault(POWERSTATE_SHUTDOWN);
}

bool CPowerManager::Powerdown()
{
  // TODO: shutdown Xbox
  return false;
}

bool CPowerManager::Suspend()
{
  return CanSuspend();
}

bool CPowerManager::Hibernate()
{
  return CanHibernate();
}

bool CPowerManager::Reboot()
{
  // TODO: reboot Xbox
  return false;
}

bool CPowerManager::CanPowerdown()
{
  return true;
}
bool CPowerManager::CanSuspend()
{
  return false;
}
bool CPowerManager::CanHibernate()
{
  return false;
}
bool CPowerManager::CanReboot()
{
  return true;
}
int CPowerManager::BatteryLevel()
{
  return 0;
}
void CPowerManager::ProcessEvents()
{
}

void CPowerManager::SettingOptionsShutdownStatesFiller(const SettingConstPtr& setting,
                                                       std::vector<IntegerSettingOption>& list,
                                                       int& current,
                                                       void* data)
{
  list.emplace_back(g_localizeStrings.Get(13005), POWERSTATE_SHUTDOWN);
  list.emplace_back(g_localizeStrings.Get(13009), POWERSTATE_QUIT);
}
