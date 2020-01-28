// TradingAPIConnectorService.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "main.h"
#include "QuikAPIConnector.h"

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE	hInst;                                // current instance
HANDLE		hMutexOneInstance;
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWndClass[MAX_LOADSTRING];                  // The title bar text
HWND m_hWnd; //Handle to appWindow

//global resources

// Forward declarations of functions included in this code module:
int		RunMyWindow();
ATOM    MyRegisterClass(HINSTANCE hInstance);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
int		RunInstance(HINSTANCE, LPWSTR);
bool	FileExist(_bstr_t fname);
bool	isExistThisPoint(_bstr_t quik_path);



int		RunMyWindow()
{
	if (!MyRegisterClass(hInst))
	{
		DWORD error_code = ::GetLastError();
		TRACE(L"MyRegisterClass return error_code = %d Exit(-2)", error_code);
		return -2;
	}

	m_hWnd = CreateWindowW(szWndClass, szTitle, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInst, nullptr);

	if (!m_hWnd)
	{
		DWORD error_code = ::GetLastError();
		TRACE(L"CreateWindowW return error_code = %d Exit(-3)", error_code);
		return -3;
	}

	ShowWindow(m_hWnd, SW_HIDE);
	UpdateWindow(m_hWnd);


	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return (int)msg.wParam;
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
	hMutexOneInstance = NULL;
	m_hWnd = NULL;

	LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadStringW(hInstance, IDS_WND_CLASS, szWndClass, MAX_LOADSTRING);

	Logger::Init(szTitle, Logger::LMBoth);
	TRACE(L"Run TradingAPIConnectionPoint");

    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(nCmdShow);

	
	int res = RunInstance(hInstance, lpCmdLine);
	/*if (res == 0)
	{
		res = RunMyWindow();
	}*/

	if (hMutexOneInstance != NULL)
	{
		::ReleaseMutex(hMutexOneInstance);
		::CloseHandle(hMutexOneInstance);
	}

	return res;
}



bool isExistThisPoint(_bstr_t quik_path)
{
	_bstr_t name = TradingAPIConnector::APIConnectorPoint::MakePointInstanceName(quik_path);
	hMutexOneInstance = ::CreateMutexW(NULL, FALSE, (LPCWSTR)name);
	int err_code = ::GetLastError();
	TRACE(L"::CreateMutexW return %d", err_code);
	
	ASSERT(hMutexOneInstance != NULL);
	if (err_code == ERROR_ALREADY_EXISTS)
		hMutexOneInstance = NULL;

	return hMutexOneInstance == NULL;
}

int RunInstance(HINSTANCE hInstance, LPWSTR lpCmdLine)
{
	_bstr_t quik_path(lpCmdLine);
	TRACE(L"RunInstance hInstance=0x%p quik_path =\"%s\"", hInstance,(LPCWSTR)quik_path);
	hInst = hInstance; // Store instance handle in our global variable
	if (quik_path.length() && FileExist(quik_path))
	{
		if (isExistThisPoint(quik_path))
		{
			TRACE(L"RunInstance return 1 (OK ExistThisPoint)");
			//Уже запущен инстанс с этим терминалом Quik
			return 1;
		}
		TRACE(L"RunInstanceThisPoint");
		TradingAPIConnectionPoint::QuikAPIConnector connector;
		TRACE(L"start wait connections.....");
		connector.Join();
		TRACE(L"RunInstance return 0 (OK)");
		return 0;
	}
	TRACE(L"RunInstance return -1 (quik_path not Exist)");
	return -1;
}

bool FileExist(_bstr_t fname)
{
	return::GetFileAttributesW((LPCWSTR)fname) != DWORD(-1);
}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = 0;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = NULL;
	wcex.hCursor = NULL;
	wcex.hbrBackground = NULL;
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = szWndClass;
	wcex.hIconSm = NULL;

	return RegisterClassExW(&wcex);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_DESTROY:
		TRACE(L"WM_DESTROY");
		PostQuitMessage(0);
		break;
	default:
		TRACE(L"message for %p = %X", hWnd, message);
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}