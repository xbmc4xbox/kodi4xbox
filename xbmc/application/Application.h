#pragma once

/*
 *      Copyright (C) 2005-2013 Team XBMC
 *      http://xbmc.org
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
 *  along with XBMC; see the file COPYING.  If not, see
 *  <http://www.gnu.org/licenses/>.
 *
 */

#include "application/ApplicationComponents.h"
#include "application/ApplicationEnums.h"
#include "application/ApplicationPlayerCallback.h"
#include "application/ApplicationSettingsHandling.h"
#include "guilib/IMsgTargetCallback.h"
#include "guilib/IWindowManagerCallback.h"
#include "messaging/IMessageTarget.h"
#include "playlists/PlayListTypes.h"
#include "utils/GlobalsHandling.h"
#include "guilib/Resolution.h"

#include <atomic>
#include <chrono>
#include <deque>
#include <memory>
#include <string>
#include <vector>

class CAction;
class CFileItem;
class CFileItemList;
class CGUIComponent;
class CKey;
class CServiceManager;

namespace ADDON
{
  class CAddonInfo;
}

namespace ANNOUNCEMENT
{
  class CAnnouncementManager;
}

namespace PLAYLIST
{
  class CPlayList;
}

#include "ApplicationPlayer.h"
#include "FileItem.h"

#define VOLUME_MINIMUM -6000  // -60dB
#define VOLUME_MAXIMUM 0      // 0dB

class CApplication : public IWindowManagerCallback,
                     public IMsgTargetCallback,
                     public KODI::MESSAGING::IMessageTarget,
                     public CApplicationComponents,
                     public CApplicationPlayerCallback,
                     public CApplicationSettingsHandling
{
public:
  CApplication(void);
  virtual ~CApplication(void);

  bool Create();
  bool Initialize();
  int Run();
  bool Cleanup();

  void FrameMove(bool processEvents, bool processGUI = true) override;
  void Render() override;

  bool CreateGUI();
  bool InitWindow(RESOLUTION res = RES_INVALID);

  bool Stop(int exitCode);
  bool IsCurrentThread() const;
  const std::string& CurrentFile();
  CFileItem& CurrentFileItem();
  std::shared_ptr<CFileItem> CurrentFileItemPtr();
  CFileItem& CurrentUnstackedItem();
  bool OnMessage(CGUIMessage& message) override;
  std::string GetCurrentPlayer();

  int  GetMessageMask() override;
  void OnApplicationMessage(KODI::MESSAGING::ThreadMessage* pMsg) override;

  bool PlayMedia(CFileItem& item, const std::string& player, PLAYLIST::Id playlistId);
  bool ProcessAndStartPlaylist(const std::string& strPlayList,
                               PLAYLIST::CPlayList& playlist,
                               PLAYLIST::Id playlistId,
                               int track = 0);
  bool PlayFile(CFileItem item, const std::string& player, bool bRestart = false);
  void StopPlaying();
  void Restart(bool bSamePosition = true);

  void Process() override;
  void ProcessSlow();
  /*!
   \brief Returns the total time in fractional seconds of the currently playing media

   Beware that this method returns fractional seconds whereas IPlayer::GetTotalTime() returns milliseconds.
   */
  double GetTotalTime() const;
  /*!
   \brief Returns the current time in fractional seconds of the currently playing media

   Beware that this method returns fractional seconds whereas IPlayer::GetTime() returns milliseconds.
   */
  double GetTime() const;
  float GetPercentage() const;

  // Get the percentage of data currently cached/buffered (aq/vq + FileCache) from the input stream if applicable.
  float GetCachePercentage() const;

  int GetVolume(bool percentage = true) const;
  void SetVolume(long iValue, bool isPercentage = true);
  int GetDynamicRangeCompressionLevel() { return 0; };
  bool IsMuted() const;
  void ToggleMute(void);

  bool SetLanguage(const std::string &strLanguage);
  bool LoadLanguage(bool reload);

  void SetLoggingIn(bool switchingProfiles);

  void SeekPercentage(float percent);
  void SeekTime( double dTime = 0.0 );

  void UpdateLibraries();

  void UpdateCurrentPlayArt();

  bool ExecuteXBMCAction(std::string action, const CGUIListItemPtr &item = NULL);

  std::string m_strPlayListFile;

  bool OnAction(const CAction &action);

  CApplicationPlayer* m_pPlayer;

  bool IsStandAlone()
  {
    return true;
  }

  std::unique_ptr<CServiceManager> m_ServiceManager;

  bool SwitchToFullScreen(bool force = false);

protected:
  std::shared_ptr<ANNOUNCEMENT::CAnnouncementManager> m_pAnnouncementManager;
  std::unique_ptr<CGUIComponent> m_pGUI;

  // timer information
  CStopWatch m_frameTime;
  CStopWatch m_slowTimer;
  XbmcThreads::EndTime<> m_guiRefreshTimer;

  bool m_bInitializing = true;

  std::chrono::time_point<std::chrono::steady_clock> m_lastRenderTime;
  bool m_skipGuiRender = false;

  std::vector<std::shared_ptr<ADDON::CAddonInfo>>
      m_incompatibleAddons; /*!< Result of addon migration (incompatible addon infos) */

public:
  bool m_bStop{false};
  bool m_AppFocused{true};

private:
  void PrintStartupLog();

  mutable CCriticalSection m_critSection; /*!< critical section for all changes to this class, except for changes to triggers */

  CCriticalSection m_frameMoveGuard;              /*!< critical section for synchronizing GUI actions from inside and outside (python) */
  int m_ExitCode{EXITCODE_QUIT};
};

XBMC_GLOBAL_REF(CApplication,g_application);
#define g_application XBMC_GLOBAL_USE(CApplication)
