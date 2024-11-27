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
using namespace std;
CInitSock initSock;
typedef struct ThreadParam {
	SOCKET socket;
	DWORD dwThreadId;
	fd_set *fd;
} THREAD_PARAM, *PTHREAD_PARAM;
deque<char> gd;
char tty_getchar();
/*
DWORD WINAPI threadProc(LPVOID param) {
	PTHREAD_PARAM s = ((PTHREAD_PARAM)param);
	SOCKET sClient = s->socket;
	char buff[1024];
	int nRecv = 0;
	while (1) {
		//Sleep(1000);

		char ch = tty_getchar();
		printf("\r输入命令:");
		if (ch == -1 || ch == 0) continue;
		gd.push_back(ch);
		printf("\n");


	}
	//puts("threadProc FD_CLR");
	//FD_CLR(sClient, s->fd);
	return 0;
}*/
char tty_getchar()
{
	HANDLE stdinHandle = GetStdHandle(STD_INPUT_HANDLE);

	DWORD rc = WaitForSingleObject(stdinHandle, 1000);

	if (rc == WAIT_TIMEOUT)   return  0;
	if (rc == WAIT_ABANDONED) return -1;
	if (rc == WAIT_FAILED)    return -1;
	INPUT_RECORD buf[100];
	DWORD num = 0;
	memset(buf, 0, sizeof(buf));
	ReadConsoleInput(stdinHandle, buf, 100, &num);
	FlushConsoleInputBuffer(stdinHandle);
	for (DWORD i = 0; i < num; i++)
	{
		if (buf[i].EventType != KEY_EVENT) continue;
		KEY_EVENT_RECORD KeyEvent = buf[i].Event.KeyEvent;
		if (KeyEvent.bKeyDown != TRUE) continue;
		return KeyEvent.uChar.AsciiChar;
	}
	return 0;
}
int main()
{
	SOCKET s = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (s == INVALID_SOCKET)
	{
		printf(" Failed socket() \n");
		return 0;
	}

	sockaddr_in servAddr;
	servAddr.sin_family = AF_INET;
	servAddr.sin_port = htons(6789);
	servAddr.sin_addr.S_un.S_addr = inet_addr("192.168.109.140");

	if (::connect(s, (sockaddr*)&servAddr, sizeof(servAddr)) == -1)
	{
		printf(" Failed connect() \n");
		return 0;
	}



	char sendBuf[1024];


	HANDLE hFile = CreateFile("E:\\temp\\12345.dat", GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, NULL, NULL);
	if (hFile == INVALID_HANDLE_VALUE) {
		printf("CreateFile error\n");
		return -1;
	}
	// 获取文件大小
	//GetFileSize(hFile, NULL);
	LARGE_INTEGER liFileSize;
	if (GetFileSizeEx(hFile, &liFileSize) == FALSE) {
		printf("GetFileSizeEx error\n");
		return -1;
	}
	//while (1) 
	{
		// 使用TransmitFile发送文件
		if (TransmitFile(s, hFile, 0, 0, NULL, NULL, TF_USE_DEFAULT_WORKER) == FALSE) {
			printf("TransmitFile function failed with error: %ld\n", WSAGetLastError());
			//break;
		}
	
	}

	CloseHandle(hFile);
	::closesocket(s);
	return 0;
}
