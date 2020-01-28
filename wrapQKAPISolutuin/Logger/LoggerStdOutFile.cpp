#include "stdafx.h"
#include "LoggerStdOutFile.h"

namespace Logger
{
	CLoggerStdOutFile::CLoggerStdOutFile(LPCWSTR root_folder)
		:base(root_folder)
	{
	}

	CLoggerStdOutFile::~CLoggerStdOutFile()
	{
	}


	void CLoggerStdOutFile::outMsg(const WCHAR *pszMsg)
	{
		wprintf(pszMsg);
		base::outMsg(pszMsg);
	}

}