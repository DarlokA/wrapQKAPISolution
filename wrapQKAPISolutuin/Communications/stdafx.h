// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>


#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // some CString constructors will be explicit

#include <atlbase.h>
#include <atlstr.h>

#include <COMDEF.H>
#include <comutil.h>
#include <thread>
#pragma comment( lib, "comsuppw.lib" )

#include "..\Logger\Interface\LogApi.h"


//#define COMMUNICATOR_DLL
// TODO: reference additional headers your program requires here
