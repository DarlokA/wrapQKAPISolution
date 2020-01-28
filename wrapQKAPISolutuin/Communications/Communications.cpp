// Communications.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "interface/CommunicationsAPI.h"
#include "PipeChannelServer.h"
#include "PipeClient.h"

CommunicationsAPI::IServerChannel* __cdecl  PipeServer()
{
	return new CommunicationsAPI::CPipeServer();
}

CommunicationsAPI::IClientChannel* __cdecl  PipeClient()
{
	return new CommunicationsAPI::CPipeClient();
}

CommunicationsAPI::IServerChannel* __cdecl  SharedMemServer()
{
	return NULL;
}

CommunicationsAPI::IClientChannel* __cdecl  SharedMemClient()
{
	return NULL;
}