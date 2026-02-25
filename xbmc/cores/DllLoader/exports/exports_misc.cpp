/*
 *      Copyright (C) 2005-2013 Team XBMC
 *      http://xbmc.org
 *
 *  This Program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2, or (at your option)
 *  any later version.
 *
 *  This Program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with XBMC; see the file COPYING.  If not, see
 *  <http://www.gnu.org/licenses/>.
 *
 */

#include "emu_misc.h"

#include "../DllLoader.h"
#include "emu_msvcrt.h"

Export export_winmm[] =
{
#ifndef NXDK
  { "timeGetTime",                static_cast<unsigned long>(-1), timeGetTime,                   NULL },
#else
  { "timeGetTime",                static_cast<unsigned long>(-1), GetTickCount,                  NULL },
#endif
  { "DefDriverProc",              static_cast<unsigned long>(-1), dllDefDriverProc,              NULL },
  { "timeGetDevCaps",             static_cast<unsigned long>(-1), dlltimeGetDevCaps,             NULL },
  { "timeBeginPeriod",            static_cast<unsigned long>(-1), dlltimeBeginPeriod,            NULL },
  { "timeEndPeriod",              static_cast<unsigned long>(-1), dlltimeEndPeriod,              NULL },
  { "waveOutGetNumDevs",          static_cast<unsigned long>(-1), dllwaveOutGetNumDevs,          NULL },
  { NULL, NULL, NULL, NULL }
};

Export export_msdmo[] =
{
  { "MoFreeMediaType",            static_cast<unsigned long>(-1), dllMoFreeMediaType,            NULL },
  { "MoCopyMediaType",            static_cast<unsigned long>(-1), dllMoCopyMediaType,            NULL },
  { "MoInitMediaType",            static_cast<unsigned long>(-1), dllMoInitMediaType,            NULL },
  { NULL, NULL, NULL, NULL }
};

