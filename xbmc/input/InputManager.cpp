/*
 *  Copyright (C) 2005-2018 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#include "InputManager.h"

#include "ButtonTranslator.h"
#include "ServiceBroker.h"
#include "XBMC_vkeys.h"
#include "application/Application.h"
#include "application/ApplicationComponents.h"
#include "application/ApplicationPowerHandling.h"
#include "guilib/GUIAudioManager.h"
#include "guilib/GUIComponent.h"
#include "guilib/GUIControl.h"
#include "guilib/GUIWindow.h"
#include "guilib/GUIWindowManager.h"
#include "input/Key.h"
#include "messaging/ApplicationMessenger.h"
#include "settings/Settings.h"
#include "settings/SettingsComponent.h"
#include "settings/lib/Setting.h"
#include "utils/ExecString.h"
#include "utils/Geometry.h"
#include "utils/StringUtils.h"
#include "utils/log.h"

#include <algorithm>
#include <math.h>
#include <mutex>

using namespace KODI;

const std::string CInputManager::SETTING_INPUT_ENABLE_CONTROLLER = "input.enablejoystick";

CInputManager::CInputManager()
  : m_buttonTranslator(new CButtonTranslator)
{
  // Register settings
  std::set<std::string> settingSet;
  settingSet.insert(CSettings::SETTING_INPUT_ENABLEMOUSE);
  settingSet.insert(SETTING_INPUT_ENABLE_CONTROLLER);
  CServiceBroker::GetSettingsComponent()->GetSettings()->RegisterCallback(this, settingSet);
}

CInputManager::~CInputManager()
{
  Deinitialize();

  // Unregister settings
  CServiceBroker::GetSettingsComponent()->GetSettings()->UnregisterCallback(this);
}

void CInputManager::InitializeInputs()
{
  m_enableController = CServiceBroker::GetSettingsComponent()->GetSettings()->GetBool(
      SETTING_INPUT_ENABLE_CONTROLLER);
}

void CInputManager::Deinitialize()
{
}

bool CInputManager::ProcessPeripherals(float frameTime)
{
  CKey key;
  // TODO: read input from Xbox joypad and create appropriate CKey and pass it to OnKey method
  // For more info check CApplication::ProcessGamepad(...) from XBMC4Xbox
  // https://github.com/antonic901/xbmc4xbox-redux/blob/master/xbmc/Application.cpp#L2946
  return false;
}

void CInputManager::ProcessQueuedActions()
{
  std::vector<CAction> queuedActions;
  {
    std::unique_lock<CCriticalSection> lock(m_actionMutex);
    queuedActions.swap(m_queuedActions);
  }

  for (const CAction& action : queuedActions)
    g_application.OnAction(action);
}

void CInputManager::QueueAction(const CAction& action)
{
  std::unique_lock<CCriticalSection> lock(m_actionMutex);

  // Avoid dispatching multiple analog actions per frame with the same ID
  if (action.IsAnalog())
  {
    m_queuedActions.erase(std::remove_if(m_queuedActions.begin(), m_queuedActions.end(),
                                         [&action](const CAction& queuedAction) {
                                           return action.GetID() == queuedAction.GetID();
                                         }),
                          m_queuedActions.end());
  }

  m_queuedActions.push_back(action);
}

bool CInputManager::Process(int windowId, float frameTime)
{
  // process input actions
  ProcessPeripherals(frameTime);
  ProcessQueuedActions();

  return true;
}

// OnKey() translates the key into a CAction which is sent on to our Window Manager.
// The window manager will return true if the event is processed, false otherwise.
// If not already processed, this routine handles global keypresses.  It returns
// true if the key has been processed, false otherwise.

bool CInputManager::OnKey(const CKey& key)
{
  bool bHandled = false;

  if (bHandled)
  {
    m_LastKey.Reset();
  }
  else
  {
    if (key.GetButtonCode() == m_LastKey.GetButtonCode() &&
        (m_LastKey.GetButtonCode() & CKey::MODIFIER_LONG))
    {
      // Do not repeat long presses
    }
    else
    {
      // Event server keyboard doesn't give normal key up and key down, so don't
      // process for long press if that is the source
      if (key.GetFromService() ||
          !m_buttonTranslator->HasLongpressMapping(
              CServiceBroker::GetGUI()->GetWindowManager().GetActiveWindowOrDialog(), key))
      {
        m_LastKey.Reset();
        bHandled = HandleKey(key);
      }
      else
      {
        if (key.GetButtonCode() != m_LastKey.GetButtonCode() &&
            (key.GetButtonCode() & CKey::MODIFIER_LONG))
        {
          m_LastKey = key; // OnKey is reentrant; need to do this before entering
          bHandled = HandleKey(key);
        }

        m_LastKey = key;
      }
    }
  }

  return bHandled;
}

bool CInputManager::HandleKey(const CKey& key)
{
  // get the current active window
  int iWin = CServiceBroker::GetGUI()->GetWindowManager().GetActiveWindowOrDialog();

  // this will be checked for certain keycodes that need
  // special handling if the screensaver is active
  CAction action = m_buttonTranslator->GetAction(iWin, key);

  // a key has been pressed.
  // reset Idle Timer
  auto& components = CServiceBroker::GetAppComponents();
  const auto appPower = components.GetComponent<CApplicationPowerHandling>();
  appPower->ResetSystemIdleTimer();
  bool processKey = AlwaysProcess(action);

  if (StringUtils::StartsWithNoCase(action.GetName(), "CECToggleState") ||
      StringUtils::StartsWithNoCase(action.GetName(), "CECStandby"))
  {
    // do not wake up the screensaver right after switching off the playing device
    if (StringUtils::StartsWithNoCase(action.GetName(), "CECToggleState"))
    {
      CLog::LogF(LOGDEBUG, "action {} [{}], toggling state of playing device", action.GetName(),
                 action.GetID());
      bool result;
      CServiceBroker::GetAppMessenger()->SendMsg(TMSG_CECTOGGLESTATE, 0, 0,
                                                 static_cast<void*>(&result));
      if (!result)
        return true;
    }
    else
    {
      CServiceBroker::GetAppMessenger()->PostMsg(TMSG_CECSTANDBY);
      return true;
    }
  }

  appPower->ResetScreenSaver();

  // allow some keys to be processed while the screensaver is active
  if (appPower->WakeUpScreenSaverAndDPMS(processKey) && !processKey)
  {
    CLog::LogF(LOGDEBUG, "{} pressed, screen saver/dpms woken up",
               (int)key.GetButtonCode());
    return true;
  }

  if (iWin != WINDOW_FULLSCREEN_VIDEO && iWin != WINDOW_FULLSCREEN_GAME)
  {
    // current active window isnt the fullscreen window
    // just use corresponding section from keymap.xml
    // to map key->action

    // first determine if we should use keyboard input directly
    bool useKeyboard =
        key.FromKeyboard() && (iWin == WINDOW_DIALOG_KEYBOARD || iWin == WINDOW_DIALOG_NUMERIC);
    CGUIWindow* window = CServiceBroker::GetGUI()->GetWindowManager().GetWindow(iWin);
    if (window)
    {
      CGUIControl* control = window->GetFocusedControl();
      if (control)
      {
        // If this is an edit control set usekeyboard to true. This causes the
        // keypress to be processed directly not through the key mappings.
        if (control->GetControlType() == CGUIControl::GUICONTROL_EDIT)
          useKeyboard = true;

        // If the key pressed is shift-A to shift-Z set usekeyboard to true.
        // This causes the keypress to be used for list navigation.
        if (control->IsContainer() && key.GetModifiers() == CKey::MODIFIER_SHIFT &&
            key.GetUnicode())
          useKeyboard = true;
      }
    }
    if (useKeyboard)
    {
      // use the virtualkeyboard section of the keymap, and send keyboard-specific or navigation
      // actions through if that's what they are
      CAction action = m_buttonTranslator->GetAction(WINDOW_DIALOG_KEYBOARD, key);
      if (!(action.GetID() == ACTION_MOVE_LEFT || action.GetID() == ACTION_MOVE_RIGHT ||
            action.GetID() == ACTION_MOVE_UP || action.GetID() == ACTION_MOVE_DOWN ||
            action.GetID() == ACTION_SELECT_ITEM || action.GetID() == ACTION_ENTER ||
            action.GetID() == ACTION_PREVIOUS_MENU || action.GetID() == ACTION_NAV_BACK ||
            action.GetID() == ACTION_VOICE_RECOGNIZE))
      {
        // the action isn't plain navigation - check for a keyboard-specific keymap
        action = m_buttonTranslator->GetAction(WINDOW_DIALOG_KEYBOARD, key, false);
        if (!(action.GetID() >= REMOTE_0 && action.GetID() <= REMOTE_9) ||
            action.GetID() == ACTION_BACKSPACE || action.GetID() == ACTION_SHIFT ||
            action.GetID() == ACTION_SYMBOLS || action.GetID() == ACTION_CURSOR_LEFT ||
            action.GetID() == ACTION_CURSOR_RIGHT)
          action = CAction(0); // don't bother with this action
      }
      // else pass the keys through directly
      if (!action.GetID())
      {
        if (key.GetFromService())
          action = CAction(key.GetButtonCode() != KEY_INVALID ? key.GetButtonCode() : 0,
                           key.GetUnicode());
        else
        {
          // Check for paste keypress
#ifdef TARGET_WINDOWS
          // In Windows paste is ctrl-V
          if (key.GetVKey() == XBMCVK_V && key.GetModifiers() == CKey::MODIFIER_CTRL)
#elif defined(TARGET_LINUX)
          // In Linux paste is ctrl-V
          if (key.GetVKey() == XBMCVK_V && key.GetModifiers() == CKey::MODIFIER_CTRL)
#elif defined(TARGET_DARWIN_OSX)
          // In OSX paste is cmd-V
          if (key.GetVKey() == XBMCVK_V && key.GetModifiers() == CKey::MODIFIER_META)
#else
          // Placeholder for other operating systems
          if (false)
#endif
            action = CAction(ACTION_PASTE);
          // If the unicode is non-zero the keypress is a non-printing character
          else if (key.GetUnicode())
            action = CAction(KEY_UNICODE, key.GetUnicode());
          // The keypress is a non-printing character
          else
            action = CAction(key.GetVKey() | KEY_VKEY);
        }
      }

      CLog::LogF(LOGDEBUG, "{} pressed, trying keyboard action {:x}",
                 (int)key.GetButtonCode(), action.GetID());

      if (g_application.OnAction(action))
        return true;
      // failed to handle the keyboard action, drop down through to standard action
    }
    if (key.GetFromService())
    {
      if (key.GetButtonCode() != KEY_INVALID)
        action = m_buttonTranslator->GetAction(iWin, key);
    }
    else
      action = m_buttonTranslator->GetAction(iWin, key);
  }
  if (!key.IsAnalogButton())
    CLog::LogF(LOGDEBUG, "{} pressed, window {}, action is {}",
               (int)key.GetButtonCode(), iWin, action.GetName());

  return ExecuteInputAction(action);
}

void CInputManager::OnKeyUp(const CKey& key)
{
  if (m_LastKey.GetButtonCode() != KEY_INVALID &&
      !(m_LastKey.GetButtonCode() & CKey::MODIFIER_LONG))
  {
    CKey key = m_LastKey;
    m_LastKey.Reset(); // OnKey is reentrant; need to do this before entering
    HandleKey(key);
  }
  else
    m_LastKey.Reset();
}

bool CInputManager::AlwaysProcess(const CAction& action)
{
  // check if this button is mapped to a built-in function
  if (!action.GetName().empty())
  {
    const CExecString exec(action.GetName());
    if (exec.IsValid())
    {
      const std::string builtInFunction = exec.GetFunction();

      // should this button be handled normally or just cancel the screensaver?
      if (builtInFunction == "powerdown" || builtInFunction == "reboot" ||
          builtInFunction == "restart" || builtInFunction == "restartapp" ||
          builtInFunction == "suspend" || builtInFunction == "hibernate" ||
          builtInFunction == "quit" || builtInFunction == "shutdown" ||
          builtInFunction == "volumeup" || builtInFunction == "volumedown" ||
          builtInFunction == "mute" || builtInFunction == "RunAppleScript" ||
          builtInFunction == "RunAddon" || builtInFunction == "RunPlugin" ||
          builtInFunction == "RunScript" || builtInFunction == "System.Exec" ||
          builtInFunction == "System.ExecWait")
      {
        return true;
      }
    }
  }

  return false;
}

bool CInputManager::ExecuteInputAction(const CAction& action)
{
  bool bResult = false;
  CGUIComponent* gui = CServiceBroker::GetGUI();

  // play sound before the action unless the button is held,
  // where we execute after the action as held actions aren't fired every time.
  if (action.GetHoldTime())
  {
    bResult = g_application.OnAction(action);
    if (bResult && gui)
      gui->GetAudioManager().PlayActionSound(action);
  }
  else
  {
    if (gui)
      gui->GetAudioManager().PlayActionSound(action);

    bResult = g_application.OnAction(action);
  }
  return bResult;
}

bool CInputManager::HasBuiltin(const std::string& command)
{
  return false;
}

int CInputManager::ExecuteBuiltin(const std::string& execute,
                                  const std::vector<std::string>& params)
{
  return 0;
}

void CInputManager::SetMouseActive(bool active /* = true */)
{

}

