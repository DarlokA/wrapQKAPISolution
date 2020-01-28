#include "stdafx.h"
#include "Logger2File.h"
#include <shlobj.h>

namespace Logger
{
	CLogger2File::CLogger2File(LPCWSTR root_folder)
		:base()
	{
		HMODULE hMoule = GetModuleHandle(NULL);
		DWORD pid = ::GetCurrentProcessId();
		DWORD sz = 1024;
		CString root(root_folder);
		CString module_name;
		CString dir;
		sz = GetModuleFileNameW(hMoule, module_name.GetBufferSetLength(sz), sz);
		int pos = module_name.ReverseFind(L'\\');
		module_name = module_name.Right(module_name.GetLength() - pos);
		pos = module_name.ReverseFind(L'.');
		module_name = module_name.Left(pos);
		module_name = module_name.Trim(_T('\\'));
		CString locAppPath;
		sz = 1024;
		SHGetSpecialFolderPathW(NULL, locAppPath.GetBufferSetLength(sz), CSIDL_LOCAL_APPDATA, FALSE);
		if (root.GetLength())//Создаем корневую папку
		{
			dir.Format(L"%s\\%s", locAppPath.GetString(), root.GetString());
			CreateDirectoryW(dir, NULL);
		}
		m_file_path.Format(L"%s\\%X_%s.log", dir.GetString(), pid, module_name.GetString());
		FILE* stream = NULL;
		wprintf(L"Creating log file %s\n", m_file_path.GetString());//TODO: убрать 
		if (ERROR_SUCCESS == _wfopen_s(&stream, m_file_path, L"w") && stream != NULL)
			fclose(stream);

	}

	CLogger2File::~CLogger2File()
	{
	}


	void CLogger2File::outMsg(const WCHAR *pszMsg)
	{
		if (pszMsg == NULL) return;
		FILE* stream;
		if (ERROR_SUCCESS == _wfopen_s(&stream, m_file_path, L"a") && stream != NULL)
		{
			__try
			{
				fwprintf_s(stream, pszMsg);
			}
			__finally
			{
				fclose(stream);
			}
		}
	}

}