#pragma once

namespace Logger
{
	interface ILoggerObj
	{
		virtual ~ILoggerObj() {};
		virtual void trace(const WCHAR *pszMsg) = 0;
		virtual void trace_line(const WCHAR *pszMsg) = 0;
		virtual void trace_time(const WCHAR *pszMsg) = 0;
		virtual void trace_end_line(const WCHAR *pszMsg) = 0;
	};
}