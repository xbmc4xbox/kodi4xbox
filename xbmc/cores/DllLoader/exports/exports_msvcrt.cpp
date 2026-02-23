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
//#include "emu_msvcrt.h"

#ifndef _WIN32
#include <math.h>
#endif

#ifndef __GNUC__
#pragma warning (disable:4391)
#pragma warning (disable:4392)
#endif

extern "C" void* dll_close();
extern "C" void* dll_lseek();
extern "C" void* dll_read();
extern "C" void* dll_write();
extern "C" void* dll__dllonexit();
extern "C" void* __mb_cur_max();
extern "C" void* _assert();
extern "C" void* _errno();
extern "C" void* _ftime();
extern "C" void* _iob();
extern "C" void* _isctype();
extern "C" void* dll_lseeki64();
extern "C" void* dll_open();
#ifndef NXDK
extern "C" void* _snprintf();
extern "C" void* _stricmp();
extern "C" void* _strnicmp();
extern "C" void* _vsnprintf();
#endif
extern "C" void* dllabort();
#ifndef NXDK
extern "C" void* atof();
extern "C" void* atoi();
extern "C" void* cos();
extern "C" void* cosh();
extern "C" void* exp();
#endif
extern "C" void* dll_fflush();
#ifndef NXDK
extern "C" void* floor();
#endif
extern "C" void* dll_fprintf();
extern "C" void* dllfree();
#ifndef NXDK
extern "C" void* frexp();
extern "C" void* gmtime();
extern "C" void* ldexp();
extern "C" void* localtime();
extern "C" void* log();
extern "C" void* log10();
#endif
extern "C" void* dllmalloc();
#ifndef NXDK
extern "C" void* memcpy();
extern "C" void* memmove();
extern "C" void* memset();
extern "C" void* mktime();
#endif
extern "C" void* dllperror();
extern "C" void* dllprintf();
extern "C" void* dllvprintf();
extern "C" void* dll_putchar();
extern "C" void* dllputs();
#ifndef NXDK
extern "C" void* qsort();
#endif
extern "C" void* dllrealloc();
#ifndef NXDK
extern "C" void* sin();
extern "C" void* sinh();
extern "C" void* sprintf();
extern "C" void* sqrt();
extern "C" void* sscanf();
extern "C" void* strchr();
extern "C" void* strcmp();
extern "C" void* strcpy();
extern "C" void* strlen();
extern "C" void* strncpy();
extern "C" void* strrchr();
extern "C" void* strtod();
extern "C" void* strtok();
extern "C" void* strtol();
extern "C" void* strtoul();
extern "C" void* tan();
extern "C" void* tanh();
extern "C" void* time();
extern "C" void* toupper();
#endif
extern "C" void* _memccpy();
extern "C" void* _msize();
extern "C" void* dll_fstat();
extern "C" void* dll_mkdir();
extern "C" void* dll_pclose();
extern "C" void* dll_popen();
extern "C" void* dll_sleep();
extern "C" void* dll_stat();
extern "C" void* dll_strdup();
extern "C" void* _swab();
extern "C" void* dll_findclose();
extern "C" void* dll_findfirst();
extern "C" void* dll_findnext();
extern "C" void* _fullpath();
extern "C" void* _pctype();
extern "C" void* dllcalloc();
#ifndef NXDK
extern "C" void* ceil();
extern "C" void* ctime();
#endif
extern "C" void* dllexit();
extern "C" void* dll_fclose();
extern "C" void* dll_feof();
extern "C" void* dll_fgets();
extern "C" void* dll_fopen();
extern "C" void* dll_fputc();
extern "C" void* dll_fputs();
extern "C" void* dll_fread();
extern "C" void* dll_fseek();
extern "C" void* dll_ftell();
extern "C" void* dll_getc();
extern "C" void* dll_getenv();
extern "C" void* dll_putc();
#ifndef NXDK
extern "C" void* rand();
extern "C" void* remove();
#endif
extern "C" void* dll_rewind();
#ifndef NXDK
extern "C" void* setlocale();
#endif
extern "C" void* dll_signal();
#ifndef NXDK
extern "C" void* srand();
extern "C" void* strcat();
extern "C" void* strcoll();
#endif
extern "C" void* dllstrerror();
#ifndef NXDK
extern "C" void* strncat();
extern "C" void* strncmp();
extern "C" void* strpbrk();
extern "C" void* strstr();
extern "C" void* tolower();
extern "C" void* acos();
extern "C" void* atan();
extern "C" void* memchr();
#endif
extern "C" void* dll_getc();
extern "C" void* _CIpow();
extern "C" int _purecall();
extern "C" void* _adjust_fdiv();
extern "C" void* dll_initterm();
extern "C" void* swscanf();
extern "C" void* dllfree();
#ifndef NXDK
extern "C" void* iswspace();
extern "C" void* wcscmp();
#endif
extern "C" void* dll_vfprintf();
#ifndef NXDK
extern "C" void* vsprintf();
#endif
extern "C" void* longjmp();
extern "C" void* _ftol();
#ifndef NXDK
extern "C" void* strspn();
extern "C" void* strcspn();
#endif
extern "C" void* dll_fgetpos();
extern "C" void* dll_fsetpos();
extern "C" void* dll_stati64();
extern "C" void* dll_fstati64();
extern "C" void* dll_telli64();
extern "C" void* dll_tell();
extern "C" void* dll_setmode();
extern "C" void* dll_beginthreadex();
extern "C" void* dll_endthreadex();
extern "C" void* dll_fileno();
extern "C" void* dll_getcwd();
extern "C" void* _isatty();
extern "C" void* dll_putenv();
extern "C" void* _atoi64();
extern "C" void* dll_ctype();
extern "C" void* _filbuf();
extern "C" void* _fmode();
#ifndef NXDK
extern "C" int _setjmp(int);
#else
extern "C" int setjmp(int);
#endif
#ifndef NXDK
extern "C" void* asin();
extern "C" void* atol();
extern "C" void* atol();
extern "C" void* bsearch();
#endif
extern "C" void* dll_ferror();
extern "C" void* dll_freopen();
#ifndef NXDK
extern "C" void* fscanf();
extern "C" void* localeconv();
#endif
extern "C" void* raise();
#ifndef NXDK
extern "C" void* setvbuf();
extern "C" void* strftime();
extern "C" void* strxfrm();
#endif
extern "C" void* dll_ungetc();
extern "C" void* dll_fdopen();
extern "C" void* dll_system();
extern "C" void* _flsbuf();
#ifndef NXDK
extern "C" void* isdigit();
extern "C" void* isalnum();
extern "C" void* isxdigit();
extern "C" void* pow();
#endif
extern "C" void* dll_onexit();
#ifndef NXDK
extern "C" void* modf();
#endif
extern "C" void* _get_osfhandle();
extern "C" void* _itoa();
#ifndef NXDK
extern "C" void* memcmp();
#endif
extern "C" void* _except_handler3();
extern "C" void* __CxxFrameHandler3();
#if 0
extern "C" void* __stdcall __CxxLongjmpUnwind(void*);
#endif
extern "C" void* __stdcall _CxxThrowException(void*, void*);
#ifndef NXDK
extern "C" void* abort();
extern "C" void* free();
extern "C" void* malloc();
extern "C" void* _strdup();
extern "C" void* exit();
extern "C" void* strerror();
#endif
extern "C" void* strcmpi();
#ifndef NXDK
extern "C" void* fabs();
#endif
extern "C" void* dllmalloc71();
extern "C" void* dllfree71();
#ifndef NXDK
extern "C" void* wcslen();
#endif
extern "C" void* _wcsicmp();
extern "C" void* _wcsnicmp();
extern "C" void* _CIacos();
extern "C" void* _CIasin();
extern "C" void* _CIcos();
extern "C" void* _CIsin();
extern "C" void* _CIlog();
extern "C" void* _CIexp();
extern "C" void* _CIatan2();
extern "C" void* dllfree71();
#ifndef NXDK
extern "C" void* isalpha();
#endif
#if 0
extern "C" void* _setjmp3();
#endif
#ifndef NXDK
extern "C" void* isprint();
extern "C" void* abs();
extern "C" void* labs();
extern "C" void* islower();
extern "C" void* isupper();
extern "C" void* wcscoll();
#endif
extern "C" void* _CIsinh();
extern "C" void* _CIcosh();
extern "C" void* _isnan();
extern "C" void* _finite();
extern "C" void* _CIfmod();
#ifndef NXDK
extern "C" void* atan2();
extern "C" void* fmod();
#endif
extern "C" void* _endthread();
extern "C" void* _beginthread();
#ifndef NXDK
extern "C" void* clock();
#endif
extern "C" void* _hypot();
#ifndef NXDK
extern "C" void* asctime();
#endif
extern "C" void* __security_error_handler();
#if 0
extern "C" void* __CppXcptFilter();
#endif
extern "C" void* _tzset();
extern "C" void* _tzname();
extern "C" void* _daylight();
extern "C" void* _timezone();
#ifndef NXDK
extern "C" void* _sys_nerr();
#endif
extern "C" void* _sys_errlist();
extern "C" void* dll_getpid();
extern "C" void* _HUGE();
#ifndef NXDK
extern "C" void* isspace();
#endif
extern "C" void* dll_fwrite();
#ifndef NXDK
extern "C" void* fsetpos();
#endif
extern "C" void* _strtoi64();
extern "C" void* dll_clearerr();
extern "C" void* dll__commit();
extern "C" void* dll___p__environ();
extern "C" void* _tempnam();
#ifndef NXDK
extern "C" void* _aligned_malloc();
extern "C" void* _aligned_free();
#endif
extern "C" void* _aligned_realloc();
extern "C" void* _callnewh();

