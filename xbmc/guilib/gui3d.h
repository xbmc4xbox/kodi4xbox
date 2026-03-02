/*
 *  Copyright (C) 2005-2018 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#pragma once

#include "PlatformDefs.h" // for TARGET_WINDOWS types

#define GAMMA_RAMP_FLAG  D3DSGR_CALIBRATE

#if _XBOX
#define D3DPRESENTFLAG_INTERLACED   1
#define D3DPRESENTFLAG_WIDESCREEN   2
#define D3DPRESENTFLAG_PROGRESSIVE  4
#define D3DPRESENTFLAG_MODE3DSBS    8
#define D3DPRESENTFLAG_MODE3DTB    16

/* what types are important for mode setting */
#define D3DPRESENTFLAG_MODEMASK ( D3DPRESENTFLAG_INTERLACED \
                                | D3DPRESENTFLAG_MODE3DSBS  \
                                | D3DPRESENTFLAG_MODE3DTB   )
#endif

#define D3DFMT_LIN_A8R8G8B8 D3DFMT_A8R8G8B8
#define D3DFMT_LIN_X8R8G8B8 D3DFMT_X8R8G8B8
#define D3DFMT_LIN_L8       D3DFMT_L8
#define D3DFMT_LIN_D16      D3DFMT_D16
#define D3DFMT_LIN_A8       D3DFMT_A8

#define D3DPIXELSHADERDEF DWORD

struct D3DTexture
{
  DWORD Common;
  DWORD Data;
  DWORD Lock;

  DWORD Format;   // Format information about the texture.
  DWORD Size;     // Size of a non power-of-2 texture, must be zero otherwise
};

#define D3DCOMMON_TYPE_MASK 0x0070000
#define D3DCOMMON_TYPE_TEXTURE 0x0040000

struct D3DPalette
{
  DWORD Common;
  DWORD Data;
  DWORD Lock;
};

typedef D3DPalette* LPDIRECT3DPALETTE8;

