/*
 *      Copyright (C) 2005-2015 Team XBMC
 *      http://kodi.tv
 *
 *  This Program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2, or (at your option)
 *  any later version.
 *
 *  This Program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with Kodi; see the file COPYING.  If not, see
 *  <http://www.gnu.org/licenses/>.
 *
 */

#include "Application.h"

#include "GUIInfoManager.h"
#include "LangInfo.h"
#include "PlayListPlayer.h"
#include "ServiceManager.h"
#include "URL.h"
#include "Util.h"
#include "addons/AddonManager.h"
#include "addons/RepositoryUpdater.h"
#include "addons/Service.h"
#include "addons/Skin.h"
#include "application/AppParams.h"
#include "application/ApplicationActionListeners.h"
#include "application/ApplicationPlayer.h"
#include "application/ApplicationPowerHandling.h"
#include "application/ApplicationSkinHandling.h"
#include "application/ApplicationStackHelper.h"
#include "dialogs/GUIDialogKaiToast.h"
#include "guilib/GUIComponent.h"
#include "guilib/GUIFontManager.h"
#include "interfaces/generic/ScriptInvocationManager.h"
#include "playlists/PlayListFactory.h"

#include "GUIPassword.h"
#include "ServiceBroker.h"
#include "TextureCache.h"
#include "filesystem/Directory.h"
#include "filesystem/SpecialProtocol.h"
#include "guilib/LocalizeStrings.h"
#include "input/KeyboardLayoutManager.h"
#include "input/Key.h"
#include "messaging/ApplicationMessenger.h"
#include "messaging/ThreadMessage.h"

#include "playlists/PlayList.h"
#include "playlists/SmartPlayList.h"
#include "profiles/ProfileManager.h"
#include "settings/AdvancedSettings.h"
#include "settings/DisplaySettings.h"
#include "settings/MediaSettings.h"
#include "settings/Settings.h"
#include "settings/SettingsComponent.h"
#include "threads/SingleLock.h"
#include "utils/CPUInfo.h"
#include "utils/SystemInfo.h"
#include "utils/Splash.h"
#include "utils/TimeUtils.h"
#include "utils/log.h"

#include "interfaces/AnnouncementManager.h"

// Windows includes
#include "guilib/GUIWindowManager.h"
#include "video/PlayerController.h"

#include "DatabaseManager.h"
#include "input/InputManager.h"
#include "storage/MediaManager.h"

#include "addons/AddonSystemSettings.h"

#include "windowing/WindowingFactory.h"

#include <mutex>

using namespace ADDON;
using namespace XFILE;
using namespace KODI::MESSAGING;

using namespace std::chrono_literals;

#define D3DCREATE_MULTITHREADED 0

CApplication::CApplication(void)
  : m_pPlayer(new CApplicationPlayer)
{
  // register application components
  RegisterComponent(std::make_shared<CApplicationActionListeners>(m_critSection));
  RegisterComponent(std::make_shared<CApplicationPlayer>());
  RegisterComponent(std::make_shared<CApplicationPowerHandling>());
  RegisterComponent(std::make_shared<CApplicationSkinHandling>(this, this, m_bInitializing));
  RegisterComponent(std::make_shared<CApplicationStackHelper>());
}

CApplication::~CApplication(void)
{
  DeregisterComponent(typeid(CApplicationStackHelper));
  DeregisterComponent(typeid(CApplicationSkinHandling));
  DeregisterComponent(typeid(CApplicationPowerHandling));
  DeregisterComponent(typeid(CApplicationPlayer));
  DeregisterComponent(typeid(CApplicationActionListeners));
}

