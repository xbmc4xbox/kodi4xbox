/*
 *  Copyright (C) 2023-2025 Team XBMC
 *  This file is part of XBMC - https://xbmc.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#include "application/AppEnvironment.h"
#include "application/AppParamParser.h"
#include "platform/xbmc.h"

#include <hal/video.h>

int main()
{
  XVideoSetMode(640, 480, 32, REFRESH_DEFAULT);

  char* argv[] = {"", "--fullscreen", "--standalone"};
  CAppParamParser appParamParser;
  appParamParser.Parse(argv, 3);

  CAppEnvironment::SetUp(appParamParser.GetAppParams());
  int status = XBMC_Run(true);
  CAppEnvironment::TearDown();

  return status;
}
