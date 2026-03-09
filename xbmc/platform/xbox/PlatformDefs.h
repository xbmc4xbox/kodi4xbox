/*
 *  Copyright (C) 2005-2018 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#pragma once

#include "../win32/PlatformDefs.h"

#include <sys/stat.h>

#define strcmpi stricmp
#define unlink remove

#define SOCKET int
#define INVALID_SOCKET -1
#define SOCKET_ERROR -1
