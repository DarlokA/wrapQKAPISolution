// TradingAPIConnector.cpp: определяет экспортированные функции для приложения DLL.
//

#include "stdafx.h"
#include "QuikAPIConnector.h"

TradingAPIConnector::ITradingConnector* __cdecl  APIConnector()
{
	TRACE(L"CreateConnector()");
	return TradingAPIConnector::CQuikAPIConnector::Instace();
}

bool	__cdecl RunProcess(_bstr_t path, _bstr_t cmd_line)
{
	return TradingAPIConnector::CQuikAPIConnector::RunProcess(path, cmd_line);
}

_bstr_t	__cdecl GetMyRunDir()
{
	WCHAR name[MAX_PATH];
	GetModuleFileNameW(NULL, name, MAX_PATH);
	_bstr_t res = name;
	return TradingAPIConnector::CQuikAPIConnector::extract_path(res);
}

