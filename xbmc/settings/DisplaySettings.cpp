/*
 *  Copyright (C) 2013-2018 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#include "DisplaySettings.h"

#include "ServiceBroker.h"
#include "dialogs/GUIDialogFileBrowser.h"
#include "guilib/LocalizeStrings.h"
#include "messaging/helpers/DialogHelper.h"
#include "settings/AdvancedSettings.h"
#include "settings/Settings.h"
#include "settings/SettingsComponent.h"
#include "settings/lib/Setting.h"
#include "settings/lib/SettingDefinitions.h"
#include "storage/MediaManager.h"
#include "utils/StringUtils.h"
#include "utils/Variant.h"
#include "utils/XMLUtils.h"
#include "utils/log.h"
#include "guilib/GraphicContext.h"

#include <algorithm>
#include <cstdlib>
#include <float.h>
#include <mutex>
#include <string>
#include <utility>
#include <vector>

#ifdef TARGET_WINDOWS
#include "rendering/dx/DeviceResources.h"
#endif

using namespace KODI::MESSAGING;

using KODI::MESSAGING::HELPERS::DialogResponse;

// 0.1 second increments
#define MAX_REFRESH_CHANGE_DELAY 200

static RESOLUTION_INFO EmptyResolution;
static RESOLUTION_INFO EmptyModifiableResolution;

CDisplaySettings::CDisplaySettings()
{
  m_resolutions.resize(RES_CUSTOM);

  m_zoomAmount = 1.0f;
  m_pixelRatio = 1.0f;
  m_resolutionChangeAborted = false;
}

CDisplaySettings::~CDisplaySettings() = default;

CDisplaySettings& CDisplaySettings::GetInstance()
{
  static CDisplaySettings sDisplaySettings;
  return sDisplaySettings;
}

bool CDisplaySettings::Load(const TiXmlNode *settings)
{
  std::unique_lock<CCriticalSection> lock(m_critical);
  m_calibrations.clear();

  if (settings == NULL)
    return false;

  const TiXmlElement *pElement = settings->FirstChildElement("resolutions");
  if (!pElement)
  {
    CLog::Log(LOGERROR, "CDisplaySettings: settings file doesn't contain <resolutions>");
    return false;
  }

  const TiXmlElement *pResolution = pElement->FirstChildElement("resolution");
  while (pResolution)
  {
    // get the data for this calibration
    RESOLUTION_INFO cal;

    XMLUtils::GetString(pResolution, "description", cal.strMode);
    XMLUtils::GetInt(pResolution, "subtitles", cal.iSubtitles);
    XMLUtils::GetFloat(pResolution, "pixelratio", cal.fPixelRatio);

    const TiXmlElement *pOverscan = pResolution->FirstChildElement("overscan");
    if (pOverscan)
    {
      XMLUtils::GetInt(pOverscan, "left", cal.Overscan.left);
      XMLUtils::GetInt(pOverscan, "top", cal.Overscan.top);
      XMLUtils::GetInt(pOverscan, "right", cal.Overscan.right);
      XMLUtils::GetInt(pOverscan, "bottom", cal.Overscan.bottom);
    }

    // mark calibration as not updated
    // we must not delete those, resolution just might not be available
    cal.iWidth = cal.iHeight = 0;

    // store calibration, avoid adding duplicates
    bool found = false;
    for (ResolutionInfos::const_iterator  it = m_calibrations.begin(); it != m_calibrations.end(); ++it)
    {
      if (StringUtils::EqualsNoCase(it->strMode, cal.strMode))
      {
        found = true;
        break;
      }
    }
    if (!found)
      m_calibrations.push_back(cal);

    // iterate around
    pResolution = pResolution->NextSiblingElement("resolution");
  }

  ApplyCalibrations();
  return true;
}

bool CDisplaySettings::Save(TiXmlNode *settings) const
{
  if (settings == NULL)
    return false;

  std::unique_lock<CCriticalSection> lock(m_critical);
  TiXmlElement xmlRootElement("resolutions");
  TiXmlNode *pRoot = settings->InsertEndChild(xmlRootElement);
  if (pRoot == NULL)
    return false;

  // save calibrations
  for (ResolutionInfos::const_iterator it = m_calibrations.begin(); it != m_calibrations.end(); ++it)
  {
    // Write the resolution tag
    TiXmlElement resElement("resolution");
    TiXmlNode *pNode = pRoot->InsertEndChild(resElement);
    if (pNode == NULL)
      return false;

    // Now write each of the pieces of information we need...
    XMLUtils::SetString(pNode, "description", it->strMode);
    XMLUtils::SetInt(pNode, "subtitles", it->iSubtitles);
    XMLUtils::SetFloat(pNode, "pixelratio", it->fPixelRatio);

    // create the overscan child
    TiXmlElement overscanElement("overscan");
    TiXmlNode *pOverscanNode = pNode->InsertEndChild(overscanElement);
    if (pOverscanNode == NULL)
      return false;

    XMLUtils::SetInt(pOverscanNode, "left", it->Overscan.left);
    XMLUtils::SetInt(pOverscanNode, "top", it->Overscan.top);
    XMLUtils::SetInt(pOverscanNode, "right", it->Overscan.right);
    XMLUtils::SetInt(pOverscanNode, "bottom", it->Overscan.bottom);
  }

  return true;
}

void CDisplaySettings::Clear()
{
  std::unique_lock<CCriticalSection> lock(m_critical);
  m_calibrations.clear();
  m_resolutions.clear();
  m_resolutions.resize(RES_CUSTOM);

  m_zoomAmount = 1.0f;
  m_pixelRatio = 1.0f;
}

void CDisplaySettings::OnSettingAction(const std::shared_ptr<const CSetting>& setting)
{
  if (setting == NULL)
    return;
}

bool CDisplaySettings::OnSettingChanging(const std::shared_ptr<const CSetting>& setting)
{
  if (setting == NULL)
    return false;

  const std::string &settingId = setting->GetId();
  if (settingId == CSettings::SETTING_VIDEOSCREEN_RESOLUTION)
  {
    RESOLUTION oldRes = GetCurrentResolution();
    RESOLUTION newRes = (RESOLUTION)std::static_pointer_cast<const CSettingInt>(setting)->GetValue();

    SetCurrentResolution(newRes, false);
    g_graphicsContext.SetVideoResolution(newRes);

    // check if this setting is temporarily blocked from showing the dialog
    if (oldRes != newRes)
    {
      if (!m_resolutionChangeAborted)
      {
        if (HELPERS::ShowYesNoDialogText(CVariant{13110}, CVariant{13111}, CVariant{""},
                                         CVariant{""}, 15000) != DialogResponse::CHOICE_YES)
        {
          m_resolutionChangeAborted = true;
          return false;
        }
      }
      else
        m_resolutionChangeAborted = false;
    }
  }
#if 0 // XBOX SPECIFIC SETTINGS
  else if (settingId == "videoscreen.flickerfilter" || settingId == "videoscreen.soften")
    g_graphicsContext.SetVideoResolution(CDisplaySettings::Get().GetCurrentResolution(), TRUE);
  else if (StringUtils::StartsWith(settingId, "videooutput."))
  {
    if (settingId == "videooutput.aspect")
    {
      switch(((CSettingInt*)setting)->GetValue())
      {
      case VIDEO_NORMAL:
        g_videoConfig.SetNormal();
        break;
      case VIDEO_LETTERBOX:
        g_videoConfig.SetLetterbox(true);
        break;
      case VIDEO_WIDESCREEN:
        g_videoConfig.SetWidescreen(true);
        break;
      }
    }
    else if (settingId == "videooutput.hd480p")
      g_videoConfig.Set480p(((CSettingBool*)setting)->GetValue());
    else if (settingId == "videooutput.hd720p")
      g_videoConfig.Set720p(((CSettingBool*)setting)->GetValue());
    else if (settingId == "videooutput.hd1080i")
      g_videoConfig.Set1080i(((CSettingBool*)setting)->GetValue());

    if (g_videoConfig.NeedsSave())
      g_videoConfig.Save();
  }
#endif

  return true;
}

bool CDisplaySettings::OnSettingUpdate(const std::shared_ptr<CSetting>& setting,
                                       const char* oldSettingId,
                                       const TiXmlNode* oldSettingNode)
{
  if (setting == NULL)
    return false;

  const std::string &settingId = setting->GetId();
  if (settingId == CSettings::SETTING_VIDEOSCREEN_RESOLUTION)
  {
    std::shared_ptr<CSettingString> screenmodeSetting = std::static_pointer_cast<CSettingString>(setting);
    std::string screenmode = screenmodeSetting->GetValue();
    // in Eden there was no character ("i" or "p") indicating interlaced/progressive
    // at the end so we just add a "p" and assume progressive
    if (screenmode.size() == 20)
      return screenmodeSetting->SetValue(screenmode + "p");
  }

  return false;
}

void CDisplaySettings::OnSettingChanged(const std::shared_ptr<const CSetting>& setting)
{
  if (!setting)
    return;
}

void CDisplaySettings::SetCurrentResolution(RESOLUTION resolution, bool save /* = false */)
{
  if (save)
  { // Save videoscreen.resolution setting
    CServiceBroker::GetSettingsComponent()->GetSettings()->SetInt(CSettings::SETTING_VIDEOSCREEN_RESOLUTION, (int)resolution);
  }
  else if (resolution != m_currentResolution)
  {
    m_currentResolution = resolution;
    SetChanged();
  }
}

