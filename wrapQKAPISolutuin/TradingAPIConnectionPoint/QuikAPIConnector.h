#pragma once
#include "../TradingAPIConnector/interface/TradingAPIConnectorInterface.h"
#include "../Communications/interface/CommunicationsAPI.h"
#include "interface/ITradingAPIConnectionPoint.h"


namespace TradingAPIConnectionPoint
{
	class QuikAPIConnector
	{
	private:
		struct ServerChannelDestroyer
		{
			ServerChannelDestroyer();
			void operator () (CommunicationsAPI::IServerChannel* p_server_channel) const;
		};

		struct SApiConnectorDestroyer
		{
			SApiConnectorDestroyer();
			void operator () (TradingAPIConnector::ITradingConnector* pconnector) const;
		};

	private:
		std::unique_ptr<CommunicationsAPI::IServerChannel, QuikAPIConnector::ServerChannelDestroyer> m_pConnector;
		std::unique_ptr<TradingAPIConnector::ITradingConnector, QuikAPIConnector::SApiConnectorDestroyer> m_pQuikAPI;
	public:
		QuikAPIConnector();
		~QuikAPIConnector();
	public:
		void Join();
	};
}