bool CApplication::Create()
{
  m_bStop = false;

  RegisterSettings();

  CServiceBroker::RegisterCPUInfo(CCPUInfo::GetCPUInfo());

  // Register JobManager service
  CServiceBroker::RegisterJobManager(std::make_shared<CJobManager>());

  // Announcement service
  m_pAnnouncementManager = std::make_shared<ANNOUNCEMENT::CAnnouncementManager>();
  m_pAnnouncementManager->Start();
  CServiceBroker::RegisterAnnouncementManager(m_pAnnouncementManager);

  const auto appMessenger = std::make_shared<CApplicationMessenger>();
  CServiceBroker::RegisterAppMessenger(appMessenger);

  const auto keyboardLayoutManager = std::make_shared<CKeyboardLayoutManager>();
  CServiceBroker::RegisterKeyboardLayoutManager(keyboardLayoutManager);

  m_ServiceManager.reset(new CServiceManager());

  if (!m_ServiceManager->InitStageOne())
  {
    return false;
  }

  // here we register all global classes for the CApplicationMessenger,
  // after that we can send messages to the corresponding modules
  appMessenger->RegisterReceiver(this);
  appMessenger->RegisterReceiver(&CServiceBroker::GetPlaylistPlayer());
  appMessenger->SetGUIThread(CThread::GetCurrentThreadId());
  appMessenger->SetProcessThread(CThread::GetCurrentThreadId());

  // copy required files
  CUtil::CopyUserDataIfNeeded("special://masterprofile/", "RssFeeds.xml");
  CUtil::CopyUserDataIfNeeded("special://masterprofile/", "favourites.xml");
  CUtil::CopyUserDataIfNeeded("special://masterprofile/", "Lircmap.xml");

  CLog::Init(CSpecialProtocol::TranslatePath("special://logpath"));

#ifdef TARGET_POSIX //! @todo Win32 has no special://home/ mapping by default, so we
  //!       must create these here. Ideally this should be using special://home/ and
  //!      be platform agnostic (i.e. unify the InitDirectories*() functions)
  if (!CServiceBroker::GetAppParams()->HasPlatformDirectories())
#endif
  {
    CDirectory::Create("special://xbmc/addons");
  }

  PrintStartupLog();

  // TODO: initialize network adapter and network protocols

  CLog::Log(LOGINFO, "loading settings");
  const auto settingsComponent = CServiceBroker::GetSettingsComponent();
  if (!settingsComponent->Load())
    return false;

  CLog::Log(LOGINFO, "creating subdirectories");
  const std::shared_ptr<CProfileManager> profileManager = settingsComponent->GetProfileManager();
  const std::shared_ptr<CSettings> settings = settingsComponent->GetSettings();
  CLog::Log(LOGINFO, "userdata folder: {}",
            CURL::GetRedacted(profileManager->GetProfileUserDataFolder()));
  CLog::Log(LOGINFO, "recording folder: {}",
            CURL::GetRedacted(settings->GetString(CSettings::SETTING_AUDIOCDS_RECORDINGPATH)));
  CLog::Log(LOGINFO, "screenshots folder: {}",
            CURL::GetRedacted(settings->GetString(CSettings::SETTING_DEBUG_SCREENSHOTPATH)));
  CDirectory::Create(profileManager->GetUserDataFolder());
  CDirectory::Create(profileManager->GetProfileUserDataFolder());
  profileManager->CreateProfileFolders();

  if (!m_ServiceManager->InitStageTwo(
          settingsComponent->GetProfileManager()->GetProfileUserDataFolder()))
  {
    return false;
  }

  // load the keyboard layouts
  if (!keyboardLayoutManager->Load())
  {
    CLog::Log(LOGFATAL, "CApplication::Create: Unable to load keyboard layouts");
    return false;
  }

  CUtil::InitRandomSeed();

  m_lastRenderTime = std::chrono::steady_clock::now();
  return true;
}

