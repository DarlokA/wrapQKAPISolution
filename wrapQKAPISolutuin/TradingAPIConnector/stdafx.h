// stdafx.h: включаемый файл дл€ стандартных системных включаемых файлов
// или включаемых файлов дл€ конкретного проекта, которые часто используютс€, но
// не часто измен€ютс€
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // »сключите редко используемые компоненты из заголовков Windows
// ‘айлы заголовков Windows:
#include <windows.h>


#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // некоторые конструкторы CString будут €вными

#include <atlbase.h>
#include <atlstr.h>

#include <string.h>  
#include <stdio.h>  
#include <stdlib.h>  
#include <stdexcept>

#include <COMDEF.H>
#include <comutil.h>

#include ".\..\Logger\Interface\LogApi.h"