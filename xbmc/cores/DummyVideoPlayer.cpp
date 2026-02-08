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

#include "DummyVideoPlayer.h"

#include "application/Application.h"
#include "ServiceBroker.h"
#include "guilib/GUIFont.h"
#include "guilib/GUIFontManager.h"
#include "guilib/GUITextLayout.h"
#include "settings/AdvancedSettings.h"
#include "settings/SettingsComponent.h"
#include "utils/log.h"

CDummyVideoPlayer::CDummyVideoPlayer(IPlayerCallback& callback) : IPlayer(callback), CThread("CDummyVideoPlayer")
{
  m_paused = false;
  m_clock = 0;
  m_lastTime = std::chrono::steady_clock::now();
  m_speed = 1;
}

CDummyVideoPlayer::~CDummyVideoPlayer()
{
  CloseFile();
}

bool CDummyVideoPlayer::OpenFile(const CFileItem& file, const CPlayerOptions &options)
{
  try
  {
    Create();
    if( options.starttime > 0 )
      SeekTime( (int64_t)(options.starttime * 1000) );
    return true;
  }
  catch(...)
  {
    CLog::Log(LOGERROR,"{} - Exception thrown on open", __FUNCTION__);
    return false;
  }
}

bool CDummyVideoPlayer::CloseFile(bool reopen /* = false */)
{
  StopThread();
  return true;
}

bool CDummyVideoPlayer::IsPlaying() const
{
  return !m_bStop;
}

void CDummyVideoPlayer::Process()
{
  m_clock = 0;
  m_lastTime = std::chrono::steady_clock::now();

  m_callback.OnPlayBackStarted();
  while (!m_bStop)
  {
    auto now = std::chrono::steady_clock::now();
    if (!m_paused)
    {
      auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - m_lastTime);
      m_clock += duration.count() * m_speed;
    }
    m_lastTime = std::chrono::steady_clock::now();
    Sleep(std::chrono::milliseconds(0));
    g_graphicsContext.Lock();
    if (g_graphicsContext.IsFullScreenVideo())
    {
#if 0
      g_graphicsContext.Get3DDevice()->BeginScene();
#endif
      g_graphicsContext.Clear();
      g_graphicsContext.SetRenderingResolution(g_graphicsContext.GetResInfo(), false);
      Render();
#if 0
      if (g_application.NeedRenderFullScreen())
        g_application.RenderFullScreen();
      g_graphicsContext.Get3DDevice()->EndScene();
#endif
    }
    g_graphicsContext.Unlock();
  }
  if (m_bStop)
    m_callback.OnPlayBackEnded();
}

void CDummyVideoPlayer::Pause()
{
  if (m_paused)
    m_callback.OnPlayBackResumed();
  else
	  m_callback.OnPlayBackPaused();
  m_paused = !m_paused;
}

bool CDummyVideoPlayer::IsPaused() const
{
  return m_paused;
}

bool CDummyVideoPlayer::HasVideo() const
{
  return true;
}

bool CDummyVideoPlayer::HasAudio() const
{
  return true;
}

void CDummyVideoPlayer::SwitchToNextLanguage()
{
}

void CDummyVideoPlayer::ToggleSubtitles()
{
}

bool CDummyVideoPlayer::CanSeek()
{
  return GetTotalTime() > 0;
}

void CDummyVideoPlayer::Seek(bool bPlus, bool bLargeStep, bool bChapterOverride)
{
  if (CServiceBroker::GetSettingsComponent()->GetAdvancedSettings()->m_videoUseTimeSeeking && GetTotalTime() > 2000*CServiceBroker::GetSettingsComponent()->GetAdvancedSettings()->m_videoTimeSeekForwardBig)
  {
    int seek = 0;
    if (bLargeStep)
      seek = bPlus ? CServiceBroker::GetSettingsComponent()->GetAdvancedSettings()->m_videoTimeSeekForwardBig : CServiceBroker::GetSettingsComponent()->GetAdvancedSettings()->m_videoTimeSeekBackwardBig;
    else
      seek = bPlus ? CServiceBroker::GetSettingsComponent()->GetAdvancedSettings()->m_videoTimeSeekForward : CServiceBroker::GetSettingsComponent()->GetAdvancedSettings()->m_videoTimeSeekBackward;
    // do the seek
    SeekTime(GetTime() + seek * 1000);
  }
  else
  {
    float percent = GetPercentage();
    if (bLargeStep)
      percent += bPlus ? CServiceBroker::GetSettingsComponent()->GetAdvancedSettings()->m_videoPercentSeekForwardBig : CServiceBroker::GetSettingsComponent()->GetAdvancedSettings()->m_videoPercentSeekBackwardBig;
    else
      percent += bPlus ? CServiceBroker::GetSettingsComponent()->GetAdvancedSettings()->m_videoPercentSeekForward : CServiceBroker::GetSettingsComponent()->GetAdvancedSettings()->m_videoPercentSeekBackward;

    if (percent >= 0 && percent <= 100)
    {
      // should be modified to seektime
      SeekPercentage(percent);
    }
  }
}