bool CApplication::CreateGUI()
{
  m_frameMoveGuard.lock();

  const auto appPower = GetComponent<CApplicationPowerHandling>();
  appPower->SetRenderGUI(true);

  // Initialize core peripheral port support. Note: If these parameters
  // are 0 and NULL, respectively, then the default number and types of
  // controllers will be initialized.
  if (!g_Windowing.InitWindowSystem())
  {
    CLog::Log(LOGFATAL, "CApplication::Create: Unable to init windowing system");
    return false;
  }

  // Retrieve the matching resolution based on GUI settings
  bool sav_res = false;
  CDisplaySettings::GetInstance().SetCurrentResolution(CDisplaySettings::GetInstance().GetDisplayResolution());
  CLog::Log(LOGINFO, "Checking resolution {}",
            CDisplaySettings::GetInstance().GetCurrentResolution());
  if (!g_graphicsContext.IsValidResolution(CDisplaySettings::GetInstance().GetCurrentResolution()))
  {
    // TODO: get best resolution for Xbox and set that one
    CLog::Log(LOGINFO, "Setting safe mode {}", RES_DESKTOP);
    // defer saving resolution after window was created
    CDisplaySettings::GetInstance().SetCurrentResolution(RES_DESKTOP);
    sav_res = true;
  }

  // update the window resolution
  const std::shared_ptr<CSettings> settings = CServiceBroker::GetSettingsComponent()->GetSettings();
  g_Windowing.SetWindowResolution(settings->GetInt(CSettings::SETTING_WINDOW_WIDTH), settings->GetInt(CSettings::SETTING_WINDOW_HEIGHT));

  if (CServiceBroker::GetSettingsComponent()->GetAdvancedSettings()->m_startFullScreen && CDisplaySettings::GetInstance().GetCurrentResolution() == RES_WINDOW)
  {
    // defer saving resolution after window was created
    CDisplaySettings::GetInstance().SetCurrentResolution(RES_DESKTOP);
    sav_res = true;
  }

  if (!g_graphicsContext.IsValidResolution(CDisplaySettings::GetInstance().GetCurrentResolution()))
  {
    // Oh uh - doesn't look good for starting in their wanted screenmode
    CLog::Log(LOGERROR, "The screen resolution requested is not valid, resetting to a valid mode");
    CDisplaySettings::GetInstance().SetCurrentResolution(RES_DESKTOP);
    sav_res = true;
  }
  if (!InitWindow())
  {
    return false;
  }

  // Set default screen saver mode
  auto screensaverModeSetting = std::static_pointer_cast<CSettingString>(settings->GetSetting(CSettings::SETTING_SCREENSAVER_MODE));
  {
    // If OS has no screen saver, use Kodi one by default
    screensaverModeSetting->SetDefault("screensaver.xbmc.builtin.dim");
  }

  if (sav_res)
    CDisplaySettings::GetInstance().SetCurrentResolution(RES_DESKTOP, true);

  m_pGUI.reset(new CGUIComponent());
  m_pGUI->Init();

  // Splash requires gui component!!
  CSplash::GetInstance().Show("");

  // The key mappings may already have been loaded by a peripheral
  CLog::Log(LOGINFO, "load keymapping");
  if (!CServiceBroker::GetInputManager().LoadKeymaps())
    return false;

  RESOLUTION_INFO info = g_graphicsContext.GetResInfo();
  CLog::Log(LOGINFO, "GUI format {}x{}, Display {}", info.iWidth, info.iHeight, info.strMode);

  return true;
}

bool CApplication::InitWindow(RESOLUTION res)
{
  if (res == RES_INVALID)
    res = CDisplaySettings::GetInstance().GetCurrentResolution();

  bool bFullScreen = res != RES_WINDOW;
  if (!g_Windowing.CreateNewWindow(CSysInfo::GetAppName(),
                                                      bFullScreen, CDisplaySettings::GetInstance().GetResolutionInfo(res)))
  {
    CLog::Log(LOGFATAL, "CApplication::Create: Unable to create window");
    return false;
  }

  if (!g_Windowing.InitRenderSystem())
  {
    CLog::Log(LOGFATAL, "CApplication::Create: Unable to init rendering system");
    return false;
  }
  // set GUI res and force the clear of the screen
  g_graphicsContext.SetVideoResolution(res, false);
  return true;
}

