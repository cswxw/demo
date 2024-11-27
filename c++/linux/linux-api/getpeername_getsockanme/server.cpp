/*服务器端*/  
#define MAXLINE 4096  
#define PORT 6563  
#define LISTENQ 1024  
#include<stdio.h>  
#include<sys/socket.h>  
#include<netinet/in.h>  
#include<unistd.h>  
#include<string.h>  
#include<arpa/inet.h>  
  
int main() {  
	int listenfd, connfd;  
	struct sockaddr_in servaddr;//服务器绑定的地址  
	struct sockaddr_in listendAddr, connectedAddr, peerAddr;//分别表示监听的地址，连接的本地地址，连接的对端地址  
	int listendAddrLen, connectedAddrLen, peerLen;  
	char ipAddr[INET_ADDRSTRLEN];//保存点分十进制的地址  
	listenfd = socket(AF_INET, SOCK_STREAM, 0);  
	memset(&servaddr, 0, sizeof(servaddr));  
  
	servaddr.sin_family = AF_INET;  
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);  
	servaddr.sin_port = htons(PORT);  
	  
	bind(listenfd, (struct sockaddr*)&servaddr, sizeof(servaddr));//服务器端绑定地址  
  
	listen(listenfd, LISTENQ);  
	listendAddrLen = sizeof(listendAddr);  
	getsockname(listenfd, (struct sockaddr *)&listendAddr, &listendAddrLen);//获取监听的地址和端口  
	printf("listen address = %s:%d\n", inet_ntoa(listendAddr.sin_addr), ntohs(listendAddr.sin_port));  
  
	while(1) {  
		connfd = accept(listenfd, (struct sockaddr *)NULL, NULL);  
		connectedAddrLen = sizeof(connectedAddr);  
		getsockname(connfd, (struct sockaddr *)&connectedAddr, &connectedAddrLen);//获取connfd表示的连接上的本地地址  
		printf("connected server address = %s:%d\n", inet_ntoa(connectedAddr.sin_addr), ntohs(connectedAddr.sin_port));  
		getpeername(connfd, (struct sockaddr *)&peerAddr, &peerLen); //获取connfd表示的连接上的对端地址  
		printf("connected peer address = %s:%d\n", inet_ntop(AF_INET, &peerAddr.sin_addr, ipAddr, sizeof(ipAddr)), ntohs(peerAddr.sin_port));  
	}  
	return 0;  
}  