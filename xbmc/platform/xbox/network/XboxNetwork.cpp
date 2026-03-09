/*
 *  Copyright (C) 2005-2018 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#include "XboxNetwork.h"

#include "utils/StringUtils.h"

#include <lwip/dns.h>
#include <lwip/netdb.h>

bool CNetworkInterfaceXbox::IsEnabled() const
{
  struct netif* netif = netif_default;
  return netif == NULL;
}

bool CNetworkInterfaceXbox::IsConnected() const
{
  struct netif* netif = netif_default;
  if (netif == NULL || !netif_is_up(netif) || !netif_is_link_up(netif))
  {
    return false;
  }
  return true;
}

std::string CNetworkInterfaceXbox::GetCurrentIPAddress() const
{
  struct netif* netif = netif_default;
  if (netif == NULL || !netif_is_up(netif ) || !netif_is_link_up(netif))
  {
    return "0.0.0.0";
  }
  return ip4addr_ntoa(netif_ip4_addr(netif));
}

std::string CNetworkInterfaceXbox::GetCurrentNetmask() const
{
  struct netif* netif = netif_default;
  if (netif == NULL || !netif_is_up(netif) || !netif_is_link_up(netif))
  {
    return "0.0.0.0";
  }
  return ip4addr_ntoa(netif_ip4_netmask(netif));
}

std::string CNetworkInterfaceXbox::GetMacAddress() const
{
  char rawMac[6];
  GetMacAddressRaw(rawMac);
  return StringUtils::Format("{:02X}:{:02X}:{:02X}:{:02X}:{:02X}:{:02X}", (uint8_t)rawMac[0],
                              (uint8_t)rawMac[1], (uint8_t)rawMac[2],
                              (uint8_t)rawMac[3], (uint8_t)rawMac[4],
                              (uint8_t)rawMac[5]);
}

void CNetworkInterfaceXbox::GetMacAddressRaw(char rawMac[6]) const
{
  ULONG type;
  ExQueryNonVolatileSetting(XC_FACTORY_ETHERNET_ADDR, &type, &rawMac, sizeof(rawMac), NULL);
}

std::string CNetworkInterfaceXbox::GetCurrentDefaultGateway() const
{
  struct netif* netif = netif_default;
  if (netif == NULL || !netif_is_up(netif) || !netif_is_link_up(netif))
  {
    return "0.0.0.0";
  }
  return ip4addr_ntoa(netif_ip4_gw(netif));
}

bool CNetworkInterfaceXbox::GetHostMacAddress(unsigned long host, std::string& mac) const
{
  return false;
}

std::unique_ptr<CNetworkBase> CNetworkBase::GetNetwork()
{
  return std::make_unique<CNetworkXbox>();
}

CNetworkXbox::CNetworkXbox()
{
  m_interfaces.push_back(new CNetworkInterfaceXbox());
}

CNetworkXbox::~CNetworkXbox()
{
  for (auto m_interface : m_interfaces)
  {
    delete m_interface;
  }
  m_interfaces.clear();
}

std::vector<CNetworkInterface*>& CNetworkXbox::GetInterfaceList()
{
  return m_interfaces;
}

CNetworkInterface* CNetworkXbox::GetFirstConnectedInterface()
{
  for (auto m_interface : m_interfaces)
  {
    if (m_interface->IsConnected())
      return m_interface;
  }
  return nullptr;
}

bool CNetworkXbox::PingHost(unsigned long host, unsigned int timeout_ms/* = 2000 */)
{
  // TODO: how to ping host?
  return false;
}

std::vector<std::string> CNetworkXbox::GetNameServers()
{
  std::vector<std::string> result;

  struct netif* netif = netif_default;
  if (netif != NULL && netif_is_up(netif) && netif_is_link_up(netif))
  {
    const ip_addr_t* dns = dns_getserver(0);
    if (!ip_addr_isany(dns))
      result.push_back(ipaddr_ntoa(dns));
    dns = dns_getserver(1);
    if (!ip_addr_isany(dns))
      result.push_back(ipaddr_ntoa(dns));
  }
  return result;
}
