// TcpClient.cpp : �������̨Ӧ�ó������ڵ㡣
//


#include <iostream>
#include <vector>
#include <deque>
#include <winsock2.h>
#pragma comment(lib, "WS2_32")	// ���ӵ�WS2_32.lib
#include <stdlib.h>

#include <MSWSock.h>
#pragma comment(lib, "Mswsock.lib ")

#define P(...) printf("%s : %s \n", #__VA_ARGS__, __VA_ARGS__);


class CInitSock
{
public:
	CInitSock(BYTE minorVer = 2, BYTE majorVer = 2)
	{
		// ��ʼ��WS2_32.dll
		WSADATA wsaData;
		WORD sockVersion = MAKEWORD(minorVer, majorVer);
		if (::WSAStartup(sockVersion, &wsaData) != 0)
		{
			exit(0);
		}
	}
	~CInitSock()
	{
		::WSACleanup();
	}
};
CInitSock a;
int main()
{
	// ��ʼ����ɣ�����һ��TCP��socket  
	SOCKET sServer = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	//����Ƿ񴴽�ʧ��  
	if (sServer == INVALID_SOCKET)
	{
		printf("socket failed\n");
		return -1;
	}
	printf("Create socket OK\n");
	//���а󶨲���  
	SOCKADDR_IN addrServ;
	addrServ.sin_family = AF_INET; // Э���ΪIPV4��  
	addrServ.sin_port = htons(6789); // �˿�  ��Ϊ������С��ģʽ�������Ǵ��ģʽ������htons�ѱ����ֽ���תΪ�����ֽ���  
	addrServ.sin_addr.S_un.S_addr = INADDR_ANY; // ip��ַ��INADDR_ANY��ʾ�󶨵�������������IP  
												//��ɰ󶨲���  
	int ret = bind(sServer, (sockaddr *)&addrServ, sizeof(sockaddr));
	//�����Ƿ�ɹ�  
	if (SOCKET_ERROR == ret)
	{
		printf("socket bind failed\n");
		WSACleanup(); // �ͷ����绷��  
		closesocket(sServer); // �ر���������  
		return -1;
	}
	printf("socket bind OK\n");
	// �󶨳ɹ������м���  
	ret = listen(sServer, 10);
	//����Ƿ�����ɹ�  
	if (SOCKET_ERROR == ret)
	{
		printf("socket listen failed\n");
		WSACleanup();
		closesocket(sServer);
		return -1;
	}
	printf("socket listen OK\n");
	// �����ɹ�  
	sockaddr_in addrClient; // ���ڱ���ͻ��˵�����ڵ����Ϣ  
	int addrClientLen = sizeof(sockaddr_in);
	
	//�½�һ��socket�����ڿͻ���  
	SOCKET sClient;
	//�ȴ��ͻ��˵�����  
	sClient = accept(sServer, (sockaddr*)&addrClient, &addrClientLen);
	if (INVALID_SOCKET == sClient)
	{
		printf("socket accept failed\n");

		closesocket(sServer);

		return -1;
	}
	HANDLE hFile = CreateFile("d:\\1.txt", FILE_SHARE_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, NULL, NULL);
	if (hFile == INVALID_HANDLE_VALUE) {
		printf("CreateFile error\n");
		return -1;
	}
	char recvBuf[1024*8+1];
	int len = 0;
	DWORD dwWritenSize;
	while (1) {
		len = recv(sClient, recvBuf, 1024 * 8, 0);
		if (len == 0) {
			break;
		}
		else if (len > 0) {
			BOOL  ret = WriteFile(hFile, recvBuf, len, &dwWritenSize, NULL);
			if (ret == 0) {
				printf("WriteFile failed\n");
				break;
			}
		}
		else {
			int errCode = WSAGetLastError();
			printf("errCode:%d\n", errCode);
		}
			
	}
	CloseHandle(hFile);
	closesocket(sServer);

	
	return 0;
}
