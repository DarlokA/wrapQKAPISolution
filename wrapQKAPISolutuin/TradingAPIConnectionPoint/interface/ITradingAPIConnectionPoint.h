#pragma once 
inline _bstr_t makePipePointName(DWORD processID, _bstr_t point_name)
{
	CString res;
	res.Format(_T("\\\\.\\pipe\\%X%s"),processID, (LPCTSTR)point_name);
	return _bstr_t(res.GetString());
}


__interface  ITradingAPIConnectionPoint
{

};