bool CApplication::Initialize()
{
#if defined(HAS_DVD_DRIVE) && !defined(TARGET_WINDOWS) // somehow this throws an "unresolved external symbol" on win32
  // turn off cdio logging
  cdio_loglevel_default = CDIO_LOG_ERROR;
#endif

  // load the language and its translated strings
  if (!LoadLanguage(false))
    return false;

  // load media manager sources (e.g. root addon type sources depend on language strings to be available)
  CServiceBroker::GetMediaManager().LoadSources();

  const std::shared_ptr<CProfileManager> profileManager = CServiceBroker::GetSettingsComponent()->GetProfileManager();

  // TODO: wait for network connection

  // initialize (and update as needed) our databases
  CDatabaseManager &databaseManager = m_ServiceManager->GetDatabaseManager();

  CEvent event(true);
#ifndef NXDK
  // CEvent and lambdas in combination with Job Manager are broken for some reason
  CServiceBroker::GetJobManager()->Submit([&databaseManager, &event]() {
    databaseManager.Initialize();
    event.Set();
  });

  std::string localizedStr = g_localizeStrings.Get(24150);
  int iDots = 1;
  while (!event.Wait(1000ms))
  {
    if (databaseManager.IsUpgrading())
      CSplash::GetInstance().Show(std::string(iDots, ' ') + localizedStr + std::string(iDots, '.'));

    if (iDots == 3)
      iDots = 1;
    else
      ++iDots;
  }
#else
  std::string localizedStr = "";
  int iDots = 1;
  databaseManager.Initialize();
#endif
  CSplash::GetInstance().Show("");

  // GUI depends on seek handler
  GetComponent<CApplicationPlayer>()->GetSeekHandler().Configure();

  const auto skinHandling = GetComponent<CApplicationSkinHandling>();

  bool uiInitializationFinished = false;

  if (CServiceBroker::GetGUI()->GetWindowManager().Initialized())
  {
    const auto settings = CServiceBroker::GetSettingsComponent()->GetSettings();

    CServiceBroker::GetGUI()->GetWindowManager().CreateWindows();

    skinHandling->m_confirmSkinChange = false;

    std::vector<AddonInfoPtr> incompatibleAddons;
    event.Reset();

    // Addon migration
    if (CServiceBroker::GetAddonMgr().GetIncompatibleEnabledAddonInfos(incompatibleAddons))
    {
      if (CAddonSystemSettings::GetInstance().GetAddonAutoUpdateMode() == AUTO_UPDATES_ON)
      {
        CServiceBroker::GetJobManager()->Submit(
            [&event, &incompatibleAddons]() {
              if (CServiceBroker::GetRepositoryUpdater().CheckForUpdates())
                CServiceBroker::GetRepositoryUpdater().Await();

              incompatibleAddons = CServiceBroker::GetAddonMgr().MigrateAddons();
              event.Set();
            },
            CJob::PRIORITY_DEDICATED);
        localizedStr = g_localizeStrings.Get(24151);
        iDots = 1;
        while (!event.Wait(1000ms))
        {
          CSplash::GetInstance().Show(std::string(iDots, ' ') + localizedStr +
                                      std::string(iDots, '.'));
          if (iDots == 3)
            iDots = 1;
          else
            ++iDots;
        }
        m_incompatibleAddons = incompatibleAddons;
      }
      else
      {
        // If no update is active disable all incompatible addons during start
        m_incompatibleAddons =
            CServiceBroker::GetAddonMgr().DisableIncompatibleAddons(incompatibleAddons);
      }
    }

    // Start splashscreen and load skin
    CSplash::GetInstance().Show("");
    skinHandling->m_confirmSkinChange = true;

    auto setting = settings->GetSetting(CSettings::SETTING_LOOKANDFEEL_SKIN);
    if (!setting)
    {
      CLog::Log(LOGFATAL, "Failed to load setting for: {}", CSettings::SETTING_LOOKANDFEEL_SKIN);
      return false;
    }

    CServiceBroker::RegisterTextureCache(std::make_shared<CTextureCache>());

    std::string skinId = settings->GetString(CSettings::SETTING_LOOKANDFEEL_SKIN);
    if (!skinHandling->LoadSkin(skinId))
    {
      CLog::Log(LOGERROR, "Failed to load skin '{}'", skinId);
      std::string defaultSkin =
          std::static_pointer_cast<const CSettingString>(setting)->GetDefault();
      if (!skinHandling->LoadSkin(defaultSkin))
      {
        CLog::Log(LOGFATAL, "Default skin '{}' could not be loaded! Terminating..", defaultSkin);
        return false;
      }
    }

    // initialize splash window after splash screen disappears
    // because we need a real window in the background which gets
    // rendered while we load the main window or enter the master lock key
    CServiceBroker::GetGUI()->GetWindowManager().ActivateWindow(WINDOW_SPLASH);

    if (settings->GetBool(CSettings::SETTING_MASTERLOCK_STARTUPLOCK) &&
        profileManager->GetMasterProfile().getLockMode() != LOCK_MODE_EVERYONE &&
        !profileManager->GetMasterProfile().getLockCode().empty())
    {
      g_passwordManager.CheckStartUpLock();
    }

    // check if we should use the login screen
    if (profileManager->UsingLoginScreen())
    {
      CServiceBroker::GetGUI()->GetWindowManager().ActivateWindow(WINDOW_LOGIN_SCREEN);
    }
    else
    {
      // activate the configured start window
      int firstWindow = g_SkinInfo->GetFirstWindow();
      CServiceBroker::GetGUI()->GetWindowManager().ActivateWindow(firstWindow);

      if (CServiceBroker::GetGUI()->GetWindowManager().IsWindowActive(WINDOW_STARTUP_ANIM))
      {
        CLog::Log(LOGWARNING, "CApplication::Initialize - startup.xml taints init process");
      }

      // the startup window is considered part of the initialization as it most likely switches to the final window
      uiInitializationFinished = firstWindow != WINDOW_STARTUP_ANIM;
    }
  }
  else //No GUI Created
  {
    uiInitializationFinished = true;
  }

  if (!m_ServiceManager->InitStageThree(profileManager))
  {
    CLog::Log(LOGERROR, "Application - Init3 failed");
  }

  g_sysinfo.Refresh();

  CLog::Log(LOGINFO, "removing tempfiles");
  CUtil::RemoveTempFiles();

  if (!profileManager->UsingLoginScreen())
  {
    UpdateLibraries();
    SetLoggingIn(false);
  }

  m_slowTimer.StartZero();

  // register action listeners
  const auto appListener = GetComponent<CApplicationActionListeners>();
  const auto appPlayer = GetComponent<CApplicationPlayer>();
  appListener->RegisterActionListener(&appPlayer->GetSeekHandler());
  appListener->RegisterActionListener(&CPlayerController::GetInstance());

  CServiceBroker::GetRepositoryUpdater().Start();
  if (!profileManager->UsingLoginScreen())
    CServiceBroker::GetServiceAddons().Start();

  CLog::Log(LOGINFO, "initialize done");

  const auto appPower = GetComponent<CApplicationPowerHandling>();
  appPower->CheckOSScreenSaverInhibitionSetting();
  // reset our screensaver (starts timers etc.)
  appPower->ResetScreenSaver();

  // if the user interfaces has been fully initialized let everyone know
  if (uiInitializationFinished)
  {
    CGUIMessage msg(GUI_MSG_NOTIFY_ALL, 0, 0, GUI_MSG_UI_READY);
    CServiceBroker::GetGUI()->GetWindowManager().SendThreadMessage(msg);
  }

  return true;
}

