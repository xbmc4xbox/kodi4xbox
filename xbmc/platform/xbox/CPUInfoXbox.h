/*
 *  Copyright (C) 2005-2018 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#pragma once

#include "utils/CPUInfo.h"
#include "utils/Temperature.h"

class CCPUInfoXbox : public CCPUInfo
{
public:
  CCPUInfoXbox();
  ~CCPUInfoXbox();

  int GetUsedPercentage() override;
  float GetCPUFrequency() override;
  bool GetTemperature(CTemperature& temperature) override;

  virtual bool SupportsCPUUsage() const { return false; }
};
