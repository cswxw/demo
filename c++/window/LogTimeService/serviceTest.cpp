#include <windows.h>  

static TCHAR* name = TEXT("MyService");//��������  
SERVICE_STATUS_HANDLE hStatus;//����״̬���  
SERVICE_STATUS ServiceStatus;//��ǰ�����״̬��Ϣ  
/*
��RegisterServiceCtrlHandlerEx����Ӧ�ó�����Ļص�����������������ʹ������Ϊ���ƴ�������ܵ��ض�����
dwControl�����ƴ���
		SERVICE_CONTROL_NETBINDDISABLE��֪ͨ����������֮һ�ѱ����á�����Ӧ���ض����İ���Ϣ��ɾ���󶨡�Ӧ�ó���Ӧ���ò弴�ù��ܡ�
		SERVICE_CONTROL_NETBINDENABLE��֪ͨ��������������ѽ��õİ󶨡�����Ӧ���ض����İ���Ϣ��������µİ󶨡�Ӧ�ó���Ӧ���ò弴�ù��ܡ�
		SERVICE_CONTROL_NETBINDREMOVE��֪ͨ�������󶨵�����ѱ�ɾ��������Ӧ���ض����İ���Ϣ��ȡ���󶨴���ɾ���������Ӧ�ó���Ӧ���ò弴�ù��ܡ�
		SERVICE_CONTROL_PARAMCHANGE��֪ͨ�����ض��ڷ�������������Ѹ��ġ�����Ӧ���ض�������������
		SERVICE_CONTROL_PAUSE��֪ͨ��Ӧ����ͣ����
		SERVICE_CONTROL_PRESHUTDOWN��֪ͨ����ϵͳ���رա���ϵͳ�ػ�ʱ��Ҫ�����ʱ����ִ����������Ƚ��ȵ�ʱ��ķ������ʹ�ô�֪ͨ��Windows Server 2003 �� Windows XP: ��֧�ִ�ֵ��
		SERVICE_CONTROL_SHUTDOWN��֪ͨϵͳ���ڹرգ���������������ִ����������ķ�����ע��ע�� SERVICE_CONTROL_PRESHUTDOWN ֪ͨ�ķ����޷����մ�֪ͨ����Ϊ�����Ѿ�ֹͣ�ˡ����������ܴ˿��ƴ��룬������ֹͣ����ִ�����������񲢷��� NO_ERROR��SCM ���ʹ˿��ƴ���֮����������������������ƴ��롣
		SERVICE_CONTROL_STOP��֪ͨӦֹͣ�������������ܴ˿��ƴ��룬���������յ���ֹͣ������ NO_ERROR��SCM ���ʹ˿��ƴ���֮����������������������ƴ��롣
	Windows XP: ������񷵻� NO_ERROR���������У����������տ��ƴ��롣������Ϊ�� Windows Server 2003 �� Windows XP sp2 ��ʼ�ı��ˡ�
	�˲���Ҳ�������������չ�Ŀ��ƴ���֮һ����ע�⣬��Щ���ƴ��벻֧���ɴ��������������(�÷������ע���Խ�����Щ֪ͨ��ʹ�� RegisterDeviceNotification ������)
		SERVICE_CONTROL_DEVICEEVENT��֪ͨ�����豸�¼���DwEventType �� lpEventData ��������������Ϣ��
		SERVICE_CONTROL_HARDWAREPROFILECHANGE��֪ͨ�Ѹ��ļ������Ӳ�������ļ�����DwEventType ����������������Ϣ��
		SERVICE_CONTROL_POWEREVENT��֪ͨϵͳ��Դ�¼�����DwEventType ����������������Ϣ����� dwEventType �� PBT_POWERSETTINGCHANGE��lpEventData ����������������Ϣ��
		SERVICE_CONTROL_SESSIONCHANGE��֪ͨ����ĻỰ�����¼�����ע�⣬ֻ�����Ƿ�������ȫ����֮ǰ���е�¼����ֻ���û���¼��֪ͨ����DwEventType �� lpEventData ��������������Ϣ��
		SERVICE_CONTROL_TIMECHANGE��֪ͨ�����ϵͳʱ���Ѹ��ġ�LpEventData ����������������Ϣ����ʹ�� dwEventType ������Windows Server 2008�� Windows Vista�� Windows Server 2003 �� Windows XP: �˿��ƴ��벻��֧�֡�
		SERVICE_CONTROL_TRIGGEREVENT:֪ͨ����ע��Ϊ�ѷ����¼��ķ��񴥷��¼���Windows Server 2008�� Windows Vista�� Windows Server 2003 �� Windows XP: �˿��ƴ��벻��֧�֡�
		SERVICE_CONTROL_USERMODEREBOOT:֪ͨ������û��ѿ�ʼ����������Windows Server 2008 R2�� Windows 7�� Windows Server 2008��Windows Vista�� Windows Server 2003 �� Windows XP: �˿��ƴ��벻��֧�֡�
		�û��Զ���Ŀ��ƴ��룺�� 128 �� 255��

dwEventType���ѷ������¼������͡���һ�㲻��Ҫ�����dwControl��SERVICE_CONTROL_DEVICEEVENT��SERVICE_CONTROL_HARDWAREPROFILECHANGE��SERVICE_CONTROL_POWEREVENT��SERVICE_CONTROL_SESSIONCHANGE����ʹ�ô˲��������������㡣
	���dwControl��SERVICE_CONTROL_DEVICEEVENT���˲�������������ֵ֮һ��
		DBT_DEVICEARRIVAL
		DBT_DEVICEREMOVECOMPLETE
		DBT_DEVICEQUERYREMOVE
		DBT_DEVICEQUERYREMOVEFAILED
		DBT_DEVICEREMOVEPENDING
		DBT_CUSTOMEVENT
	���dwControl��SERVICE_CONTROL_HARDWAREPROFILECHANGE���˲�������������ֵ֮һ��
		DBT_CONFIGCHANGED
		DBT_QUERYCHANGECONFIG
		DBT_CONFIGCHANGECANCELED
	��� dwControl �� SERVICE_CONTROL_POWEREVENT���˲�������ָ�� WM_POWERBROADCAST ��Ϣ�� wParam �����е�ֵ֮һ����� dwControl �� SERVICE_CONTROL_SESSIONCHANGE���˲�������ָ�� WM_WTSSESSION_CHANGE ��Ϣ�� wParam �����е�ֵ֮һ��
lpEventData�������豸��Ϣ��һ�㲻��Ҫ��
	��� dwControl �� SERVICE_CONTROL_DEVICEEVENT�������ݶ�Ӧ�� lParam ������Ӧ�ó��������Ϊ WM_DEVICECHANGE ��Ϣ��һ���֡�
	��� dwControl �� SERVICE_CONTROL_POWEREVENT �� dwEventType �� PBT_POWERSETTINGCHANGE�����������ָ�� POWERBROADCAST_SETTING �ṹ��ָ�롣
	��� dwControl �� SERVICE_CONTROL_SESSIONCHANGE���˲�����ָ�� WTSSESSION_NOTIFICATION �ṹ��ָ�롣
	��� dwControl �� SERVICE_CONTROL_TIMECHANGE�����������ָ�� SERVICE_TIMECHANGE_INFO �ṹ��ָ�롣
lpContext���û���RegisterServiceCtrlHandlerEx���ݵ����ݡ������������һ������ʱ��lpContext�������԰�����ʶ����
����ֵ��
	�����ķ��񲻴�����ƣ�����ERROR_CALL_NOT_IMPLEMENTED��Ȼ�������ķ���Ӧ��Ϊ����SERVICE_CONTROL_INTERROGATE��ʹ��ķ����ܴ�������
	�����������SERVICE_CONTROL_STOP��SERVICE_CONTROL_SHUTDOWNʱ������NO_ERROR��
	������ķ����� SERVICE_CONTROL_DEVICEEVENT�����ؿ���������ʹ������ܾ�������
	������ķ����� SERVICE_CONTROL_HARDWAREPROFILECHANGE�����ؿ���������ʹ������ܾ�������
	������ķ����� SERVICE_CONTROL_POWEREVENT�����ؿ���������ʹ������ܾ�������
	���������������ƴ���������������� NO_ERROR��
*/
DWORD WINAPI HandlerEx(_In_ DWORD  dwControl, _In_ DWORD  dwEventType, _In_ LPVOID lpEventData, _In_ LPVOID lpContext)
{
	switch (dwControl)
	{
	case SERVICE_CONTROL_STOP://���ƴ��룺Ҫ��ֹֹͣͣ  
		ServiceStatus.dwWin32ExitCode = 0;
		ServiceStatus.dwCurrentState = SERVICE_STOPPED;
		/*
		BOOL WINAPI SetServiceStatus(
		_In_ SERVICE_STATUS_HANDLE hServiceStatus,
		_In_ LPSERVICE_STATUS      lpServiceStatus
		);
		hServiceStatus����ǰ�����״̬��Ϣ�ṹ�ľ�����˾������RegisterServiceCtrlHandlerEx�������صġ�
		lpServiceStatus��ָ��SERVICE_STATUS�ṹ��ָ�룬�˽ṹ�������÷��������״̬��Ϣ��
		����ֵ:����˺����ɹ�������ֵ��Ϊ�㡣�������ʧ�ܣ�����ֵΪ�㡣
		typedef struct _SERVICE_STATUS {
			DWORD dwServiceType;
			DWORD dwCurrentState;
			DWORD dwControlsAccepted;
			DWORD dwWin32ExitCode;
			DWORD dwServiceSpecificExitCode;
			DWORD dwCheckPoint;
			DWORD dwWaitHint;
		} SERVICE_STATUS, *LPSERVICE_STATUS;
		dwServiceType����������
			SERVICE_FILE_SYSTEM_DRIVER���������ļ�ϵͳ��������
			SERVICE_KERNEL_DRIVER��������һ���豸��������
			SERVICE_WIN32_OWN_PROCESS���������������Լ��Ľ��̡�
			SERVICE_WIN32_SHARE_PROCESS������������������һ�����̡�
			������������� SERVICE_WIN32_OWN_PROCESS �� SERVICE_WIN32_SHARE_PROCESS�����Ҹ÷��������ڱ���ϵͳ�ʻ����������У�Ҳ����ָ���������ͣ�
			SERVICE_INTERACTIVE_PROCESS��������������潻����
		dwCurrentState������ĵ�ǰ״̬���˳�Ա����������ֵ֮һ��
			SERVICE_CONTINUE_PENDING������������ڹ���״̬��
			SERVICE_PAUSE_PENDING��������ͣ������
			SERVICE_PAUSED����������ͣ��
			SERVICE_RUNNING���÷����������С�
			SERVICE_START_PENDING����������������
			SERVICE_STOP_PENDING����������ֹͣ��
			SERVICE_STOPPED��������ֹͣ��
		dwControlsAccepted��
			SERVICE_ACCEPT_NETBINDCHANGE����������������������Խ��ܸ�������е�û�б�ֹͣ�������������˿��ƴ���������� SERVICE_CONTROL_NETBINDADD�� SERVICE_CONTROL_NETBINDREMOVE�� SERVICE_CONTROL_NETBINDENABLE �� SERVICE_CONTROL_NETBINDDISABLE ��֪ͨ����
			SERVICE_ACCEPT_PARAMCHANGE���÷�������ض�����������û�б�ֹͣ�������������˿��ƴ���������� SERVICE_CONTROL_PARAMCHANGE ֪ͨ����
			SERVICE_ACCEPT_PAUSE_CONTINUE��������ͣ�ͼ������񡣴˿��ƴ���������� SERVICE_CONTROL_PAUSE �� SERVICE_CONTROL_CONTINUE ֪ͨ����
			SERVICE_ACCEPT_PRESHUTDOWN���÷������ִ�� preshutdown ���񡣴˿��ƴ���ʹ�����ܹ����� SERVICE_CONTROL_PRESHUTDOWN ֪ͨ����ע�⣬����� ControlServiceEx ���ܷ��ʹ�֪ͨ;ֻ��ϵͳ���Է�������Windows Server 2003 �� Windows XP: ��֧�ִ�ֵ��
			SERVICE_ACCEPT_SHUTDOWN��ϵͳ�ػ�ʱ����֪ͨ���񡣴˿��ƴ���������� SERVICE_CONTROL_SHUTDOWN ֪ͨ������ע�⣬����� ControlServiceEx ���ܷ��ʹ�֪ͨ;ֻ��ϵͳ���Է�������
			SERVICE_ACCEPT_STOP������ֹͣ�÷��񡣴˿��ƴ���������� SERVICE_CONTROL_STOP ֪ͨ����
			�˳�Ա�����԰����������չ�Ŀ��ƴ��룬���� HandlerEx ֧�֡�(��ע�⣬��Щ���ƴ����޷�ͨ��ControlServiceEx����)��
			SERVICE_ACCEPT_HARDWAREPROFILECHANGE�����������Ӳ�������ļ���������ʱ����֪ͨ������ʹϵͳ�ܹ�������� SERVICE_CONTROL_HARDWAREPROFILECHANGE ֪ͨ��
			SERVICE_ACCEPT_POWEREVENT�����������Դ״̬����֪ͨ������ʹϵͳ�ܹ�������� SERVICE_CONTROL_POWEREVENT ֪ͨ��
			SERVICE_ACCEPT_SESSIONCHANGE����������ĻỰ״̬��������ʱ����֪ͨ������ʹϵͳ�ܹ�������� SERVICE_CONTROL_SESSIONCHANGE ֪ͨ��
			SERVICE_ACCEPT_TIMECHANGE����ϵͳʱ���Ѹ���֪ͨ������ʹϵͳ�ܹ�������� SERVICE_CONTROL_TIMECHANGE ֪ͨ��Windows Server 2008�� Windows Vista�� Windows Server 2003 �� Windows XP: �˿��ƴ��벻��֧�֡�
			SERVICE_ACCEPT_TRIGGEREVENT������ע��Ϊ���¼�����ʱ֪ͨ������ʹϵͳ�ܹ�������� SERVICE_CONTROL_TRIGGEREVENT ֪ͨ��Windows Server 2008�� Windows Vista�� Windows Server 2003 �� Windows XP: �˿��ƴ��벻��֧�֡�
			SERVICE_ACCEPT_USERMODEREBOOT�����û�������������ʱ����֪ͨ����Windows Server 2008 R2�� Windows 7�� Windows Server 2008��Windows Vista�� Windows Server 2003 �� Windows XP: �˿��ƴ��벻��֧�֡�
		dwWin32ExitCode������������ʹ�ñ��浱��������ֹͣʱ�����Ĵ�����Ҫ���ط����ض��Ĵ�����룬�÷�����뽫��ֵ����ERROR_SERVICE_SPECIFIC_ERROR��ָʾdwServiceSpecificExitCode��Ա����������롣��������ʱ��������ֹӦ����ֵ���ó�NO_ERROR��
		dwServiceSpecificExitCode��dwWin32ExitCode��Ա����ERROR_SERVICE_SPECIFIC_ERRORʱ���˲�����ʾ�÷��񷵻ط����������������������ֹͣʱ�ķ����ض��Ĵ�����롣������Դ�ֵ��
		dwCheckPoint����0����
		dwWaitHintΪ���������,����Ĺ���ʱ��ֹͣ.��ͣ���������;�Ժ���Ϊ��λ
		*/
		SetServiceStatus(hStatus, &ServiceStatus);//�����������״̬  
		return NO_ERROR;
	case SERVICE_CONTROL_SHUTDOWN://���ƴ��룺�ػ�  
		ServiceStatus.dwWin32ExitCode = 0;
		ServiceStatus.dwCurrentState = SERVICE_STOPPED;
		SetServiceStatus(hStatus, &ServiceStatus);//�����������״̬  
		return NO_ERROR;
	default:
		break;
	}
	ServiceStatus.dwCurrentState = SERVICE_RUNNING;
	SetServiceStatus(hStatus, &ServiceStatus);//�����������״̬  
	return NO_ERROR;
}
/*
ServiceMain�����������������ص�������
	dwArgc��lpszArgv�������ַ�������
	lpszArgv����������

	��������ƹ������յ�������������ʱ����������������(�����δ����)��������̵����̵߳���StartServiceCtrlDispatcher�����õ���SERVICE_TABLE_ENTRY�ṹ�����ָ�롣Ȼ�������ƹ����������������͵�������Ƶ��ȳ���Ϊ�˷���Ľ��̡�������Ƶ��ȳ��򽫴���һ�����߳���ִ�з�����������ServiceMain������
	ServiceMain����Ӧ��������RegisterServiceCtrlHandlerEx������ָ��һ��HandlerEx����������������󡣽���������Ӧ�õ���SetServiceStatus��������״̬��Ϣ���͵�������ƹ���������Щ�������֮�󣬸ú���Ӧ��ɳ�ʼ������
	��Ӧ�ڷ����ʼ���ڼ�����κ�ϵͳ�Ĺ��ܡ�ֻ����������SERVICE_RUNNING״̬�󣬷������ſ��Ե���ϵͳ������
*/
VOID WINAPI ServiceMain(_In_ DWORD  dwArgc, _In_ LPTSTR *lpszArgv)
{
	/*
	//�������ܣ�ע��һ��������������չ�ķ����������
	SERVICE_STATUS_HANDLE WINAPI RegisterServiceCtrlHandlerEx(
	_In_     LPCTSTR               lpServiceName,
	_In_     LPHANDLER_FUNCTION_EX lpHandlerProc,
	_In_opt_ LPVOID                lpContext
	);
	lpServiceName��ͨ�������߳����еķ�������ơ����Ƿ�����Ƴ����ڴ�������ʱ��CreateService������ָ���ķ������ơ���������˵��SERVICE_TABLE_ENTRY�е�lpServiceNameһ�¼��ɣ�
	lpHandlerProc��ָ��Ҫע��Ĵ����������HandlerEx����ָ��
	lpContext�������ġ������������һ�����̣���ʾ�κ��û���������ݡ��˲������ݸ���������������԰���ʶ����񡣣��ɿգ�
	����ֵ:����˺����ɹ�������ֵ�Ƿ���״̬�ľ�����������ʧ�ܣ�����ֵΪ�㡣
	*/
	hStatus = RegisterServiceCtrlHandlerEx(name, &HandlerEx, NULL);
	if (hStatus == (SERVICE_STATUS_HANDLE)0)
	{
		//�����Ƕ�RegisterServiceCtrlHandlerʧ�ܵĴ���  
		return;
	}

	RtlZeroMemory(&ServiceStatus, sizeof(SERVICE_STATUS));//�ṹ�����  
	ServiceStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
	ServiceStatus.dwCurrentState = SERVICE_RUNNING;
	ServiceStatus.dwControlsAccepted = SERVICE_ACCEPT_SHUTDOWN | SERVICE_ACCEPT_STOP;//���ܹػ���ֹͣ����  
	SetServiceStatus(hStatus, &ServiceStatus);//�����������״̬  

	if (ServiceStatus.dwCurrentState == SERVICE_RUNNING){
		//ֻ����������SERVICE_RUNNING״̬�󣬷������ſ��Ե���ϵͳ����
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
	�������ܣ����ӵ�������ƹ�����
	������һ��SERVICE_TABLE_ENTRY�ṹ��ָ�룬��Ա���е����һ��������NULLֵ����ָ�����ĩβ��
		����˺����ɹ�������ֵ��Ϊ�㡣�������ʧ�ܣ�����ֵΪ�㡣
		typedef struct _SERVICE_TABLE_ENTRY {
			LPTSTR                  lpServiceName;//Ҫ�ڴ˷�����������еķ������ƣ����ܺ�������������ˡ���һ������������Ϊע�ᵽϵͳ�ķ�������һ�¡�
			                                      //�������װ����SERVICE_WIN32_OWN_PROCESS�������ͣ��˳�Ա�������ԣ�������Ϊ NULL�������ǿ��ַ���("")
			LPSERVICE_MAIN_FUNCTION lpServiceProc; //ָ��ServiceMain������
		} SERVICE_TABLE_ENTRY, *LPSERVICE_TABLE_ENTRY;
	*/
	StartServiceCtrlDispatcher(ServiceTable);
	return 0;
}