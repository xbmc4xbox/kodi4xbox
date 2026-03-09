/*
 *  Copyright (C) 2013-2018 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#pragma once

#include "settings/lib/ISettingCallback.h"

class CSettings;

class CNetworkServices : public ISettingCallback
{
public:
  CNetworkServices();
  ~CNetworkServices() override;

  bool OnSettingChanging(const std::shared_ptr<const CSetting>& setting) override;
  void OnSettingChanged(const std::shared_ptr<const CSetting>& setting) override;
  bool OnSettingUpdate(const std::shared_ptr<CSetting>& setting,
                       const char* oldSettingId,
                       const TiXmlNode* oldSettingNode) override;

  void Start();
  void Stop(bool bWait);

  enum ESERVERS
  {
    ES_UNKNOWN = -1
  };

  bool StartServer(enum ESERVERS server, bool start);

private:
  CNetworkServices(const CNetworkServices&);
  CNetworkServices const& operator=(CNetworkServices const&);

  bool ValidatePort(int port);

  // Construction parameters
  std::shared_ptr<CSettings> m_settings;
};
