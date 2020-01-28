#include "stdafx.h"
#include "SharedMemServer.h"

namespace CommunicationsAPI
{
	CSharedMemServer::CSharedMemServer()
	{

	}


	CSharedMemServer::~CSharedMemServer()
	{

	}

	void CSharedMemServer::Dispose()
	{
		delete this;
	}
}