void CApplication::Render()
{
  // do not render if we are stopped or in background
  if (m_bStop)
    return;

  const auto appPlayer = GetComponent<CApplicationPlayer>();
  const auto appPower = GetComponent<CApplicationPowerHandling>();

  bool hasRendered = false;

  // Whether externalplayer is playing and we're unfocused
  bool extPlayerActive = appPlayer->IsExternalPlaying() && !m_AppFocused;

  if (!extPlayerActive && g_graphicsContext.IsFullScreenVideo() &&
      !appPlayer->IsPausedPlayback())
  {
    appPower->ResetScreenSaver();
  }

  if (!g_Windowing.BeginRender())
    return;

  // render gui layer
  if (appPower->GetRenderGUI() && !m_skipGuiRender)
  {
    {
      hasRendered |= CServiceBroker::GetGUI()->GetWindowManager().Render();
    }
    // execute post rendering actions (finalize window closing)
    CServiceBroker::GetGUI()->GetWindowManager().AfterRender();

    m_lastRenderTime = std::chrono::steady_clock::now();
  }

  // render video layer
  CServiceBroker::GetGUI()->GetWindowManager().RenderEx();

  g_Windowing.EndRender();

  // reset our info cache - we do this at the end of Render so that it is
  // fresh for the next process(), or after a windowclose animation (where process()
  // isn't called)
  CGUIInfoManager& infoMgr = CServiceBroker::GetGUI()->GetInfoManager();
  infoMgr.ResetCache();
  infoMgr.GetInfoProviders().GetGUIControlsInfoProvider().ResetContainerMovingCache();

  if (hasRendered)
  {
    infoMgr.GetInfoProviders().GetSystemInfoProvider().UpdateFPS();
  }

  g_graphicsContext.Flip(hasRendered,
                         appPlayer->IsRenderingVideoLayer());

  CTimeUtils::UpdateFrameTime(hasRendered);
}

