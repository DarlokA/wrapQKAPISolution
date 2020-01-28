#include "stdafx.h"
#include "PipeChannelServer.h"

namespace CommunicationsAPI
{
	void CPipeServer::pipeServerThread()
	{
		
		TRACE(_T("=>%X PipeServerThread"), ::GetCurrentThreadId());

		while (m_bIsWork)
		{
			TRACE(L"%X wait for create pipe....", ::GetCurrentThreadId());
			while (m_hNamedPipe == INVALID_HANDLE_VALUE)
				::Sleep(100);
			TRACE(L"%X pipeName (%s) createdOK", ::GetCurrentThreadId(), (LPCTSTR)m_sName);
			TRACE(L"%X Waiting for connect...", ::GetCurrentThreadId());
			// ќжидаем соединени€ со стороны клиента
			m_bConnected = ConnectNamedPipe(m_hNamedPipe, &oConnect);
			if (m_bConnected != TRUE)
			{
				DWORD err = GetLastError();
				m_bConnected = err == ERROR_PIPE_CONNECTED;
				switch (err)
				{
				case ERROR_NO_DATA:
					TRACE(L"ConnectNamedPipe: ERROR_NO_DATA");
					//Stop();
					break;

				case ERROR_PIPE_CONNECTED:
				{
					TRACE(L"ConnectNamedPipe: ERROR_PIPE_CONNECTED Continue listening");
					if (SetEvent(oConnect.hEvent))
						m_bConnected = TRUE;
				}
				break;

				case ERROR_PIPE_LISTENING:
					TRACE(L"ConnectNamedPipe: ERROR_PIPE_LISTENING");
					//Stop();
					break;

				case ERROR_CALL_NOT_IMPLEMENTED:
					TRACE(L"ConnectNamedPipe: ERROR_CALL_NOT_IMPLEMENTED");
					//Stop();
					break;

				default:
					TRACE(L"ConnectNamedPipe: Error %ld\n", err);
					//Stop();
					break;
				}
			}
			if (m_bConnected)
			{
				TRACE(L"ConnectNamedPipe: OK");
			}
			else
			{
				Sleep(100);
			}
		}
		TRACE(_T("<=%X EndPipeServerThread"), ::GetCurrentThreadId());
		return;
	}


	CPipeServer::CPipeServer()
		:m_bIsWork(true), m_bConnected(FALSE), m_hNamedPipe(INVALID_HANDLE_VALUE), m_sName(_T(""))
		, m_serverThread(&CPipeServer::pipeServerThread, this), lpPipeInst(NULL), hConnectEvent(NULL)
		, fPendingIO(FALSE)
	{
		ZeroMemory(&oConnect, sizeof(oConnect));
	}

	CPipeServer::~CPipeServer()
	{
		Stop();
		m_bIsWork = false;
		if (NULL != m_serverThread.native_handle())
			m_serverThread.join();
	}

	bool CPipeServer::Start(_bstr_t name)
	{
		Stop();

		hConnectEvent = CreateEvent(
			NULL,    // default security attribute
			TRUE,    // manual reset event 
			TRUE,    // initial state = signaled 
			NULL);   // unnamed event object 
		if (hConnectEvent == NULL)
		{

			TRACE(L"CreateEvent failed with %d.\n", GetLastError());
			return false;
		}
		oConnect.hEvent = hConnectEvent;


		m_sName = L"\\\\.\\pipe\\" + name;

		return CreateAndConnectInstance();
	}

	BOOL CPipeServer::CreateAndConnectInstance()
	{
		m_hNamedPipe = ::CreateNamedPipe(m_sName, PIPE_ACCESS_DUPLEX |      // read/write access 
			FILE_FLAG_OVERLAPPED,     // overlapped mode 
			PIPE_TYPE_MESSAGE |       // message-type pipe 
			PIPE_READMODE_MESSAGE |   // message read mode 
			PIPE_WAIT,                // blocking mode 
			PIPE_UNLIMITED_INSTANCES, // unlimited instances 
			BUFSIZE * sizeof(TCHAR),    // output buffer size 
			BUFSIZE * sizeof(TCHAR),    // input buffer size 
			PIPE_TIMEOUT,             // client time-out 
			NULL);                    // default security attributes
		if (m_hNamedPipe == INVALID_HANDLE_VALUE)
		{
			TRACE(L"CreateNamedPipe: Error % ld\n", GetLastError());
			return FALSE;
		}
		TRACE(L"CPipeServer %s created", (LPCTSTR)m_sName);

		return TRUE;
	}

	bool CPipeServer::Join()
	{
		if (NULL != m_serverThread.native_handle())
		{
			m_serverThread.join();
			return true;
		}
		return false;
	}

	void CPipeServer::Stop()
	{
		if (m_hNamedPipe != INVALID_HANDLE_VALUE)
		{
			TRACE(L"UninitializeChannel %s", (LPCWSTR)m_sName);
			CloseHandle(m_hNamedPipe);
			m_hNamedPipe = INVALID_HANDLE_VALUE;
			CloseHandle(hConnectEvent);
			hConnectEvent = NULL;
		}
	}

	void CPipeServer::Dispose()
	{
		Stop();
		delete this;
	}
}
