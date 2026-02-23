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

#include "../DllLoader.h"
#include "emu_ole32.h"

Export export_ole32[] =
{
  { "CoInitialize",               static_cast<unsigned long>(-1), dllCoInitialize,               NULL },
  { "CoUninitialize",             static_cast<unsigned long>(-1), dllCoUninitialize,             NULL },
  { "CoCreateInstance",           static_cast<unsigned long>(-1), dllCoCreateInstance,           NULL },
  { "CoFreeUnusedLibraries",      static_cast<unsigned long>(-1), dllCoFreeUnusedLibraries,      NULL },
  { "StringFromGUID2",            static_cast<unsigned long>(-1), dllStringFromGUID2,            NULL },
  { "CoTaskMemFree",              static_cast<unsigned long>(-1), dllCoTaskMemFree,              NULL },
  { "CoTaskMemAlloc",             static_cast<unsigned long>(-1), dllCoTaskMemAlloc,             NULL },
  { NULL,                         static_cast<unsigned long>(-1), NULL,                          NULL }
};
