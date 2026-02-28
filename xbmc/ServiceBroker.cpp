/*
 *  Copyright (C) 2005-2018 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#include "ServiceBroker.h"

#include "ServiceManager.h"
#include "application/Application.h"
#include "profiles/ProfileManager.h"
#include "settings/SettingsComponent.h"
#include "utils/log.h"
#include "windowing/WinSystem.h"

#include <stdexcept>
#include <utility>

using namespace KODI;

CServiceBroker::CServiceBroker()
  : m_pGUI(nullptr), m_pWinSystem(nullptr)
{
}

CServiceBroker::~CServiceBroker()
{
}

std::shared_ptr<CAppParams> CServiceBroker::GetAppParams()
{
  if (!g_serviceBroker.m_appParams)
    throw std::logic_error("AppParams not yet available / not available anymore.");

  return g_serviceBroker.m_appParams;
}

void CServiceBroker::RegisterAppParams(const std::shared_ptr<CAppParams>& appParams)
{
  g_serviceBroker.m_appParams = appParams;
}

void CServiceBroker::UnregisterAppParams()
{
  g_serviceBroker.m_appParams.reset();
}

// announcement
std::shared_ptr<ANNOUNCEMENT::CAnnouncementManager> CServiceBroker::GetAnnouncementManager()
{
  return g_serviceBroker.m_pAnnouncementManager;
}
void CServiceBroker::RegisterAnnouncementManager(
    std::shared_ptr<ANNOUNCEMENT::CAnnouncementManager> port)
{
  g_serviceBroker.m_pAnnouncementManager = std::move(port);
}

void CServiceBroker::UnregisterAnnouncementManager()
{
  g_serviceBroker.m_pAnnouncementManager.reset();
}

ADDON::CAddonMgr& CServiceBroker::GetAddonMgr()
{
  return g_application.m_ServiceManager->GetAddonMgr();
}

ADDON::CBinaryAddonCache& CServiceBroker::GetBinaryAddonCache()
{
  return g_application.m_ServiceManager->GetBinaryAddonCache();
}

ADDONS::CExtsMimeSupportList& CServiceBroker::GetExtsMimeSupportList()
{
  return g_application.m_ServiceManager->GetExtsMimeSupportList();
}

CContextMenuManager& CServiceBroker::GetContextMenuManager()
{
  return g_application.m_ServiceManager->GetContextMenuManager();
}

CDataCacheCore& CServiceBroker::GetDataCacheCore()
{
  return g_application.m_ServiceManager->GetDataCacheCore();
}

CPlatform& CServiceBroker::GetPlatform()
{
  return g_application.m_ServiceManager->GetPlatform();
}

PLAYLIST::CPlayListPlayer& CServiceBroker::GetPlaylistPlayer()
{
  return g_application.m_ServiceManager->GetPlaylistPlayer();
}

void CServiceBroker::RegisterSettingsComponent(const std::shared_ptr<CSettingsComponent>& settings)
{
  g_serviceBroker.m_pSettingsComponent = settings;
}

void CServiceBroker::UnregisterSettingsComponent()
{
  g_serviceBroker.m_pSettingsComponent.reset();
}

std::shared_ptr<CSettingsComponent> CServiceBroker::GetSettingsComponent()
{
  return g_serviceBroker.m_pSettingsComponent;
}

CFavouritesService& CServiceBroker::GetFavouritesService()
{
  return g_application.m_ServiceManager->GetFavouritesService();
}

ADDON::CServiceAddonManager& CServiceBroker::GetServiceAddons()
{
  return g_application.m_ServiceManager->GetServiceAddons();
}

ADDON::CRepositoryUpdater& CServiceBroker::GetRepositoryUpdater()
{
  return g_application.m_ServiceManager->GetRepositoryUpdater();
}

CInputManager& CServiceBroker::GetInputManager()
{
  return g_application.m_ServiceManager->GetInputManager();
}

CFileExtensionProvider& CServiceBroker::GetFileExtensionProvider()
{
  return g_application.m_ServiceManager->GetFileExtensionProvider();
}

bool CServiceBroker::IsAddonInterfaceUp()
{
  return g_application.m_ServiceManager && g_application.m_ServiceManager->init_level > 1;
}

bool CServiceBroker::IsServiceManagerUp()
{
  return g_application.m_ServiceManager && g_application.m_ServiceManager->init_level == 3;
}

CWinSystemBase* CServiceBroker::GetWinSystem()
{
  return g_serviceBroker.m_pWinSystem;
}

void CServiceBroker::RegisterWinSystem(CWinSystemBase* winsystem)
{
  g_serviceBroker.m_pWinSystem = winsystem;
}

void CServiceBroker::UnregisterWinSystem()
{
  g_serviceBroker.m_pWinSystem = nullptr;
}

CRenderSystemBase* CServiceBroker::GetRenderSystem()
{
  if (g_serviceBroker.m_pWinSystem)
    return g_serviceBroker.m_pWinSystem->GetRenderSystem();

  return nullptr;
}

CPowerManager& CServiceBroker::GetPowerManager()
{
  return g_application.m_ServiceManager->GetPowerManager();
}

CWeatherManager& CServiceBroker::GetWeatherManager()
{
  return g_application.m_ServiceManager->GetWeatherManager();
}

CPlayerCoreFactory& CServiceBroker::GetPlayerCoreFactory()
{
  return g_application.m_ServiceManager->GetPlayerCoreFactory();
}

CDatabaseManager& CServiceBroker::GetDatabaseManager()
{
  return g_application.m_ServiceManager->GetDatabaseManager();
}

CMediaManager& CServiceBroker::GetMediaManager()
{
  return g_application.m_ServiceManager->GetMediaManager();
}

CApplicationComponents& CServiceBroker::GetAppComponents()
{
  return g_application;
}

CGUIComponent* CServiceBroker::GetGUI()
{
  return g_serviceBroker.m_pGUI;
}

void CServiceBroker::RegisterGUI(CGUIComponent* gui)
{
  g_serviceBroker.m_pGUI = gui;
}

void CServiceBroker::UnregisterGUI()
{
  g_serviceBroker.m_pGUI = nullptr;
}

std::shared_ptr<CCPUInfo> CServiceBroker::GetCPUInfo()
{
  return g_serviceBroker.m_cpuInfo;
}

void CServiceBroker::RegisterCPUInfo(std::shared_ptr<CCPUInfo> cpuInfo)
{
  g_serviceBroker.m_cpuInfo = std::move(cpuInfo);
}

void CServiceBroker::UnregisterCPUInfo()
{
  g_serviceBroker.m_cpuInfo.reset();
}

void CServiceBroker::RegisterTextureCache(const std::shared_ptr<CTextureCache>& cache)
{
  g_serviceBroker.m_textureCache = cache;
}

void CServiceBroker::UnregisterTextureCache()
{
  g_serviceBroker.m_textureCache.reset();
}

std::shared_ptr<CTextureCache> CServiceBroker::GetTextureCache()
{
  return g_serviceBroker.m_textureCache;
}

void CServiceBroker::RegisterJobManager(const std::shared_ptr<CJobManager>& jobManager)
{
  g_serviceBroker.m_jobManager = jobManager;
}

void CServiceBroker::UnregisterJobManager()
{
  g_serviceBroker.m_jobManager.reset();
}

std::shared_ptr<CJobManager> CServiceBroker::GetJobManager()
{
  return g_serviceBroker.m_jobManager;
}

void CServiceBroker::RegisterAppMessenger(
    const std::shared_ptr<KODI::MESSAGING::CApplicationMessenger>& appMessenger)
{
  g_serviceBroker.m_appMessenger = appMessenger;
}

void CServiceBroker::UnregisterAppMessenger()
{
  g_serviceBroker.m_appMessenger.reset();
}

std::shared_ptr<KODI::MESSAGING::CApplicationMessenger> CServiceBroker::GetAppMessenger()
{
  return g_serviceBroker.m_appMessenger;
}

void CServiceBroker::RegisterKeyboardLayoutManager(
    const std::shared_ptr<CKeyboardLayoutManager>& keyboardLayoutManager)
{
  g_serviceBroker.m_keyboardLayoutManager = keyboardLayoutManager;
}

void CServiceBroker::UnregisterKeyboardLayoutManager()
{
  g_serviceBroker.m_keyboardLayoutManager.reset();
}

std::shared_ptr<CKeyboardLayoutManager> CServiceBroker::GetKeyboardLayoutManager()
{
  return g_serviceBroker.m_keyboardLayoutManager;
}
