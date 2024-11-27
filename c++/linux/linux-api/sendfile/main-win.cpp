// TcpClient.cpp : 定义控制台应用程序的入口点。
//


#include <iostream>
#include <vector>
#include <deque>
#include <winsock2.h>
#pragma comment(lib, "WS2_32")	// 链接到WS2_32.lib
#include <stdlib.h>

#include <MSWSock.h>
#pragma comment(lib, "Mswsock.lib ")

#define P(...) printf("%s : %s \n", #__VA_ARGS__, __VA_ARGS__);


class CInitSock
{
public:
	CInitSock(BYTE minorVer = 2, BYTE majorVer = 2)
	{
		// 初始化WS2_32.dll
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
	// 初始化完成，创建一个TCP的socket  
	SOCKET sServer = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	//检查是否创建失败  
	if (sServer == INVALID_SOCKET)
	{
		printf("socket failed\n");
		return -1;
	}
	printf("Create socket OK\n");
	//进行绑定操作  
	SOCKADDR_IN addrServ;
	addrServ.sin_family = AF_INET; // 协议簇为IPV4的  
	addrServ.sin_port = htons(6789); // 端口  因为本机是小端模式，网络是大端模式，调用htons把本机字节序转为网络字节序  
	addrServ.sin_addr.S_un.S_addr = INADDR_ANY; // ip地址，INADDR_ANY表示绑定电脑上所有网卡IP  
												//完成绑定操作  
	int ret = bind(sServer, (sockaddr *)&addrServ, sizeof(sockaddr));
	//检查绑定是否成功  
	if (SOCKET_ERROR == ret)
	{
		printf("socket bind failed\n");
		WSACleanup(); // 释放网络环境  
		closesocket(sServer); // 关闭网络连接  
		return -1;
	}
	printf("socket bind OK\n");
	// 绑定成功，进行监听  
	ret = listen(sServer, 10);
	//检查是否监听成功  
	if (SOCKET_ERROR == ret)
	{
		printf("socket listen failed\n");
		WSACleanup();
		closesocket(sServer);
		return -1;
	}
	printf("socket listen OK\n");
	// 监听成功  
	sockaddr_in addrClient; // 用于保存客户端的网络节点的信息  
	int addrClientLen = sizeof(sockaddr_in);
	
	//新建一个socket，用于客户端  
	SOCKET sClient;
	//等待客户端的连接  
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
