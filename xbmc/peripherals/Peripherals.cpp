/*
 *  Copyright (C) 2005-2024 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#include "Peripherals.h"

#include "input/keyboard/Key.h"

using namespace PERIPHERALS;

CPeripherals::CPeripherals()
{
  m_joystick.SetEnabled(true);
}

CPeripherals::~CPeripherals()
{
  Clear();
}

void CPeripherals::Initialise()
{
  Clear();
}

void CPeripherals::Clear()
{
}

bool CPeripherals::GetNextKeypress(float frameTime, CKey& key)
{
  m_joystick.Update();
  uint32_t buttonCode = m_joystick.TranslateButton();
  if (buttonCode)
  {
    CKey newKey(buttonCode, static_cast<unsigned int>(0));
    key = newKey;
    return true;
  }
  return false;
}