RESOLUTION CDisplaySettings::GetDisplayResolution() const
{
  return (RESOLUTION)CServiceBroker::GetSettingsComponent()->GetSettings()->GetInt(CSettings::SETTING_VIDEOSCREEN_RESOLUTION);
}

const RESOLUTION_INFO& CDisplaySettings::GetResolutionInfo(size_t index) const
{
  std::unique_lock<CCriticalSection> lock(m_critical);
  if (index >= m_resolutions.size())
    return EmptyResolution;

  return m_resolutions[index];
}

const RESOLUTION_INFO& CDisplaySettings::GetResolutionInfo(RESOLUTION resolution) const
{
  if (resolution <= RES_INVALID)
    return EmptyResolution;

  return GetResolutionInfo((size_t)resolution);
}

RESOLUTION_INFO& CDisplaySettings::GetResolutionInfo(size_t index)
{
  std::unique_lock<CCriticalSection> lock(m_critical);
  if (index >= m_resolutions.size())
  {
    EmptyModifiableResolution = RESOLUTION_INFO();
    return EmptyModifiableResolution;
  }

  return m_resolutions[index];
}

RESOLUTION_INFO& CDisplaySettings::GetResolutionInfo(RESOLUTION resolution)
{
  if (resolution <= RES_INVALID)
  {
    EmptyModifiableResolution = RESOLUTION_INFO();
    return EmptyModifiableResolution;
  }

  return GetResolutionInfo((size_t)resolution);
}

