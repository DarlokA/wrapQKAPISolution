#pragma once
#include "interface\TradingAPIConnectorInterface.h"
#include <map>
#include <set>
//TODO: !!!Implement NotifyOrderStatus to clients!!!

namespace TradingAPIConnector
{
	__interface IAPIReplyListener
	{
		virtual void OnTransactionOK(TTransactionID transId, TOrderID orderID, long nTransactionReplyCode, _bstr_t msg) = 0;
		virtual void OnTransactionFailed(TTransactionID transId, long nTransactionReplyCode, _bstr_t msg) = 0;
		virtual void OnOrderStatus() = 0;
		virtual void OnTrade() = 0;
	};

	class COrderManager
		: public IOrderManager
		, public IAPIReplyListener
	{
	public:
		explicit COrderManager(ITradingConnector* pconnector);
		virtual ~COrderManager();
	public:
		// Inherited via IOrderManager
		virtual void Advise2OrderStatus(IOrderStausCallBack* pStatusListener) override;
		virtual void UnadviseFromOrderStatus(IOrderStausCallBack* pStatusListener) override;

		virtual TTransactionID PlaceOrder(_bstr_t account, _bstr_t client_code, _bstr_t instrument, 
			_bstr_t class_code, SOrder order, _bstr_t comment) override;
		virtual TTransactionID ModifyOrder(TOrderID id, const SOrder order, _bstr_t comment) override;
		virtual TTransactionID KillOrder(TOrderID id, _bstr_t comment) override;
	public:
		// Inherited via IAPIReplyListener
		virtual void OnTransactionOK(TTransactionID transId, TOrderID orderID, long nTransactionReplyCode, _bstr_t msg) override;
		virtual void OnTransactionFailed(TTransactionID transId, long nTransactionReplyCode, _bstr_t msg) override;
		virtual void OnOrderStatus() override;
		virtual void OnTrade() override;
	private:
		struct SOrderPos
		{
			_bstr_t account;
			_bstr_t client_code;
			_bstr_t instrument;
			_bstr_t class_code;
			SOrderPos()
				: account(L"")
				, client_code(L"")
				, instrument(L"")
				, class_code(L"")
			{}
			SOrderPos(_bstr_t _account,  _bstr_t _client_code, _bstr_t _instrument, _bstr_t _class_code)
				: account(_account)
				, client_code(_client_code)
				, instrument(_instrument)
				, class_code(_class_code)
			{}
		};

		struct SOrderStatus
		{
			OrderState	status;
			_bstr_t		message;
			long		filled_qty;
			double		avg_fill;
			SOrderStatus()
				:status(osNew)
				, message(L"New order")
				, filled_qty(0)
				, avg_fill(0.0)
			{}
			SOrderStatus(const SOrderStatus& right)
				:status(right.status)
				, message(right.message)
				, filled_qty(right.filled_qty)
				, avg_fill(right.avg_fill)
			{}

			bool isFinal() const
			{
				switch (status)
				{
				case osCanceled:
				case osFilled:
				case osRejected:
					return true;
				default:
					return false;
				}
			}

			bool isFilled() const
			{
				return osFilled == status || osPartialFilled == status;
			}
		};

		typedef std::map<TTransactionID, TOrderID> TTrans2OrderMap;
		typedef std::map<TOrderID, SOrder> TOrderMap;
		typedef std::map<TOrderID, SOrderPos> TOrderPosMap;
		typedef std::map<TOrderID, SOrderStatus> TOrderSatusMap;
		typedef std::set<IOrderStausCallBack*> TStatusListeners;
	private:
		void setup_call_backs();
		void register_new_order(TTransactionID transactionID, TOrderID orderID, _bstr_t account, _bstr_t client_code, _bstr_t instrument,
			_bstr_t class_code, SOrder order, SOrderStatus st);
		void notify_order(TOrderID orderID, SOrder _order, SOrderPos _pos, SOrderStatus _status);
		void reject_order(TTransactionID transactionID, TOrderID orderID, _bstr_t msg);
		bool erase_order(TOrderID orderID, SOrder* porder, SOrderPos* ppos, SOrderStatus* pStatus);
		static _bstr_t TransactionResult2String(long res);
	private:
		ATL::CComAutoCriticalSection m_orders_cs;
		TTrans2OrderMap m_trans2order;
		TOrderMap		m_orders;
		TOrderPosMap	m_order_pos;
		TOrderSatusMap  m_order_status;
		ITradingConnector* m_pQKConnector;
		TStatusListeners	m_advisers;
		static TTransactionID m_reject_transaction_id;
		const TOrderID invalid_order_id;
	};
}

