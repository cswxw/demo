#include <windows.h>  

static TCHAR* name = TEXT("MyService");//服务名称  
SERVICE_STATUS_HANDLE hStatus;//服务状态句柄  
SERVICE_STATUS ServiceStatus;//当前服务的状态信息  
/*
用RegisterServiceCtrlHandlerEx函数应用程序定义的回调函数。服务程序可以使用它作为控制处理程序功能的特定服务。
dwControl：控制代码
		SERVICE_CONTROL_NETBINDDISABLE：通知网络服务及其绑定之一已被禁用。服务应该重读它的绑定信息和删除绑定。应用程序应改用插即用功能。
		SERVICE_CONTROL_NETBINDENABLE：通知网络服务已启用已禁用的绑定。服务应该重读它的绑定信息，并添加新的绑定。应用程序应改用插即用功能。
		SERVICE_CONTROL_NETBINDREMOVE：通知网络服务绑定的组件已被删除。服务应该重读它的绑定信息和取消绑定从已删除的组件。应用程序应改用插即用功能。
		SERVICE_CONTROL_PARAMCHANGE：通知服务特定于服务的启动参数已更改。服务应该重读其启动参数。
		SERVICE_CONTROL_PAUSE：通知它应该暂停服务。
		SERVICE_CONTROL_PRESHUTDOWN：通知服务，系统将关闭。在系统关机时需要额外的时间来执行清理任务等紧迫的时间的服务可以使用此通知。Windows Server 2003 和 Windows XP: 不支持此值。
		SERVICE_CONTROL_SHUTDOWN：通知系统正在关闭，所以这项服务可以执行清理任务的服务。请注意注册 SERVICE_CONTROL_PRESHUTDOWN 通知的服务无法接收此通知，因为他们已经停止了。如果服务接受此控制代码，它必须停止后它执行其清理任务并返回 NO_ERROR。SCM 发送此控制代码之后，它不会向服务发送其他控制代码。
		SERVICE_CONTROL_STOP：通知应停止服务。如果服务接受此控制代码，它必须在收到后停止并返回 NO_ERROR。SCM 发送此控制代码之后，它不会向服务发送其他控制代码。
	Windows XP: 如果服务返回 NO_ERROR，继续运行，它继续接收控制代码。这种行为与 Windows Server 2003 和 Windows XP sp2 开始改变了。
	此参数也可以是下面的扩展的控制代码之一。请注意，这些控制代码不支持由处理程序函数处理：。(该服务必须注册以接收这些通知，使用 RegisterDeviceNotification 函数。)
		SERVICE_CONTROL_DEVICEEVENT：通知服务设备事件。DwEventType 和 lpEventData 参数包含附加信息。
		SERVICE_CONTROL_HARDWAREPROFILECHANGE：通知已更改计算机的硬件配置文件服务。DwEventType 参数包含的其他信息。
		SERVICE_CONTROL_POWEREVENT：通知系统电源事件服务。DwEventType 参数包含的其他信息。如果 dwEventType 是 PBT_POWERSETTINGCHANGE，lpEventData 参数还包含其他信息。
		SERVICE_CONTROL_SESSIONCHANGE：通知服务的会话更改事件。请注意，只有在是否它是完全加载之前进行登录尝试只将用户登录的通知服务。DwEventType 和 lpEventData 参数包含附加信息。
		SERVICE_CONTROL_TIMECHANGE：通知服务的系统时间已更改。LpEventData 参数包含的其他信息。不使用 dwEventType 参数。Windows Server 2008、 Windows Vista、 Windows Server 2003 和 Windows XP: 此控制代码不受支持。
		SERVICE_CONTROL_TRIGGEREVENT:通知服务注册为已发生事件的服务触发事件。Windows Server 2008、 Windows Vista、 Windows Server 2003 和 Windows XP: 此控制代码不受支持。
		SERVICE_CONTROL_USERMODEREBOOT:通知服务的用户已开始重新启动。Windows Server 2008 R2、 Windows 7、 Windows Server 2008，Windows Vista、 Windows Server 2003 和 Windows XP: 此控制代码不受支持。
		用户自定义的控制代码：从 128 到 255。

dwEventType：已发生的事件的类型。（一般不需要）如果dwControl是SERVICE_CONTROL_DEVICEEVENT、SERVICE_CONTROL_HARDWAREPROFILECHANGE、SERVICE_CONTROL_POWEREVENT或SERVICE_CONTROL_SESSIONCHANGE，则使用此参数。否则，它是零。
	如果dwControl是SERVICE_CONTROL_DEVICEEVENT，此参数可以是下列值之一：
		DBT_DEVICEARRIVAL
		DBT_DEVICEREMOVECOMPLETE
		DBT_DEVICEQUERYREMOVE
		DBT_DEVICEQUERYREMOVEFAILED
		DBT_DEVICEREMOVEPENDING
		DBT_CUSTOMEVENT
	如果dwControl是SERVICE_CONTROL_HARDWAREPROFILECHANGE，此参数可以是下列值之一：
		DBT_CONFIGCHANGED
		DBT_QUERYCHANGECONFIG
		DBT_CONFIGCHANGECANCELED
	如果 dwControl 是 SERVICE_CONTROL_POWEREVENT，此参数可以指定 WM_POWERBROADCAST 消息的 wParam 参数中的值之一。如果 dwControl 是 SERVICE_CONTROL_SESSIONCHANGE，此参数可以指定 WM_WTSSESSION_CHANGE 消息的 wParam 参数中的值之一。
lpEventData：其他设备信息（一般不需要）
	如果 dwControl 是 SERVICE_CONTROL_DEVICEEVENT，此数据对应的 lParam 参数的应用程序接收作为 WM_DEVICECHANGE 消息的一部分。
	如果 dwControl 是 SERVICE_CONTROL_POWEREVENT 和 dwEventType 是 PBT_POWERSETTINGCHANGE，这个数据是指向 POWERBROADCAST_SETTING 结构的指针。
	如果 dwControl 是 SERVICE_CONTROL_SESSIONCHANGE，此参数是指向 WTSSESSION_NOTIFICATION 结构的指针。
	如果 dwControl 是 SERVICE_CONTROL_TIMECHANGE，这个数据是指向 SERVICE_TIMECHANGE_INFO 结构的指针。
lpContext：用户从RegisterServiceCtrlHandlerEx传递的数据。当多个服务共享一个进程时，lpContext参数可以帮助标识服务。
返回值：
	如果你的服务不处理控制，返回ERROR_CALL_NOT_IMPLEMENTED。然而，您的服务应可为返回SERVICE_CONTROL_INTERROGATE即使你的服务不能处理它。
	如果您服务处理SERVICE_CONTROL_STOP或SERVICE_CONTROL_SHUTDOWN时，返回NO_ERROR。
	如果您的服务处理 SERVICE_CONTROL_DEVICEEVENT，返回可授予请求和错误代码拒绝该请求。
	如果您的服务处理 SERVICE_CONTROL_HARDWAREPROFILECHANGE，返回可授予请求和错误代码拒绝该请求。
	如果您的服务处理 SERVICE_CONTROL_POWEREVENT，返回可授予请求和错误代码拒绝该请求。
	对于所有其他控制代码你服务句柄，返回 NO_ERROR。
*/
DWORD WINAPI HandlerEx(_In_ DWORD  dwControl, _In_ DWORD  dwEventType, _In_ LPVOID lpEventData, _In_ LPVOID lpContext)
{
	switch (dwControl)
	{
	case SERVICE_CONTROL_STOP://控制代码：要求停止停止  
		ServiceStatus.dwWin32ExitCode = 0;
		ServiceStatus.dwCurrentState = SERVICE_STOPPED;
		/*
		BOOL WINAPI SetServiceStatus(
		_In_ SERVICE_STATUS_HANDLE hServiceStatus,
		_In_ LPSERVICE_STATUS      lpServiceStatus
		);
		hServiceStatus：当前服务的状态信息结构的句柄。此句柄是由RegisterServiceCtrlHandlerEx函数返回的。
		lpServiceStatus：指向SERVICE_STATUS结构的指针，此结构包含调用服务的最新状态信息。
		返回值:如果此函数成功，返回值不为零。如果函数失败，返回值为零。
		typedef struct _SERVICE_STATUS {
			DWORD dwServiceType;
			DWORD dwCurrentState;
			DWORD dwControlsAccepted;
			DWORD dwWin32ExitCode;
			DWORD dwServiceSpecificExitCode;
			DWORD dwCheckPoint;
			DWORD dwWaitHint;
		} SERVICE_STATUS, *LPSERVICE_STATUS;
		dwServiceType：服务类型
			SERVICE_FILE_SYSTEM_DRIVER：服务是文件系统驱动程序。
			SERVICE_KERNEL_DRIVER：服务是一个设备驱动程序。
			SERVICE_WIN32_OWN_PROCESS：服务运行在它自己的进程。
			SERVICE_WIN32_SHARE_PROCESS：服务与其他服务共享一个进程。
			如果服务类型是 SERVICE_WIN32_OWN_PROCESS 或 SERVICE_WIN32_SHARE_PROCESS，并且该服务运行在本地系统帐户的上下文中，也可以指定以下类型：
			SERVICE_INTERACTIVE_PROCESS：服务可以与桌面交互。
		dwCurrentState：服务的当前状态。此成员可以是下列值之一。
			SERVICE_CONTINUE_PENDING：服务继续处于挂起状态。
			SERVICE_PAUSE_PENDING：服务暂停被挂起。
			SERVICE_PAUSED：服务已暂停。
			SERVICE_RUNNING：该服务正在运行。
			SERVICE_START_PENDING：服务正在启动。
			SERVICE_STOP_PENDING：服务正在停止。
			SERVICE_STOPPED：服务已停止。
		dwControlsAccepted：
			SERVICE_ACCEPT_NETBINDCHANGE：服务是网络组件，它可以接受更改其绑定中的没有被停止并重新启动。此控制代码允许接收 SERVICE_CONTROL_NETBINDADD、 SERVICE_CONTROL_NETBINDREMOVE、 SERVICE_CONTROL_NETBINDENABLE 和 SERVICE_CONTROL_NETBINDDISABLE 的通知服务。
			SERVICE_ACCEPT_PARAMCHANGE：该服务可以重读其启动参数没有被停止并重新启动。此控制代码允许接收 SERVICE_CONTROL_PARAMCHANGE 通知服务。
			SERVICE_ACCEPT_PAUSE_CONTINUE：可以暂停和继续服务。此控制代码允许接收 SERVICE_CONTROL_PAUSE 和 SERVICE_CONTROL_CONTINUE 通知服务。
			SERVICE_ACCEPT_PRESHUTDOWN：该服务可以执行 preshutdown 任务。此控制代码使服务能够接收 SERVICE_CONTROL_PRESHUTDOWN 通知。请注意，这个和 ControlServiceEx 不能发送此通知;只有系统可以发送它。Windows Server 2003 和 Windows XP: 不支持此值。
			SERVICE_ACCEPT_SHUTDOWN：系统关机时，将通知服务。此控制代码允许接收 SERVICE_CONTROL_SHUTDOWN 通知服务。请注意，这个和 ControlServiceEx 不能发送此通知;只有系统可以发送它。
			SERVICE_ACCEPT_STOP：可以停止该服务。此控制代码允许接收 SERVICE_CONTROL_STOP 通知服务。
			此成员还可以包含下面的扩展的控制代码，仅由 HandlerEx 支持。(请注意，这些控制代码无法通过ControlServiceEx发送)。
			SERVICE_ACCEPT_HARDWAREPROFILECHANGE：当计算机的硬件配置文件发生更改时，将通知服务。这使系统能够向服务发送 SERVICE_CONTROL_HARDWAREPROFILECHANGE 通知。
			SERVICE_ACCEPT_POWEREVENT：当计算机电源状态更改通知服务。这使系统能够向服务发送 SERVICE_CONTROL_POWEREVENT 通知。
			SERVICE_ACCEPT_SESSIONCHANGE：当计算机的会话状态发生更改时，将通知服务。这使系统能够向服务发送 SERVICE_CONTROL_SESSIONCHANGE 通知。
			SERVICE_ACCEPT_TIMECHANGE：当系统时间已更改通知服务。这使系统能够向服务发送 SERVICE_CONTROL_TIMECHANGE 通知。Windows Server 2008、 Windows Vista、 Windows Server 2003 和 Windows XP: 此控制代码不受支持。
			SERVICE_ACCEPT_TRIGGEREVENT：服务注册为该事件发生时通知服务。这使系统能够向服务发送 SERVICE_CONTROL_TRIGGEREVENT 通知。Windows Server 2008、 Windows Vista、 Windows Server 2003 和 Windows XP: 此控制代码不受支持。
			SERVICE_ACCEPT_USERMODEREBOOT：当用户启动重新启动时，将通知服务。Windows Server 2008 R2、 Windows 7、 Windows Server 2008，Windows Vista、 Windows Server 2003 和 Windows XP: 此控制代码不受支持。
		dwWin32ExitCode：错误代码服务使用报告当它启动或停止时发生的错误。若要返回服务特定的错误代码，该服务必须将此值设置ERROR_SERVICE_SPECIFIC_ERROR以指示dwServiceSpecificExitCode成员包含错误代码。服务运行时和正常终止应将此值设置成NO_ERROR。
		dwServiceSpecificExitCode：dwWin32ExitCode成员设置ERROR_SERVICE_SPECIFIC_ERROR时，此参数表示该服务返回发生错误而服务正在启动或停止时的服务特定的错误代码。否则忽略此值。
		dwCheckPoint：置0即可
		dwWaitHint为挂起的启动,所需的估计时间停止.暂停或继续操作;以毫秒为单位
		*/
		SetServiceStatus(hStatus, &ServiceStatus);//报告服务运行状态  
		return NO_ERROR;
	case SERVICE_CONTROL_SHUTDOWN://控制代码：关机  
		ServiceStatus.dwWin32ExitCode = 0;
		ServiceStatus.dwCurrentState = SERVICE_STOPPED;
		SetServiceStatus(hStatus, &ServiceStatus);//报告服务运行状态  
		return NO_ERROR;
	default:
		break;
	}
	ServiceStatus.dwCurrentState = SERVICE_RUNNING;
	SetServiceStatus(hStatus, &ServiceStatus);//报告服务运行状态  
	return NO_ERROR;
}
/*
ServiceMain函数：（服务启动回调函数）
	dwArgc：lpszArgv数组中字符串个数
	lpszArgv：启动参数

	当服务控制管理器收到请求启动服务时，便会启动服务进程(如果尚未运行)。服务进程的主线程调用StartServiceCtrlDispatcher函数用到的SERVICE_TABLE_ENTRY结构数组的指针。然后服务控制管理器将启动请求发送到服务控制调度程序为此服务的进程。服务控制调度程序将创建一个新线程来执行服务正在启动ServiceMain函数。
	ServiceMain函数应立即调用RegisterServiceCtrlHandlerEx函数来指定一个HandlerEx函数来处理控制请求。接下来，它应该调用SetServiceStatus函数来将状态信息发送到服务控制管理器。这些调用完成之后，该函数应完成初始化服务。
	不应在服务初始化期间调用任何系统的功能。只有在它报告SERVICE_RUNNING状态后，服务代码才可以调用系统函数。
*/
VOID WINAPI ServiceMain(_In_ DWORD  dwArgc, _In_ LPTSTR *lpszArgv)
{
	/*
	//函数功能：注册一个函数来处理扩展的服务控制请求。
	SERVICE_STATUS_HANDLE WINAPI RegisterServiceCtrlHandlerEx(
	_In_     LPCTSTR               lpServiceName,
	_In_     LPHANDLER_FUNCTION_EX lpHandlerProc,
	_In_opt_ LPVOID                lpContext
	);
	lpServiceName：通过调用线程运行的服务的名称。这是服务控制程序在创建服务时，CreateService函数中指定的服务名称。（和上面说的SERVICE_TABLE_ENTRY中的lpServiceName一致即可）
	lpHandlerProc：指向要注册的处理程序函数（HandlerEx）的指针
	lpContext：上下文。当多个服务共享一个进程，表示任何用户定义的数据。此参数传递给处理程序函数，可以帮助识别服务。（可空）
	返回值:如果此函数成功，返回值是服务状态的句柄。如果函数失败，返回值为零。
	*/
	hStatus = RegisterServiceCtrlHandlerEx(name, &HandlerEx, NULL);
	if (hStatus == (SERVICE_STATUS_HANDLE)0)
	{
		//这里是对RegisterServiceCtrlHandler失败的处理  
		return;
	}

	RtlZeroMemory(&ServiceStatus, sizeof(SERVICE_STATUS));//结构体清空  
	ServiceStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
	ServiceStatus.dwCurrentState = SERVICE_RUNNING;
	ServiceStatus.dwControlsAccepted = SERVICE_ACCEPT_SHUTDOWN | SERVICE_ACCEPT_STOP;//接受关机和停止控制  
	SetServiceStatus(hStatus, &ServiceStatus);//报告服务运行状态  

	if (ServiceStatus.dwCurrentState == SERVICE_RUNNING){
		//只有在它报告SERVICE_RUNNING状态后，服务代码才可以调用系统函数
	}
}

