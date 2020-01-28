#include "stdafx.h"
#include "trans2quik_api.h"
#include "QuikAPIConnector.h"
#include <psapi.h>
#include <tlhelp32.h>

namespace TradingAPIConnector
{

	CQuikAPIConnector* CQuikAPIConnector::m_pQuikAPIConnector = NULL;

	CQuikAPIConnector* CQuikAPIConnector::Instace()
	{
		if (CQuikAPIConnector::m_pQuikAPIConnector == NULL)
			m_pQuikAPIConnector = new CQuikAPIConnector();
			return m_pQuikAPIConnector;
	}

	CQuikAPIConnector::CQuikAPIConnector()
		: m_pconnection_thread(NULL)
		, m_nExtendedErrorCode(0)
		, m_quik_file_path(L"")
		, m_is_connecting( false )
		, m_connecting_result( QKAPIErrorCode::QKAPI_SUCCESS )
		, m_isDllConnected(false)
		, m_isQKServerConnected(false)
		, m_OrderManager(this)
	{
		TRACE(L"CQuikAPIConnector ctor");
		ASSERT(m_pQuikAPIConnector == NULL);
		memset(m_szErrorMessage, 0, sizeof(m_szErrorMessage));
		SetConnectionStatusCallback();
	}

	CQuikAPIConnector& CQuikAPIConnector::operator=(const CQuikAPIConnector & right)
	{
		ASSERT(false && L"!!!Singletone operator= not available!!!");
		return *this;
	}


	CQuikAPIConnector::~CQuikAPIConnector()
	{
		TRACE(L"=>CQuikAPIConnector dtor");
		WaitForConnectionWork();
		long m_nResult = ::TRANS2QUIK_SET_CONNECTION_STATUS_CALLBACK(NULL, &m_nExtendedErrorCode, m_szErrorMessage, sizeof(m_szErrorMessage));
		if (IsDllConnected())
		{
			ASSERT(false&&L"!!!ALARMA!!! CQuikAPIConnector::dtor: TRANS2QUIK.dll is Connected");
			Disconnect();
		}
		m_pQuikAPIConnector = NULL;
		TRACE(L"<=CQuikAPIConnector dtor");
	}

	IConnectionStatusListener * CQuikAPIConnector::ConnectionListener()
	{
		return m_pQuikAPIConnector;
	}

	IAPIReplyListener*			CQuikAPIConnector::APIReplyListener()
	{
		if (m_pQuikAPIConnector == NULL)
		{
			throw std::bad_typeid::__construct_from_string_literal("m_pQuikAPIConnector == NULL");
		}
		return CQuikAPIConnector::Instace()->APIReplyListener();
	}

	void CQuikAPIConnector::EnableDebugPriv()
	{
		HANDLE hToken;
		LUID luid;
		TOKEN_PRIVILEGES tkp;

		OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken);

		LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &luid);

		tkp.PrivilegeCount = 1;
		tkp.Privileges[0].Luid = luid;
		tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

		AdjustTokenPrivileges(hToken, false, &tkp, sizeof(tkp), NULL, NULL);

		CloseHandle(hToken);
	}

	bool CQuikAPIConnector::RunProcess(_bstr_t file_path, _bstr_t cmd_line)
	{
		TRACE(L"CQuikAPIConnector::RunProcess %s", (LPCWSTR)file_path);
		STARTUPINFOW si;
		memset(&si, 0, sizeof(si));
		si.cb = sizeof(si);
		PROCESS_INFORMATION pi;
		memset(&pi, 0, sizeof(pi));
		LPWSTR _cmd_line = cmd_line;
		_bstr_t path = extract_path(file_path);
		_bstr_t exe_w_arg = file_path;
		exe_w_arg += L" ";
		exe_w_arg += cmd_line;
		if (TradingAPIConnector::APIConnectorPoint::isExistThisPoint(cmd_line))
		{
			TRACE(L"!!!!ConnectionPoint Process Exist for quik_path =\"%s\"", (LPCWSTR)cmd_line);
			return true;
		}
		if (CreateProcessW(NULL, (LPWSTR)exe_w_arg, NULL, NULL, FALSE, 0, NULL, (LPCWSTR)path, &si, &pi))
		{
			DWORD res = 0;
			if (GetExitCodeProcess(pi.hProcess, &res) && res != STILL_ACTIVE)
			{
				TRACE(L"Process at %s return ExitCode %d", (LPCWSTR)file_path, res);
			}else
				TRACE(L"Process started at %s", (LPCWSTR)file_path);
			CloseHandle(pi.hProcess);
			CloseHandle(pi.hThread);
			return true;
		}
		DWORD err_code = ::GetLastError();
		TRACE(L"Process start FAILED at %s with err_code=%d", (LPCWSTR)file_path, err_code);
		return false;
	}

	bool CQuikAPIConnector::IsProcessStarted(_bstr_t path)
	{
		//EnableDebugPriv();
		bool isRun = false;
		HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
		PROCESSENTRY32 pe;
		pe.dwSize = sizeof(PROCESSENTRY32);
		if (Process32First(hSnapshot, &pe))
		{
			do {
				if (strcmp(pe.szExeFile, "info.exe") == 0)
				{
					isRun = true;
					HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pe.th32ProcessID);
					if (hProcess != NULL)
					{
						wchar_t filename[MAX_PATH];
						if (GetModuleFileNameExW(hProcess, NULL, filename, sizeof(filename) / sizeof(wchar_t)) == 0)
						{
							if (_wcsicmp((LPCWSTR)filename, (LPCWSTR)path) != 0)
								isRun = false;
						}
						CloseHandle(hProcess);
					}
					if (isRun) break;
				}
			} while (Process32Next(hSnapshot, &pe));
		}
		return isRun;
	}

	_bstr_t CQuikAPIConnector::extract_path(_bstr_t file_name)
	{
		std::wstring str((LPCWSTR)file_name);
		size_t pos = str.find_last_of(L'\\');
		if (pos > 0)
			pos += 1;
		if (pos < 0)
			return file_name;
		_bstr_t res(str.substr(0, pos).c_str());
		return res;
	}

	_bstr_t CQuikAPIConnector::QuikFilePath() const
	{
		return m_quik_file_path;
	}

	bool CQuikAPIConnector::IsConnecting() const
	{
		return m_is_connecting;
	}

	void CQuikAPIConnector::SetConnectingResult(QKAPIErrorCode res)
	{
		m_connecting_result = res;
	}

	_bstr_t CQuikAPIConnector::QKAPIErrorCode2String(QKAPIErrorCode code)
	{
		switch (code)
		{
		case QKAPI_SUCCESS:
			return L"QKAPI_SUCCESS";
		case QKAPI_FAILED:
			return L"QKAPI_FAILED";
		case QKAPI_QUIK_TERMINAL_NOT_FOUND:
			return L"QKAPI_QUIK_TERMINAL_NOT_FOUND";
		case QKAPI_DLL_VERSION_NOT_SUPPORTED:
			return L"QKAPI_DLL_VERSION_NOT_SUPPORTED";
		case QKAPI_ALREADY_CONNECTED_TO_QUIK:
			return L"QKAPI_ALREADY_CONNECTED_TO_QUIK";
		case QKAPI_WRONG_SYNTAX:
			return L"QKAPI_WRONG_SYNTAX";
		case QKAPI_QUIK_NOT_CONNECTED:
			return L"QKAPI_QUIK_NOT_CONNECTED";
		case QKAPI_DLL_NOT_CONNECTED:
			return L"QKAPI_DLL_NOT_CONNECTED";
		case QKAPI_QUIK_CONNECTED:
			return L"QKAPI_QUIK_CONNECTED";
		case QKAPI_QUIK_DISCONNECTED:
			return L"QKAPI_QUIK_DISCONNECTED";
		case QKAPI_DLL_CONNECTED:
			return L"QKAPI_DLL_CONNECTED";
		case QKAPI_DLL_DISCONNECTED:
			return L"QKAPI_DLL_DISCONNECTED";
		case QKAPI_MEMORY_ALLOCATION_ERROR:
			return L"QKAPI_MEMORY_ALLOCATION_ERROR";
		case QKAPI_WRONG_CONNECTION_HANDLE:
			return L"QKAPI_WRONG_CONNECTION_HANDLE";
		case QKAPI_WRONG_INPUT_PARAMS:
			return L"QKAPI_WRONG_INPUT_PARAMS";
		}
		return _bstr_t(L"QKAPI_Unknown");
	}

	QKAPIErrorCode CQuikAPIConnector::Connect(_bstr_t quik_file_path, bool is_sync)
	{
		if (IsDllConnected()) return m_connecting_result;
		m_quik_file_path = quik_file_path;
		TRACE(L"CQuikAPIConnector::Connect %s", (LPCWSTR)quik_file_path);
		if (!IsProcessStarted(quik_file_path))
		{
			if (!RunProcess(quik_file_path, _bstr_t()))
				return QKAPIErrorCode::QKAPI_DLL_NOT_CONNECTED;
		}
		m_is_connecting = true;
		m_connecting_result = ConnectImpl(extract_path(QuikFilePath()));
		if (m_connecting_result != QKAPIErrorCode::QKAPI_SUCCESS)
		{
			m_pconnection_thread = new std::thread(DoConnect2Quik, this);
			if (is_sync)
			{
				m_connecting_result = WaitForConnectionWork();
			}
		}
		else
			StopConnect();
		TRACE(L"DoConnect2Quik....%s", (LPCWSTR)QKAPIErrorCode2String(m_connecting_result));
		return m_connecting_result;
	}

	QKAPIErrorCode CQuikAPIConnector::WaitForConnectionWork()
	{
		if (m_pconnection_thread != NULL)
		{
			m_pconnection_thread->join();
			delete m_pconnection_thread;
			m_pconnection_thread = NULL;
			m_is_connecting = false;
		}
		return m_connecting_result;
	}

	QKAPIErrorCode CQuikAPIConnector::Disconnect()
	{
		TRACE(L"CQuikAPIConnector::Disconnect");
		m_is_connecting = false;
		QKAPIErrorCode res = WaitForConnectionWork();
		res = (QKAPIErrorCode)TRANS2QUIK_DISCONNECT(&m_nExtendedErrorCode, m_szErrorMessage, sizeof(m_szErrorMessage));
		TRACE(L"TRANS2QUIK_DISCONNECT return %s msg=%s", (LPCWSTR)QKAPIErrorCode2String(res), (LPCWSTR)_bstr_t(m_szErrorMessage));
		return res;
	}

	bool CQuikAPIConnector::IsDllConnected() const
	{
		if (!m_isDllConnected)
		{
			QKAPIErrorCode res = (QKAPIErrorCode)TRANS2QUIK_IS_DLL_CONNECTED(&m_nExtendedErrorCode, m_szErrorMessage, sizeof(m_szErrorMessage));
			TRACE(L"TRANS2QUIK_IS_DLL_CONNECTED return %s msg=%s", (LPCWSTR)QKAPIErrorCode2String(res), (LPCWSTR)_bstr_t(m_szErrorMessage));
			m_isDllConnected = res == QKAPIErrorCode::QKAPI_DLL_CONNECTED;
		}
		return m_isDllConnected;
	}

	bool CQuikAPIConnector::IsQKServerConnected() const
	{
		if (!m_isQKServerConnected)
		{
			QKAPIErrorCode res = (QKAPIErrorCode)TRANS2QUIK_IS_QUIK_CONNECTED(&m_nExtendedErrorCode, m_szErrorMessage, sizeof(m_szErrorMessage));
			TRACE(L"TRANS2QUIK_IS_QUIK_CONNECTED return %s msg=%s", (LPCWSTR)QKAPIErrorCode2String(res), (LPCWSTR)_bstr_t(m_szErrorMessage));
			m_isQKServerConnected = QKAPIErrorCode::QKAPI_QUIK_CONNECTED == res;
		}
		return m_isQKServerConnected;
	}

	QKConnectionState CQuikAPIConnector::GetConnectionState() const
	{
		if (IsConnecting())
			return QKConnectionState::QKCS_Connecting;
		else
		{
			if (IsDllConnected())
			{
				if (IsQKServerConnected())
					return QKConnectionState::QKCS_Connected;
				else
					return QKConnectionState::QKCS_Connecting;
			}
		}

		return QKConnectionState::QKCS_Disconnected;
	}

	QKAPIErrorCode CQuikAPIConnector::SendSyncTransaction(TTransactionID trans_id, _bstr_t transaction_text, TOrderID* order_id, _bstr_t& msg)
	{
		long nReturnCode;
		DWORD dwTransId;
#ifdef x86
		double _orderID;
#else
		TOrderID _orderID;
#endif
		char szResultMessage[1024];
		long nExtendedErrorCode;
		
		QKAPIErrorCode nResult = (QKAPIErrorCode)TRANS2QUIK_SEND_SYNC_TRANSACTION((LPSTR)transaction_text, &nReturnCode, &dwTransId, &_orderID, szResultMessage, sizeof(szResultMessage), &nExtendedErrorCode, m_szErrorMessage, sizeof(m_szErrorMessage));
		*order_id = (TOrderID)_orderID;
		msg = m_szErrorMessage;
		TRACE(L"CQuikAPIConnector::SendAsyncTransaction[%I32u] (%s) orderID=%I64u return %s msg=%s", trans_id, (LPCWSTR)transaction_text, *order_id, (LPCWSTR)QKAPIErrorCode2String((QKAPIErrorCode)nReturnCode), (LPCWSTR)msg);
		return (QKAPIErrorCode)nReturnCode;
	}
	//%llu
	QKAPIErrorCode CQuikAPIConnector::SendAsyncTransaction(TTransactionID trans_id, _bstr_t transaction_text, _bstr_t& msg)
	{
		QKAPIErrorCode nResult = (QKAPIErrorCode)TRANS2QUIK_SEND_ASYNC_TRANSACTION((LPSTR)transaction_text, &m_nExtendedErrorCode, m_szErrorMessage, sizeof(m_szErrorMessage));
		msg = m_szErrorMessage;
		TRACE(L"CQuikAPIConnector::SendAsyncTransaction[%I32u](%s) return %s msg=%s", trans_id, (LPCWSTR)transaction_text, (LPCWSTR)QKAPIErrorCode2String(nResult), (LPCWSTR)msg);
		return nResult;
	}

	IOrderManager*	CQuikAPIConnector::OrderManager()
	{
		return &m_OrderManager;
	}

	void CQuikAPIConnector::Dispose()
	{
		TRACE(L"CQuikAPIConnector::Dispose() delete this");
		delete this;
	}

	void CQuikAPIConnector::OnDllConnected()
	{
		TRACE(L"CQuikAPIConnector::OnDllConnected()");
		m_isDllConnected = true;
	}

	void CQuikAPIConnector::OnDllDisconnected()
	{
		TRACE(L"CQuikAPIConnector::OnDllDisconnected()");
		m_isDllConnected = false;
	}

	void CQuikAPIConnector::OnQuikConnected()
	{
		TRACE(L"CQuikAPIConnector::OnQuikConnected()");
		m_isQKServerConnected = true;
	}

	void CQuikAPIConnector::OnQuikDisconnected()
	{
		TRACE(L"CQuikAPIConnector::OnQuikDisconnected()");
		m_isQKServerConnected = false;
	}

	extern "C" static void __stdcall TRANS2QUIK_ConnectionStatusCallback(long nConnectionEvent, long nExtendedErrorCode, LPCSTR lpcstrInfoMessage)
	{
		IConnectionStatusListener* plistener = CQuikAPIConnector::ConnectionListener();
		if (plistener != NULL)
		{
			switch (nConnectionEvent)
			{
			case TRANS2QUIK_DLL_CONNECTED:
				plistener->OnDllConnected();
				break;
			case TRANS2QUIK_DLL_DISCONNECTED:
				plistener->OnDllDisconnected();
				break;
			case TRANS2QUIK_QUIK_CONNECTED:
				plistener->OnQuikConnected();
				break;
			case TRANS2QUIK_QUIK_DISCONNECTED:
				plistener->OnQuikDisconnected();
				break;
			default:
				break;
			}
		}
	}

	void CQuikAPIConnector::SetConnectionStatusCallback()
	{
		QKAPIErrorCode nResult = (QKAPIErrorCode)::TRANS2QUIK_SET_CONNECTION_STATUS_CALLBACK(TRANS2QUIK_ConnectionStatusCallback, &m_nExtendedErrorCode, m_szErrorMessage, sizeof(m_szErrorMessage));
		TRACE(L"CQuikAPIConnector::SetConnectionStatusCallback()....%s msg: %s", (LPCWSTR)CQuikAPIConnector::QKAPIErrorCode2String(nResult), (LPCWSTR)_bstr_t(m_szErrorMessage));
	}

	QKAPIErrorCode CQuikAPIConnector::ConnectImpl(_bstr_t qkPath)
	{
		QKAPIErrorCode nResult = (QKAPIErrorCode)TRANS2QUIK_CONNECT((LPSTR)qkPath, &m_nExtendedErrorCode, m_szErrorMessage, sizeof(m_szErrorMessage));
		TRACE(L"TRANS2QUIK_CONNECT....%s msg: %s", (LPCWSTR)CQuikAPIConnector::QKAPIErrorCode2String(nResult), (LPCWSTR)_bstr_t(m_szErrorMessage));
		m_connecting_result = nResult;
		return nResult;

	}

	void	CQuikAPIConnector::StopConnect()
	{
		m_is_connecting = false;
	}

	void DoConnect2Quik(CQuikAPIConnector* pAPIConnector)
	{
		TRACE(L"===>DoConnect2Quik");
		_bstr_t qkPath = CQuikAPIConnector::extract_path(pAPIConnector->QuikFilePath());
		bool is_continue = true;
		QKAPIErrorCode nResult = QKAPIErrorCode::QKAPI_SUCCESS;
		do
		{
			if (!pAPIConnector->IsConnecting())
				break;
			nResult = pAPIConnector->ConnectImpl(qkPath);
			switch (nResult)
			{
			case QKAPI_SUCCESS:
			case QKAPI_DLL_VERSION_NOT_SUPPORTED:
			case QKAPI_ALREADY_CONNECTED_TO_QUIK:
			case QKAPI_WRONG_SYNTAX:
			case QKAPI_MEMORY_ALLOCATION_ERROR:
			case QKAPI_WRONG_INPUT_PARAMS:
				pAPIConnector->StopConnect();
				is_continue = false;
				break;
			}
			if (is_continue) ::Sleep(1000);
		} while (is_continue);
		TRACE(L"<===DoConnect2Quik");
	}
}