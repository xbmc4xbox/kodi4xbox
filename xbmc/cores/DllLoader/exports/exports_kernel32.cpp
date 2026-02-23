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

#include "emu_kernel32.h"
#include "../dll_tracker_library.h"
#include "../dll_tracker_memory.h"
#include "../dll_tracker_critical_section.h"

Export export_kernel32[] =
{
  { "AddAtomA",                                     static_cast<unsigned long>(-1), dllAddAtomA,                                  NULL },
  { "FindAtomA",                                    static_cast<unsigned long>(-1), dllFindAtomA,                                 NULL },
  { "GetAtomNameA",                                 static_cast<unsigned long>(-1), dllGetAtomNameA,                              NULL },
  { "CreateThread",                                 static_cast<unsigned long>(-1), dllCreateThread,                              NULL },
  { "FindClose",                                    static_cast<unsigned long>(-1), dllFindClose,                                 NULL },
  { "FindFirstFileA",                               static_cast<unsigned long>(-1), FindFirstFileA,                               NULL },
  { "FindNextFileA",                                static_cast<unsigned long>(-1), FindNextFileA,                                NULL },
  { "GetFileAttributesA",                           static_cast<unsigned long>(-1), dllGetFileAttributesA,                        NULL },
  { "GetLastError",                                 static_cast<unsigned long>(-1), GetLastError,                                 NULL },
  { "SetUnhandledExceptionFilter",                  static_cast<unsigned long>(-1), dllSetUnhandledExceptionFilter,               NULL },
  { "Sleep",                                        static_cast<unsigned long>(-1), dllSleep,                                     NULL },
  { "SleepEx",                                      static_cast<unsigned long>(-1), SleepEx,                                      NULL },
  { "TerminateThread",                              static_cast<unsigned long>(-1), dllTerminateThread,                           NULL },
  { "GetCurrentThread",                             static_cast<unsigned long>(-1), dllGetCurrentThread,                          NULL },
  { "QueryPerformanceCounter",                      static_cast<unsigned long>(-1), QueryPerformanceCounter,                      NULL },
  { "QueryPerformanceFrequency",                    static_cast<unsigned long>(-1), QueryPerformanceFrequency,                    NULL },
  { "SetThreadPriority",                            static_cast<unsigned long>(-1), SetThreadPriority,                            NULL },
  { "GetTickCount",                                 static_cast<unsigned long>(-1), GetTickCount,                                 NULL },
  { "GetCurrentThreadId",                           static_cast<unsigned long>(-1), GetCurrentThreadId,                           NULL },
  { "GetCurrentProcessId",                          static_cast<unsigned long>(-1), dllGetCurrentProcessId,                       NULL },
  { "GetSystemTimeAsFileTime",                      static_cast<unsigned long>(-1), GetSystemTimeAsFileTime,                      NULL },
  { "OutputDebugStringA",                           static_cast<unsigned long>(-1), OutputDebugString,                            NULL },
  { "DisableThreadLibraryCalls",                    static_cast<unsigned long>(-1), dllDisableThreadLibraryCalls,                 NULL },
#if 0
  { "GlobalMemoryStatus",                           static_cast<unsigned long>(-1), GlobalMemoryStatus,                           NULL },
#endif
  { "CreateEventA",                                 static_cast<unsigned long>(-1), CreateEventA,                                 NULL },
  { "ResetEvent",                                   static_cast<unsigned long>(-1), ResetEvent,                                   NULL },
  { "WaitForSingleObject",                          static_cast<unsigned long>(-1), dllWaitForSingleObject,                       NULL },
  { "LoadLibraryA",                                 static_cast<unsigned long>(-1), dllLoadLibraryA,                              track_LoadLibraryA },
  { "FreeLibrary",                                  static_cast<unsigned long>(-1), dllFreeLibrary,                               track_FreeLibrary },
  { "GetProcAddress",                               static_cast<unsigned long>(-1), dllGetProcAddress,                            NULL },
  { "LeaveCriticalSection",                         static_cast<unsigned long>(-1), dllLeaveCriticalSection,                      NULL },
  { "EnterCriticalSection",                         static_cast<unsigned long>(-1), dllEnterCriticalSection,                      NULL },
  { "DeleteCriticalSection",                        static_cast<unsigned long>(-1), dllDeleteCriticalSection,                     track_DeleteCriticalSection },
  { "InitializeCriticalSection",                    static_cast<unsigned long>(-1), dllInitializeCriticalSection,                 track_InitializeCriticalSection },
  { "GetSystemInfo",                                static_cast<unsigned long>(-1), dllGetSystemInfo,                             NULL },
  { "CloseHandle",                                  static_cast<unsigned long>(-1), CloseHandle,                                  NULL },
  { "GetPrivateProfileIntA",                        static_cast<unsigned long>(-1), dllGetPrivateProfileIntA,                     NULL },
  { "WaitForMultipleObjects",                       static_cast<unsigned long>(-1), dllWaitForMultipleObjects,                    NULL },
  { "SetEvent",                                     static_cast<unsigned long>(-1), SetEvent,                                     NULL },
  { "TlsAlloc",                                     static_cast<unsigned long>(-1), dllTlsAlloc,                                  NULL },
  { "TlsFree",                                      static_cast<unsigned long>(-1), dllTlsFree,                                   NULL },
  { "TlsGetValue",                                  static_cast<unsigned long>(-1), dllTlsGetValue,                               NULL },
  { "TlsSetValue",                                  static_cast<unsigned long>(-1), dllTlsSetValue,                               NULL },
#if 0
  { "HeapFree",                                     static_cast<unsigned long>(-1), HeapFree,                                     NULL },
  { "HeapAlloc",                                    static_cast<unsigned long>(-1), HeapAlloc,                                    NULL },
  { "LocalFree",                                    static_cast<unsigned long>(-1), LocalFree,                                    NULL },
  { "LocalAlloc",                                   static_cast<unsigned long>(-1), LocalAlloc,                                   NULL },
  { "LocalReAlloc",                                 static_cast<unsigned long>(-1), LocalReAlloc,                                 NULL },
  { "LocalLock",                                    static_cast<unsigned long>(-1), LocalLock,                                    NULL },
  { "LocalUnlock",                                  static_cast<unsigned long>(-1), LocalUnlock,                                  NULL },
  { "LocalHandle",                                  static_cast<unsigned long>(-1), LocalHandle,                                  NULL },
#endif
  { "InterlockedIncrement",                         static_cast<unsigned long>(-1), InterlockedIncrement,                         NULL },
  { "InterlockedDecrement",                         static_cast<unsigned long>(-1), InterlockedDecrement,                         NULL },
  { "InterlockedExchange",                          static_cast<unsigned long>(-1), InterlockedExchange,                          NULL },
#if 0
  { "GetProcessHeap",                               static_cast<unsigned long>(-1), GetProcessHeap,                               NULL },
#endif
  { "GetModuleHandleA",                             static_cast<unsigned long>(-1), dllGetModuleHandleA,                          NULL },
  { "InterlockedCompareExchange",                   static_cast<unsigned long>(-1), InterlockedCompareExchange,                   NULL },
  { "GetVersionExA",                                static_cast<unsigned long>(-1), dllGetVersionExA,                             NULL },
  { "GetVersionExW",                                static_cast<unsigned long>(-1), dllGetVersionExW,                             NULL },
  { "GetProfileIntA",                               static_cast<unsigned long>(-1), dllGetProfileIntA,                            NULL },
  { "CreateFileA",                                  static_cast<unsigned long>(-1), dllCreateFileA,                               NULL },
#if 0
  { "DeviceIoControl",                              static_cast<unsigned long>(-1), DeviceIoControl,                              NULL },
#endif
  { "ReadFile",                                     static_cast<unsigned long>(-1), ReadFile,                                     NULL },
  { "dllDVDReadFile",                               static_cast<unsigned long>(-1), dllDVDReadFileLayerChangeHack,                NULL },
  { "SetFilePointer",                               static_cast<unsigned long>(-1), SetFilePointer,                               NULL },
#ifdef _XBOX
  { "xboxopendvdrom",                               static_cast<unsigned long>(-1), xboxopendvdrom,                               NULL },
#endif
  { "GetVersion",                                   static_cast<unsigned long>(-1), dllGetVersion,                                NULL },
#if 0
  { "MulDiv",                                       static_cast<unsigned long>(-1), MulDiv,                                       NULL },
  { "lstrlenA",                                     static_cast<unsigned long>(-1), lstrlenA,                                     NULL },
  { "lstrlenW",                                     static_cast<unsigned long>(-1), lstrlenW,                                     NULL },
#endif
  { "LoadLibraryExA",                               static_cast<unsigned long>(-1), dllLoadLibraryExA,                            track_LoadLibraryExA },

  { "DeleteFileA",                                  static_cast<unsigned long>(-1), DeleteFileA,                                  NULL },
  { "GetModuleFileNameA",                           static_cast<unsigned long>(-1), dllGetModuleFileNameA,                        NULL },
#if 0
  { "GlobalAlloc",                                  static_cast<unsigned long>(-1), GlobalAlloc,                                  NULL },
  { "GlobalLock",                                   static_cast<unsigned long>(-1), GlobalLock,                                   NULL },
  { "GlobalUnlock",                                 static_cast<unsigned long>(-1), GlobalUnlock,                                 NULL },
  { "GlobalHandle",                                 static_cast<unsigned long>(-1), GlobalHandle,                                 NULL },
  { "GlobalFree",                                   static_cast<unsigned long>(-1), GlobalFree,                                   NULL },
#endif
  { "FreeEnvironmentStringsW",                      static_cast<unsigned long>(-1), dllFreeEnvironmentStringsW,                   NULL },
  { "SetLastError",                                 static_cast<unsigned long>(-1), SetLastError,                                 NULL },
  { "RestoreLastError",                             static_cast<unsigned long>(-1), SetLastError,                                 NULL },
  { "GetOEMCP",                                     static_cast<unsigned long>(-1), dllGetOEMCP,                                  NULL },
  { "SetEndOfFile",                                 static_cast<unsigned long>(-1), SetEndOfFile,                                 NULL },
  { "RtlUnwind",                                    static_cast<unsigned long>(-1), dllRtlUnwind,                                 NULL },
  { "GetCommandLineA",                              static_cast<unsigned long>(-1), dllGetCommandLineA,                           NULL },
#if 0
  { "HeapReAlloc",                                  static_cast<unsigned long>(-1), HeapReAlloc,                                  NULL },
#endif
  { "ExitProcess",                                  static_cast<unsigned long>(-1), dllExitProcess,                               NULL },
  { "TerminateProcess",                             static_cast<unsigned long>(-1), dllTerminateProcess,                          NULL },
  { "GetCurrentProcess",                            static_cast<unsigned long>(-1), dllGetCurrentProcess,                         NULL },
#if 0
  { "HeapSize",                                     static_cast<unsigned long>(-1), HeapSize,                                     NULL },
#endif
  { "WriteFile",                                    static_cast<unsigned long>(-1), WriteFile,                                    NULL },
  { "GetACP",                                       static_cast<unsigned long>(-1), dllGetACP,                                    NULL },
  { "SetHandleCount",                               static_cast<unsigned long>(-1), dllSetHandleCount,                            NULL },
  { "GetStdHandle",                                 static_cast<unsigned long>(-1), dllGetStdHandle,                              NULL },
  { "GetFileType",                                  static_cast<unsigned long>(-1), dllGetFileType,                               NULL },
  { "GetStartupInfoA",                              static_cast<unsigned long>(-1), dllGetStartupInfoA,                           NULL },
  { "FreeEnvironmentStringsA",                      static_cast<unsigned long>(-1), dllFreeEnvironmentStringsA,                   NULL },
  { "WideCharToMultiByte",                          static_cast<unsigned long>(-1), dllWideCharToMultiByte,                       NULL },
  { "GetEnvironmentStrings",                        static_cast<unsigned long>(-1), dllGetEnvironmentStrings,                     NULL },
  { "GetEnvironmentStringsW",                       static_cast<unsigned long>(-1), dllGetEnvironmentStringsW,                    NULL },
  { "GetEnvironmentVariableA",                      static_cast<unsigned long>(-1), dllGetEnvironmentVariableA,                   NULL },
#if 0
  { "HeapDestroy",                                  static_cast<unsigned long>(-1), HeapDestroy,                                  track_HeapDestroy },
  { "HeapCreate",                                   static_cast<unsigned long>(-1), HeapCreate,                                   track_HeapCreate },
#endif
  { "VirtualFree",                                  static_cast<unsigned long>(-1), VirtualFree,                                  track_VirtualFree },
  { "VirtualFreeEx",                                static_cast<unsigned long>(-1), VirtualFreeEx,                                track_VirtualFreeEx },
  { "VirtualAlloc",                                 static_cast<unsigned long>(-1), VirtualAlloc,                                 track_VirtualAlloc },
  { "VirtualAllocEx",                               static_cast<unsigned long>(-1), VirtualAllocEx,                               track_VirtualAllocEx },
  { "MultiByteToWideChar",                          static_cast<unsigned long>(-1), dllMultiByteToWideChar,                       NULL },
  { "LCMapStringA",                                 static_cast<unsigned long>(-1), dllLCMapStringA,                              NULL },
  { "LCMapStringW",                                 static_cast<unsigned long>(-1), dllLCMapStringW,                              NULL },
  { "IsBadWritePtr",                                static_cast<unsigned long>(-1), IsBadWritePtr,                                NULL },
  { "SetStdHandle",                                 static_cast<unsigned long>(-1), dllSetStdHandle,                              NULL },
#if 0
  { "FlushFileBuffers",                             static_cast<unsigned long>(-1), FlushFileBuffers,                             NULL },
#endif
  { "GetStringTypeA",                               static_cast<unsigned long>(-1), dllGetStringTypeA,                            NULL },
  { "GetStringTypeW",                               static_cast<unsigned long>(-1), dllGetStringTypeW,                            NULL },
#if 0
  { "IsBadReadPtr",                                 static_cast<unsigned long>(-1), IsBadReadPtr,                                 NULL },
  { "IsBadCodePtr",                                 static_cast<unsigned long>(-1), IsBadCodePtr,                                 NULL },
#endif
  { "GetCPInfo",                                    static_cast<unsigned long>(-1), dllGetCPInfo,                                 NULL },

  { "CreateMutexA",                                 static_cast<unsigned long>(-1), CreateMutexA,                                 NULL },
  { "CreateSemaphoreA",                             static_cast<unsigned long>(-1), CreateSemaphore,                              NULL },
  { "PulseEvent",                                   static_cast<unsigned long>(-1), PulseEvent,                                   NULL },
  { "ReleaseMutex",                                 static_cast<unsigned long>(-1), ReleaseMutex,                                 NULL },
  { "ReleaseSemaphore",                             static_cast<unsigned long>(-1), ReleaseSemaphore,                             NULL },

  { "GetThreadLocale",                              static_cast<unsigned long>(-1), dllGetThreadLocale,                           NULL },
  { "SetPriorityClass",                             static_cast<unsigned long>(-1), dllSetPriorityClass,                          NULL },
  { "FormatMessageA",                               static_cast<unsigned long>(-1), dllFormatMessageA,                            NULL },
  { "GetFullPathNameA",                             static_cast<unsigned long>(-1), dllGetFullPathNameA,                          NULL },
  { "ExpandEnvironmentStringsA",                    static_cast<unsigned long>(-1), dllExpandEnvironmentStringsA,                 NULL },
#if 0
  { "GetVolumeInformationA",                        static_cast<unsigned long>(-1), GetVolumeInformationA,                        NULL },
#endif
  { "GetWindowsDirectoryA",                         static_cast<unsigned long>(-1), dllGetWindowsDirectoryA,                      NULL },
  { "GetSystemDirectoryA",                          static_cast<unsigned long>(-1), dllGetSystemDirectoryA,                       NULL },
  { "DuplicateHandle",                              static_cast<unsigned long>(-1), dllDuplicateHandle,                           NULL },
  { "GetShortPathNameA",                            static_cast<unsigned long>(-1), dllGetShortPathName,                          NULL },
  { "GetTempPathA",                                 static_cast<unsigned long>(-1), dllGetTempPathA,                              NULL },
  { "SetErrorMode",                                 static_cast<unsigned long>(-1), dllSetErrorMode,                              NULL },
  { "IsProcessorFeaturePresent",                    static_cast<unsigned long>(-1), dllIsProcessorFeaturePresent,                 NULL },
  { "FileTimeToLocalFileTime",                      static_cast<unsigned long>(-1), FileTimeToLocalFileTime,                      NULL },
  { "FileTimeToSystemTime",                         static_cast<unsigned long>(-1), FileTimeToSystemTime,                         NULL },
  { "GetTimeZoneInformation",                       static_cast<unsigned long>(-1), GetTimeZoneInformation,                       NULL },

  { "GetCurrentDirectoryA",                         static_cast<unsigned long>(-1), dllGetCurrentDirectoryA,                      NULL },
  { "SetCurrentDirectoryA",                         static_cast<unsigned long>(-1), dllSetCurrentDirectoryA,                      NULL },

  { "SetEnvironmentVariableA",                      static_cast<unsigned long>(-1), dllSetEnvironmentVariableA,                   NULL },
  { "CreateDirectoryA",                             static_cast<unsigned long>(-1), dllCreateDirectoryA,                          NULL },

  { "GetProcessAffinityMask",                       static_cast<unsigned long>(-1), dllGetProcessAffinityMask,                    NULL },

#if 0
  { "lstrcpyA",                                     static_cast<unsigned long>(-1), lstrcpyA,                                     NULL },
#endif
  { "GetProcessTimes",                              static_cast<unsigned long>(-1), dllGetProcessTimes,                           NULL },

  { "GetLocaleInfoA",                               static_cast<unsigned long>(-1), dllGetLocaleInfoA,                            NULL },
  { "GetConsoleCP",                                 static_cast<unsigned long>(-1), dllGetConsoleCP,                              NULL },
  { "GetConsoleOutputCP",                           static_cast<unsigned long>(-1), dllGetConsoleOutputCP,                        NULL },
  { "SetConsoleCtrlHandler",                        static_cast<unsigned long>(-1), dllSetConsoleCtrlHandler,                     NULL },
  { "GetExitCodeThread",                            static_cast<unsigned long>(-1), GetExitCodeThread,                            NULL },
  { "ResumeThread",                                 static_cast<unsigned long>(-1), ResumeThread,                                 NULL },
  { "ExitThread",                                   static_cast<unsigned long>(-1), ExitThread,                                   NULL },
  { "VirtualQuery",                                 static_cast<unsigned long>(-1), VirtualQuery,                                 NULL },
  { "VirtualQueryEx",                               static_cast<unsigned long>(-1), VirtualQueryEx,                               NULL },
#if 0
  { "VirtualProtect",                               static_cast<unsigned long>(-1), VirtualProtect,                               NULL },
  { "VirtualProtectEx",                             static_cast<unsigned long>(-1), VirtualProtectEx,                             NULL },
  { "UnhandledExceptionFilter",                     static_cast<unsigned long>(-1), UnhandledExceptionFilter,                     NULL },
#endif
  { "RaiseException",                               static_cast<unsigned long>(-1), RaiseException,                               NULL },
#if 0
  { "DebugBreak",                                   static_cast<unsigned long>(-1), DebugBreak,                                   NULL },
  { "GetThreadTimes",                               static_cast<unsigned long>(-1), GetThreadTimes,                               NULL },
#endif
  { "EncodePointer",                                static_cast<unsigned long>(-1), dllEncodePointer,                             NULL },
  { "DecodePointer",                                static_cast<unsigned long>(-1), dllDecodePointer,                             NULL },

  { "LockFile",                                     static_cast<unsigned long>(-1), dllLockFile,                                  NULL },
  { "LockFileEx",                                   static_cast<unsigned long>(-1), dllLockFileEx,                                NULL },
  { "UnlockFile",                                   static_cast<unsigned long>(-1), dllUnlockFile,                                NULL },

  { "GetSystemTime",                                static_cast<unsigned long>(-1), GetSystemTime,                                NULL },
  { "GetFileSize",                                  static_cast<unsigned long>(-1), GetFileSize,                                  NULL },
  { "FindResourceA",                                static_cast<unsigned long>(-1), dllFindResourceA,                             NULL },
  { "LoadResource",                                 static_cast<unsigned long>(-1), dllLoadResource,                              NULL },
  { NULL,                                         NULL, NULL,                                         NULL }
};
