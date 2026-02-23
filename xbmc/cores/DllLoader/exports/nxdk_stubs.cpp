#include <windows.h>

#include <iostream>
#include <math.h>
#include <string>
#include <sys/stat.h>

struct _timeb
{
  time_t time;
  unsigned short millitm;
  short timezone;
  short dstflag;
};

extern "C"
{

int __mb_cur_max = 1;

FILE _iob[20];

int _adjust_fdiv = 0;

int * _fmode;

char *_tzname[2] = { { "PST" }, { "PDT" } };
long _timezone = 28800;
int _daylight = 1;

double _HUGE;

char *_sys_errlist[44];

unsigned short ***_pctype;

void _assert(void *, void *, unsigned)
{
  assert(0);
}

int * _errno(void)
{
  assert(0);
  return nullptr;
}

void _ftime(struct _timeb *)
{
  assert(0);
}

int _isctype(int c, int mask)
{
  assert(0);
  return 0;
}

char * _itoa(int val, char *buf, int radix)
{
  assert(0);
  return nullptr;
}

void * _memccpy(void *dest, const void *src, int c, size_t count)
{
  assert(0);
  return nullptr;
}

size_t _msize(void *data)
{
  assert(0);
  return 0;
}

void _swab(char *src, char *dest, int nbytes)
{
  assert(0);
}

int strcmpi(const char *s1, const char *s2)
{
  return stricmp(s1, s2);
}

void _CIpow()
{
  assert(0);
}

int  swscanf(const char *string, const char *format, ...)
{
  assert(0);
  return 0;
}

long _ftol(float f)
{
  return static_cast<long>(f);
}

long _ftol2(float f)
{
  return _ftol(2);
}

int _isatty(int fh)
{
  assert(0);
  return 0;
}

__int64 _atoi64(const char *value)
{
  return static_cast<__int64>(atoi(value));
}

int _filbuf(FILE *str)
{
  assert(0);
  return 0;
}

int _flsbuf(int ch, FILE *str)
{
  assert(0);
  return 0;
}

intptr_t _get_osfhandle(int fh)
{
  assert(0);
  return -1;
}

int _wcsicmp(const wchar_t * dst, const wchar_t * src)
{
  return wcscmp(dst, src);
}

int _wcsnicmp(const wchar_t * first, const wchar_t * last, size_t count)
{
  return wcsncmp(first, last, count);
}

double _CIacos(double x)
{
  return acos(x);
}

double _CIasin(double x)
{
  return asin(x);
}

double _CIexp(double x)
{
  return exp(x);
}

double _CIcos(double x)
{
  return cos(x);
}

double _CIlog(double x)
{
  return log(x);
}

double _CIsin(double x)
{
  return sin(x);
}

double _CIatan2(double y, double x)
{
  return atan2(y, x);
}

double _CIsinh(double x)
{
  return sinh(x);
}

double _CIcosh(double x)
{
  return cosh(x);
}

double _isnan(double x)
{
  return isnan(x);
}

bool _finite(double x)
{
  return isfinite(x);
}

double _CIfmod(double x, double y)
{
  return fmod(x, y);
}

float _hypot(float x, float y)
{
  return hypot(x, y);
}

void __security_error_handler(int code, void *data)
{
  assert(0);
}

void _tzset()
{
  assert(0);
}

__int64 _strtoi64(const char *nptr, char **endptr, int ibase)
{
  return strtoll(nptr, endptr, ibase);
}

char *_tempnam(const char *dir, const char *prefix)
{
  assert(0);
  return nullptr;
}

void * _aligned_realloc(void *memblock, size_t size, size_t align, const char * f_name, int line_n)
{
  assert(0);
  return nullptr;
}

int _callnewh(size_t size)
{
  assert(0);
  return 0;
}

uintptr_t _beginthread (void (* initialcode) (void *), unsigned stacksize, void * argument)
{
  assert(0);
  return 0;
}

void _endthread()
{
  assert(0);
  ExitThread(0);
}

}
