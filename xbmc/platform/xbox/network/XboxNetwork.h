/*
 *  Copyright (C) 2005-2018 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#pragma once

#include "network/Network.h"

#include <string>
#include <vector>

class CNetworkXbox;

class CNetworkInterfaceXbox : public CNetworkInterface
{
public:
  CNetworkInterfaceXbox() = default;
  virtual ~CNetworkInterfaceXbox() override = default;

  bool IsEnabled() const override;
  bool IsConnected() const override;
  std::string GetCurrentIPAddress() const override;
  std::string GetCurrentNetmask() const override;

  std::string GetMacAddress() const override;
  void GetMacAddressRaw(char rawMac[6]) const override;

  std::string GetCurrentDefaultGateway() const override;
  bool GetHostMacAddress(unsigned long host, std::string& mac) const override;
};

class CNetworkXbox : public CNetworkBase
{
public:
  CNetworkXbox();
  virtual ~CNetworkXbox() override;

  std::vector<CNetworkInterface*>& GetInterfaceList() override;
  CNetworkInterface* GetFirstConnectedInterface() override;

  bool PingHost(unsigned long host, unsigned int timeout_ms = 2000) override;
  std::vector<std::string> GetNameServers(void) override;

  int GetSocket() { return m_sock; }

protected:
  std::vector<CNetworkInterface*> m_interfaces;

private:
  int m_sock;
};
