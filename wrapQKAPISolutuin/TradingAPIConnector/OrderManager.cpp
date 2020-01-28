#include "stdafx.h"
#include "trans2quik_api.h"
#include "OrderManager.h"
#include "TransactionBulder.h"
#include "..\Logger\Locker.h"
#include "QuikAPIConnector.h"

namespace TradingAPIConnector
{
/////////QuikAPI CallBack fuctions////////////////////
#ifdef x86
	extern "C" static void __stdcall TRANS2QUIK_TransactionReplyCallback(long nTransactionResult, long nTransactionExtendedErrorCode, long nTransactionReplyCode, DWORD dwTransId, double nOrderNum      , LPCSTR lpcstrTransactionReplyMessage)
#else
	extern "C" static void __stdcall TRANS2QUIK_TransactionReplyCallback(long nTransactionResult, long nTransactionExtendedErrorCode, long nTransactionReplyCode, DWORD dwTransId, EntityNumber nOrderNum, LPCSTR lpcstrTransactionReplyMessage, TransactionReplyDescriptor transReplyDescriptor)
#endif
	{
		_bstr_t msg(lpcstrTransactionReplyMessage);
		QKAPIErrorCode nResult = (QKAPIErrorCode)nTransactionResult;
		switch (nResult)
		{
		case QKAPIErrorCode::QKAPI_SUCCESS:
			TRACE(L"Trascation %d return success", dwTransId);
			CQuikAPIConnector::APIReplyListener()->OnTransactionOK(TTransactionID(dwTransId), TOrderID(nOrderNum), nTransactionReplyCode, msg);
			return;
		case QKAPIErrorCode::QKAPI_WRONG_SYNTAX:
			TRACE(L"Trascation %d return wrong syntax", dwTransId);
			break;
		case QKAPIErrorCode::QKAPI_DLL_DISCONNECTED:
			TRACE(L"Trascation %d return dll disconnected", dwTransId);
			break;
		case QKAPIErrorCode::QKAPI_QUIK_NOT_CONNECTED:
			TRACE(L"Trascation %d return quik not connected to server", dwTransId);
			break;
		case QKAPIErrorCode::QKAPI_FAILED:
			TRACE(L"Trascation %d return failed ext_code=%d", dwTransId, nTransactionExtendedErrorCode);
			break;
		default:
			TRACE(L"Trascation %d return %d reply_code=%d ext_code=%d", dwTransId, nTransactionResult, nTransactionReplyCode, nTransactionExtendedErrorCode);
			break;
		}
		CQuikAPIConnector::APIReplyListener()->OnTransactionFailed(TTransactionID(dwTransId), nTransactionReplyCode, msg);

		return;
	}
//////////////////////////////////////////////////////
#ifdef x86
	extern "C" static void __stdcall TRANS2QUIK_OrderStatusCallback(long nMode, DWORD dwTransID, double dNumber      , LPCSTR ClassCode, LPCSTR SecCode, double dPrice, long nBalance     , double dValue, long nIsSell, long nStatus, long nOrderDescriptor)
#else
	extern "C" static void __stdcall TRANS2QUIK_OrderStatusCallback(long nMode, DWORD dwTransID, EntityNumber dNumber, LPCSTR ClassCode, LPCSTR SecCode, double dPrice, Quantity  nBalance, double dValue, long nIsSell, long nStatus, OrderDescriptor orderDescriptor)
#endif
	{
		//TODO: Implemet TRANS2QUIK_OrderStatusCallback
		return;
	}
//////////////////////////////////////////////////////
#ifdef x86
	extern "C" static void __stdcall TRANS2QUIK_TradeStatusCallback(long nMode, double dNumber      , double dOrderNumber      , LPCSTR ClassCode, LPCSTR SecCode, double dPrice, long nQty    , double dValue, long nIsSell, long nTradeDescriptor)
#else
	extern "C" static void __stdcall TRANS2QUIK_TradeStatusCallback(long nMode, EntityNumber dNumber, EntityNumber dOrderNumber, LPCSTR ClassCode, LPCSTR SecCode, double dPrice, Quantity nQty, double dValue, long nIsSell, TradeDescriptor tradeDescriptor)
#endif
	{
		//TODO: Implement TRANS2QUIK_TradeStatusCallback
	}
/////////OrderManager implementation////////////////////

