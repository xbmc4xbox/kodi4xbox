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
#include "emu_registry.h"

Export export_advapi32[] =
{
  { "RegCloseKey",                static_cast<unsigned long>(-1), dllRegCloseKey,                NULL },
  { "RegOpenKeyExA",              static_cast<unsigned long>(-1), dllRegOpenKeyExA,              NULL },
  { "RegOpenKeyA",                static_cast<unsigned long>(-1), dllRegOpenKeyA,                NULL },
  { "RegSetValueA",               static_cast<unsigned long>(-1), dllRegSetValueA,               NULL },
  { "RegEnumKeyExA",              static_cast<unsigned long>(-1), dllRegEnumKeyExA,              NULL },
  { "RegDeleteKeyA",              static_cast<unsigned long>(-1), dllRegDeleteKeyA,              NULL },
  { "RegQueryValueExA",           static_cast<unsigned long>(-1), dllRegQueryValueExA,           NULL },
  { "RegQueryValueExW",           static_cast<unsigned long>(-1), dllRegQueryValueExW,           NULL },
  { "RegCreateKeyA",              static_cast<unsigned long>(-1), dllRegCreateKeyA,              NULL },
  { "RegSetValueExA",             static_cast<unsigned long>(-1), dllRegSetValueExA,             NULL },
  { "RegCreateKeyExA",            static_cast<unsigned long>(-1), dllRegCreateKeyExA,            NULL },
  { "RegEnumValueA",              static_cast<unsigned long>(-1), dllRegEnumValueA,              NULL },
  { "RegQueryInfoKeyA",           static_cast<unsigned long>(-1), dllRegQueryInfoKeyA,           NULL },
  { "CryptAcquireContextA",       static_cast<unsigned long>(-1), dllCryptAcquireContextA,       NULL },
  { "CryptGenRandom",             static_cast<unsigned long>(-1), dllCryptGenRandom,             NULL },
  { "CryptReleaseContext",        static_cast<unsigned long>(-1), dllCryptReleaseContext,        NULL },
  { "RegQueryValueA",             static_cast<unsigned long>(-1), dllRegQueryValueA,             NULL },
  { NULL,                         static_cast<unsigned long>(-1), NULL,                          NULL }
};
