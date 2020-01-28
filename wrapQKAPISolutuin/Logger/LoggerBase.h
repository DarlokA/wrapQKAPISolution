#pragma once
#include "ILoggerObj.h"
#include "Locker.h"
namespace Logger
{
	class CLoggerBase:
		public ILoggerObj
	{
	public:
		CLoggerBase();
		~CLoggerBase();
	public:
		void trace_time(const WCHAR *pszMsg) override;
		void trace(const WCHAR *pszMsg) override;
		void trace_line(const WCHAR *pszMsg) override;
		void trace_end_line(const WCHAR *pszMsg) override;
	public:
		static CString timeStamp();
	protected:
		static CString longwZ(WORD w);
		static CString longwZZ(WORD w);
		virtual void outMsg(const WCHAR *pszMsg) = 0;
	protected:
		ATL::CComAutoCriticalSection m_cs;
	};
}
