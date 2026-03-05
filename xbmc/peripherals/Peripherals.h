/*
 *  Copyright (C) 2005-2024 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#pragma once

#include "SDLJoystick.h"

class CKey;

namespace PERIPHERALS
{
/*!
 * \ingroup peripherals
 */
class CPeripherals
{
public:
  explicit CPeripherals();

  ~CPeripherals();

  /*!
   * @brief Initialise the peripherals manager.
   */
  void Initialise();

  /*!
   * @brief Clear all data known by the peripherals manager.
   */
  void Clear();

  /*!
   * @brief Try to get a keypress from a peripheral.
   * @param frameTime The current frametime.
   * @param key The fetched key.
   * @return True when a keypress was fetched, false otherwise.
   */
  bool GetNextKeypress(float frameTime, CKey& key);

  inline void UpdateJoystick(SDL_Event& joyEvent)
  {
    m_joystick.Update(joyEvent);
  }

private:
  CJoystick m_joystick;
};
} // namespace PERIPHERALS
