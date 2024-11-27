#include "main.h"
#include "LogLoginTime.h"
#define MAX_DBG_MSG_LEN 1024


extern HWND g_hwnd;

void LogEvent(LPCTSTR format, ...)
{
	FILE * logfp = NULL;
	//if (logfp == NULL){
		logfp = fopen("C:\\log\\info.log", "a+");
	//}
	char buf[MAX_DBG_MSG_LEN];
	va_list ap;
	va_start(ap, format);
	_vsnprintf(buf, sizeof(buf), format, ap);
	va_end(ap);
	fwrite(buf, 1, strlen(buf), logfp);
	fclose(logfp);
	logfp = NULL;
}
void logTime(char * temp){
	char tmp[1024] = { 0 };
	time_t t = time(0);

	strftime(tmp, sizeof(tmp), "[%Y/%m/%d %X %A]", localtime(&t));//[2018/05/10 13:47:06 Thursday]
	strcat(tmp, " ");
	if (temp != NULL)
		strcat(tmp, temp);
	strcat(tmp, "\r\n");
	LogEvent(tmp);
}
  


SERVICE_STATUS_HANDLE hStatus;//服务状态句柄  
SERVICE_STATUS ServiceStatus;//当前服务的状态信息  
HANDLE hThread = NULL;
void cleanup() {
	PostMessage(g_hwnd, WM_DESTROY, 0, 0);
	if (hThread) {
		CloseHandle(hThread);
		hThread = NULL;
	}
}
BOOL EnablePriv()

{

	HANDLE hToken;

	if (OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES, &hToken))

	{

		TOKEN_PRIVILEGES tkp;

		LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &tkp.Privileges[0].Luid);//修改进程权限

		tkp.PrivilegeCount = 1;

		tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

		AdjustTokenPrivileges(hToken, FALSE, &tkp, sizeof tkp, NULL, NULL);//通知系统修改进程权限

		return((GetLastError() == ERROR_SUCCESS));

	}

	return TRUE;

}
DWORD WINAPI HandlerEx(_In_ DWORD  dwControl, _In_ DWORD  dwEventType, _In_ LPVOID lpEventData, _In_ LPVOID lpContext)
{
	__dbg_printf("---Enter HandlerEx--- dwControl : %x",dwControl);
	switch (dwControl)
	{
	case SERVICE_CONTROL_SESSIONCHANGE:
		switch (dwEventType)
		{
		case WTS_SESSION_LOGOFF:
			__dbg_printf("******WTS_SESSION_LOGOFF***");
			logTime("session is logoff");
			break;
		case WTS_SESSION_LOGON:
			__dbg_printf("******WTS_SESSION_LOGON***");
			logTime("session is logon");
			break;
		case WTS_SESSION_LOCK:
			__dbg_printf("******WTS_SESSION_LOCK***");
			logTime("session is lock");
			break;
		case WTS_SESSION_UNLOCK:
			__dbg_printf("******WTS_SESSION_UNLOCK***");
			logTime("session is unlock");
			break;
		default:
			break;
		}
		break;
	case SERVICE_CONTROL_STOP://控制代码：要求停止停止  
		__dbg_printf("---HandlerEx  SERVICE_CONTROL_STOP---");
		cleanup();

		ServiceStatus.dwWin32ExitCode = 0;
		ServiceStatus.dwCurrentState = SERVICE_STOPPED;
		
		SetServiceStatus(hStatus, &ServiceStatus);//报告服务运行状态  
		
		goto HandlerEx_exit;
	case SERVICE_CONTROL_SHUTDOWN://控制代码：关机  
		__dbg_printf("---HandlerEx  SERVICE_CONTROL_SHUTDOWN---");
		cleanup();

		ServiceStatus.dwWin32ExitCode = 0;
		ServiceStatus.dwCurrentState = SERVICE_STOPPED;
		SetServiceStatus(hStatus, &ServiceStatus);//报告服务运行状态  
		logTime("system is shutting down\n");
		
		goto HandlerEx_exit;
	case SERVICE_CONTROL_CONTINUE:
		__dbg_printf("---HandlerEx  SERVICE_CONTROL_CONTINUE---");
		ServiceStatus.dwWin32ExitCode = 0;
		ServiceStatus.dwCurrentState = SERVICE_RUNNING;
		SetServiceStatus(hStatus, &ServiceStatus);//报告服务运行状态 
		goto HandlerEx_exit;
	case SERVICE_CONTROL_INTERROGATE: 
		//Notifies a service to report its current status information to the service control manager.
		//The handler should simply return NO_ERROR; the SCM is aware of the current state of the service
		break;
	case CONTROL_MOUSE_BUTTONDOWN:
		__dbg_printf("---HandlerEx  CONTROL_MOUSE_BUTTONDOWN---");
		break;
	default:
		break;
	}
	ServiceStatus.dwCurrentState = SERVICE_RUNNING;
	SetServiceStatus(hStatus, &ServiceStatus);//报告服务运行状态  
HandlerEx_exit:
	__dbg_printf("---Leave HandlerEx---");
	return NO_ERROR;
}

VOID WINAPI ServiceMain(_In_ DWORD  dwArgc, _In_ LPTSTR *lpszArgv)
{
	__dbg_printf("---Enter ServiceMain---");
	hStatus = RegisterServiceCtrlHandlerEx(myServiceName, &HandlerEx, NULL);
	if (hStatus == (SERVICE_STATUS_HANDLE)0)
	{
		//这里是对RegisterServiceCtrlHandler失败的处理  
		return;
	}

	RtlZeroMemory(&ServiceStatus, sizeof(SERVICE_STATUS));//结构体清空  
	ServiceStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
	ServiceStatus.dwCurrentState = SERVICE_RUNNING;
	ServiceStatus.dwControlsAccepted = SERVICE_ACCEPT_SESSIONCHANGE | SERVICE_ACCEPT_SHUTDOWN | SERVICE_ACCEPT_STOP;//接受关机和停止控制  
	SetServiceStatus(hStatus, &ServiceStatus);//报告服务运行状态  

	if (ServiceStatus.dwCurrentState == SERVICE_RUNNING){
		//只有在它报告SERVICE_RUNNING状态后，服务代码才可以调用系统函数
		//__dbg_printf("[thread]CreateThread");
		//EnablePriv();
		//hThread = CreateThread(NULL, 0, CreateLogLoginTime, NULL, 0, NULL);
		
		
	}
	__dbg_printf("---Leave ServiceMain---");
}

int WINAPI WinMain(HINSTANCE hinstance, HINSTANCE hprevinstance, LPSTR lpcmdline, int ncmdshow)
{
	__dbg_printf("---Enter WinMain---");
	logTime("program start now");
	SERVICE_TABLE_ENTRY ServiceTable[2];
	ServiceTable[0].lpServiceName = myServiceName;
	ServiceTable[0].lpServiceProc = (LPSERVICE_MAIN_FUNCTION)&ServiceMain;
	ServiceTable[1].lpServiceName = NULL;
	ServiceTable[1].lpServiceProc = NULL;

	StartServiceCtrlDispatcher(ServiceTable);
	__dbg_printf("---Leave WinMain---");
	return 0;
}