/*
 *  Copyright (C) 2023 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#include "GPUInfoXbox.h"

std::unique_ptr<CGPUInfo> CGPUInfo::GetGPUInfo()
{
  return std::make_unique<CGPUInfoXbox>();
}

bool CGPUInfoXbox::SupportsCustomTemperatureCommand() const
{
  return false;
}

bool CGPUInfoXbox::SupportsPlatformTemperature() const
{
  return false;
}

bool CGPUInfoXbox::GetGPUPlatformTemperature(CTemperature& temperature) const
{
  return false;
}

bool CGPUInfoXbox::GetGPUTemperatureFromCommand(CTemperature& temperature,
                                                 const std::string& cmd) const
{
  return false;
}
