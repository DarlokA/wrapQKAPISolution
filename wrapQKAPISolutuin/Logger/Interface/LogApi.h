//Logger API version 1.0 (Copyright DarlokA @2016)
#pragma once
#ifndef LoggerApi
#define LoggerApi __declspec(dllimport)
#endif


namespace Logger
{
	enum LoggerApi LoggerMode{ LMstdOut = 0x1, LMFile = 0x2, LMBoth = 0x3, };
	LoggerApi void __cdecl Init(LPCWSTR root_folder, LoggerMode mode = LMstdOut);
	LoggerApi void __cdecl trace_time(_In_z_ _Printf_format_string_  LPCWSTR pszFormat, ...);
	LoggerApi void __cdecl trace_end_line(_In_z_ _Printf_format_string_  LPCWSTR pszFormat, ...);
	LoggerApi void __cdecl trace(_In_z_ _Printf_format_string_  LPCWSTR pszFormat, ...);
	LoggerApi void __cdecl tracel(_In_z_ _Printf_format_string_  LPCWSTR pszFormat, ...);
	LoggerApi void __cdecl assertfuncline(LPCWSTR pszExpression, LPCWSTR pszFile, LPCWSTR pszFunc, int Line);
}

#define TRACE(...) Logger::tracel(__VA_ARGS__)
#define BEGIN_TRACE_LINE(...) Logger::trace_time(__VA_ARGS__)
#define TRACE_TEXT(...) Logger::trace(__VA_ARGS__)
#define END_TRACE_LINE(...) Logger::trace_end_line(__VA_ARGS__)
#define ASSERT(_Expression) if (!(_Expression)) Logger::assertfuncline(_CRT_WIDE(#_Expression), __FILEW__, __FUNCTIONW__, __LINE__);

//stream log type
#define LOGA(message) {						\
	std::stringstream strm;					\
	strm << message << std::endl << '\0';	\
	strm.flush();\
	TRACE( (LPCWSTR)_bstr_t(strm.str().c_str()));\
	}