// NXDK function with more then one declare
extern "C" void* dll_floor();
extern "C" void* dll_ldexp();
extern "C" void* dll_frexp();
extern "C" void* dll_cos();
extern "C" void* dll_cosh();
extern "C" void* dll_exp();
extern "C" void* dll_log();
extern "C" void* dll_log10();
extern "C" void* dll_sin();
extern "C" void* dll_sinh();
extern "C" void* dll_sqrt();
extern "C" void* dll_strchr();
extern "C" void* dll_strrchr();
extern "C" void* dll_tan();
extern "C" void* dll_tanh();
extern "C" void* dll_ceil();
extern "C" void* dll_strpbrk();
extern "C" void* dll_strstr();
extern "C" void* dll_acos();
extern "C" void* dll_atan();
extern "C" void* dll_memchr();
extern "C" void* dll_pow();
extern "C" void* dll_fabs();
extern "C" void* dll_asin();
extern "C" void* dll_abs();
extern "C" void* dll_modf();
extern "C" void* dll_atan2();
extern "C" void* dll_fmod();

// tracker functions
extern "C" void* track_close();
extern "C" void* track_open();
extern "C" void* track_free();
extern "C" void* track_malloc();
extern "C" void* track_realloc();
extern "C" void* track_strdup();
extern "C" void* track_calloc();
extern "C" void* track_fclose();
extern "C" void* track_fopen();
extern "C" void* track_freopen();

