#pragma once
#ifdef COMMUNICATOR_DLL
#define COMMUNICATOR_API __declspec(dllexport) 
#else
#define COMMUNICATOR_API __declspec(dllimport) 
#endif




namespace CommunicationsAPI
{
	
	
	__interface COMMUNICATOR_API IServerChannel
	{
		virtual bool Start(_bstr_t name) = 0;
		virtual bool Join() = 0;
		virtual void Stop() = 0;
		virtual void Dispose() = 0;
	};

	__interface COMMUNICATOR_API IClientChannel
	{
		virtual void	Dispose() = 0;
	};

}


COMMUNICATOR_API CommunicationsAPI::IServerChannel*	__cdecl PipeServer();
COMMUNICATOR_API CommunicationsAPI::IClientChannel*	__cdecl PipeClient();
COMMUNICATOR_API CommunicationsAPI::IServerChannel*	__cdecl SharedMemServer();
COMMUNICATOR_API CommunicationsAPI::IClientChannel*	__cdecl SharedMemClient();