	void COrderManager::setup_call_backs()
	{
		long nExtendedErrorCode;
		char szErrorMessage[1024];
		QKAPIErrorCode nResult = (QKAPIErrorCode)::TRANS2QUIK_SET_TRANSACTIONS_REPLY_CALLBACK(TRANS2QUIK_TransactionReplyCallback, &nExtendedErrorCode, szErrorMessage, sizeof(szErrorMessage));
		TRACE(L"CQuikAPIConnector::SetConnectionStatusCallback()....%s msg: %s", (LPCWSTR)CQuikAPIConnector::QKAPIErrorCode2String(nResult), (LPCWSTR)_bstr_t(szErrorMessage));
		//TODO: Subsribe to orders and trades
	}

	_bstr_t COrderManager::TransactionResult2String(long res)
	{
		switch (res)
		{
#define TrasactionCode(code, msg) case code: return msg;
#include "TransactionResults.hpp"
#undef TrasactionCode
		}
		return L"Unknown transaction replyCode";
	}
	TTransactionID COrderManager::m_reject_transaction_id = -1;
	
	COrderManager::COrderManager(ITradingConnector* pconnector)
		:m_pQKConnector(pconnector), invalid_order_id(TOrderID(-1))
	{
		TRACE(L"===>COrderManager ctor");
		if (m_pQKConnector == NULL)
		{
			ASSERT(m_pQKConnector != NULL);
			throw std::invalid_argument("m_pQKConnector == NULL");
		}
		TRACE(L"<===COrderManager ctor");
	}


	COrderManager::~COrderManager()
	{
		TRACE(L"===>COrderManager dtor");
		m_pQKConnector = NULL;
		TRACE(L"<===COrderManager dtor");
	}

	void COrderManager::Advise2OrderStatus(IOrderStausCallBack * const pStatusListener)
	{
		ASSERT(pStatusListener != NULL);
		m_advisers.insert(pStatusListener);
		TRACE(L"COrderManager::Advise2OrderStatus %p", pStatusListener);
	}

	void COrderManager::UnadviseFromOrderStatus(IOrderStausCallBack * const pStatusListener)
	{
		ASSERT(pStatusListener != NULL);
		m_advisers.erase(pStatusListener);
		TRACE(L"COrderManager::UnadviseFromOrderStatus %p", pStatusListener);
	}

	TTransactionID COrderManager::PlaceOrder(_bstr_t account, _bstr_t client_code, _bstr_t instrument, _bstr_t class_code, SOrder order, _bstr_t comment)
	{
		TTransactionID transId = 0;
		_bstr_t pzPOTransaction = CTransactionBulder::PlaceOrderTransaction(account, client_code, instrument, class_code, order.action, order.type, order.stop, order.limit, order.quantity, order.tif, order.expiry_date, comment, &transId);
		if (pzPOTransaction.length() == 0)
		{
			TRACE(L"!!!COrderManager::PlaceOrder ERROR - invalid order params!!!");
		}
		else
		{
			_bstr_t msg(L"");
			register_new_order(transId, TOrderID(transId), account, client_code, instrument, class_code, order, SOrderStatus());
			QKAPIErrorCode res = m_pQKConnector->SendAsyncTransaction(transId, pzPOTransaction, msg);
			if (res != QKAPIErrorCode::QKAPI_SUCCESS)
			{
				reject_order(transId, transId, msg);
			}
		}
		return transId;
	}
	
