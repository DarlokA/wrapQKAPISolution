// wrapQKAPIConsoleTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "..\Logger\Interface\LogApi.h"
#include "..\TradingAPIConnector\interface\TradingAPIConnectorInterface.h"
#include "wrapQKAPIConsoleTest.h"


void TestTradingAPIConnector_dll()
{
	std::unique_ptr<TradingAPIConnector::ITradingConnector, TradingAPIConnector::SApiConnectorDestroyer> pconnector(APIConnector(), TradingAPIConnector::SApiConnectorDestroyer());
	TRACE(L"CreateConnector()");
	ASSERT(pconnector != NULL);
	if (pconnector)
	{
		pconnector->Connect("c:\\Program Files\\QUIK-Junior\\info.exe", false);
		pconnector->WaitForConnectionWork();
		while (pconnector->GetConnectionState() != TradingAPIConnector::QKConnectionState::QKCS_Connected)
		{
			::Sleep(1000);
		}
		pconnector->Disconnect();
		//SApiConnectorDestroyer call delete pconnector object now
	}
}

void TestTradingAPIConnectionPoint_exe()
{
	_bstr_t point_service_path = GetMyRunDir();
	point_service_path += L"TradingAPIConnectionPoint.exe";
	RunProcess(point_service_path, L"C:\\Program Files\\QUIK-Junior\\info.exe");
}

int main()
{
	Logger::Init(L"wrapQKAPIConsoleTest", Logger::LMBoth);
	//TestTradingAPIConnector_dll();
	TestTradingAPIConnectionPoint_exe();
	//TestTradingAPIConnector_dll();
    return 0;
}

