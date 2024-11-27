#include "mylib.h"


int getFirstIP(char *buf,int size)
{
	// TODO: Add your control notification handler code here
	WORD wVersionRequested;
	WSADATA wsaData;
	int ret;
	//Start up WinSock
	wVersionRequested = MAKEWORD(1, 1);
	//The WSAStartup function returns zero if successful
	ret = WSAStartup(wVersionRequested, &wsaData);
	if (ret != 0){
		__dbg_printf("WSAStartup failed!");
		return LIB_ERROR;
	}

	hostent *p;
	char s[128];
	char *p2;

	//获得计算机名字
	gethostname(s, sizeof(s));
	p = gethostbyname(s);
	if (p == 0){
		__dbg_printf("gethostbyname failed!");
		return -1;
	}
	memset(buf, '\0',size);
	//获得IP地址
	p2 = inet_ntoa(*((in_addr *)p->h_addr));
	memcpy(buf, p2, strlen(p2));

	WSACleanup();
	return LIB_OK;
}
// create service
BOOL WINAPI InstallService(LPCTSTR ServiceName,
	LPCTSTR DisplayName,
	LPCTSTR szPath,
	DWORD StartType,
	DWORD ErrorControl = SERVICE_ERROR_NORMAL,
	DWORD ServiceType = SERVICE_WIN32_OWN_PROCESS){
	/*
	OpenSCManager 打开服务控制管理器
		lpMachineName：置NULL
		lpDatabaseName：服务控制管理器数据库，NULL表示默认数据库
		dwDesiredAccess：访问权限
			SC_MANAGER_CREATE_SERVICE：需要调用 CreateService 函数来创建一个服务。 
			SC_MANAGER_ALL_ACCESS：包括 STANDARD_RIGHTS_REQUIRED，除了在此表中的所有访问权限之外。
	*/
	SC_HANDLE hSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_CREATE_SERVICE);
	if (hSCManager != NULL){
		/*
		SC_HANDLE WINAPI CreateService(
		_In_      SC_HANDLE hSCManager,
		_In_      LPCTSTR   lpServiceName,
		_In_opt_  LPCTSTR   lpDisplayName,
		_In_      DWORD     dwDesiredAccess,
		_In_      DWORD     dwServiceType,
		_In_      DWORD     dwStartType,
		_In_      DWORD     dwErrorControl,
		_In_opt_  LPCTSTR   lpBinaryPathName,
		_In_opt_  LPCTSTR   lpLoadOrderGroup,
		_Out_opt_ LPDWORD   lpdwTagId,
		_In_opt_  LPCTSTR   lpDependencies,
		_In_opt_  LPCTSTR   lpServiceStartName,
		_In_opt_  LPCTSTR   lpPassword
		);
		CreateService 创建服务
			hSCManager：服务控制管理器句柄。此句柄由 OpenSCManager 函数返回，并且必须具有 SC_MANAGER_CREATE_SERVICE 的访问权限。
			lpServiceName：要安装的服务的名称。最大字符串长度为 256 个字符。服务控制管理器数据库保留字符的大小写，即服务名称区分大小写。正斜线 (/) 和反斜杠 (\) 不是有效的服务名称字符。
			lpDisplayName：要由用户界面程序用于标识服务的显示名称。该字符串可以有最大长度为 256 个字符。
			dwDesiredAccess：访问权限，要创建服务置SERVICE_ALL_ACCESS。
			dwServiceType：服务类型。此参数可以是下列值之一：
				SERVICE_ADAPTER：保留。
				SERVICE_FILE_SYSTEM_DRIVER：文件系统驱动程序服务。
				SERVICE_KERNEL_DRIVER：驱动程序服务。
				SERVICE_RECOGNIZER_DRIVER：保留。
				SERVICE_WIN32_OWN_PROCESS：在它自己的进程中运行的服务。
				SERVICE_WIN32_SHARE_PROCESS：与一个或多个其他服务共享一个进程的服务。
				如果您指定 SERVICE_WIN32_OWN_PROCESS 或 SERVICE_WIN32_SHARE_PROCESS，并且该服务运行在本地系统帐户的上下文中，您还可以指定以下值：
				SERVICE_INTERACTIVE_PROCESS：服务可以与桌面交互。
			dwStartType：该服务启动选项。此参数可以是下列值之一：
				SERVICE_AUTO_START：由服务控制管理器在系统启动时自动启动的服务。（自动）
				SERVICE_BOOT_START：由系统加载程序启动的设备驱动程序。此值是仅对驱动程序服务有效。
				SERVICE_DEMAND_START：当一个过程调用 StartService 函数由服务控制管理器启动的服务。（手动）
				SERVICE_DISABLED：无法启动服务。（禁用）
				SERVICE_SYSTEM_START：由 IoInitSystem 函数启动的设备驱动程序。此值是仅对驱动程序服务有效。
			dwErrorControl：严重性错误，以及采取的行动，如果这项服务无法启动。此参数可以是下列值之一：
				SERVICE_ERROR_CRITICAL：如果可能的话，启动程序在事件日志中记录错误。如果上次已知正确的配置正在启动，启动操作将失败。否则，与最后一次正确的配置重新启动系统。
				SERVICE_ERROR_IGNORE：启动程序忽略错误并继续启动操作。
				SERVICE_ERROR_NORMAL：启动程序记录错误事件日志但继续启动操作。
				SERVICE_ERROR_SEVERE：启动程序在事件日志中记录错误。如果上次已知的正确配置正被启动，启动操作将继续。否则，与上次已知的正确配置重新启动系统。
			lpBinaryPathName：服务二进制文件的完全限定的路径。如果路径中包含空格，它必须被引用，例如，"d:\\my share\\myservice.exe"应该被指定为"\"d:\\my share\\myservice.exe\""。
			lpLoadOrderGroup：加载顺序的组此服务是成员的名称。如果该服务不属于组，指定为 NULL 或空字符串。
			lpdwTagId：指向接收是唯一在 lpLoadOrderGroup 参数中指定组中的一个标记值的变量的指针。如果您不更改现有标记，请指定 NULL。
			lpDependencies：指向的 NULL 分隔名称的服务或加载顺序组系统必须启动此服务前双空终止数组的指针。如果该服务没有依赖项，指定为 NULL 或空字符串。
			lpServiceStartName：服务应在其下运行的帐户的名称。如果此参数为 NULL，CreateService 将使用本地系统帐户。如果服务类型指定SERVICE_INTERACTIVE_PROCESS，该服务必须运行在本地系统帐户。
			lpPassword：指定的帐户的密码。如果该帐户没有密码，或在本地、 网络服务或本地系统帐户运行该服务，请指定空字符串。
			返回值：如果此函数成功，返回值是服务的句柄。如果函数失败，返回值为 NULL。
		*/
		SC_HANDLE hService = CreateService(hSCManager, ServiceName, DisplayName, SERVICE_ALL_ACCESS, ServiceType, StartType, ErrorControl, szPath, NULL, NULL, NULL, NULL, TEXT(""));
		if (hService != NULL){
			CloseServiceHandle(hService);
			CloseServiceHandle(hSCManager);
			return TRUE;
		}
		/*
		CloseServiceHandle 关闭服务控制管理器句柄或服务句柄
			hSCObject：要释放的服务控制管理器句柄或服务句柄
		*/
		CloseServiceHandle(hSCManager);
		return FALSE;
	}
	else return FALSE;
}

