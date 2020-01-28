#pragma once
#include "interface\TradingAPIConnectorInterface.h"
#include <thread>
#include "OrderManager.h"
namespace TradingAPIConnector
{
	__interface IConnectionStatusListener
	{
		virtual void OnDllConnected() = 0;
		virtual void OnDllDisconnected() = 0;
		virtual void OnQuikConnected() = 0;
		virtual void OnQuikDisconnected() = 0;
	};

	class CQuikAPIConnector 
		: public ITradingConnector
		, public IConnectionStatusListener
	{
	private:
		CQuikAPIConnector();
		CQuikAPIConnector& operator=(const CQuikAPIConnector& right);
	public:
		static CQuikAPIConnector* Instace();
		virtual ~CQuikAPIConnector();
	public:
		static IConnectionStatusListener* ConnectionListener();
		static IAPIReplyListener* APIReplyListener();
		static void EnableDebugPriv();
		static bool RunProcess(_bstr_t exe_file_path, _bstr_t cmd_line);
		static bool IsProcessStarted(_bstr_t path);
		static _bstr_t extract_path(_bstr_t file_name);
		static _bstr_t QKAPIErrorCode2String(QKAPIErrorCode code);
	public:
		_bstr_t QuikFilePath() const;
		bool	IsConnecting() const;
		void	SetConnectingResult(QKAPIErrorCode res);
		QKAPIErrorCode ConnectImpl(_bstr_t qkPath);
		void	StopConnect();
	public:
		// Унаследовано через ITradingConnector
		virtual QKAPIErrorCode		Connect(_bstr_t quik_file_path, bool is_sync) override;
		virtual QKAPIErrorCode		WaitForConnectionWork() override;
		virtual QKAPIErrorCode		Disconnect() override;
		virtual bool				IsDllConnected() const override;
		virtual bool				IsQKServerConnected() const override;
		virtual QKConnectionState	GetConnectionState() const override;

		virtual QKAPIErrorCode SendSyncTransaction(TTransactionID trans_id, _bstr_t transaction_text, TOrderID* order_id, _bstr_t& msg) override;
		virtual QKAPIErrorCode SendAsyncTransaction(TTransactionID trans_id, _bstr_t transaction_text, _bstr_t& msg) override;

		virtual IOrderManager*		OrderManager() override;

		virtual void				Dispose() override;

		//Унаследовано через IConnectionStatusListener
		virtual void OnDllConnected() override;
		virtual void OnDllDisconnected() override;
		virtual void OnQuikConnected() override;
		virtual void OnQuikDisconnected() override;
	private:
		void	SetConnectionStatusCallback();
	private:
		std::thread*						m_pconnection_thread;
		static CQuikAPIConnector*			m_pQuikAPIConnector;
		mutable long						m_nExtendedErrorCode;
		mutable char						m_szErrorMessage[1024];
		_bstr_t								m_quik_file_path;
		mutable bool						m_is_connecting;
		QKAPIErrorCode						m_connecting_result;
		mutable bool						m_isDllConnected;
		mutable bool						m_isQKServerConnected;
		COrderManager						m_OrderManager;
	};

	static void DoConnect2Quik(CQuikAPIConnector* pAPIConnector);
}
