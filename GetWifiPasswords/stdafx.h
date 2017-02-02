// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>
// Common Standard Headers
#include <cstdio>
#include <cstdlib>
#include <map>
#include <set>
#include <new>
#include <queue>
#include <string>
#include <sstream>
#include <list> 

// Common Windows Headers

#define GDIPVER			0x0110
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <tchar.h>
#include <wchar.h>
#include <tlhelp32.h>
#include <userenv.h>
#include <wtsApi32.h>
#include <objidl.h>
#include <gdiplus.h>
#include <shlwapi.h>
#include <shlobj.h>
#include <dshow.h>
#include <wincred.h>
#include <WinCrypt.h>
#include <LMCons.h>
#include <Psapi.h>

// Common ATL Headers

#include <atlbase.h>
#include <atltime.h>
#include <atlstr.h>
#include <string.h>
#include <shellapi.h>

// Common misc Headers

#include "pstore.h"			// pstore.h is not a part of VS2013 and was added manually

#pragma comment(lib, "Psapi.lib")

#pragma comment(lib, "userenv.lib")
#pragma comment(lib, "secur32.lib")
#pragma comment(lib, "wtsApi32.lib")
#pragma comment(lib, "shlwapi.lib")
#pragma comment(lib, "shell32.lib")
#pragma comment(lib, "Strmiids.lib")




// TODO: reference additional headers your program requires here