bool CApplication::OnAction(const CAction &action)
{
  return false;
}

int CApplication::GetMessageMask()
{
  return 0;
}

void CApplication::OnApplicationMessage(ThreadMessage* pMsg)
{
}

void CApplication::FrameMove(bool processEvents, bool processGUI)
{
  const auto appPlayer = GetComponent<CApplicationPlayer>();
  bool renderGUI = GetComponent<CApplicationPowerHandling>()->GetRenderGUI();
  if (processEvents)
  {
    // currently we calculate the repeat time (ie time from last similar keypress) just global as fps
    float frameTime = m_frameTime.GetElapsedSeconds();
    m_frameTime.StartZero();
    // never set a frametime less than 2 fps to avoid problems when debugging and on breaks
    if (frameTime > 0.5f)
      frameTime = 0.5f;

    if (processGUI && renderGUI)
    {
      std::unique_lock<CCriticalSection> lock(g_graphicsContext);
      // check if there are notifications to display
      CGUIDialogKaiToast *toast = CServiceBroker::GetGUI()->GetWindowManager().GetWindow<CGUIDialogKaiToast>(WINDOW_DIALOG_KAI_TOAST);
      if (toast && toast->DoWork())
      {
        if (!toast->IsDialogRunning())
        {
          toast->Open();
        }
      }
    }

    CServiceBroker::GetInputManager().Process(CServiceBroker::GetGUI()->GetWindowManager().GetFocusedWindow(), frameTime);

    if (processGUI && renderGUI)
    {
      appPlayer->GetSeekHandler().FrameMove();
    }
  }

  if (processGUI && renderGUI)
  {
    m_skipGuiRender = false;

    /*! @todo look into the possibility to use this for GBM
    int fps = 0;

    // This code reduces rendering fps of the GUI layer when playing videos in fullscreen mode
    // it makes only sense on architectures with multiple layers
    if (CServiceBroker::GetWinSystem()->GetGfxContext().IsFullScreenVideo() && !m_appPlayer.IsPausedPlayback() && m_appPlayer.IsRenderingVideoLayer())
      fps = CServiceBroker::GetSettingsComponent()->GetSettings()->GetInt(CSettings::SETTING_VIDEOPLAYER_LIMITGUIUPDATE);

    auto now = std::chrono::steady_clock::now();

    auto frameTime = std::chrono::duration_cast<std::chrono::milliseconds>(now - m_lastRenderTime).count();
    if (fps > 0 && frameTime * fps < 1000)
      m_skipGuiRender = true;
    */

    if (CServiceBroker::GetSettingsComponent()->GetAdvancedSettings()->m_guiSmartRedraw && m_guiRefreshTimer.IsTimePast())
    {
      CServiceBroker::GetGUI()->GetWindowManager().SendMessage(GUI_MSG_REFRESH_TIMER, 0, 0);
      m_guiRefreshTimer.Set(500ms);
    }

    if (!m_bStop)
    {
      if (!m_skipGuiRender)
        CServiceBroker::GetGUI()->GetWindowManager().Process(CTimeUtils::GetFrameTime());
    }
    CServiceBroker::GetGUI()->GetWindowManager().FrameMove();
  }

  appPlayer->FrameMove();
}