void CInputManager::SetMouseEnabled(bool mouseEnabled /* = true */)
{

}

bool CInputManager::IsMouseActive()
{
  return false;
}

bool CInputManager::IsControllerEnabled() const
{
  return m_enableController;
}

void CInputManager::OnSettingChanged(const std::shared_ptr<const CSetting>& setting)
{
  if (setting == nullptr)
    return;

  const std::string& settingId = setting->GetId();
  if (settingId == SETTING_INPUT_ENABLE_CONTROLLER)
    m_enableController = std::dynamic_pointer_cast<const CSettingBool>(setting)->GetValue();
}

bool CInputManager::OnAction(const CAction& action)
{
  if (action.GetID() != ACTION_NONE)
  {
    if (action.IsAnalog())
    {
      QueueAction(action);
    }
    else
    {
      // If button was pressed this frame, send action
      if (action.GetHoldTime() == 0)
      {
        QueueAction(action);
      }
      else
      {
        // Only send repeated actions for basic navigation commands
        bool bIsNavigation = false;

        switch (action.GetID())
        {
          case ACTION_MOVE_LEFT:
          case ACTION_MOVE_RIGHT:
          case ACTION_MOVE_UP:
          case ACTION_MOVE_DOWN:
          case ACTION_PAGE_UP:
          case ACTION_PAGE_DOWN:
            bIsNavigation = true;
            break;

          default:
            break;
        }

        if (bIsNavigation)
          QueueAction(action);
      }
    }

    return true;
  }

  return false;
}

bool CInputManager::LoadKeymaps()
{
  bool bSuccess = false;

  if (m_buttonTranslator->Load())
  {
    bSuccess = true;
  }

  SetChanged();
  NotifyObservers(ObservableMessageButtonMapsChanged);

  return bSuccess;
}

bool CInputManager::ReloadKeymaps()
{
  return LoadKeymaps();
}

void CInputManager::ClearKeymaps()
{
  m_buttonTranslator->Clear();

  SetChanged();
  NotifyObservers(ObservableMessageButtonMapsChanged);
}

void CInputManager::AddKeymap(const std::string& keymap)
{
  if (m_buttonTranslator->AddDevice(keymap))
  {
    SetChanged();
    NotifyObservers(ObservableMessageButtonMapsChanged);
  }
}

void CInputManager::RemoveKeymap(const std::string& keymap)
{
  if (m_buttonTranslator->RemoveDevice(keymap))
  {
    SetChanged();
    NotifyObservers(ObservableMessageButtonMapsChanged);
  }
}

CAction CInputManager::GetAction(int window, const CKey& key, bool fallback /* = true */)
{
  return m_buttonTranslator->GetAction(window, key, fallback);
}
