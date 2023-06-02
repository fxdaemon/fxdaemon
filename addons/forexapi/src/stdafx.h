#ifndef STDAFX_H_230422_
#define STDAFX_H_230422_

#define TIMEOUT 30000

#include <algorithm>
#include <string>
#include <vector>
#include <map>
#include <queue>
#include <set>
#include <atomic>
#include <napi.h>

#ifdef WIN32

#include <windows.h>

#else

#ifndef FALSE
#define FALSE 0
#endif
#ifndef TRUE
#define TRUE 1
#endif
typedef long long INT64;
typedef unsigned long DWORD, *LPDWORD;
typedef int BOOL, *LPBOOL;
typedef unsigned char BYTE, *LPBYTE;
typedef unsigned short WORD, *LPWORD;
typedef int *LPINT;
typedef long LONG, *LPLONG, HRESULT;
typedef void *LPVOID, *HINTERNET, *HANDLE, *HMODULE;
typedef const void *LPCVOID;
typedef unsigned int UINT, *PUINT;
typedef char *LPSTR, *LPTSTR, _TCHAR;
typedef const char *LPCSTR, *LPCTSTR;
typedef wchar_t WCHAR, *PWCHAR, *LPWCH, *PWCH, *LPWSTR, *PWSTR;
typedef const WCHAR *LPCWCH, *PCWCH, *LPCWSTR, *PCWSTR;

#define SOCKET_ERROR (-1)
#define closesocket close
#define localtime_s(st, ltime) localtime_r(ltime, st)
#define gmtime_s(st, ltime) gmtime_r(ltime, st)
#define stricmp strcasecmp
#define strnicmp strncasecmp
#define sprintf_s snprintf
#define _I64_MAX 9223372036854775807

#include <stdarg.h>
#include <math.h>
#include <sys/time.h>
#include <pthread.h>
#include <unistd.h>
#include "./common/winevent.h"

#endif

#endif