int CApplication::Run()
{
  CLog::Log(LOGINFO, "Running the application...");

  std::chrono::time_point<std::chrono::steady_clock> lastFrameTime;
  std::chrono::milliseconds frameTime;
  const unsigned int noRenderFrameTime = 15; // Simulates ~66fps

  CFileItemList& playlist = CServiceBroker::GetAppParams()->GetPlaylist();
  if (playlist.Size() > 0)
  {
    CServiceBroker::GetPlaylistPlayer().Add(PLAYLIST::TYPE_MUSIC, playlist);
    CServiceBroker::GetPlaylistPlayer().SetCurrentPlaylist(PLAYLIST::TYPE_MUSIC);
    CServiceBroker::GetAppMessenger()->PostMsg(TMSG_PLAYLISTPLAYER_PLAY, -1);
  }

  // Run the app
  while (!m_bStop)
  {
    // Animate and render a frame

    lastFrameTime = std::chrono::steady_clock::now();
    Process();

    bool renderGUI = GetComponent<CApplicationPowerHandling>()->GetRenderGUI();
    if (!m_bStop)
    {
      FrameMove(true, renderGUI);
    }

    if (renderGUI && !m_bStop)
    {
      Render();
    }
    else if (!renderGUI)
    {
      auto now = std::chrono::steady_clock::now();
      frameTime = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastFrameTime);
      if (frameTime.count() < noRenderFrameTime)
        KODI::TIME::Sleep(std::chrono::milliseconds(noRenderFrameTime - frameTime.count()));
    }
  }

  Cleanup();

  CLog::Log(LOGINFO, "Exiting the application...");
  return m_ExitCode;
}

bool CApplication::Cleanup()
{
  // TODO: implement this
  return false;
}

bool CApplication::Stop(int exitCode)
{
  // TODO: implement this
  return false;
}

bool CApplication::PlayMedia(CFileItem& item, const std::string& player, PLAYLIST::Id playlistId)
{
  //nothing special just play
  return PlayFile(item, player, false);
}

bool CApplication::PlayFile(CFileItem item, const std::string& player, bool bRestart)
{
  return false;
}

void CApplication::StopPlaying()
{
}

bool CApplication::OnMessage(CGUIMessage& message)
{
  return false;
}

void CApplication::Process()
{
  // dispatch the messages generated by python or other threads to the current window
  CServiceBroker::GetGUI()->GetWindowManager().DispatchThreadMessages();

  // process messages which have to be send to the gui
  // (this can only be done after CServiceBroker::GetGUI()->GetWindowManager().Render())
  CServiceBroker::GetAppMessenger()->ProcessWindowMessages();

  // handle any active scripts

  {
    // Allow processing of script threads to let them shut down properly.
    CSingleExit ex(g_graphicsContext);
    m_frameMoveGuard.unlock();
    CScriptInvocationManager::GetInstance().Process();
    m_frameMoveGuard.lock();
  }

  // process messages, even if a movie is playing
  CServiceBroker::GetAppMessenger()->ProcessMessages();
  if (m_bStop) return; //we're done, everything has been unloaded

  // update sound
  GetComponent<CApplicationPlayer>()->DoAudioWork();

  // do any processing that isn't needed on each run
  if( m_slowTimer.GetElapsedMilliseconds() > 500 )
  {
    m_slowTimer.Reset();
    ProcessSlow();
  }
}

// We get called every 500ms
void CApplication::ProcessSlow()
{
  // TODO: clean textures, process jobs etc.
}

void CApplication::Restart(bool bSamePosition)
{
}

const std::string& CApplication::CurrentFile()
{
  return StringUtils::Empty;
}

std::shared_ptr<CFileItem> CApplication::CurrentFileItemPtr()
{
  return m_itemCurrentFile;
}

CFileItem& CApplication::CurrentFileItem()
{
  return *m_itemCurrentFile;
}

CFileItem& CApplication::CurrentUnstackedItem()
{
  return *CFileItemPtr();
}

// Returns the total time in seconds of the current media.  Fractional
// portions of a second are possible - but not necessarily supported by the
// player class.  This returns a double to be consistent with GetTime() and
// SeekTime().
double CApplication::GetTotalTime() const
{
  return 0.0;
}

// Returns the current time in seconds of the currently playing media.
// Fractional portions of a second are possible.  This returns a double to
// be consistent with GetTotalTime() and SeekTime().
double CApplication::GetTime() const
{
  return 0.0;
}

