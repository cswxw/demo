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

	//��ü��������
	gethostname(s, sizeof(s));
	p = gethostbyname(s);
	if (p == 0){
		__dbg_printf("gethostbyname failed!");
		return -1;
	}
	memset(buf, '\0',size);
	//���IP��ַ
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
	OpenSCManager �򿪷�����ƹ�����
		lpMachineName����NULL
		lpDatabaseName��������ƹ��������ݿ⣬NULL��ʾĬ�����ݿ�
		dwDesiredAccess������Ȩ��
			SC_MANAGER_CREATE_SERVICE����Ҫ���� CreateService ����������һ������ 
			SC_MANAGER_ALL_ACCESS������ STANDARD_RIGHTS_REQUIRED�������ڴ˱��е����з���Ȩ��֮�⡣
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
		CreateService ��������
			hSCManager��������ƹ�����������˾���� OpenSCManager �������أ����ұ������ SC_MANAGER_CREATE_SERVICE �ķ���Ȩ�ޡ�
			lpServiceName��Ҫ��װ�ķ�������ơ�����ַ�������Ϊ 256 ���ַ���������ƹ��������ݿⱣ���ַ��Ĵ�Сд���������������ִ�Сд����б�� (/) �ͷ�б�� (\) ������Ч�ķ��������ַ���
			lpDisplayName��Ҫ���û�����������ڱ�ʶ�������ʾ���ơ����ַ�����������󳤶�Ϊ 256 ���ַ���
			dwDesiredAccess������Ȩ�ޣ�Ҫ����������SERVICE_ALL_ACCESS��
			dwServiceType���������͡��˲�������������ֵ֮һ��
				SERVICE_ADAPTER��������
				SERVICE_FILE_SYSTEM_DRIVER���ļ�ϵͳ�����������
				SERVICE_KERNEL_DRIVER�������������
				SERVICE_RECOGNIZER_DRIVER��������
				SERVICE_WIN32_OWN_PROCESS�������Լ��Ľ��������еķ���
				SERVICE_WIN32_SHARE_PROCESS����һ����������������һ�����̵ķ���
				�����ָ�� SERVICE_WIN32_OWN_PROCESS �� SERVICE_WIN32_SHARE_PROCESS�����Ҹ÷��������ڱ���ϵͳ�ʻ����������У���������ָ������ֵ��
				SERVICE_INTERACTIVE_PROCESS��������������潻����
			dwStartType���÷�������ѡ��˲�������������ֵ֮һ��
				SERVICE_AUTO_START���ɷ�����ƹ�������ϵͳ����ʱ�Զ������ķ��񡣣��Զ���
				SERVICE_BOOT_START����ϵͳ���س����������豸�������򡣴�ֵ�ǽ����������������Ч��
				SERVICE_DEMAND_START����һ�����̵��� StartService �����ɷ�����ƹ����������ķ��񡣣��ֶ���
				SERVICE_DISABLED���޷��������񡣣����ã�
				SERVICE_SYSTEM_START���� IoInitSystem �����������豸�������򡣴�ֵ�ǽ����������������Ч��
			dwErrorControl�������Դ����Լ���ȡ���ж��������������޷��������˲�������������ֵ֮һ��
				SERVICE_ERROR_CRITICAL��������ܵĻ��������������¼���־�м�¼��������ϴ���֪��ȷ��������������������������ʧ�ܡ����������һ����ȷ��������������ϵͳ��
				SERVICE_ERROR_IGNORE������������Դ��󲢼�������������
				SERVICE_ERROR_NORMAL�����������¼�����¼���־����������������
				SERVICE_ERROR_SEVERE�������������¼���־�м�¼��������ϴ���֪����ȷ�������������������������������������ϴ���֪����ȷ������������ϵͳ��
			lpBinaryPathName������������ļ�����ȫ�޶���·�������·���а����ո������뱻���ã����磬"d:\\my share\\myservice.exe"Ӧ�ñ�ָ��Ϊ"\"d:\\my share\\myservice.exe\""��
			lpLoadOrderGroup������˳�����˷����ǳ�Ա�����ơ�����÷��������飬ָ��Ϊ NULL ����ַ�����
			lpdwTagId��ָ�������Ψһ�� lpLoadOrderGroup ������ָ�����е�һ�����ֵ�ı�����ָ�롣��������������б�ǣ���ָ�� NULL��
			lpDependencies��ָ��� NULL �ָ����Ƶķ�������˳����ϵͳ���������˷���ǰ˫����ֹ�����ָ�롣����÷���û�������ָ��Ϊ NULL ����ַ�����
			lpServiceStartName������Ӧ���������е��ʻ������ơ�����˲���Ϊ NULL��CreateService ��ʹ�ñ���ϵͳ�ʻ��������������ָ��SERVICE_INTERACTIVE_PROCESS���÷�����������ڱ���ϵͳ�ʻ���
			lpPassword��ָ�����ʻ������롣������ʻ�û�����룬���ڱ��ء� �������򱾵�ϵͳ�ʻ����и÷�����ָ�����ַ�����
			����ֵ������˺����ɹ�������ֵ�Ƿ���ľ�����������ʧ�ܣ�����ֵΪ NULL��
		*/
		SC_HANDLE hService = CreateService(hSCManager, ServiceName, DisplayName, SERVICE_ALL_ACCESS, ServiceType, StartType, ErrorControl, szPath, NULL, NULL, NULL, NULL, TEXT(""));
		if (hService != NULL){
			CloseServiceHandle(hService);
			CloseServiceHandle(hSCManager);
			return TRUE;
		}
		/*
		CloseServiceHandle �رշ�����ƹ���������������
			hSCObject��Ҫ�ͷŵķ�����ƹ���������������
		*/
		CloseServiceHandle(hSCManager);
		return FALSE;
	}
	else return FALSE;
}