//向服务发送控制码
//这个代码可以在管理员权限的程序中执行也可以在非管理员权限的进程中执行。原因是权限设定：SC_MANAGER_CONNECT和SERVICE_USER_DEFINED_CONTROL限制了服务句柄只能用于发送用户自定义控制码。
BOOL WINAPI ControlServiceCustomCode(LPCTSTR ServiceName,DWORD dwControl){
	SC_HANDLE hSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_CONNECT);
	if (hSCManager != NULL){
		SC_HANDLE hService = OpenService(hSCManager, ServiceName, SERVICE_USER_DEFINED_CONTROL);
		if (hService != NULL){
			SERVICE_STATUS status;
			if (!ControlService(hService, dwControl, &status)){
				//发生控制码失败  
				__dbg_printf("发生控制码失败");
				return 0;
			}
			CloseServiceHandle(hService);
		}
		else{
			//OpenService失败  
			__dbg_printf("OpenService失败 ");
			return 0;
		}
		CloseServiceHandle(hSCManager);
	}
	else{
		//OpenSCManager失败  
		__dbg_printf("OpenSCManager失败 ");
		return 0;
	}
	return 1;
}


//delete service
BOOL WINAPI DeleteService(LPCTSTR ServiceName){
	SC_HANDLE hSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_CONNECT);
	if (hSCManager != NULL){
		/*
		SC_HANDLE WINAPI OpenService(
		_In_ SC_HANDLE hSCManager,
		_In_ LPCTSTR   lpServiceName,
		_In_ DWORD     dwDesiredAccess
		);
		hSCManager：服务控制管理器句柄。此句柄由 OpenSCManager 函数返回。
		lpServiceName：要打开的服务的名称。这是由指向函数的参数的 CreateService 时，创建服务对象指定的名称，不是显示名称。
		dwDesiredAccess：打开权限
			SERVICE_ALL_ACCESS：包括 STANDARD_RIGHTS_REQUIRED，除了在此表中的所有访问权限。
			SERVICE_CHANGE_CONFIG：需要调用 ChangeServiceConfig 或 ChangeServiceConfig2 函数来更改服务配置。因为这授予调用方有权修改系统运行的可执行文件，它应只授予管理员。
			SERVICE_ENUMERATE_DEPENDENTS：需要调用 EnumDependentServices 函数来枚举所有依赖于该服务的服务。
			SERVICE_INTERROGATE：需要调用ControlService函数来要求服务报告他的运行状态。
			SERVICE_PAUSE_CONTINUE：调用这个函数所需暂停或继续服务。
			SERVICE_QUERY_CONFIG：调用的 QueryServiceConfig 和 QueryServiceConfig2 的功能查询的服务配置。
			SERVICE_QUERY_STATUS：需要调用 QueryServiceStatus 或 QueryServiceStatusEx 的功能询问服务的状态。要调用 NotifyServiceStatusChange 函数来接收通知时服务的状态改变时需要此权限。
			SERVICE_START：需要调用 StartService 函数来启动该服务。
			SERVICE_STOP：调用 ControlService 函数停止服务。
			SERVICE_USER_DEFINED_CONTROL：调用 ControlService 函数来发送指定用户定义的控制代码。
			DELETE：需要删除服务
		返回值：如果此函数成功，返回值是服务的句柄。如果函数失败，返回值为 NULL。
		*/
		SC_HANDLE hService = OpenService(hSCManager, ServiceName, DELETE);
		if (hService != NULL){
			/*
			DeleteService 删除服务
				hService：服务句柄，需要有DELETE权限。
			返回值：如果此函数成功，返回值不为零。如果函数失败，返回值为零。
			*/
			if (DeleteService(hService))
			{
				CloseServiceHandle(hService);
				CloseServiceHandle(hSCManager);
				return TRUE;
			}
			CloseServiceHandle(hService);
			CloseServiceHandle(hSCManager);
			return FALSE;
		}
		CloseServiceHandle(hSCManager);
		return FALSE;
	}
	else return FALSE;
}
/*
QueryServiceStatus 获取服务状态
BOOL WINAPI QueryServiceStatus(
_In_  SC_HANDLE        hService,
_Out_ LPSERVICE_STATUS lpServiceStatus
);
hService：服务句柄，必须有SERVICE_QUERY_STATUS权限
lpServiceStatus：指向SERVICE_STATUS结构的指针，用于返回服务状态
	typedef struct _SERVICE_STATUS {
		DWORD dwServiceType;
		DWORD dwCurrentState; //服务状态
			//SERVICE_CONTINUE_PENDING：服务继续处于挂起状态。
			//SERVICE_PAUSE_PENDING：服务暂停被挂起。
			//SERVICE_PAUSED：服务已暂停。
			//SERVICE_RUNNING：该服务正在运行。
			//SERVICE_START_PENDING：服务正在启动。
			//SERVICE_STOP_PENDING：服务正在停止。
			//SERVICE_STOPPED：服务没有运行。
		DWORD dwControlsAccepted;
		DWORD dwWin32ExitCode;
		DWORD dwServiceSpecificExitCode;
		DWORD dwCheckPoint;
		DWORD dwWaitHint;
	} SERVICE_STATUS, *LPSERVICE_STATUS;
*/
/*
StartService 启动服务
BOOL WINAPI StartService(
_In_     SC_HANDLE hService,
_In_     DWORD     dwNumServiceArgs,
_In_opt_ LPCTSTR   *lpServiceArgVectors
);
hService：服务句柄，需要有SERVICE_START权限。
dwNumServiceArgs：LpServiceArgVectors 数组中的字符串的数目。如果 lpServiceArgVectors 为 NULL，则此参数可以是零。
lpServiceArgVectors：要作为参数传递给 ServiceMain 的字符串。如果没有参数，此参数可以为 NULL。否则为第一个参数 (lpServiceArgVectors[0]) 是服务的名称，其后任何附加参数。
返回值：如果此函数成功，返回值不为零。如果函数失败，返回值为零。
*/
/*
ControlService 发送控制码
BOOL WINAPI ControlService(
_In_  SC_HANDLE        hService,
_In_  DWORD            dwControl,
_Out_ LPSERVICE_STATUS lpServiceStatus
);
hService：服务句柄
dwControl：控制码，
SERVICE_CONTROL_STOP：停止服务，服务句柄需要SERVICE_STOP权限
128-255：自定义控制码，服务句柄需要SERVICE_USER_DEFINED_CONTROL权限
lpServiceStatus：指向接收最新的服务状态信息的 SERVICE_STATUS 结构的指针。SERVICE_STATUS 结构见上面
返回值：如果此函数成功，返回值不为零。如果函数失败，返回值为零。
*/
/*
void ShowMessage(LPWSTR lpszMessage, LPWSTR lpszTitle)
{
	// 获得当前Session ID  
	DWORD dwSession = WTSGetActiveConsoleSessionId();
	DWORD dwResponse = 0;

	// 显示消息对话框  
	WTSSendMessageW(WTS_CURRENT_SERVER_HANDLE, dwSession,
		lpszTitle,
		static_cast<DWORD>((wcslen(lpszTitle) + 1) * sizeof(wchar_t)),
		lpszMessage,
		static_cast<DWORD>((wcslen(lpszMessage) + 1) * sizeof(wchar_t)),
		0, 0, &dwResponse, FALSE);
}
服务程序开发 Session 0隔离
void CreateUserProcess(LPCTSTR Filename)
{
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	RtlZeroMemory(&si, sizeof(STARTUPINFO));
	RtlZeroMemory(&pi, sizeof(PROCESS_INFORMATION));
	si.cb = sizeof(STARTUPINFO);
	//si.lpDesktop = TEXT("Winsta0\\default");  

	HANDLE hToken, hDuplicatedToken = NULL;
	WTSQueryUserToken(WTSGetActiveConsoleSessionId(), &hToken);//获取用户令牌  
	DuplicateTokenEx(hToken, MAXIMUM_ALLOWED, NULL, SecurityIdentification, TokenPrimary, &hDuplicatedToken);//复制令牌  
	LPVOID lpEnvironment = NULL;
	CreateEnvironmentBlock(&lpEnvironment, hDuplicatedToken, FALSE);//创建当前用户环境  

	//在当前用户创建进程（CREATE_UNICODE_ENVIRONMENT表示用户环境是Unicode字符串）  
	if (CreateProcessAsUser(hDuplicatedToken, Filename, NULL, NULL, NULL, FALSE, CREATE_NEW_CONSOLE | CREATE_UNICODE_ENVIRONMENT, lpEnvironment, NULL, &si, &pi) == 0){
		ShowMessage(L"在用户界面创建进程失败", L"Error");
	}

	//释放资源  
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);
	CloseHandle(hToken);
	CloseHandle(hDuplicatedToken);
	DestroyEnvironmentBlock(lpEnvironment);
}
void CreateUserSysProcess(LPCTSTR Filename)
{
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	RtlZeroMemory(&si, sizeof(STARTUPINFO));
	RtlZeroMemory(&pi, sizeof(PROCESS_INFORMATION));
	si.cb = sizeof(STARTUPINFO);
	//si.lpDesktop = TEXT("Winsta0\\default");  

	HANDLE hToken, hDuplicatedToken = NULL;
	OpenProcessToken(GetCurrentProcess(), TOKEN_ALL_ACCESS, &hToken);//获取服务自己的令牌  
	DuplicateTokenEx(hToken, TOKEN_ALL_ACCESS, NULL, SecurityIdentification, TokenPrimary, &hDuplicatedToken);//复制令牌  
	DWORD SessionId = WTSGetActiveConsoleSessionId();//获取会话ID  
	SetTokenInformation(hDuplicatedToken, TokenSessionId, &SessionId, sizeof(DWORD));//设置令牌信息TokenSessionId  
	LPVOID lpEnvironment = NULL;
	CreateEnvironmentBlock(&lpEnvironment, hDuplicatedToken, FALSE);//创建用户环境  

	//在当前用户创建进程（CREATE_UNICODE_ENVIRONMENT表示用户环境是Unicode字符串）  
	if (CreateProcessAsUser(hDuplicatedToken, Filename, NULL, NULL, NULL, FALSE, CREATE_NEW_CONSOLE | CREATE_UNICODE_ENVIRONMENT, lpEnvironment, NULL, &si, &pi) == 0){
		ShowMessage(L"在用户界面创建进程失败", L"Error");
	}

	//释放资源  
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);
	CloseHandle(hToken);
	CloseHandle(hDuplicatedToken);
	DestroyEnvironmentBlock(lpEnvironment);
}
*/