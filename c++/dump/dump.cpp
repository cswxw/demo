#include <windows.h>
#include <Dbghelp.h>
#include <shlwapi.h>
#include <tchar.h>
#include <strsafe.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#pragma comment(lib, "Shlwapi.lib")
#pragma comment(lib, "Dbghelp.lib")



DWORD CreateMiniDump(EXCEPTION_POINTERS *pExceptionPointers)
{
	CHAR	szFileName[MAX_PATH];
	HANDLE	hDumpFile = NULL;
	char lpErrStr[1024];
	DWORD nRetCode;
	MINIDUMP_EXCEPTION_INFORMATION	mdei;
	//
	memset(&mdei, 0x00, sizeof(MINIDUMP_EXCEPTION_INFORMATION));
	mdei.ThreadId	= GetCurrentThreadId();
	mdei.ExceptionPointers	= pExceptionPointers;
	mdei.ClientPointers		= FALSE;
	//
	// create dump file
	//
	memset(szFileName, 0x00, MAX_PATH);
	GetModuleFileName(NULL, szFileName, MAX_PATH);
	PathRemoveFileSpec(szFileName);
	strcat(szFileName, "\\MyDump.dmp");	
	hDumpFile = CreateFile(
			szFileName,
			GENERIC_READ | GENERIC_WRITE,
			FILE_SHARE_READ | FILE_SHARE_WRITE,
			NULL,
			CREATE_ALWAYS,
			FILE_ATTRIBUTE_NORMAL,
			NULL
			);
	if(hDumpFile == INVALID_HANDLE_VALUE)
	{
		nRetCode = GetLastError();
		sprintf(lpErrStr, "CreateFile: %d\n", nRetCode);
		goto EXIT;
	}
	//
	BOOL bRetCode = MiniDumpWriteDump(
		GetCurrentProcess(),
		GetCurrentProcessId(),
		hDumpFile,
		MiniDumpNormal,
		&mdei,
		NULL,
		NULL
		);
	if(bRetCode != TRUE)
		return(GetLastError());
EXIT: 
	return(0);
}

inline BOOL CALLBACK MiniDumpCallback(PVOID         pParam,  
                                  const PMINIDUMP_CALLBACK_INPUT   pInput,  
                                  PMINIDUMP_CALLBACK_OUTPUT        pOutput) 
 {
   return TRUE;
} 
   

//创建Dump文件 
   
inline void CreateMiniDump(EXCEPTION_POINTERS* pep, LPCTSTR strFileName) 
{ 
    HANDLE hFile = CreateFile(strFileName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL); 
   
    if((hFile != NULL) && (hFile != INVALID_HANDLE_VALUE)) 
    { 
       MINIDUMP_EXCEPTION_INFORMATION mdei; 
   
       mdei.ThreadId           = GetCurrentThreadId(); 
   
       mdei.ExceptionPointers  = pep; 
   
       mdei.ClientPointers     = FALSE; 
   
       MINIDUMP_CALLBACK_INFORMATION mci; 
   
       mci.CallbackRoutine     = (MINIDUMP_CALLBACK_ROUTINE)MiniDumpCallback; 
   
       mci.CallbackParam       = 0; 
   
       MINIDUMP_TYPE mdt       = (MINIDUMP_TYPE)0x0000ffff; 
   
       MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), hFile, MiniDumpNormal, &mdei, NULL, &mci); 
   
       CloseHandle(hFile);  
    } 
}  

LONG __stdcall MyUnhandledExceptionFilter(EXCEPTION_POINTERS* pExceptionInfo)
{
	CreateMiniDump(pExceptionInfo);
	return EXCEPTION_EXECUTE_HANDLER;
//EXCEPTION_EXECUTE_HANDLER equ 1 表示我已经处理了异常,可以优雅地结束了  
//EXCEPTION_CONTINUE_SEARCH equ 0 表示我不处理,其他人来吧,于是windows调用默认的处理程序显示一个错误框,并结束  
//EXCEPTION_CONTINUE_EXECUTION equ -1 表示错误已经被修复,请从异常发生处继续执行 
}


