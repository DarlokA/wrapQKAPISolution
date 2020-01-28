#pragma once
#include "TargetVer.h"

#include <stdio.h>
#include <tchar.h>


#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // some CString constructors will be explicit

#include <atlbase.h>
#include <atlstr.h>

#include <COMDEF.H>
#include <comutil.h>

#include <memory>

#include "..\Logger\Interface\LogApi.h"
#include "..\Communications\interface\CommunicationsAPI.h"
