#pragma once
#include "Logger2File.h"
#include "Locker.h"
namespace Logger
{
	class CLoggerStdOutFile : public CLogger2File
	{
		typedef CLogger2File base;
	public:
		explicit CLoggerStdOutFile(LPCWSTR root_folder);
		~CLoggerStdOutFile();
	public:
		virtual void outMsg(const WCHAR *pszMsg) override;
	};

}