void CDisplaySettings::AddResolutionInfo(const RESOLUTION_INFO &resolution)
{
  std::unique_lock<CCriticalSection> lock(m_critical);
  RESOLUTION_INFO res(resolution);
  m_resolutions.push_back(res);
}

void CDisplaySettings::ApplyCalibrations()
{
  std::unique_lock<CCriticalSection> lock(m_critical);
  // apply all calibrations to the resolutions
  for (ResolutionInfos::const_iterator itCal = m_calibrations.begin(); itCal != m_calibrations.end(); ++itCal)
  {
    // find resolutions
    for (size_t res = RES_DESKTOP; res < m_resolutions.size(); ++res)
    {
      if (StringUtils::EqualsNoCase(itCal->strMode, m_resolutions[res].strMode))
      {
        // overscan
        m_resolutions[res].Overscan.left = itCal->Overscan.left;
        if (m_resolutions[res].Overscan.left < -m_resolutions[res].iWidth/4)
          m_resolutions[res].Overscan.left = -m_resolutions[res].iWidth/4;
        if (m_resolutions[res].Overscan.left > m_resolutions[res].iWidth/4)
          m_resolutions[res].Overscan.left = m_resolutions[res].iWidth/4;

        m_resolutions[res].Overscan.top = itCal->Overscan.top;
        if (m_resolutions[res].Overscan.top < -m_resolutions[res].iHeight/4)
          m_resolutions[res].Overscan.top = -m_resolutions[res].iHeight/4;
        if (m_resolutions[res].Overscan.top > m_resolutions[res].iHeight/4)
          m_resolutions[res].Overscan.top = m_resolutions[res].iHeight/4;

        m_resolutions[res].Overscan.right = itCal->Overscan.right;
        if (m_resolutions[res].Overscan.right < m_resolutions[res].iWidth / 2)
          m_resolutions[res].Overscan.right = m_resolutions[res].iWidth / 2;
        if (m_resolutions[res].Overscan.right > m_resolutions[res].iWidth * 3/2)
          m_resolutions[res].Overscan.right = m_resolutions[res].iWidth *3/2;

        m_resolutions[res].Overscan.bottom = itCal->Overscan.bottom;
        if (m_resolutions[res].Overscan.bottom < m_resolutions[res].iHeight / 2)
          m_resolutions[res].Overscan.bottom = m_resolutions[res].iHeight / 2;
        if (m_resolutions[res].Overscan.bottom > m_resolutions[res].iHeight * 3/2)
          m_resolutions[res].Overscan.bottom = m_resolutions[res].iHeight * 3/2;

        m_resolutions[res].iSubtitles = itCal->iSubtitles;
        if (m_resolutions[res].iSubtitles < 0)
          m_resolutions[res].iSubtitles = 0;
        if (m_resolutions[res].iSubtitles > m_resolutions[res].iHeight * 3 / 2)
          m_resolutions[res].iSubtitles = m_resolutions[res].iHeight * 3 / 2;

        m_resolutions[res].fPixelRatio = itCal->fPixelRatio;
        if (m_resolutions[res].fPixelRatio < 0.5f)
          m_resolutions[res].fPixelRatio = 0.5f;
        if (m_resolutions[res].fPixelRatio > 2.0f)
          m_resolutions[res].fPixelRatio = 2.0f;
        break;
      }
    }
  }
}

