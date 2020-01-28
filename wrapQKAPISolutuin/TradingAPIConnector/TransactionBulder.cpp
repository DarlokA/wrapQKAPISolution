#include "stdafx.h"
#include "TransactionBulder.h"
namespace TradingAPIConnector
{
	TTransactionID CTransactionBulder::transactionID = 0;

	_bstr_t CTransactionBulder::AddTransactionTag(TTransactionID * ptransaction_id)
	{
		*ptransaction_id = ++transactionID;
		_bstr_t val = _variant_t(*ptransaction_id);
		_bstr_t res(L"");
		return AddStringTag(res, L"TRANS_ID", val);
	}

	_bstr_t CTransactionBulder::AddStringTag(_bstr_t src, _bstr_t tag, _bstr_t val)
	{
		_bstr_t res = src;
		if (val.length() > 0)
		{
			if (res.length() > 0)
				res += L";";
			res += tag;
			res += L"=";
			res += val;
		}
		return res;
	}
	_bstr_t CTransactionBulder::Operation2String(OrderAction action)
	{
		switch (action)
		{
		case OrderAction::OABuy:
			return L"B";
		case OrderAction::OASell:
			return L"S";
		}
		return _bstr_t();
	}

	_bstr_t CTransactionBulder::Date2String(DATE dt)
	{
		SYSTEMTIME now;
		DATE dnow = 0;
		GetLocalTime(&now);
		SYSTEMTIME in;
		SystemTimeToVariantTime(&now, &dnow);
		if ((LONG)dt < (LONG)dnow)
			return _bstr_t(L"");
		VariantTimeToSystemTime(dt, &in);
		
		//ÃÃÃÃÌÌÄÄ
		_bstr_t res = _variant_t(in.wYear);
		if (in.wMonth < 10)
			res += L"0";
		res += (_bstr_t)_variant_t(in.wMonth);
		if (in.wDay < 10)
			res += L"0";
		res += (_bstr_t)_variant_t(in.wDay);
		return res;
	}

	_bstr_t CTransactionBulder::PlaceOrderTransaction(_bstr_t account, _bstr_t client_code, _bstr_t instrument,
		_bstr_t class_code, OrderAction action, 
		OrderType type, double stop, double limit, long quantity,
		OrderTIF tif, DATE expiry_date, _bstr_t comment,/*out*/TTransactionID* ptransaction_id)
	{
		_bstr_t res = AddTransactionTag(ptransaction_id);
		switch (type)
		{
		case OrderType::OTLimit:
		case OrderType::OTMarket:
			res = AddStringTag(res, L"ACTION", L"NEW_ORDER");
			break;
		case OrderType::OTStopMarket:
		case OrderType::OTStopLimit:
			res = AddStringTag(res, L"ACTION", L"NEW_STOP_ORDER");
			break;
		default:
			return _bstr_t();
		}
		res = AddStringTag(res, L"ACCOUNT", account);
		res = AddStringTag(res, L"CLIENT_CODE", client_code);
		res = AddStringTag(res, L"CLASSCODE", class_code);
		res = AddStringTag(res, L"SECCODE", instrument);
		res = AddStringTag(res, L"OPERATION", Operation2String(action));
		switch (type)
		{
		case OrderType::OTLimit:
			res = AddStringTag(res, L"TYPE", L"L");
			res = AddStringTag(res, L"PRICE", _bstr_t(_variant_t(limit)));
			break;
		case OrderType::OTMarket:
			res = AddStringTag(res, L"TYPE", L"M");
			res = AddStringTag(res, L"PRICE", _bstr_t(_variant_t(limit)));
			break;
		case OrderType::OTStopMarket:
			res = AddStringTag(res, L"TYPE", L"M");
			res = AddStringTag(res, L"PRICE", _bstr_t(_variant_t(limit)));
			res = AddStringTag(res, L"MARKET_STOP_LIMIT", L"YES");
			break;
		case OrderType::OTStopLimit:
			res = AddStringTag(res, L"TYPE", L"L");
			res = AddStringTag(res, L"PRICE", _bstr_t(_variant_t(limit)));
			res = AddStringTag(res, L"STOPPRICE", _bstr_t(_variant_t(stop)));
			break;
		default:
			return _bstr_t();
		}
		res = AddStringTag(res, L"QUANTITY", _bstr_t(_variant_t(quantity)));

		switch (tif)
		{
		case OrderTIF::TIF_GTC:
			res = AddStringTag(res, L"EXPIRY_DATE", L"GTC");
			break;
		case OrderTIF::TIF_DAY:
			res = AddStringTag(res, L"EXPIRY_DATE", L"TODAY");
			break;
		case OrderTIF::TIF_DATE:
			res = AddStringTag(res, L"EXPIRY_DATE", Date2String(expiry_date));
			break;
		}			
		res = AddStringTag(res, L"COMMENT", comment);
		return res;
	}
	_bstr_t CTransactionBulder::KillOrderTransaction(TOrderID order_id, _bstr_t comment, /*out*/TTransactionID* ptransaction_id)
	{
		_bstr_t res = AddTransactionTag(ptransaction_id);
		res = AddStringTag(res, L"ACTION", L"KILL_ORDER");
		res = AddStringTag(res, L"ORDER_KEY", _bstr_t(_variant_t(order_id)));
		res = AddStringTag(res, L"COMMENT", comment);
		return res;
	}
	_bstr_t CTransactionBulder::KillStopOrderTransaction(TOrderID stop_order_id, _bstr_t comment, /*out*/TTransactionID* ptransaction_id)
	{
		_bstr_t res = AddTransactionTag(ptransaction_id);
		res = AddStringTag(res, L"ACTION", L"KILL_STOP_ORDER");
		res = AddStringTag(res, L"ORDER_KEY", _bstr_t(_variant_t(stop_order_id)));
		res = AddStringTag(res, L"COMMENT", comment);
		return res;
	}
	_bstr_t CTransactionBulder::KillAllOrderTransaction( _bstr_t comment, /*out*/TTransactionID* ptransaction_id)
	{
		_bstr_t res = AddTransactionTag(ptransaction_id);
		res = AddStringTag(res, L"ACTION", L"KILL_ALL_ORDERS");
		res = AddStringTag(res, L"COMMENT", comment);
		return res;
	}
	_bstr_t CTransactionBulder::CustomTransaction(_bstr_t text, /*out*/TTransactionID * ptransaction_id)
	{
		_bstr_t res = AddTransactionTag(ptransaction_id);
		res += L";";
		res += text;
		return res;
	}
}
