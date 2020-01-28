#pragma once
#ifdef TRADINGAPICONNECTOR_EXPORTS
#define TRADING_CONNECTOR_API __declspec(dllexport) 
#else
#define TRADING_CONNECTOR_API __declspec(dllimport) 
#endif

namespace TradingAPIConnector
{


	//#define POINT_INSTANCE_GUID L"A2C0D2A7"
#define POINT_INSTANCE_GUID L"{80C7210E-79A6-4644-8967-51EE480E4205}"

	class  APIConnectorPoint
	{
	public:
		LPCWSTR static instance_guid()
		{
			return POINT_INSTANCE_GUID;
		}

		_bstr_t static MakePointInstanceName(_bstr_t quik_path)
		{
			DWORD hash = 0;
			CStringW lpPath((LPCTSTR)quik_path);
			lpPath.MakeLower();
			for (int i = 0; i < lpPath.GetLength(); ++i)
			{
				hash += lpPath[i];
			}
			_bstr_t res(instance_guid());
			res += (_bstr_t)_variant_t(hash);
			TRACE(L"MakePointInstanceName %s return %s", lpPath.GetString(), (LPCWSTR)res);
			return res;
		}

		bool static isExistThisPoint(_bstr_t quik_path)
		{
			HANDLE		hMutexOneInstance;
			_bstr_t name = MakePointInstanceName(quik_path);
			hMutexOneInstance = ::OpenMutexW(MUTANT_QUERY_STATE, FALSE, (LPCWSTR)name);
			if (hMutexOneInstance == NULL)
			{
				int err_code = ::GetLastError();
				TRACE(L"::OpenMutexW return %d (%s)", err_code, ERROR_FILE_NOT_FOUND == err_code ? L"NotExis" : L"");
			}
			bool res = hMutexOneInstance != NULL;
			if (res)
			{
				TRACE(L"::OpenMutexW success opened!");
				::CloseHandle(hMutexOneInstance);
			}
			return res;
		}


	};

	typedef unsigned __int64 TOrderID;
	typedef ULONG		TTransactionID;

	enum TRADING_CONNECTOR_API OrderAction
	{
		OASell = -1,
		OABuy = 1,
		OAUnk = 0,
	};

	enum TRADING_CONNECTOR_API OrderType
	{
		OTLimit = 0,
		OTMarket = 1,
		OTStopMarket = 2,
		OTStopLimit = 3,
	};

	enum TRADING_CONNECTOR_API OrderTIF
	{
		TIF_Unknown = 0,
		TIF_GTC = 1,
		TIF_DAY = 2,
		TIF_DATE = 3,
	};

	enum TRADING_CONNECTOR_API QKConnectionState
	{
		QKCS_Disconnected = 0,
		QKCS_Connecting = 1,
		QKCS_Connected = 2,
	};
	enum TRADING_CONNECTOR_API QKAPIErrorCode
	{
		QKAPI_SUCCESS						= 0,
		QKAPI_FAILED						= 1,
		QKAPI_QUIK_TERMINAL_NOT_FOUND		= 2,
		QKAPI_DLL_VERSION_NOT_SUPPORTED		= 3,
		QKAPI_ALREADY_CONNECTED_TO_QUIK		= 4,
		QKAPI_WRONG_SYNTAX					= 5,
		QKAPI_QUIK_NOT_CONNECTED			= 6,
		QKAPI_DLL_NOT_CONNECTED				= 7,
		QKAPI_QUIK_CONNECTED				= 8,
		QKAPI_QUIK_DISCONNECTED				= 9,
		QKAPI_DLL_CONNECTED					= 10,
		QKAPI_DLL_DISCONNECTED				= 11,
		QKAPI_MEMORY_ALLOCATION_ERROR		= 12,
		QKAPI_WRONG_CONNECTION_HANDLE		= 13,
		QKAPI_WRONG_INPUT_PARAMS			= 14,
	};

	struct TRADING_CONNECTOR_API SOrder
	{
		OrderAction	action;
		OrderType	type;
		double		stop;
		double		limit; 
		long		quantity;
		OrderTIF	tif; 
		DATE		expiry_date;
		SOrder()
			:action(OrderAction::OAUnk)
			,type(OrderType::OTLimit)
			,stop(0.0)
			,limit(0.0)
			,quantity(0)
			,tif(OrderTIF::TIF_DAY)
			,expiry_date(0.0)
		{}
	};

	enum TRADING_CONNECTOR_API OrderState
	{
		osNew		= 0,
		osSended	= 1,
		osAccepted	= 2,
		osExecuting = 3,
		osCanceled	= 4,
		osRejected	= 5,
		osPartialFilled = 6,
		osFilled	= 7,
		osCount		= 8,
	};

	__interface TRADING_CONNECTOR_API IOrderStausCallBack
	{
		virtual void OnOrderStatus(_bstr_t account, _bstr_t client_code, _bstr_t instrument, _bstr_t class_code, SOrder order, OrderState state, double avg_fill, long filled_qty, _bstr_t msg) = 0;
	};

	__interface TRADING_CONNECTOR_API IOrderManager
	{
		virtual void Advise2OrderStatus(IOrderStausCallBack* pStatusListener) = 0;
		virtual void UnadviseFromOrderStatus(IOrderStausCallBack* pStatusListener) = 0;
		virtual TTransactionID	PlaceOrder(_bstr_t account, _bstr_t client_code, _bstr_t instrument, _bstr_t class_code, SOrder order, _bstr_t comment) = 0;
		virtual TTransactionID	ModifyOrder(TOrderID id, const SOrder order, _bstr_t comment) = 0;
		virtual TTransactionID	KillOrder(TOrderID id, _bstr_t comment) = 0;
	};


	__interface TRADING_CONNECTOR_API ITradingConnector
	{
		virtual QKAPIErrorCode	Connect(_bstr_t quik_file_path, bool is_sync) = 0;
		virtual QKAPIErrorCode	WaitForConnectionWork() = 0;
		virtual QKAPIErrorCode	Disconnect() = 0;
		virtual bool			IsDllConnected() const = 0;
		virtual bool			IsQKServerConnected() const = 0;
		virtual QKConnectionState GetConnectionState() const = 0;

		virtual QKAPIErrorCode	SendSyncTransaction(TTransactionID trans_id, _bstr_t transaction_text, TOrderID* order_id, _bstr_t& msg) = 0;
		virtual QKAPIErrorCode	SendAsyncTransaction(TTransactionID trans_id, _bstr_t transaction_text, _bstr_t& msg) = 0;

		IOrderManager*			OrderManager() = 0;

		virtual void	Dispose() = 0;
	};

	struct SApiConnectorDestroyer
	{
		SApiConnectorDestroyer()
		{
			TRACE(L"SApiConnectorDestroyer ctor()");
		}

		void operator () (TradingAPIConnector::ITradingConnector* pconnector) const {
			TRACE(L"SApiConnectorDestroyer call delete for ITradingConnector...");
			pconnector->Disconnect();
			pconnector->Dispose();
			pconnector = NULL;
		};
	};


}

TRADING_CONNECTOR_API TradingAPIConnector::ITradingConnector*	__cdecl APIConnector();
TRADING_CONNECTOR_API bool	__cdecl RunProcess( _bstr_t path, _bstr_t cmd_line);
TRADING_CONNECTOR_API _bstr_t	__cdecl GetMyRunDir();