void ErrInfo(DWORD errcode,CString & s)
{
	// Retrieve the system error message for the last-error code

	TCHAR * lpMsgBuf;
	
	DWORD dw = errcode;

	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		dw,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(TCHAR *)&lpMsgBuf,
		0, NULL);

	// Display the error message and exit the process
	s.Format(_T("错误码:%d, %ws"), dw,lpMsgBuf);

	LocalFree(lpMsgBuf);
	
}