//生产DUMP文件
int GenerateMiniDump(HANDLE hFile, PEXCEPTION_POINTERS pExceptionPointers, PTCHAR pwAppName)
{
	BOOL bOwnDumpFile = FALSE;
	HANDLE hDumpFile = hFile;
	MINIDUMP_EXCEPTION_INFORMATION ExpParam;
 
	typedef BOOL(WINAPI * MiniDumpWriteDumpT)(
		HANDLE,
		DWORD,
		HANDLE,
		MINIDUMP_TYPE,
		PMINIDUMP_EXCEPTION_INFORMATION,
		PMINIDUMP_USER_STREAM_INFORMATION,
		PMINIDUMP_CALLBACK_INFORMATION
		);
 
	MiniDumpWriteDumpT pfnMiniDumpWriteDump = NULL;
	HMODULE hDbgHelp = LoadLibrary(_T("DbgHelp.dll"));
	if (hDbgHelp)
		pfnMiniDumpWriteDump = (MiniDumpWriteDumpT)GetProcAddress(hDbgHelp, "MiniDumpWriteDump");
 
	if (pfnMiniDumpWriteDump)
	{
		if (hDumpFile == NULL || hDumpFile == INVALID_HANDLE_VALUE)
		{
			TCHAR szPath[MAX_PATH] = { 0 };
			TCHAR szFileName[MAX_PATH] = { 0 };
			TCHAR* szAppName = pwAppName;
			TCHAR* szVersion = _T("v1.0");
			TCHAR dwBufferSize = MAX_PATH;
			SYSTEMTIME stLocalTime;
 
			GetLocalTime(&stLocalTime);
			GetTempPath(dwBufferSize, szPath);
 
			StringCchPrintf(szFileName, MAX_PATH, _T("%s%s"), szPath, szAppName);
			CreateDirectory(szFileName, NULL);
 
			StringCchPrintf(szFileName, MAX_PATH, _T("%s%s//%s-%04d%02d%02d-%02d%02d%02d-%ld-%ld.dmp"),
				szPath, szAppName, szVersion,
				stLocalTime.wYear, stLocalTime.wMonth, stLocalTime.wDay,
				stLocalTime.wHour, stLocalTime.wMinute, stLocalTime.wSecond,
				GetCurrentProcessId(), GetCurrentThreadId());
			hDumpFile = CreateFile(szFileName, GENERIC_READ | GENERIC_WRITE,
				FILE_SHARE_WRITE | FILE_SHARE_READ, 0, CREATE_ALWAYS, 0, 0);
 
			bOwnDumpFile = TRUE;
			OutputDebugString(szFileName);
		}
 
		if (hDumpFile != INVALID_HANDLE_VALUE)
		{
			ExpParam.ThreadId = GetCurrentThreadId();
			ExpParam.ExceptionPointers = pExceptionPointers;
			ExpParam.ClientPointers = FALSE;
 
			pfnMiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(),
				hDumpFile, MiniDumpWithDataSegs, (pExceptionPointers ? &ExpParam : NULL), NULL, NULL);
 
			if (bOwnDumpFile)
				CloseHandle(hDumpFile);
		}
	}
 
	if (hDbgHelp != NULL)
		FreeLibrary(hDbgHelp);
 
	return EXCEPTION_EXECUTE_HANDLER;
}
 
 
LONG WINAPI ExceptionFilter(LPEXCEPTION_POINTERS lpExceptionInfo)
{
	if (IsDebuggerPresent())
	{
		return EXCEPTION_CONTINUE_SEARCH;
	}
 
	return GenerateMiniDump(NULL, lpExceptionInfo, _T("test"));
}
 



void main()
{
 //SetUnhandledExceptionFilter(MyUnhandledExceptionFilter);
	SetUnhandledExceptionFilter(ExceptionFilter);
 *(int*)0=0; // AV
}



