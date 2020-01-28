// stdafx.h: ���������� ���� ��� ����������� ��������� ���������� ������
// ��� ���������� ������ ��� ����������� �������, ������� ����� ������������, ��
// �� ����� ����������
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // ��������� ����� ������������ ���������� �� ���������� Windows
// ����� ���������� Windows:
#include <windows.h>


#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // ��������� ������������ CString ����� ������

#include <atlbase.h>
#include <atlstr.h>

#include <string.h>  
#include <stdio.h>  
#include <stdlib.h>  
#include <stdexcept>

#include <COMDEF.H>
#include <comutil.h>

#include ".\..\Logger\Interface\LogApi.h"