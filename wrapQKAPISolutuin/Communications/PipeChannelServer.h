#pragma once
#include "interface/CommunicationsAPI.h"

namespace CommunicationsAPI
{

#define PIPE_TIMEOUT 5000
#define BUFSIZE 4096

	class CPipeServer : public CommunicationsAPI::IServerChannel
	{
	private:
		
		typedef struct
		{
			OVERLAPPED oOverlap;
			HANDLE hPipeInst;
			TCHAR chRequest[BUFSIZE];
			DWORD cbRead;
			TCHAR chReply[BUFSIZE];
			DWORD cbToWrite;
		} PIPEINST, * LPPIPEINST;


	private:
		bool		m_bIsWork;
		BOOL		m_bConnected;
		HANDLE		m_hNamedPipe;
		HANDLE		hConnectEvent;
		_bstr_t		m_sName;

		LPPIPEINST	lpPipeInst;
		OVERLAPPED	oConnect;
		BOOL		fPendingIO;
	private:
		std::thread	m_serverThread;
	public:
		CPipeServer();
		~CPipeServer();
	public:
		virtual bool Start(_bstr_t name) override;
		virtual bool Join() override;
		virtual void Stop() override;
	private:
		void pipeServerThread();
		BOOL CreateAndConnectInstance();
	public: 
		// Inherited via IServerChannel
		virtual void Dispose() override;
	};
}