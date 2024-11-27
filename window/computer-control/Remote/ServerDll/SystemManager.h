// SystemManager.h: interface for the CSystemManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SYSTEMMANAGER_H__AF94B249_140D_45EE_A50F_1B44F83B6D2B__INCLUDED_)
#define AFX_SYSTEMMANAGER_H__AF94B249_140D_45EE_A50F_1B44F83B6D2B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "Manager.h"

class CSystemManager  :public CManager
{
public:
	BOOL m_bHow;//��ʶ ����������̹���ʹ��ڹ���
	CSystemManager(CClientSocket *pClient,BOOL bHow);

	virtual ~CSystemManager();
	void CSystemManager::SendProcessList();
	LPBYTE CSystemManager::GetProcessList();

	void CSystemManager::SendWindowsList();
	LPBYTE CSystemManager::GetWindowsList();
	void CloseWindow(LPBYTE Buffer);
	void TestWindow(LPBYTE Buffer); 
	void KillProcess(LPBYTE lpBuffer, UINT nSize);    //Զ�̹رս���
	static bool CSystemManager::DebugPrivilege(const char *pName, BOOL bEnable);
	static void ShutdownWindows( DWORD dwReason );    //Զ�̹ػ�
	static bool CALLBACK EnumWindowsProc( HWND hwnd, LPARAM lParam);   //ö�ٴ���
	void OnReceive(LPBYTE lpBuffer, UINT nSize);      //�ö����Լ��Ľ������ݵĺ���
};

#endif // !defined(AFX_SYSTEMMANAGER_H__AF94B249_140D_45EE_A50F_1B44F83B6D2B__INCLUDED_)
