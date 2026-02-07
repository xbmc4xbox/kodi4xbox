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

#include <nxdk/mount.h>
#include <nxdk/path.h>

int main()
{
  nxUnmountDrive('D');
  nxMountDrive('D', "\\Device\\CdRom0");
  nxMountDrive('C', "\\Device\\Harddisk0\\Partition2\\");
  nxMountDrive('E', "\\Device\\Harddisk0\\Partition1\\");
  nxMountDrive('X', "\\Device\\Harddisk0\\Partition3\\");
  nxMountDrive('Y', "\\Device\\Harddisk0\\Partition4\\");
  nxMountDrive('Z', "\\Device\\Harddisk0\\Partition5\\");
  nxMountDrive('F', "\\Device\\Harddisk0\\Partition6\\");
  nxMountDrive('Q', "\\Device\\Harddisk0\\Partition1\\UDATA\\FFXXFFXX\\");

  char* argv[] = {"", "--fullscreen", "--standalone"};
  CAppParamParser appParamParser;
  appParamParser.Parse(argv, 3);

  CAppEnvironment::SetUp(appParamParser.GetAppParams());
  int status = XBMC_Run(true);
  CAppEnvironment::TearDown();

  return status;
}