Export export_user32[] =
{
  { "IsRectEmpty",                static_cast<unsigned long>(-1), dllIsRectEmpty,                NULL },
  { "EnableWindow",               static_cast<unsigned long>(-1), dllEnableWindow,               NULL },
  { "GetDlgItemInt",              static_cast<unsigned long>(-1), dllGetDlgItemInt,              NULL },
  { "SendDlgItemMessageA",        static_cast<unsigned long>(-1), dllSendDlgItemMessageA,        NULL },
  { "DialogBoxParamA",            static_cast<unsigned long>(-1), dllDialogBoxParamA,            NULL },
  { "GetDlgItemTextA",            static_cast<unsigned long>(-1), dllGetDlgItemTextA,            NULL },
  { "MessageBoxA",                static_cast<unsigned long>(-1), dllMessageBoxA,                NULL },
  { "GetWindowLongA",             static_cast<unsigned long>(-1), dllGetWindowLongA,             NULL },
  { "GetDlgItem",                 static_cast<unsigned long>(-1), dllGetDlgItem,                 NULL },
  { "CheckDlgButton",             static_cast<unsigned long>(-1), dllCheckDlgButton,             NULL },
  { "SetDlgItemInt",              static_cast<unsigned long>(-1), dllSetDlgItemInt,              NULL },
  { "ShowWindow",                 static_cast<unsigned long>(-1), dllShowWindow,                 NULL },
  { "EndDialog",                  static_cast<unsigned long>(-1), dllEndDialog,                  NULL },
  { "SetDlgItemTextA",            static_cast<unsigned long>(-1), dllSetDlgItemTextA,            NULL },
  { "SetWindowLongA",             static_cast<unsigned long>(-1), dllSetWindowLongA,             NULL },
  { "DestroyWindow",              static_cast<unsigned long>(-1), dllDestroyWindow,              NULL },
  { "CreateDialogParamA",         static_cast<unsigned long>(-1), dllCreateDialogParamA,         NULL },
  { "PostMessageA",               static_cast<unsigned long>(-1), dllPostMessageA,               NULL },
  { "SendMessageA",               static_cast<unsigned long>(-1), dllSendMessageA,               NULL },
  { "SetFocus",                   static_cast<unsigned long>(-1), dllSetFocus,                   NULL },
  { "wsprintfA",                  static_cast<unsigned long>(-1), dllwsprintfA,                  NULL },

  { "GetDesktopWindow",           static_cast<unsigned long>(-1), dllGetDesktopWindow,           NULL },
  { "GetDC",                      static_cast<unsigned long>(-1), dllGetDC,                      NULL },
  { "ReleaseDC",                  static_cast<unsigned long>(-1), dllReleaseDC,                  NULL },
  { "GetWindowRect",              static_cast<unsigned long>(-1), dllGetWindowRect,              NULL },
  { "ShowCursor",                 static_cast<unsigned long>(-1), dllShowCursor,                 NULL },
  { "GetSystemMetrics",           static_cast<unsigned long>(-1), dllGetSystemMetrics,           NULL },
  { "MonitorFromWindow",          static_cast<unsigned long>(-1), dllMonitorFromWindow,          NULL },
  { "MonitorFromRect",            static_cast<unsigned long>(-1), dllMonitorFromRect,            NULL },
  { "MonitorFromPoint",           static_cast<unsigned long>(-1), dllMonitorFromPoint,           NULL },
  { "EnumDisplayMonitors",        static_cast<unsigned long>(-1), dllEnumDisplayMonitors,        NULL },
  { "GetMonitorInfoA",            static_cast<unsigned long>(-1), dllGetMonitorInfoA,            NULL },

  { "EnumDisplayDevicesA",        static_cast<unsigned long>(-1), dllEnumDisplayDevicesA,        NULL },
  { "IsWindowVisible",            static_cast<unsigned long>(-1), dllIsWindowVisible,            NULL },
  { "GetActiveWindow",            static_cast<unsigned long>(-1), dllGetActiveWindow,            NULL },
  { "LoadStringA",                static_cast<unsigned long>(-1), dllLoadStringA,                NULL },
  { "GetCursorPos",               static_cast<unsigned long>(-1), dllGetCursorPos,               NULL },
  { "LoadCursorA",                static_cast<unsigned long>(-1), dllLoadCursorA,                NULL },
  { "SetCursor",                  static_cast<unsigned long>(-1), dllSetCursor,                  NULL },
  { "RegisterWindowMessageA",     static_cast<unsigned long>(-1), dllRegisterWindowMessageA,     NULL },
  { "GetSysColorBrush",           static_cast<unsigned long>(-1), dllGetSysColorBrush,           NULL },
  { "GetSysColor",                static_cast<unsigned long>(-1), dllGetSysColor,                NULL },
  { "RegisterClipboardFormatA",   static_cast<unsigned long>(-1), dllRegisterClipboardFormatA,   NULL },
  { NULL, NULL, NULL, NULL }
};

Export export_xbmc_vobsub[] =
{
  { "pf_seek",                    static_cast<unsigned long>(-1), VobSubPFSeek,                  NULL },
  { "pf_write",                   static_cast<unsigned long>(-1), VobSubPFWrite,                 NULL },
  { "pf_read",                    static_cast<unsigned long>(-1), VobSubPFRead,                  NULL },
  { "pf_open",                    static_cast<unsigned long>(-1), VobSubPFOpen,                  NULL },
  { "pf_close",                   static_cast<unsigned long>(-1), VobSubPFClose,                 NULL },
  { "pf_reserve",                 static_cast<unsigned long>(-1), VobSubPFReserve,               NULL },
  { NULL, NULL, NULL, NULL }
};

Export export_version[] =
{
  { "GetFileVersionInfoSizeA",    static_cast<unsigned long>(-1), dllGetFileVersionInfoSizeA,    NULL },
  { "VerQueryValueA",             static_cast<unsigned long>(-1), dllVerQueryValueA,             NULL },
  { "GetFileVersionInfoA",        static_cast<unsigned long>(-1), dllGetFileVersionInfoA,        NULL },
  { NULL, NULL, NULL, NULL }
};

Export export_comdlg32[] =
{
  { "GetOpenFileNameA",static_cast<unsigned long>(-1), dllGetOpenFileNameA, NULL },
  { NULL, NULL, NULL, NULL }
};

