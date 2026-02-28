/*
 *  Copyright (C) 2005-2018 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#include "ServiceManager.h"

#include "ContextMenuManager.h"
#include "DatabaseManager.h"
#include "PlayListPlayer.h"
#include "addons/AddonManager.h"
#include "addons/BinaryAddonCache.h"
#include "addons/ExtsMimeSupportList.h"
#include "addons/RepositoryUpdater.h"
#include "addons/Service.h"
#include "addons/binary-addons/BinaryAddonManager.h"
#include "cores/DataCacheCore.h"
#include "cores/playercorefactory/PlayerCoreFactory.h"
#include "favourites/FavouritesService.h"
#include "input/InputManager.h"
#include "interfaces/generic/ScriptInvocationManager.h"
#if defined(HAS_FILESYSTEM_SMB)
#include "network/IWSDiscovery.h"
#if defined(TARGET_WINDOWS)
#include "platform/win32/network/WSDiscoveryWin32.h"
#else // !defined(TARGET_WINDOWS)
#include "platform/posix/filesystem/SMBWSDiscovery.h"
#endif // defined(TARGET_WINDOWS)
#endif // HAS_FILESYSTEM_SMB
#include "powermanagement/PowerManager.h"
#include "profiles/ProfileManager.h"
#if !defined(TARGET_WINDOWS) && defined(HAS_DVD_DRIVE)
#include "storage/DetectDVDType.h"
#endif
#include "storage/MediaManager.h"
#include "utils/FileExtensionProvider.h"
#include "utils/log.h"
#include "weather/WeatherManager.h"

using namespace KODI;

CServiceManager::CServiceManager() = default;

CServiceManager::~CServiceManager()
{
  if (init_level > 2)
    DeinitStageThree();
  if (init_level > 1)
    DeinitStageTwo();
  if (init_level > 0)
    DeinitStageOne();
}

bool CServiceManager::InitForTesting()
{
  m_databaseManager.reset(new CDatabaseManager);

  m_addonMgr.reset(new ADDON::CAddonMgr());
  if (!m_addonMgr->Init())
  {
    CLog::Log(LOGFATAL, "CServiceManager::{}: Unable to start CAddonMgr", __FUNCTION__);
    return false;
  }

  m_extsMimeSupportList.reset(new ADDONS::CExtsMimeSupportList(*m_addonMgr));
  m_fileExtensionProvider.reset(new CFileExtensionProvider(*m_addonMgr));

  init_level = 1;
  return true;
}

void CServiceManager::DeinitTesting()
{
  init_level = 0;
  m_fileExtensionProvider.reset();
  m_extsMimeSupportList.reset();
  m_addonMgr.reset();
  m_databaseManager.reset();
}

bool CServiceManager::InitStageOne()
{
  m_Platform.reset(CPlatform::CreateInstance());
  if (!m_Platform->InitStageOne())
    return false;

#ifdef HAS_PYTHON
  m_XBPython.reset(new XBPython());
  CScriptInvocationManager::GetInstance().RegisterLanguageInvocationHandler(m_XBPython.get(),
                                                                            ".py");
#endif

  m_playlistPlayer.reset(new PLAYLIST::CPlayListPlayer());

  init_level = 1;
  return true;
}

bool CServiceManager::InitStageTwo(const std::string& profilesUserDataFolder)
{
  // Initialize the addon database (must be before the addon manager is init'd)
  m_databaseManager.reset(new CDatabaseManager);

  m_addonMgr.reset(new ADDON::CAddonMgr());
  if (!m_addonMgr->Init())
  {
    CLog::Log(LOGFATAL, "CServiceManager::{}: Unable to start CAddonMgr", __FUNCTION__);
    return false;
  }

  m_repositoryUpdater.reset(new ADDON::CRepositoryUpdater(*m_addonMgr));

  m_extsMimeSupportList.reset(new ADDONS::CExtsMimeSupportList(*m_addonMgr));

  m_dataCacheCore.reset(new CDataCacheCore());

  m_binaryAddonCache.reset(new ADDON::CBinaryAddonCache());
  m_binaryAddonCache->Init();

  m_favouritesService.reset(new CFavouritesService(profilesUserDataFolder));

  m_serviceAddons.reset(new ADDON::CServiceAddonManager(*m_addonMgr));

  m_contextMenuManager.reset(new CContextMenuManager(*m_addonMgr));

  m_inputManager.reset(new CInputManager());
  m_inputManager->InitializeInputs();

  m_fileExtensionProvider.reset(new CFileExtensionProvider(*m_addonMgr));

  m_powerManager.reset(new CPowerManager());
  m_powerManager->Initialize();
  m_powerManager->SetDefaults();

  m_weatherManager.reset(new CWeatherManager());

  m_mediaManager.reset(new CMediaManager());
  m_mediaManager->Initialize();

#if !defined(TARGET_WINDOWS) && defined(HAS_DVD_DRIVE)
  m_DetectDVDType = std::make_unique<MEDIA_DETECT::CDetectDVDMedia>();
#endif

#if defined(HAS_FILESYSTEM_SMB)
  m_WSDiscovery = WSDiscovery::IWSDiscovery::GetInstance();
#endif

  if (!m_Platform->InitStageTwo())
    return false;

  init_level = 2;
  return true;
}

// stage 3 is called after successful initialization of WindowManager
bool CServiceManager::InitStageThree(const std::shared_ptr<CProfileManager>& profileManager)
{
#if !defined(TARGET_WINDOWS) && defined(HAS_DVD_DRIVE)
  // Start Thread for DVD Mediatype detection
  CLog::Log(LOGINFO, "[Media Detection] starting service for optical media detection");
  m_DetectDVDType->Create(false);
#endif

  m_contextMenuManager->Init();

  m_playerCoreFactory.reset(new CPlayerCoreFactory(*profileManager));

  if (!m_Platform->InitStageThree())
    return false;

  init_level = 3;
  return true;
}

void CServiceManager::DeinitStageThree()
{
  init_level = 2;
#if !defined(TARGET_WINDOWS) && defined(HAS_DVD_DRIVE)
  m_DetectDVDType->StopThread();
  m_DetectDVDType.reset();
#endif
  m_playerCoreFactory.reset();
  m_contextMenuManager->Deinit();

  m_Platform->DeinitStageThree();
}

void CServiceManager::DeinitStageTwo()
{
  init_level = 1;

#if defined(HAS_FILESYSTEM_SMB)
  m_WSDiscovery.reset();
#endif

  m_weatherManager.reset();
  m_powerManager.reset();
  m_fileExtensionProvider.reset();
  m_inputManager.reset();
  m_contextMenuManager.reset();
  m_serviceAddons.reset();
  m_favouritesService.reset();
  m_binaryAddonCache.reset();
  m_dataCacheCore.reset();
  m_extsMimeSupportList.reset();
  m_repositoryUpdater.reset();
  m_addonMgr.reset();
  m_databaseManager.reset();

  m_mediaManager->Stop();
  m_mediaManager.reset();

  m_Platform->DeinitStageTwo();
}

void CServiceManager::DeinitStageOne()
{
  init_level = 0;

  m_playlistPlayer.reset();
#ifdef HAS_PYTHON
  CScriptInvocationManager::GetInstance().UnregisterLanguageInvocationHandler(m_XBPython.get());
  m_XBPython.reset();
#endif

  m_Platform->DeinitStageOne();
  m_Platform.reset();
}

#if defined(HAS_FILESYSTEM_SMB)
WSDiscovery::IWSDiscovery& CServiceManager::GetWSDiscovery()
{
  return *m_WSDiscovery;
}
#endif

ADDON::CAddonMgr& CServiceManager::GetAddonMgr()
{
  return *m_addonMgr;
}

ADDONS::CExtsMimeSupportList& CServiceManager::GetExtsMimeSupportList()
{
  return *m_extsMimeSupportList;
}

ADDON::CBinaryAddonCache& CServiceManager::GetBinaryAddonCache()
{
  return *m_binaryAddonCache;
}

ADDON::CServiceAddonManager& CServiceManager::GetServiceAddons()
{
  return *m_serviceAddons;
}

ADDON::CRepositoryUpdater& CServiceManager::GetRepositoryUpdater()
{
  return *m_repositoryUpdater;
}

#ifdef HAS_PYTHON
XBPython& CServiceManager::GetXBPython()
{
  return *m_XBPython;
}
#endif

#if !defined(TARGET_WINDOWS) && defined(HAS_DVD_DRIVE)
MEDIA_DETECT::CDetectDVDMedia& CServiceManager::GetDetectDVDMedia()
{
  return *m_DetectDVDType;
}
#endif

CContextMenuManager& CServiceManager::GetContextMenuManager()
{
  return *m_contextMenuManager;
}

CDataCacheCore& CServiceManager::GetDataCacheCore()
{
  return *m_dataCacheCore;
}

CPlatform& CServiceManager::GetPlatform()
{
  return *m_Platform;
}

PLAYLIST::CPlayListPlayer& CServiceManager::GetPlaylistPlayer()
{
  return *m_playlistPlayer;
}

CFavouritesService& CServiceManager::GetFavouritesService()
{
  return *m_favouritesService;
}

CInputManager& CServiceManager::GetInputManager()
{
  return *m_inputManager;
}

CFileExtensionProvider& CServiceManager::GetFileExtensionProvider()
{
  return *m_fileExtensionProvider;
}

CPowerManager& CServiceManager::GetPowerManager()
{
  return *m_powerManager;
}

// deleters for unique_ptr
void CServiceManager::delete_dataCacheCore::operator()(CDataCacheCore* p) const
{
  delete p;
}

void CServiceManager::delete_contextMenuManager::operator()(CContextMenuManager* p) const
{
  delete p;
}

void CServiceManager::delete_favouritesService::operator()(CFavouritesService* p) const
{
  delete p;
}

CWeatherManager& CServiceManager::GetWeatherManager()
{
  return *m_weatherManager;
}

CPlayerCoreFactory& CServiceManager::GetPlayerCoreFactory()
{
  return *m_playerCoreFactory;
}

CDatabaseManager& CServiceManager::GetDatabaseManager()
{
  return *m_databaseManager;
}

CMediaManager& CServiceManager::GetMediaManager()
{
  return *m_mediaManager;
}