Export export_msvcrt[] =
{
  { "_close",                     static_cast<unsigned long>(-1), dll_close,                     track_close },
  { "_lseek",                     static_cast<unsigned long>(-1), dll_lseek,                     NULL },
  { "_read",                      static_cast<unsigned long>(-1), dll_read,                      NULL },
  { "_write",                     static_cast<unsigned long>(-1), dll_write,                     NULL },
  { "__dllonexit",                static_cast<unsigned long>(-1), dll__dllonexit,                NULL },
  { "__mb_cur_max",               static_cast<unsigned long>(-1), __mb_cur_max,                  NULL },
  { "_assert",                    static_cast<unsigned long>(-1), _assert,                       NULL },
  { "_errno",                     static_cast<unsigned long>(-1), _errno,                        NULL },
  { "_ftime",                     static_cast<unsigned long>(-1), _ftime,                        NULL },
  { "_iob",                       static_cast<unsigned long>(-1), _iob,                          NULL },
  { "_isctype",                   static_cast<unsigned long>(-1), _isctype,                      NULL },
  { "_lseeki64",                  static_cast<unsigned long>(-1), dll_lseeki64,                  NULL },
  { "_open",                      static_cast<unsigned long>(-1), dll_open,                      track_open },
  { "_snprintf",                  static_cast<unsigned long>(-1), _snprintf,                     NULL },
  { "_stricmp",                   static_cast<unsigned long>(-1), _stricmp,                      NULL },
  { "_strnicmp",                  static_cast<unsigned long>(-1), _strnicmp,                     NULL },
  { "_vsnprintf",                 static_cast<unsigned long>(-1), _vsnprintf,                    NULL },
  { "abort",                      static_cast<unsigned long>(-1), dllabort,                      NULL },
  { "atof",                       static_cast<unsigned long>(-1), atof,                          NULL },
  { "atoi",                       static_cast<unsigned long>(-1), atoi,                          NULL },
  { "_itoa",                      static_cast<unsigned long>(-1), _itoa,                         NULL },
  { "cos",                        static_cast<unsigned long>(-1), dll_cos,                       NULL },
  { "cosh",                       static_cast<unsigned long>(-1), dll_cosh,                      NULL },
  { "exp",                        static_cast<unsigned long>(-1), dll_exp,                       NULL },
  { "fflush",                     static_cast<unsigned long>(-1), dll_fflush,                    NULL },
  { "floor",                      static_cast<unsigned long>(-1), dll_floor,                     NULL },
  { "fprintf",                    static_cast<unsigned long>(-1), dll_fprintf,                   NULL },
  { "free",                       static_cast<unsigned long>(-1), free,                          track_free },
  { "frexp",                      static_cast<unsigned long>(-1), dll_frexp,                     NULL },
  { "fwrite",                     static_cast<unsigned long>(-1), dll_fwrite,                    NULL },
  { "gmtime",                     static_cast<unsigned long>(-1), gmtime,                        NULL },
  { "ldexp",                      static_cast<unsigned long>(-1), dll_ldexp,                     NULL },
  { "localtime",                  static_cast<unsigned long>(-1), localtime,                     NULL },
  { "log",                        static_cast<unsigned long>(-1), dll_log,                       NULL },
  { "log10",                      static_cast<unsigned long>(-1), dll_log10,                     NULL },
  { "malloc",                     static_cast<unsigned long>(-1), malloc,                        track_malloc },
  { "memcpy",                     static_cast<unsigned long>(-1), memcpy,                        NULL },
  { "memmove",                    static_cast<unsigned long>(-1), memmove,                       NULL },
  { "memset",                     static_cast<unsigned long>(-1), memset,                        NULL },
  { "mktime",                     static_cast<unsigned long>(-1), mktime,                        NULL },
  { "perror",                     static_cast<unsigned long>(-1), dllperror,                     NULL },
  { "printf",                     static_cast<unsigned long>(-1), dllprintf,                     NULL },
  { "vprintf",                    static_cast<unsigned long>(-1), dllvprintf,                    NULL },
  { "putchar",                    static_cast<unsigned long>(-1), dll_putchar,                   NULL },
  { "puts",                       static_cast<unsigned long>(-1), dllputs,                       NULL },
  { "qsort",                      static_cast<unsigned long>(-1), qsort,                         NULL },
  { "realloc",                    static_cast<unsigned long>(-1), dllrealloc,                    track_realloc },
  { "sin",                        static_cast<unsigned long>(-1), dll_sin,                       NULL },
  { "sinh",                       static_cast<unsigned long>(-1), dll_sinh,                      NULL },
  { "sprintf",                    static_cast<unsigned long>(-1), sprintf,                       NULL },
  { "sqrt",                       static_cast<unsigned long>(-1), dll_sqrt,                      NULL },
  { "sscanf",                     static_cast<unsigned long>(-1), sscanf,                        NULL },
  { "strchr",                     static_cast<unsigned long>(-1), dll_strchr,                    NULL },
  { "strcmp",                     static_cast<unsigned long>(-1), strcmp,                        NULL },
  { "strcpy",                     static_cast<unsigned long>(-1), strcpy,                        NULL },
  { "strlen",                     static_cast<unsigned long>(-1), strlen,                        NULL },
  { "strncpy",                    static_cast<unsigned long>(-1), strncpy,                       NULL },
  { "strrchr",                    static_cast<unsigned long>(-1), dll_strrchr,                   NULL },
  { "strtod",                     static_cast<unsigned long>(-1), strtod,                        NULL },
  { "strtok",                     static_cast<unsigned long>(-1), strtok,                        NULL },
  { "strtol",                     static_cast<unsigned long>(-1), strtol,                        NULL },
  { "strtoul",                    static_cast<unsigned long>(-1), strtoul,                       NULL },
  { "tan",                        static_cast<unsigned long>(-1), dll_tan,                       NULL },
  { "tanh",                       static_cast<unsigned long>(-1), dll_tanh,                      NULL },
  { "time",                       static_cast<unsigned long>(-1), time,                          NULL },
  { "toupper",                    static_cast<unsigned long>(-1), toupper,                       NULL },
  { "_memccpy",                   static_cast<unsigned long>(-1), _memccpy,                      NULL },
  { "_fstat",                     static_cast<unsigned long>(-1), dll_fstat,                     NULL },
  { "_mkdir",                     static_cast<unsigned long>(-1), dll_mkdir,                     NULL },
  { "_msize",                     static_cast<unsigned long>(-1), _msize,                        NULL },
  { "_pclose",                    static_cast<unsigned long>(-1), dll_pclose,                    NULL },
  { "_popen",                     static_cast<unsigned long>(-1), dll_popen,                     NULL },
  { "_sleep",                     static_cast<unsigned long>(-1), dll_sleep,                     NULL },
  { "_stat",                      static_cast<unsigned long>(-1), dll_stat,                      NULL },
  { "_strdup",                    static_cast<unsigned long>(-1), _strdup,                       track_strdup },
  { "_swab",                      static_cast<unsigned long>(-1), _swab,                         NULL },
  { "_findclose",                 static_cast<unsigned long>(-1), dll_findclose,                 NULL },
  { "_findfirst",                 static_cast<unsigned long>(-1), dll_findfirst,                 NULL },
  { "_findnext",                  static_cast<unsigned long>(-1), dll_findnext,                  NULL },
  { "_fullpath",                  static_cast<unsigned long>(-1), _fullpath,                     NULL },
  { "_pctype",                    static_cast<unsigned long>(-1), _pctype,                       NULL },
  { "calloc",                     static_cast<unsigned long>(-1), dllcalloc,                     track_calloc },
  { "ceil",                       static_cast<unsigned long>(-1), dll_ceil,                      NULL },
  { "ctime",                      static_cast<unsigned long>(-1), ctime,                         NULL },
  { "exit",                       static_cast<unsigned long>(-1), dllexit,                       NULL },
  { "fclose",                     static_cast<unsigned long>(-1), dll_fclose,                    track_fclose },
  { "feof",                       static_cast<unsigned long>(-1), dll_feof,                      NULL },
  { "fgets",                      static_cast<unsigned long>(-1), dll_fgets,                     NULL },
  { "fopen",                      static_cast<unsigned long>(-1), dll_fopen,                     track_fopen },
  { "fgetc",                      static_cast<unsigned long>(-1), dll_getc,                      NULL },
  { "putc",                       static_cast<unsigned long>(-1), dll_putc,                      NULL },
  { "fputc",                      static_cast<unsigned long>(-1), dll_fputc,                     NULL },
  { "fputs",                      static_cast<unsigned long>(-1), dll_fputs,                     NULL },
  { "fread",                      static_cast<unsigned long>(-1), dll_fread,                     NULL },
  { "fseek",                      static_cast<unsigned long>(-1), dll_fseek,                     NULL },
  { "ftell",                      static_cast<unsigned long>(-1), dll_ftell,                     NULL },
  { "getc",                       static_cast<unsigned long>(-1), dll_getc,                      NULL },
  { "getenv",                     static_cast<unsigned long>(-1), dll_getenv,                    NULL },
  { "rand",                       static_cast<unsigned long>(-1), rand,                          NULL },
  { "remove",                     static_cast<unsigned long>(-1), remove,                        NULL },
  { "rewind",                     static_cast<unsigned long>(-1), dll_rewind,                    NULL },
  { "setlocale",                  static_cast<unsigned long>(-1), setlocale,                     NULL },
  { "signal",                     static_cast<unsigned long>(-1), dll_signal,                    NULL },
  { "srand",                      static_cast<unsigned long>(-1), srand,                         NULL },
  { "strcat",                     static_cast<unsigned long>(-1), strcat,                        NULL },
  { "strcoll",                    static_cast<unsigned long>(-1), strcoll,                       NULL },
  { "strerror",                   static_cast<unsigned long>(-1), strerror,                      NULL },
  { "strncat",                    static_cast<unsigned long>(-1), strncat,                       NULL },
  { "strncmp",                    static_cast<unsigned long>(-1), strncmp,                       NULL },
  { "strpbrk",                    static_cast<unsigned long>(-1), dll_strpbrk,                   NULL },
  { "strstr",                     static_cast<unsigned long>(-1), dll_strstr,                    NULL },
  { "tolower",                    static_cast<unsigned long>(-1), tolower,                       NULL },
  { "acos",                       static_cast<unsigned long>(-1), dll_acos,                      NULL },
  { "atan",                       static_cast<unsigned long>(-1), dll_atan,                      NULL },
  { "memchr",                     static_cast<unsigned long>(-1), dll_memchr,                    NULL },
  { "isdigit",                    static_cast<unsigned long>(-1), isdigit,                       NULL },
  { "_strcmpi",                   static_cast<unsigned long>(-1), strcmpi,                       NULL },
  { "_CIpow",                     static_cast<unsigned long>(-1), _CIpow,                        NULL },
  { "_adjust_fdiv",               static_cast<unsigned long>(-1), _adjust_fdiv,                  NULL },
  { "pow",                        static_cast<unsigned long>(-1), dll_pow,                       NULL },
  { "fabs",                       static_cast<unsigned long>(-1), dll_fabs,                      NULL },
  { "swscanf",                    static_cast<unsigned long>(-1), swscanf,                       NULL },
  { "??2@YAPAXI@Z",               static_cast<unsigned long>(-1), dllmalloc,                     track_malloc },
  { "??3@YAXPAX@Z",               static_cast<unsigned long>(-1), dllfree,                       track_free },
#if 0
  { "??_U@YAPAXI@Z",              static_cast<unsigned long>(-1), (void*)(operator new),         NULL },
#endif
  { "iswspace",                   static_cast<unsigned long>(-1), iswspace,                      NULL },
  { "wcscmp",                     static_cast<unsigned long>(-1), wcscmp,                        NULL },
  { "_ftol",                      static_cast<unsigned long>(-1), _ftol,                         NULL },
  { "_telli64",                   static_cast<unsigned long>(-1), dll_telli64,                   NULL },
  { "_tell",                      static_cast<unsigned long>(-1), dll_tell,                      NULL },
  { "_setmode",                   static_cast<unsigned long>(-1), dll_setmode,                   NULL },
  { "_beginthreadex",             static_cast<unsigned long>(-1), dll_beginthreadex,             NULL },
  { "_endthreadex",               static_cast<unsigned long>(-1), dll_endthreadex,               NULL },
  { "_fdopen",                    static_cast<unsigned long>(-1), dll_fdopen,                    NULL },
  { "_fileno",                    static_cast<unsigned long>(-1), dll_fileno,                    NULL },
  { "_getcwd",                    static_cast<unsigned long>(-1), dll_getcwd,                    NULL },
  { "_isatty",                    static_cast<unsigned long>(-1), _isatty,                       NULL },
  { "_putenv",                    static_cast<unsigned long>(-1), dll_putenv,                    NULL },
  { "_atoi64",                    static_cast<unsigned long>(-1), _atoi64,                       NULL },
  { "_ctype",                     static_cast<unsigned long>(-1), dll_ctype,                     NULL },
  { "_filbuf",                    static_cast<unsigned long>(-1), _filbuf,                       NULL },
  { "_fmode",                     static_cast<unsigned long>(-1), _fmode,                        NULL },
  { "_setjmp",                    static_cast<unsigned long>(-1), setjmp,                       NULL },
  { "asin",                       static_cast<unsigned long>(-1), dll_asin,                      NULL },
  { "atol",                       static_cast<unsigned long>(-1), atol,                          NULL },
  { "atol",                       static_cast<unsigned long>(-1), atol,                          NULL },
  { "bsearch",                    static_cast<unsigned long>(-1), bsearch,                       NULL },
  { "ferror",                     static_cast<unsigned long>(-1), dll_ferror,                    NULL },
  { "freopen",                    static_cast<unsigned long>(-1), dll_freopen,                   track_freopen},
  { "fscanf",                     static_cast<unsigned long>(-1), fscanf,                        NULL },
  { "localeconv",                 static_cast<unsigned long>(-1), localeconv,                    NULL },
  { "raise",                      static_cast<unsigned long>(-1), raise,                         NULL },
  { "setvbuf",                    static_cast<unsigned long>(-1), setvbuf,                       NULL },
  { "strftime",                   static_cast<unsigned long>(-1), strftime,                      NULL },
  { "strxfrm",                    static_cast<unsigned long>(-1), strxfrm,                       NULL },
  { "ungetc",                     static_cast<unsigned long>(-1), dll_ungetc,                    NULL },
  { "system",                     static_cast<unsigned long>(-1), dll_system,                    NULL },
  { "_flsbuf",                    static_cast<unsigned long>(-1), _flsbuf,                       NULL },
  { "_get_osfhandle",             static_cast<unsigned long>(-1), _get_osfhandle,                NULL },
  { "strspn",                     static_cast<unsigned long>(-1), strspn,                        NULL },
  { "strcspn",                    static_cast<unsigned long>(-1), strcspn,                       NULL },
  { "wcslen",                     static_cast<unsigned long>(-1), wcslen,                        NULL },
  { "_wcsicmp",                   static_cast<unsigned long>(-1), _wcsicmp,                      NULL },
  { "fgetpos",                    static_cast<unsigned long>(-1), dll_fgetpos,                   NULL },
  { "_wcsnicmp",                  static_cast<unsigned long>(-1), _wcsnicmp,                     NULL },
  { "_CIacos",                    static_cast<unsigned long>(-1), _CIacos,                       NULL },
  { "_CIasin",                    static_cast<unsigned long>(-1), _CIasin,                       NULL },
  { "_CIexp",                     static_cast<unsigned long>(-1), _CIexp,                        NULL },
  { "_CIcos",                     static_cast<unsigned long>(-1), _CIcos,                        NULL },
  { "_CIlog",                     static_cast<unsigned long>(-1), _CIlog,                        NULL },
  { "_CIsin",                     static_cast<unsigned long>(-1), _CIsin,                        NULL },
  { "_CIatan2",                   static_cast<unsigned long>(-1), _CIatan2,                      NULL },
  { "??_V@YAXPAX@Z",              static_cast<unsigned long>(-1), dllfree,                       track_free},
  { "isalpha",                    static_cast<unsigned long>(-1), isalpha,                       NULL },
  { "_CxxThrowException",         static_cast<unsigned long>(-1), _CxxThrowException,            NULL },
  { "__CxxFrameHandler",          static_cast<unsigned long>(-1), __CxxFrameHandler3,             NULL },
#if 0
  { "__CxxLongjmpUnwind",         static_cast<unsigned long>(-1), __CxxLongjmpUnwind,            NULL },
#endif
  { "memcmp",                     static_cast<unsigned long>(-1), memcmp,                        NULL },
  { "fsetpos",                    static_cast<unsigned long>(-1), dll_fsetpos,                   NULL },
  { "_setjmp3",                   static_cast<unsigned long>(-1), setjmp,                        NULL },
  { "longjmp",                    static_cast<unsigned long>(-1), longjmp,                       NULL },
  { "isprint",                    static_cast<unsigned long>(-1), isprint,                       NULL },
  { "vsprintf",                   static_cast<unsigned long>(-1), vsprintf,                      NULL },
  { "abs",                        static_cast<unsigned long>(-1), dll_abs,                       NULL },
  { "labs",                       static_cast<unsigned long>(-1), labs,                          NULL },
  { "islower",                    static_cast<unsigned long>(-1), islower,                       NULL },
  { "isupper",                    static_cast<unsigned long>(-1), isupper,                       NULL },
  { "wcscoll",                    static_cast<unsigned long>(-1), wcscoll,                       NULL },
  { "_CIsinh",                    static_cast<unsigned long>(-1), _CIsinh,                       NULL },
  { "_CIcosh",                    static_cast<unsigned long>(-1), _CIcosh,                       NULL },
  { "modf",                       static_cast<unsigned long>(-1), dll_modf,                      NULL },
  { "_isnan",                     static_cast<unsigned long>(-1), _isnan,                        NULL },
  { "_finite",                    static_cast<unsigned long>(-1), _finite,                       NULL },
  { "_CIfmod",                    static_cast<unsigned long>(-1), _CIfmod,                       NULL },
  { "atan2",                      static_cast<unsigned long>(-1), dll_atan2,                     NULL },
  { "fmod",                       static_cast<unsigned long>(-1), dll_fmod,                      NULL },
  { "isxdigit",                   static_cast<unsigned long>(-1), isxdigit,                      NULL },
  { "_endthread",                 static_cast<unsigned long>(-1), _endthread,                    NULL },
  { "_beginthread",               static_cast<unsigned long>(-1), _beginthread,                  NULL },
  { "clock",                      static_cast<unsigned long>(-1), clock,                         NULL },
  { "_hypot",                     static_cast<unsigned long>(-1), _hypot,                        NULL },
  { "_except_handler3",           static_cast<unsigned long>(-1), _except_handler3,              NULL },
  { "asctime",                    static_cast<unsigned long>(-1), asctime,                       NULL },
  { "__security_error_handler",   static_cast<unsigned long>(-1), __security_error_handler,      NULL },
#if 0
  { "__CppXcptFilter",            static_cast<unsigned long>(-1), __CppXcptFilter,               NULL },
#endif
  { "_tzset",                     static_cast<unsigned long>(-1), _tzset,                        NULL },
  { "_tzname",                    static_cast<unsigned long>(-1), &_tzname,                      NULL },
  { "_daylight",                  static_cast<unsigned long>(-1), &_daylight,                    NULL },
  { "_timezone",                  static_cast<unsigned long>(-1), &_timezone,                    NULL },
#if 0
  { "_sys_nerr",                  static_cast<unsigned long>(-1), &_sys_nerr,                    NULL },
#endif
  { "_sys_errlist",               static_cast<unsigned long>(-1), &_sys_errlist,                 NULL },
  { "_getpid",                    static_cast<unsigned long>(-1), dll_getpid,                    NULL },
  { "_exit",                      static_cast<unsigned long>(-1), dllexit,                       NULL },
  { "_onexit",                    static_cast<unsigned long>(-1), dll_onexit,                    NULL },
  { "_HUGE",                      static_cast<unsigned long>(-1), _HUGE,                         NULL },
  { "_initterm",                  static_cast<unsigned long>(-1), dll_initterm,                  NULL },
  { "_purecall",                  static_cast<unsigned long>(-1), _purecall,                     NULL },
  { "isalnum",                    static_cast<unsigned long>(-1), isalnum,                       NULL },
  { "isspace",                    static_cast<unsigned long>(-1), isspace,                       NULL },
  { "_stati64",                   static_cast<unsigned long>(-1), dll_stati64,                   NULL },
  { "_fstati64",                  static_cast<unsigned long>(-1), dll_fstati64,                  NULL },
  { "_strtoi64",                  static_cast<unsigned long>(-1), _strtoi64,                     NULL },
  { "clearerr",                   static_cast<unsigned long>(-1), dll_clearerr,                  NULL },
  { "_commit",                    static_cast<unsigned long>(-1), dll__commit,                   NULL },
  { "__p__environ",               static_cast<unsigned long>(-1), dll___p__environ,              NULL },
  { "vfprintf",                   static_cast<unsigned long>(-1), dll_vfprintf,                  NULL },
  { "_tempnam",                   static_cast<unsigned long>(-1), _tempnam,                      NULL },
  { "_aligned_malloc",            static_cast<unsigned long>(-1), _aligned_malloc,               NULL },
  { "_aligned_free",              static_cast<unsigned long>(-1), _aligned_free,                 NULL },
  { "_aligned_realloc",           static_cast<unsigned long>(-1), _aligned_realloc,              NULL },
  { "_callnewh",                  static_cast<unsigned long>(-1), _callnewh,                     NULL },
  { NULL,                         static_cast<unsigned long>(-1), NULL,                          NULL }
};

