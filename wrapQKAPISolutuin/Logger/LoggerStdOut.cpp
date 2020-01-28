#include "stdafx.h"
#include "LoggerStdOut.h"

namespace Logger
{
	CLoggerStdOut::CLoggerStdOut()
		:base()
	{
	}

	CLoggerStdOut::~CLoggerStdOut()
	{
	}


	void CLoggerStdOut::outMsg(const WCHAR *pszMsg)
	{
		wprintf(pszMsg);
	}

}