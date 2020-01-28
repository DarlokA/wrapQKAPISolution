#include "stdafx.h"
#include "QuikAPIConnector.h"

#include "interface/ITradingAPIConnectionPoint.h"

namespace TradingAPIConnectionPoint
{

	QuikAPIConnector::ServerChannelDestroyer::ServerChannelDestroyer()
	{
		TRACE(L"TradingAPIConnectionPoint::ServerChannelDestroyer ctor()");
	}

	void QuikAPIConnector::ServerChannelDestroyer::operator () (CommunicationsAPI::IServerChannel* p_server_channel) const
	{
		TRACE(L"ServerChannelDestroyer call delete for IServerChannel...");
		p_server_channel->Stop();
		p_server_channel->Dispose();
		p_server_channel = NULL;
	}

	QuikAPIConnector::SApiConnectorDestroyer::SApiConnectorDestroyer()
	{
		TRACE(L"SApiConnectorDestroyer ctor()");
	}

	void QuikAPIConnector::SApiConnectorDestroyer::operator()(TradingAPIConnector::ITradingConnector* pconnector) const
	{
		TRACE(L"SApiConnectorDestroyer call delete for ITradingConnector...");
		pconnector->Disconnect();
		pconnector->Dispose();
		pconnector = NULL;
	}


	QuikAPIConnector::QuikAPIConnector()
		: m_pConnector( PipeServer(), QuikAPIConnector::ServerChannelDestroyer())
		, m_pQuikAPI( APIConnector(), QuikAPIConnector::SApiConnectorDestroyer())
	{
		TRACE(_T("TradingAPIConnectionPoint::QuikAPIConnector ctor"));
		if (m_pConnector.get() == NULL)
		{
			ASSERT(m_pConnector.get() != NULL);
			return;
		}
		if (m_pQuikAPI.get() == NULL)
		{
			ASSERT(m_pQuikAPI.get() != NULL);
			return;
		}
		m_pConnector->Start(makePipePointName(::GetCurrentProcessId(), _T("QuikAPIConnector")));
	}

	QuikAPIConnector::~QuikAPIConnector()
	{
		TRACE(_T("TradingAPIConnectionPoint::QuikAPIConnector dtor"));
	}

	void QuikAPIConnector::Join()
	{
		TRACE(_T("TradingAPIConnectionPoint::QuikAPIConnector::Join"));
		m_pConnector->Join();
	}

}