//������Ϳ�����
//�����������ڹ���ԱȨ�޵ĳ�����ִ��Ҳ�����ڷǹ���ԱȨ�޵Ľ�����ִ�С�ԭ����Ȩ���趨��SC_MANAGER_CONNECT��SERVICE_USER_DEFINED_CONTROL�����˷�����ֻ�����ڷ����û��Զ�������롣
BOOL WINAPI ControlServiceCustomCode(LPCTSTR ServiceName,DWORD dwControl){
	SC_HANDLE hSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_CONNECT);
	if (hSCManager != NULL){
		SC_HANDLE hService = OpenService(hSCManager, ServiceName, SERVICE_USER_DEFINED_CONTROL);
		if (hService != NULL){
			SERVICE_STATUS status;
			if (!ControlService(hService, dwControl, &status)){
				//����������ʧ��  
				__dbg_printf("����������ʧ��");
				return 0;
			}
			CloseServiceHandle(hService);
		}
		else{
			//OpenServiceʧ��  
			__dbg_printf("OpenServiceʧ�� ");
			return 0;
		}
		CloseServiceHandle(hSCManager);
	}
	else{
		//OpenSCManagerʧ��  
		__dbg_printf("OpenSCManagerʧ�� ");
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
		hSCManager��������ƹ�����������˾���� OpenSCManager �������ء�
		lpServiceName��Ҫ�򿪵ķ�������ơ�������ָ�����Ĳ����� CreateService ʱ�������������ָ�������ƣ�������ʾ���ơ�
		dwDesiredAccess����Ȩ��
			SERVICE_ALL_ACCESS������ STANDARD_RIGHTS_REQUIRED�������ڴ˱��е����з���Ȩ�ޡ�
			SERVICE_CHANGE_CONFIG����Ҫ���� ChangeServiceConfig �� ChangeServiceConfig2 ���������ķ������á���Ϊ��������÷���Ȩ�޸�ϵͳ���еĿ�ִ���ļ�����Ӧֻ�������Ա��
			SERVICE_ENUMERATE_DEPENDENTS����Ҫ���� EnumDependentServices ������ö�����������ڸ÷���ķ���
			SERVICE_INTERROGATE����Ҫ����ControlService������Ҫ����񱨸���������״̬��
			SERVICE_PAUSE_CONTINUE�������������������ͣ���������
			SERVICE_QUERY_CONFIG�����õ� QueryServiceConfig �� QueryServiceConfig2 �Ĺ��ܲ�ѯ�ķ������á�
			SERVICE_QUERY_STATUS����Ҫ���� QueryServiceStatus �� QueryServiceStatusEx �Ĺ���ѯ�ʷ����״̬��Ҫ���� NotifyServiceStatusChange ����������֪ͨʱ�����״̬�ı�ʱ��Ҫ��Ȩ�ޡ�
			SERVICE_START����Ҫ���� StartService �����������÷���
			SERVICE_STOP������ ControlService ����ֹͣ����
			SERVICE_USER_DEFINED_CONTROL������ ControlService ����������ָ���û�����Ŀ��ƴ��롣
			DELETE����Ҫɾ������
		����ֵ������˺����ɹ�������ֵ�Ƿ���ľ�����������ʧ�ܣ�����ֵΪ NULL��
		*/
		SC_HANDLE hService = OpenService(hSCManager, ServiceName, DELETE);
		if (hService != NULL){
			/*
			DeleteService ɾ������
				hService������������Ҫ��DELETEȨ�ޡ�
			����ֵ������˺����ɹ�������ֵ��Ϊ�㡣�������ʧ�ܣ�����ֵΪ�㡣
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
QueryServiceStatus ��ȡ����״̬
BOOL WINAPI QueryServiceStatus(
_In_  SC_HANDLE        hService,
_Out_ LPSERVICE_STATUS lpServiceStatus
);
hService����������������SERVICE_QUERY_STATUSȨ��
lpServiceStatus��ָ��SERVICE_STATUS�ṹ��ָ�룬���ڷ��ط���״̬
	typedef struct _SERVICE_STATUS {
		DWORD dwServiceType;
		DWORD dwCurrentState; //����״̬
			//SERVICE_CONTINUE_PENDING������������ڹ���״̬��
			//SERVICE_PAUSE_PENDING��������ͣ������
			//SERVICE_PAUSED����������ͣ��
			//SERVICE_RUNNING���÷����������С�
			//SERVICE_START_PENDING����������������
			//SERVICE_STOP_PENDING����������ֹͣ��
			//SERVICE_STOPPED������û�����С�
		DWORD dwControlsAccepted;
		DWORD dwWin32ExitCode;
		DWORD dwServiceSpecificExitCode;
		DWORD dwCheckPoint;
		DWORD dwWaitHint;
	} SERVICE_STATUS, *LPSERVICE_STATUS;
*/
/*
StartService ��������
BOOL WINAPI StartService(
_In_     SC_HANDLE hService,
_In_     DWORD     dwNumServiceArgs,
_In_opt_ LPCTSTR   *lpServiceArgVectors
);
hService������������Ҫ��SERVICE_STARTȨ�ޡ�
dwNumServiceArgs��LpServiceArgVectors �����е��ַ�������Ŀ����� lpServiceArgVectors Ϊ NULL����˲����������㡣
lpServiceArgVectors��Ҫ��Ϊ�������ݸ� ServiceMain ���ַ��������û�в������˲�������Ϊ NULL������Ϊ��һ������ (lpServiceArgVectors[0]) �Ƿ�������ƣ�����κθ��Ӳ�����
����ֵ������˺����ɹ�������ֵ��Ϊ�㡣�������ʧ�ܣ�����ֵΪ�㡣
*/
/*
ControlService ���Ϳ�����
BOOL WINAPI ControlService(
_In_  SC_HANDLE        hService,
_In_  DWORD            dwControl,
_Out_ LPSERVICE_STATUS lpServiceStatus
);
hService��������
dwControl�������룬
SERVICE_CONTROL_STOP��ֹͣ���񣬷�������ҪSERVICE_STOPȨ��
128-255���Զ�������룬��������ҪSERVICE_USER_DEFINED_CONTROLȨ��
lpServiceStatus��ָ��������µķ���״̬��Ϣ�� SERVICE_STATUS �ṹ��ָ�롣SERVICE_STATUS �ṹ������
����ֵ������˺����ɹ�������ֵ��Ϊ�㡣�������ʧ�ܣ�����ֵΪ�㡣
*/
/*
void ShowMessage(LPWSTR lpszMessage, LPWSTR lpszTitle)
{
	// ��õ�ǰSession ID  
	DWORD dwSession = WTSGetActiveConsoleSessionId();
	DWORD dwResponse = 0;

	// ��ʾ��Ϣ�Ի���  
	WTSSendMessageW(WTS_CURRENT_SERVER_HANDLE, dwSession,
		lpszTitle,
		static_cast<DWORD>((wcslen(lpszTitle) + 1) * sizeof(wchar_t)),
		lpszMessage,
		static_cast<DWORD>((wcslen(lpszMessage) + 1) * sizeof(wchar_t)),
		0, 0, &dwResponse, FALSE);
}
������򿪷� Session 0����
void CreateUserProcess(LPCTSTR Filename)
{
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	RtlZeroMemory(&si, sizeof(STARTUPINFO));
	RtlZeroMemory(&pi, sizeof(PROCESS_INFORMATION));
	si.cb = sizeof(STARTUPINFO);
	//si.lpDesktop = TEXT("Winsta0\\default");  

	HANDLE hToken, hDuplicatedToken = NULL;
	WTSQueryUserToken(WTSGetActiveConsoleSessionId(), &hToken);//��ȡ�û�����  
	DuplicateTokenEx(hToken, MAXIMUM_ALLOWED, NULL, SecurityIdentification, TokenPrimary, &hDuplicatedToken);//��������  
	LPVOID lpEnvironment = NULL;
	CreateEnvironmentBlock(&lpEnvironment, hDuplicatedToken, FALSE);//������ǰ�û�����  

	//�ڵ�ǰ�û��������̣�CREATE_UNICODE_ENVIRONMENT��ʾ�û�������Unicode�ַ�����  
	if (CreateProcessAsUser(hDuplicatedToken, Filename, NULL, NULL, NULL, FALSE, CREATE_NEW_CONSOLE | CREATE_UNICODE_ENVIRONMENT, lpEnvironment, NULL, &si, &pi) == 0){
		ShowMessage(L"���û����洴������ʧ��", L"Error");
	}

	//�ͷ���Դ  
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
	OpenProcessToken(GetCurrentProcess(), TOKEN_ALL_ACCESS, &hToken);//��ȡ�����Լ�������  
	DuplicateTokenEx(hToken, TOKEN_ALL_ACCESS, NULL, SecurityIdentification, TokenPrimary, &hDuplicatedToken);//��������  
	DWORD SessionId = WTSGetActiveConsoleSessionId();//��ȡ�ỰID  
	SetTokenInformation(hDuplicatedToken, TokenSessionId, &SessionId, sizeof(DWORD));//����������ϢTokenSessionId  
	LPVOID lpEnvironment = NULL;
	CreateEnvironmentBlock(&lpEnvironment, hDuplicatedToken, FALSE);//�����û�����  

	//�ڵ�ǰ�û��������̣�CREATE_UNICODE_ENVIRONMENT��ʾ�û�������Unicode�ַ�����  
	if (CreateProcessAsUser(hDuplicatedToken, Filename, NULL, NULL, NULL, FALSE, CREATE_NEW_CONSOLE | CREATE_UNICODE_ENVIRONMENT, lpEnvironment, NULL, &si, &pi) == 0){
		ShowMessage(L"���û����洴������ʧ��", L"Error");
	}

	//�ͷ���Դ  
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);
	CloseHandle(hToken);
	CloseHandle(hDuplicatedToken);
	DestroyEnvironmentBlock(lpEnvironment);
}
*/