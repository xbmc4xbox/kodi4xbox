/*
 *  Copyright (C) 2005-2018 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#pragma once

#include "utils/GlobalsHandling.h"

#include <memory>

namespace ADDON
{
class CAddonMgr;
class CBinaryAddonCache;
class CServiceAddonManager;
class CRepositoryUpdater;
} // namespace ADDON

namespace ANNOUNCEMENT
{
class CAnnouncementManager;
}

namespace PLAYLIST
{
class CPlayListPlayer;
}

namespace KODI
{
namespace MESSAGING
{
class CApplicationMessenger;
}
} // namespace KODI

class CAppParams;
template<class T>
class CComponentContainer;
class CContextMenuManager;
class CDataCacheCore;
class IApplicationComponent;
class CFavouritesService;
class CInputManager;
class CFileExtensionProvider;
class CWinSystemBase;
class CRenderSystemBase;
class CPowerManager;
class CWeatherManager;
class CPlayerCoreFactory;
class CDatabaseManager;
class CGUIComponent;
class CSettingsComponent;
class CMediaManager;
class CCPUInfo;
class CLog;
class CPlatform;
class CTextureCache;
class CJobManager;
class CSlideShowDelegator;

namespace KODI
{
namespace ADDONS
{
class CExtsMimeSupportList;
}

namespace KEYBOARD
{
class CKeyboardLayoutManager;
} // namespace KEYBOARD
} // namespace KODI

class CServiceBroker
{
public:
  CServiceBroker();
  ~CServiceBroker();

  static std::shared_ptr<CAppParams> GetAppParams();
  static void RegisterAppParams(const std::shared_ptr<CAppParams>& appParams);
  static void UnregisterAppParams();

  static std::shared_ptr<ANNOUNCEMENT::CAnnouncementManager> GetAnnouncementManager();
  static void RegisterAnnouncementManager(
      std::shared_ptr<ANNOUNCEMENT::CAnnouncementManager> announcementManager);
  static void UnregisterAnnouncementManager();

  static ADDON::CAddonMgr& GetAddonMgr();
  static ADDON::CBinaryAddonCache& GetBinaryAddonCache();
  static KODI::ADDONS::CExtsMimeSupportList& GetExtsMimeSupportList();
  static CContextMenuManager& GetContextMenuManager();
  static CDataCacheCore& GetDataCacheCore();
  static CPlatform& GetPlatform();
  static PLAYLIST::CPlayListPlayer& GetPlaylistPlayer();
  static CSlideShowDelegator& GetSlideShowDelegator();
  static CFavouritesService& GetFavouritesService();
  static ADDON::CServiceAddonManager& GetServiceAddons();
  static ADDON::CRepositoryUpdater& GetRepositoryUpdater();
  static CInputManager& GetInputManager();
  static CFileExtensionProvider& GetFileExtensionProvider();
  static bool IsAddonInterfaceUp();
  static bool IsServiceManagerUp();
  static CPowerManager& GetPowerManager();
  static CWeatherManager& GetWeatherManager();
  static CPlayerCoreFactory& GetPlayerCoreFactory();
  static CDatabaseManager& GetDatabaseManager();
  static CMediaManager& GetMediaManager();
  static CComponentContainer<IApplicationComponent>& GetAppComponents();

  static CGUIComponent* GetGUI();
  static void RegisterGUI(CGUIComponent* gui);
  static void UnregisterGUI();

  static void RegisterSettingsComponent(const std::shared_ptr<CSettingsComponent>& settings);
  static void UnregisterSettingsComponent();
  static std::shared_ptr<CSettingsComponent> GetSettingsComponent();

  static void RegisterWinSystem(CWinSystemBase* winsystem);
  static void UnregisterWinSystem();
  static CWinSystemBase* GetWinSystem();
  static CRenderSystemBase* GetRenderSystem();

  static std::shared_ptr<CCPUInfo> GetCPUInfo();
  static void RegisterCPUInfo(std::shared_ptr<CCPUInfo> cpuInfo);
  static void UnregisterCPUInfo();

  static void RegisterTextureCache(const std::shared_ptr<CTextureCache>& cache);
  static void UnregisterTextureCache();
  static std::shared_ptr<CTextureCache> GetTextureCache();

  static void RegisterJobManager(const std::shared_ptr<CJobManager>& jobManager);
  static void UnregisterJobManager();
  static std::shared_ptr<CJobManager> GetJobManager();

  static void RegisterAppMessenger(
      const std::shared_ptr<KODI::MESSAGING::CApplicationMessenger>& appMessenger);
  static void UnregisterAppMessenger();
  static std::shared_ptr<KODI::MESSAGING::CApplicationMessenger> GetAppMessenger();

  static void RegisterKeyboardLayoutManager(
      const std::shared_ptr<KODI::KEYBOARD::CKeyboardLayoutManager>& keyboardLayoutManager);
  static void UnregisterKeyboardLayoutManager();
  static std::shared_ptr<KODI::KEYBOARD::CKeyboardLayoutManager> GetKeyboardLayoutManager();

private:
  std::shared_ptr<CAppParams> m_appParams;
  std::unique_ptr<CLog> m_logging;
  std::shared_ptr<ANNOUNCEMENT::CAnnouncementManager> m_pAnnouncementManager;
  CGUIComponent* m_pGUI = nullptr;
  CWinSystemBase* m_pWinSystem = nullptr;
  std::shared_ptr<CSettingsComponent> m_pSettingsComponent;
  std::shared_ptr<CCPUInfo> m_cpuInfo;
  std::shared_ptr<CTextureCache> m_textureCache;
  std::shared_ptr<CJobManager> m_jobManager;
  std::shared_ptr<KODI::MESSAGING::CApplicationMessenger> m_appMessenger;
  std::shared_ptr<KODI::KEYBOARD::CKeyboardLayoutManager> m_keyboardLayoutManager;
  std::shared_ptr<CSlideShowDelegator> m_slideshowDelegator;
};

XBMC_GLOBAL_REF(CServiceBroker, g_serviceBroker);
#define g_serviceBroker XBMC_GLOBAL_USE(CServiceBroker)
