#pragma once
#include "LoggerBase.h"
#include "Locker.h"
namespace Logger
{
	class CLogger2File : public CLoggerBase
	{
		typedef CLoggerBase base;
	public:
		explicit CLogger2File(LPCWSTR root_folder);
		~CLogger2File();
	public:
		virtual void outMsg(const WCHAR *pszMsg) override;
	private:
		CString m_file_path;
	};

}

