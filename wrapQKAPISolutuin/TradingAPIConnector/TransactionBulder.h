#pragma once
#include "interface\TradingAPIConnectorInterface.h"
namespace TradingAPIConnector
{
	class CTransactionBulder
	{
	private:
		static TTransactionID transactionID;
	private:
		static _bstr_t AddTransactionTag(/*out*/TTransactionID* ptransaction_id);
		static _bstr_t AddStringTag(_bstr_t src, _bstr_t tag, _bstr_t val);
		static _bstr_t Operation2String(OrderAction action);
		static _bstr_t Date2String(DATE dt);
	public:
		static _bstr_t PlaceOrderTransaction(_bstr_t account, _bstr_t client_code, _bstr_t instrument, _bstr_t class_code,
			OrderAction action, OrderType type, double stop, double limit, long quantity,
			OrderTIF tif, DATE expiry_date, _bstr_t comment,/*out*/TTransactionID* ptransaction_id);
		static _bstr_t KillOrderTransaction(TOrderID order_id, _bstr_t comment,/*out*/TTransactionID* ptransaction_id);
		static _bstr_t KillStopOrderTransaction(TOrderID stop_order_id, _bstr_t comment,/*out*/TTransactionID* ptransaction_id);
		static _bstr_t KillAllOrderTransaction(_bstr_t comment,/*out*/TTransactionID* ptransaction_id);
		static _bstr_t CustomTransaction(_bstr_t text, /*out*/TTransactionID* ptransaction_id);
	};
}

