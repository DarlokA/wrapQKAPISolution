#pragma once
#include "interface/CommunicationsAPI.h"

namespace CommunicationsAPI
{
	class CSharedMemServer : public IServerChannel
	{
	public:
		CSharedMemServer();
		~CSharedMemServer();

		// Inherited via IServerChannel
		virtual void Dispose() override;
	};
}

