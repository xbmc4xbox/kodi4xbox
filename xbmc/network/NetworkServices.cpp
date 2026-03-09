/*
 *  Copyright (C) 2005-2018 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#include "NetworkServices.h"

#include "ServiceBroker.h"
#include "network/Network.h"
#include "settings/AdvancedSettings.h"
#include "settings/Settings.h"
#include "settings/SettingsComponent.h"
#include "settings/lib/Setting.h"
#include "settings/lib/SettingsManager.h"

#include <utility>

CNetworkServices::CNetworkServices()
{
  std::set<std::string> settingSet{};
  m_settings = CServiceBroker::GetSettingsComponent()->GetSettings();
  m_settings->GetSettingsManager()->RegisterCallback(this, settingSet);
}

CNetworkServices::~CNetworkServices()
{
  m_settings->GetSettingsManager()->UnregisterCallback(this);
}

bool CNetworkServices::OnSettingChanging(const std::shared_ptr<const CSetting>& setting)
{
  return true;
}

void CNetworkServices::OnSettingChanged(const std::shared_ptr<const CSetting>& setting)
{
}

bool CNetworkServices::OnSettingUpdate(const std::shared_ptr<CSetting>& setting,
                                       const char* oldSettingId,
                                       const TiXmlNode* oldSettingNode)
{
  return true;
}

void CNetworkServices::Start()
{
}

void CNetworkServices::Stop(bool bWait)
{
}

bool CNetworkServices::StartServer(enum ESERVERS server, bool start)
{
  return false;
}

bool CNetworkServices::ValidatePort(int port)
{
  if (port <= 0 || port > 65535)
    return false;

#ifdef TARGET_LINUX
  if (!CUtil::CanBindPrivileged() && (port < 1024))
    return false;
#endif

  return true;
}
