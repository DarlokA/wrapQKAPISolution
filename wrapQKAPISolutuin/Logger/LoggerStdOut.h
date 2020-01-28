#pragma once
#include "LoggerBase.h"
#include "Locker.h"
namespace Logger
{
	class CLoggerStdOut : public CLoggerBase
	{
		typedef CLoggerBase base;
	public:
		CLoggerStdOut();
		~CLoggerStdOut();
	public:
		virtual void outMsg(const WCHAR *pszMsg) override;
	};

}

