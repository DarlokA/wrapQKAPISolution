#pragma once
#include "interface/CommunicationsAPI.h"

namespace CommunicationsAPI
{
	class CPipeClient : public CommunicationsAPI::IClientChannel
	{
	public:
		CPipeClient();
		~CPipeClient();
		virtual void	Dispose() override;
	private:

	};
}