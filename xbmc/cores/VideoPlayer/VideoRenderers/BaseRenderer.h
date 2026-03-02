/*
 *  Copyright (C) 2005-2018 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#pragma once

#include "cores/IPlayer.h"
#include "utils/Geometry.h"

#include <utility>
#include <vector>

#define MAX_FIELDS 3
#define NUM_BUFFERS 6

class CSetting;
struct IntegerSettingOption;

enum EFIELDSYNC
{
  FS_NONE,
  FS_TOP,
  FS_BOT
};

// Render Methods
enum RenderMethods
{
  RENDER_METHOD_AUTO     = 0,
  RENDER_METHOD_GLSL,
  RENDER_METHOD_SOFTWARE,
  RENDER_METHOD_D3D_PS,
  RENDER_METHOD_DXVA,
  RENDER_OVERLAYS        = 99   // to retain compatibility
};

struct VideoPicture;