void CDisplaySettings::UpdateCalibrations()
{
  std::unique_lock<CCriticalSection> lock(m_critical);

  // Add new (unique) resolutions
  for (ResolutionInfos::const_iterator res(m_resolutions.cbegin()); res != m_resolutions.cend(); ++res)
    if (std::find_if(m_calibrations.cbegin(), m_calibrations.cend(),
      [&](const RESOLUTION_INFO& info) { return StringUtils::EqualsNoCase(res->strMode, info.strMode); }) == m_calibrations.cend())
        m_calibrations.push_back(*res);

  for (auto &cal : m_calibrations)
  {
    ResolutionInfos::const_iterator res(std::find_if(m_resolutions.cbegin(), m_resolutions.cend(),
    [&](const RESOLUTION_INFO& info) { return StringUtils::EqualsNoCase(cal.strMode, info.strMode); }));

    if (res != m_resolutions.cend())
    {
      //! @todo erase calibrations with default values
      cal = *res;
    }
  }
}

void CDisplaySettings::ClearCalibrations()
{
  std::unique_lock<CCriticalSection> lock(m_critical);
  m_calibrations.clear();
}

static inline bool ModeSort(const StringSettingOption& i, const StringSettingOption& j)
{
  return (i.value > j.value);
}

void CDisplaySettings::SettingOptionsResolutionsFiller(const SettingConstPtr& setting,
                                                       std::vector<IntegerSettingOption>& list,
                                                       int& current,
                                                       void* data)
{
  RESOLUTION res = RES_INVALID;

  std::vector<RESOLUTION> resolutions;
  g_graphicsContext.GetAllowedResolutions(resolutions);
  for (auto resolution : resolutions)
  {
    RESOLUTION_INFO res1 = CDisplaySettings::GetInstance().GetCurrentResolutionInfo();
    RESOLUTION_INFO res2 = CDisplaySettings::GetInstance().GetResolutionInfo(resolution);

    list.emplace_back(res2.strMode, resolution);

    if (
        res1.iWidth  == res2.iWidth &&
        res1.iHeight == res2.iHeight &&
        (res1.dwFlags & D3DPRESENTFLAG_INTERLACED) == (res2.dwFlags & D3DPRESENTFLAG_INTERLACED))
      res = resolution;
  }

  if (res != RES_INVALID)
    current = res;
}

void CDisplaySettings::ClearCustomResolutions()
{
  if (m_resolutions.size() > RES_CUSTOM)
  {
    std::vector<RESOLUTION_INFO>::iterator firstCustom = m_resolutions.begin()+RES_CUSTOM;
    m_resolutions.erase(firstCustom, m_resolutions.end());
  }
}