Export export_gdi32[] =
{
  { "SetTextColor",               static_cast<unsigned long>(-1), dllSetTextColor,               NULL },
  { "BitBlt",                     static_cast<unsigned long>(-1), dllBitBlt,                     NULL },
  { "ExtTextOutA",                static_cast<unsigned long>(-1), dllExtTextOutA,                NULL },
  { "GetStockObject",             static_cast<unsigned long>(-1), dllGetStockObject,             NULL },
  { "SetBkColor",                 static_cast<unsigned long>(-1), dllSetBkColor,                 NULL },
  { "CreateCompatibleDC",         static_cast<unsigned long>(-1), dllCreateCompatibleDC,         NULL },
  { "CreateBitmap",               static_cast<unsigned long>(-1), dllCreateBitmap,               NULL },
  { "SelectObject",               static_cast<unsigned long>(-1), dllSelectObject,               NULL },
  { "CreateFontA",                static_cast<unsigned long>(-1), dllCreateFontA,                NULL },
  { "DeleteDC",                   static_cast<unsigned long>(-1), dllDeleteDC,                   NULL },
  { "SetBkMode",                  static_cast<unsigned long>(-1), dllSetBkMode,                  NULL },
  { "GetPixel",                   static_cast<unsigned long>(-1), dllGetPixel,                   NULL },
  { "DeleteObject",               static_cast<unsigned long>(-1), dllDeleteObject,               NULL },
  { "GetDeviceCaps",              static_cast<unsigned long>(-1), dllGetDeviceCaps,              NULL },
  { "CreatePalette",              static_cast<unsigned long>(-1), dllCreatePalette,              NULL },
  { NULL, NULL, NULL, NULL }
};

Export export_ddraw[] =
{
  { "DirectDrawCreate",static_cast<unsigned long>(-1), dllDirectDrawCreate, NULL },
  { NULL, NULL, NULL, NULL }
};

Export export_comctl32[] =
{
  { "CreateUpDownControl", 16, dllCreateUpDownControl, NULL },
//  { "InitCommonControls", 17, dllInitCommonControls, NULL },
  { NULL, NULL, NULL, NULL }
};

extern "C" void* libiconv();
extern "C" void* libiconv_close();
extern "C" void* libiconv_open();
#if 0
extern "C" void* libiconv_set_relocation_prefix();
#endif
extern "C" void* libiconvctl();
extern "C" void* libiconvlist();

Export export_iconvx[] =
{
  //{ "_libiconv_version",static_cast<unsigned long>(-1), &_libiconv_version, NULL },  // seems to be missing in our version
  { "libiconv",                   static_cast<unsigned long>(-1), libiconv,                      NULL },
  { "libiconv_close",             static_cast<unsigned long>(-1), libiconv_close,                NULL },
  { "libiconv_open",              static_cast<unsigned long>(-1), libiconv_open,                 NULL },
#if 0
  { "libiconv_set_relocation_prefix",static_cast<unsigned long>(-1), libiconv_set_relocation_prefix, NULL },
#endif
  { "libiconvctl",                static_cast<unsigned long>(-1), libiconvctl,                   NULL },
  { "libiconvlist",               static_cast<unsigned long>(-1), libiconvlist,                  NULL },
  { NULL, NULL, NULL, NULL }
};

extern "C" void* inflate();
extern "C" void* inflateEnd();
extern "C" void* inflateInit2_();
extern "C" void* inflateInit_();
extern "C" void* inflateSync();
extern "C" void* inflateReset();
extern "C" void* uncompress();

Export export_zlib[] =
{
  { "inflate",       static_cast<unsigned long>(-1), inflate,        NULL },
  { "inflateEnd",    static_cast<unsigned long>(-1), inflateEnd,     NULL },
  { "inflateInit2_", static_cast<unsigned long>(-1), inflateInit2_,  NULL },
  { "inflateInit_",  static_cast<unsigned long>(-1), inflateInit_,   NULL },
  { "inflateSync",   static_cast<unsigned long>(-1), inflateSync,    NULL },
  { "uncompress",    static_cast<unsigned long>(-1), uncompress,     NULL },
  { "inflateReset",  static_cast<unsigned long>(-1), inflateReset,   NULL },
  { NULL, NULL, NULL, NULL }
};
