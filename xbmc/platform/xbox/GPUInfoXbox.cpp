/*
 *  Copyright (C) 2023 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#include "GPUInfoXbox.h"

#include <windows.h>

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
  return true;
}

bool CGPUInfoXbox::GetGPUPlatformTemperature(CTemperature& temperature) const
{
  unsigned long temp;
  NTSTATUS smb = HalReadSMBusValue(0x98, 0x00, FALSE, &temp);
  if (smb != STATUS_SUCCESS)
  {
    // If it fails, its probably a 1.6. Read SMC instead
    HalReadSMBusValue(0x20, 0x0A, FALSE, &temp);
  }
  temperature = CTemperature::CreateFromCelsius(static_cast<double>(temp));
  return true;
}

bool CGPUInfoXbox::GetGPUTemperatureFromCommand(CTemperature& temperature,
                                                 const std::string& cmd) const
{
  return false;
}
