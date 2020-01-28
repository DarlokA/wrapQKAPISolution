// Logger.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "Interface/LogApi.h"
#include "LoggerStdOut.h"
#include "Logger2File.h"
#include "LoggerStdOutFile.h"

namespace Logger
{
	typedef std::shared_ptr<ILoggerObj> ILoggerPtr;
	static ILoggerPtr pLogger;

	LoggerApi void __cdecl Init(LPCWSTR root_folder, LoggerMode mode)
	{
		if (pLogger == NULL)
		{
			switch (mode)
			{
			case LMBoth:
				pLogger.reset(new CLoggerStdOutFile(root_folder));
				break;
			case LMFile:
				pLogger.reset(new CLogger2File(root_folder));
				break;
			case LMstdOut:
			default:
				pLogger.reset(new CLoggerStdOut());
				break;
			}
		}
		return;
	}

	
	LoggerApi ILoggerObj& __cdecl Instance()
	{
		if (pLogger == NULL)
		{
			MessageBox(NULL, L"StartLoging w/o params.\nPlease fix it. Insert Logger::Init(_T(module_name), params); before TRACE.", L"Attach", MB_OK);
			Init(L"");
		}
		return *pLogger;
	}

#define FORMAT_STR \
		CString res; \
		ATLASSERT(AtlIsValidString(pszFormat)); \
		va_list argList; \
		va_start(argList, pszFormat); \
		res.FormatV(pszFormat, argList); \
		va_end(argList)

	LoggerApi void __cdecl trace(_In_z_ _Printf_format_string_  LPCWSTR pszFormat, ...)
	{
		FORMAT_STR;
		Instance().trace(res);
	}

	LoggerApi void __cdecl tracel(_In_z_ _Printf_format_string_ LPCWSTR pszFormat, ...)
	{
		FORMAT_STR;
		Instance().trace_line(res);
	}

	LoggerApi void __cdecl trace_time(_In_z_ _Printf_format_string_  LPCWSTR pszFormat, ...)
	{
		FORMAT_STR;
		Instance().trace_time(res);
	}

	LoggerApi void __cdecl trace_end_line(_In_z_ _Printf_format_string_  LPCWSTR pszFormat, ...)
	{
		FORMAT_STR;
		Instance().trace_end_line(res);
	}


	LoggerApi void __cdecl assertfuncline(LPCWSTR pszExpression, LPCWSTR pszFile, LPCWSTR pszFunc, int Line)
	{
		Logger::tracel(L"Assertion failed(%s) in file %s at %s Line(%d)", pszExpression, pszFile, pszFunc, Line);
		wchar_t Message[1024] = L"";
		wsprintf(
			Message, L"Assertion failed:\n"
			L"%s\n"
			L"at %s Line(%d)\n"
			L"in file:\n"
			L"%s\n",
			pszExpression, pszFunc, Line, pszFile);
		::MessageBoxW(NULL, Message, L"Assertion failed", MB_ICONERROR);
	}
}
