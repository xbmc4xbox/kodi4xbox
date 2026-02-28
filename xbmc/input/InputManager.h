/*
 *  Copyright (C) 2005-2018 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#pragma once

#include "input/Key.h"
#include "input/actions/Action.h"
#include "interfaces/IActionListener.h"
#include "settings/lib/ISettingCallback.h"
#include "threads/CriticalSection.h"
#include "utils/Observer.h"

#include <map>
#include <memory>
#include <string>
#include <vector>

class CButtonTranslator;
class CKey;
class CProfileManager;

/// \addtogroup input
/// \{

/*!
 * \ingroup input keyboard mouse touch joystick
 * \brief Main input processing class.
 *
 * This class consolidates all input generated from different sources such as
 * mouse, keyboard, joystick or touch (in \ref OnEvent).
 *
 * \copydoc keyboard
 * \copydoc mouse
 */
class CInputManager : public ISettingCallback, public IActionListener, public Observable
{
public:
  CInputManager();
  CInputManager(const CInputManager&) = delete;
  CInputManager const& operator=(CInputManager const&) = delete;
  ~CInputManager() override;

  /*! \brief decode an event from peripherals.
   *
   * \param frameTime Time in seconds since last call
   * \return true if event is handled, false otherwise
   */
  bool ProcessPeripherals(float frameTime);

  /*! \brief Process all inputs
   *
   * \param windowId Currently active window
   * \param frameTime Time in seconds since last call
   * \return true on success, false otherwise
   */
  bool Process(int windowId, float frameTime);

  /*!
   * \brief Call once during application startup to initialize peripherals that need it
   */
  void InitializeInputs();

  /*!
   * \brief Deinitialize input and keymaps
   */
  void Deinitialize();

  /*! \brief Control if the mouse is actively used or not
   *
   * \param[in] active sets mouse active or inactive
   */
  void SetMouseActive(bool active = true);

  /*! \brief Control if we should use a mouse or not
   *
   * \param[in] mouseEnabled sets mouse enabled or disabled
   */
  void SetMouseEnabled(bool mouseEnabled = true);

  /*! \brief Check if the mouse is currently active
   *
   * \return true if active, false otherwise
   */
  bool IsMouseActive();

  /*! \brief Get the status of the controller-enable setting
   * \return True if controller input is enabled for the UI, false otherwise
   */
  bool IsControllerEnabled() const;

  /*! \brief Returns whether or not we can handle a given built-in command.
   *
   */
  bool HasBuiltin(const std::string& command);

  /*! \brief Parse a builtin command and execute any input action
   *  currently only LIRC commands implemented
   *
   * \param[in] execute Command to execute
   * \param[in] params  parameters that was passed to the command
   * \return 0 on success, -1 on failure
   */
  int ExecuteBuiltin(const std::string& execute, const std::vector<std::string>& params);

  // Button translation
  bool LoadKeymaps();
  bool ReloadKeymaps();
  void ClearKeymaps();
  void AddKeymap(const std::string& keymap);
  void RemoveKeymap(const std::string& keymap);

  /*! \brief Obtain the action configured for a given window and key
   *
   * \param window the window id
   * \param key the key to query the action for
   * \param fallback if no action is directly configured for the given window, obtain the action
   * from fallback window, if exists or from global config as last resort
   *
   * \return the action matching the key
   */
  CAction GetAction(int window, const CKey& key, bool fallback = true);

  /*!
   * \brief Queue an action to be processed on the next call to Process()
   */
  void QueueAction(const CAction& action);

  // implementation of ISettingCallback
  void OnSettingChanged(const std::shared_ptr<const CSetting>& setting) override;

  // implementation of IActionListener
  bool OnAction(const CAction& action) override;

private:
  /*! \brief Process keyboard event and translate into an action
   *
   * \param key keypress details
   * \return true on successfully handled event
   * \sa CKey
   */
  bool OnKey(const CKey& key);

  /*! \brief Process key up event
   *
   * \param key details of released key
   * \sa CKey
   */
  void OnKeyUp(const CKey& key);

  /*! \brief Handle keypress
   *
   * \param key keypress details
   * \return true on successfully handled event
   */
  bool HandleKey(const CKey& key);

  /*! \brief Determine if an action should be processed or just
   *   cancel the screensaver
   *
   * \param action Action that is about to be processed
   * \return true on any poweractions such as shutdown/reboot/sleep/suspend, false otherwise
   * \sa CAction
   */
  bool AlwaysProcess(const CAction& action);

  /*! \brief Send the Action to CApplication for further handling,
   *   play a sound before or after sending the action.
   *
   * \param action Action to send to CApplication
   * \return result from CApplication::OnAction
   * \sa CAction
   */
  bool ExecuteInputAction(const CAction& action);

  /*! \brief Dispatch actions queued since the last call to Process()
   */
  void ProcessQueuedActions();

  CKey m_LastKey;

  std::map<std::string, std::map<int, float>> m_lastAxisMap;

  std::vector<CAction> m_queuedActions;
  CCriticalSection m_actionMutex;

  // Button translation
  std::unique_ptr<CButtonTranslator> m_buttonTranslator;

  // Input state
  bool m_enableController = true;

  // Settings
  static const std::string SETTING_INPUT_ENABLE_CONTROLLER;
};

/// \}