Export export_pncrt[] =
{
  { "malloc",                     static_cast<unsigned long>(-1), malloc,                        track_malloc },
  { "??3@YAXPAX@Z",               static_cast<unsigned long>(-1), free,                          track_free },
  { "memmove",                    static_cast<unsigned long>(-1), memmove,                       NULL },
  { "_purecall",                  static_cast<unsigned long>(-1), _purecall,                     NULL },
  { "_ftol",                      static_cast<unsigned long>(-1), _ftol,                         NULL },
  { "_CIpow",                     static_cast<unsigned long>(-1), _CIpow,                        NULL },
  { "??2@YAPAXI@Z",               static_cast<unsigned long>(-1), malloc,                        track_malloc },
  { "free",                       static_cast<unsigned long>(-1), free,                          track_free },
  { "_initterm",                  static_cast<unsigned long>(-1), dll_initterm,                  NULL },
  { "_adjust_fdiv",               static_cast<unsigned long>(-1), &_adjust_fdiv,                 NULL },
  { "_beginthreadex",             static_cast<unsigned long>(-1), dll_beginthreadex,             NULL },
  { "_endthreadex",               static_cast<unsigned long>(-1), dll_endthreadex,               NULL },
  { "_iob",                       static_cast<unsigned long>(-1), &_iob,                         NULL },
  { "fprintf",                    static_cast<unsigned long>(-1), dll_fprintf,                   NULL },
  { "floor",                      static_cast<unsigned long>(-1), dll_floor,                     NULL },
  { "_assert",                    static_cast<unsigned long>(-1), _assert,                       NULL },
  { "__dllonexit",                static_cast<unsigned long>(-1), dll__dllonexit,                NULL },
  { "calloc",                     static_cast<unsigned long>(-1), dllcalloc,                     track_calloc },
  { "strncpy",                    static_cast<unsigned long>(-1), strncpy,                       NULL },
  { "ldexp",                      static_cast<unsigned long>(-1), dll_ldexp,                     NULL },
  { "frexp",                      static_cast<unsigned long>(-1), dll_frexp,                     NULL },
  { "rand",                       static_cast<unsigned long>(-1), rand,                          NULL },
  { NULL,                         static_cast<unsigned long>(-1), NULL,                          NULL }
};