void CDummyVideoPlayer::GetAudioInfo(std::string& strAudioInfo)
{
  strAudioInfo = "DummyVideoPlayer - nothing to see here";
}

void CDummyVideoPlayer::GetVideoInfo(std::string& strVideoInfo)
{
  strVideoInfo = "DummyVideoPlayer - nothing to see here";
}

void CDummyVideoPlayer::GetGeneralInfo(std::string& strGeneralInfo)
{
  strGeneralInfo = "DummyVideoPlayer - what are you still looking for?";
}

void CDummyVideoPlayer::SwitchToNextAudioLanguage()
{
}

void CDummyVideoPlayer::SeekPercentage(float iPercent)
{
  int64_t iTime = (int64_t)(GetTotalTime() * iPercent / 100);
  SeekTime(iTime);
}

float CDummyVideoPlayer::GetPercentage()
{
  int64_t iTotalTime = GetTotalTime();

  if (iTotalTime != 0)
  {
    return GetTime() * 100 / (float)iTotalTime;
  }

  return 0.0f;
}

//This is how much audio is delayed to video, we count the oposite in the dvdplayer
void CDummyVideoPlayer::SetAVDelay(float fValue)
{
}

float CDummyVideoPlayer::GetAVDelay()
{
  return 0.0f;
}

void CDummyVideoPlayer::SetSubTitleDelay(float fValue)
{
}

float CDummyVideoPlayer::GetSubTitleDelay()
{
  return 0.0;
}

void CDummyVideoPlayer::SeekTime(int64_t iTime)
{
  int seekOffset = (int)(iTime - m_clock);
  m_clock = iTime;
  m_callback.OnPlayBackSeek((int)iTime, seekOffset);
}

// return the time in milliseconds
int64_t CDummyVideoPlayer::GetTime()
{
  return m_clock;
}

// return length in milliseconds
int64_t CDummyVideoPlayer::GetTotalTime()
{
  return 1000000;
}

void CDummyVideoPlayer::SetSpeed(float speed)
{
  m_speed = (int)speed;
  m_callback.OnPlayBackSpeedChanged(m_speed);
}

float CDummyVideoPlayer::GetSpeed()
{
  return (float)m_speed;
}

void CDummyVideoPlayer::ShowOSD(bool bOnoff)
{
}

std::string CDummyVideoPlayer::GetPlayerState()
{
  return "";
}

bool CDummyVideoPlayer::SetPlayerState(std::string state)
{
  return true;
}

void CDummyVideoPlayer::Render()
{
  const CRect vw = g_graphicsContext.GetViewWindow();
#if 0
  D3DVIEWPORT8 newviewport;
  D3DVIEWPORT8 oldviewport;
  g_graphicsContext.Get3DDevice()->GetViewport(&oldviewport);
  newviewport.MinZ = 0.0f;
  newviewport.MaxZ = 1.0f;
  newviewport.X = (DWORD)vw.x1;
  newviewport.Y = (DWORD)vw.y1;
  newviewport.Width = (DWORD)vw.Width();
  newviewport.Height = (DWORD)vw.Height();
  g_graphicsContext.SetClipRegion(vw.x1, vw.y1, vw.Width(), vw.Height());
  CGUIFont *font = g_fontManager.GetFont("font13");
  if (font)
  {
    // minor issue: The font rendering here is performed in screen coords
    // so shouldn't really be scaled
    int mins = (int)(m_clock / 60000);
    int secs = (int)((m_clock / 1000) % 60);
    int ms = (int)(m_clock % 1000);
    std::string currentTime;
    currentTime.Format("Video goes here %02i:%02i:%03i", mins, secs, ms);
    float posX = (vw.x1 + vw.x2) * 0.5f;
    float posY = (vw.y1 + vw.y2) * 0.5f;
    CGUITextLayout::DrawText(font, posX, posY, 0xffffffff, 0, currentTime, XBFONT_CENTER_X | XBFONT_CENTER_Y);
  }
#endif
  g_graphicsContext.RestoreClipRegion();
}
