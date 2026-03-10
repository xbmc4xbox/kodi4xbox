/*
 *  Copyright (C) 2005-2018 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#include "CPUInfoXbox.h"

#include <windows.h>
#include <xboxkrnl/xboxkrnl.h>

std::shared_ptr<CCPUInfo> CCPUInfo::GetCPUInfo()
{
  return std::make_shared<CCPUInfoXbox>();
}

CCPUInfoXbox::CCPUInfoXbox()
{

  m_cpuModel = "Intel Pentium 3";
  m_cpuCount = 1;
}

CCPUInfoXbox::~CCPUInfoXbox()
{
}

int CCPUInfoXbox::GetUsedPercentage()
{
  // TODO: calculate CPU usage
  return 0;
}

float CCPUInfoXbox::GetCPUFrequency()
{
  DWORD Twin_fsb, Twin_result;
  double Tcpu_fsb, Tcpu_result, Fcpu;

  Tcpu_fsb = __rdtsc();
  Twin_fsb = KeTickCount;

  Sleep(300);

  Tcpu_result = __rdtsc();
  Twin_result = KeTickCount;

  Fcpu  = (Tcpu_result-Tcpu_fsb);
  Fcpu /= (Twin_result-Twin_fsb);

  return Fcpu / 1000;
}

bool CCPUInfoXbox::GetTemperature(CTemperature& temperature)
{
  unsigned long temp;
  NTSTATUS scpu = HalReadSMBusValue(0x98, 0x01, FALSE, &temp);
  if (scpu != STATUS_SUCCESS)
  {
    // If it fails, its probably a 1.6. Read SMC instead
    HalReadSMBusValue(0x20, 0x09, FALSE, &temp);
  }
  temperature = CTemperature::CreateFromCelsius(static_cast<double>(temp));
  return true;
}