// Sets the current position of the currently playing media to the specified
// time in seconds.  Fractional portions of a second are valid.  The passed
// time is the time offset from the beginning of the file as opposed to a
// delta from the current position.  This method accepts a double to be
// consistent with GetTime() and GetTotalTime().
void CApplication::SeekTime( double dTime )
{
}

float CApplication::GetPercentage() const
{
  return 0.0f;
}

float CApplication::GetCachePercentage() const
{
  return 0.0f;
}

void CApplication::SeekPercentage(float percent)
{
}

// SwitchToFullScreen() returns true if a switch is made, else returns false
bool CApplication::SwitchToFullScreen(bool force /* = false */)
{
  return false;
}

bool CApplication::IsMuted() const
{
  return false;
}

void CApplication::ToggleMute(void)
{
}

void CApplication::SetVolume(long iValue, bool isPercentage /* = true */)
{
}

int CApplication::GetVolume(bool percentage /* = true */) const
{
  return 0;
}

bool CApplication::IsCurrentThread() const
{
  return true;
}

void CApplication::UpdateCurrentPlayArt()
{

}

bool CApplication::ProcessAndStartPlaylist(const std::string& strPlayList,
                                           PLAYLIST::CPlayList& playlist,
                                           PLAYLIST::Id playlistId,
                                           int track)
{
  CLog::Log(LOGDEBUG, "CApplication::ProcessAndStartPlaylist({}, {})", strPlayList, playlistId);

  // initial exit conditions
  // no songs in playlist just return
  if (playlist.size() == 0)
    return false;

  // illegal playlist
  if (playlistId == PLAYLIST::TYPE_NONE || playlistId == PLAYLIST::TYPE_PICTURE)
    return false;

  // setup correct playlist
  CServiceBroker::GetPlaylistPlayer().ClearPlaylist(playlistId);

  // if the playlist contains an internet stream, this file will be used
  // to generate a thumbnail for musicplayer.cover
  m_strPlayListFile = strPlayList;

  // add the items to the playlist player
  CServiceBroker::GetPlaylistPlayer().Add(playlistId, playlist);

  // if we have a playlist
  if (CServiceBroker::GetPlaylistPlayer().GetPlaylist(playlistId).size())
  {
    // start playing it
    CServiceBroker::GetPlaylistPlayer().SetCurrentPlaylist(playlistId);
    CServiceBroker::GetPlaylistPlayer().Reset();
    CServiceBroker::GetPlaylistPlayer().Play(track, "");
    return true;
  }
  return false;
}

std::string CApplication::GetCurrentPlayer()
{
  return m_pPlayer->GetCurrentPlayer();
}

void CApplication::UpdateLibraries()
{
}

bool CApplication::SetLanguage(const std::string &strLanguage)
{
  // nothing to be done if the language hasn't changed
  if (strLanguage == CServiceBroker::GetSettingsComponent()->GetSettings()->GetString(CSettings::SETTING_LOCALE_LANGUAGE))
    return true;

  return CServiceBroker::GetSettingsComponent()->GetSettings()->SetString(CSettings::SETTING_LOCALE_LANGUAGE, strLanguage);
}

bool CApplication::LoadLanguage(bool reload)
{
  // load the configured language
  if (!g_langInfo.SetLanguage("", reload))
    return false;

  // set the proper audio and subtitle languages
  const std::shared_ptr<CSettings> settings = CServiceBroker::GetSettingsComponent()->GetSettings();
  g_langInfo.SetAudioLanguage(settings->GetString(CSettings::SETTING_LOCALE_AUDIOLANGUAGE));
  g_langInfo.SetSubtitleLanguage(settings->GetString(CSettings::SETTING_LOCALE_SUBTITLELANGUAGE));

  return true;
}

void CApplication::SetLoggingIn(bool switchingProfiles)
{
}

void CApplication::PrintStartupLog()
{
  CLog::Log(LOGINFO, "-----------------------------------------------------------------------");
  CLog::Log(LOGNOTICE, "Starting Kodi. Built on {}", __DATE__);
  CSpecialProtocol::LogPaths();
  CLog::Log(LOGINFO, "-----------------------------------------------------------------------");
}