int WINAPI WinMain(HINSTANCE hinstance, HINSTANCE hprevinstance, LPSTR lpcmdline, int ncmdshow)
{
	SERVICE_TABLE_ENTRY ServiceTable[2];
	ServiceTable[0].lpServiceName = name;
	ServiceTable[0].lpServiceProc = (LPSERVICE_MAIN_FUNCTION)&ServiceMain;
	ServiceTable[1].lpServiceName = NULL;
	ServiceTable[1].lpServiceProc = NULL;
	/*
	函数功能：连接到服务控制管理器
	参数：一个SERVICE_TABLE_ENTRY结构的指针，成员表中的最后一项必须具有NULL值，来指定表的末尾。
		如果此函数成功，返回值不为零。如果函数失败，返回值为零。
		typedef struct _SERVICE_TABLE_ENTRY {
			LPTSTR                  lpServiceName;//要在此服务进程中运行的服务名称，不能和其他程序的重了。我一般把这个名称设为注册到系统的服务名称一致。
			                                      //如果服务安装的是SERVICE_WIN32_OWN_PROCESS服务类型，此成员将被忽略，但不能为 NULL，可以是空字符串("")
			LPSERVICE_MAIN_FUNCTION lpServiceProc; //指向ServiceMain函数。
		} SERVICE_TABLE_ENTRY, *LPSERVICE_TABLE_ENTRY;
	*/
	StartServiceCtrlDispatcher(ServiceTable);
	return 0;
}