	TTransactionID COrderManager::ModifyOrder(TOrderID id, const SOrder order, _bstr_t comment)
	{
		//TODO: Implemet COrderManager::ModifyOrder
		return TTransactionID();
	}
	
	TTransactionID COrderManager::KillOrder(TOrderID id, _bstr_t comment)
	{
		//TODO: Implemet COrderManager::KillOrder
		return TTransactionID();
	}

	bool COrderManager::erase_order(TOrderID orderID, SOrder* porder,  SOrderPos* ppos, SOrderStatus* pStatus)
	{
		CritSecMethod(m_orders_cs);
		auto orderIt = m_orders.find(orderID);
		if (orderIt == m_orders.end())
			return false;
		SOrder old_order = orderIt->second;
		m_orders.erase(orderID);
		auto posIt = m_order_pos.find(orderID);
		if (posIt != m_order_pos.end())
			*ppos = posIt->second;
		m_order_pos.erase(orderID);
		auto st_it = m_order_status.find(orderID);
		if (st_it != m_order_status.end())
			*pStatus = st_it->second;
		m_order_status.erase(orderID);
		return true;
	}

	void COrderManager::reject_order(TTransactionID transactionID, TOrderID _orderID, _bstr_t msg)
	{
		CritSecMethod(m_orders_cs);
		auto trans_it = m_trans2order.find(transactionID);
		TOrderID orderID = _orderID;
		if (trans_it != m_trans2order.end())
		{
			orderID = trans_it->second;
		}
		SOrderStatus _status;
		SOrder _order;
		SOrderPos _pos;
		if (!erase_order(orderID, &_order, &_pos, &_status))
			return;
		if (!_status.isFinal())
		{
			if (_status.isFilled())
			{
				_status.status = osFilled;
			}
			else
				_status.status = osRejected;
			_status.message = msg;
		}
		notify_order(orderID, _order, _pos, _status);
		
	}

	void COrderManager::register_new_order(TTransactionID transactionID, TOrderID orderID,
		_bstr_t account,
		_bstr_t client_code, _bstr_t instrument,
		_bstr_t class_code, SOrder order, SOrderStatus st)
	{
		CritSecMethod(m_orders_cs);
		m_trans2order[transactionID] = orderID;
		m_orders[orderID] = order;
		SOrderPos pos(account, client_code, instrument, class_code);
		m_order_pos[orderID] = pos;
		m_order_status[orderID] = st;
		notify_order(orderID, order, pos, st);
	}

	void COrderManager::notify_order(TOrderID orderID, SOrder _order, SOrderPos _pos, SOrderStatus _status)
	{
		for each(IOrderStausCallBack* pStatusListener in m_advisers)
		{
			pStatusListener->OnOrderStatus(_pos.account, _pos.client_code, _pos.instrument, _pos.class_code, _order, _status.status, _status.avg_fill, _status.filled_qty, _status.message);
		}
	}


	void COrderManager::OnTransactionOK(TTransactionID transId, TOrderID orderID, long nTransactionReplyCode, _bstr_t msg)
	{
		CritSecMethod(m_orders_cs);
		auto trans_it = m_trans2order.find(transId);
		if (trans_it != m_trans2order.end())//Order Transaction
		{
			TOrderID oldId = trans_it->second;
			trans_it->second = orderID;
			SOrderStatus _status;
			SOrder _order;
			SOrderPos _pos;
			if (!erase_order(oldId, &_order, &_pos, &_status))
				return;
			register_new_order(transId, orderID, _pos.account, _pos.client_code, _pos.instrument, _pos.class_code, _order, _status);
		}
	}

	void COrderManager::OnTransactionFailed(TTransactionID transId, long nTransactionReplyCode, _bstr_t msg)
	{
		//TODO: implement COrderManager::OnTransactionFailed
	}

	void COrderManager::OnOrderStatus()
	{
		//TODO: implement COrderManager::OnOrderStatus
	}

	void COrderManager::OnTrade()
	{
		//TODO: COrderManager::OnTrade()
	}


}
