#include "stdafx.h"
#include "LoggerBase.h"

namespace Logger
{


	CLoggerBase::CLoggerBase()
	{
	}


	CLoggerBase::~CLoggerBase()
	{
	}

	void CLoggerBase::trace_time(const WCHAR *pszMsg)
	{
		CritSecMethod(m_cs);
		outMsg(timeStamp());
	}

	CString CLoggerBase::longwZ(WORD w)
	{
		CString res;
		if (w < 10)
			res.Format(L"0%u", w);
		else
			res.Format(L"%u", w);
		return res;
	}

	CString CLoggerBase::longwZZ(WORD w)
	{
		CString res;
		if (w < 10)
			res.Format(L"00%u", w);
		else
		{
			if (w < 100)
				res.Format(L"0%u", w);
			else
				res.Format(L"%u", w);
		}
		return res;
	}


	CString CLoggerBase::timeStamp()
	{
		//DWORD _pid = ::GetCurrentProcessId();
		DWORD _id = GetCurrentThreadId();
		SYSTEMTIME systime;
		GetLocalTime(&systime);
		CString str;
		str.Format(_T("[0x%X][%s.%s.%u][%s:%s:%s.%s]\t"), _id,
			longwZ(systime.wDay).GetString(), longwZ(systime.wMonth).GetString(), systime.wYear,
			longwZ(systime.wHour).GetString(), longwZ(systime.wMinute).GetString(), longwZ(systime.wSecond).GetString(),
			longwZZ(systime.wMilliseconds).GetString());
		return str;
	}

	void CLoggerBase::trace(const WCHAR *pszMsg)
	{
		if (pszMsg == NULL) return;
		CritSecMethod(m_cs);
		outMsg(pszMsg);
	}

	void CLoggerBase::trace_line(const WCHAR *pszMsg)
	{
		if (pszMsg == NULL) return;
		CritSecMethod(m_cs);
		CString msg;
		msg.Format(L"%s%s\n", timeStamp().GetString(), pszMsg);
		trace(msg);
	}

	void CLoggerBase::trace_end_line(const WCHAR *pszMsg)
	{
		if (pszMsg == NULL) return;
		CritSecMethod(m_cs);
		CString msg;
		msg.Format(L"%s\n", pszMsg);
		trace(msg);
	}
}
