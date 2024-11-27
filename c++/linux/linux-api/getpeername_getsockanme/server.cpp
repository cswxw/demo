/*��������*/  
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
	struct sockaddr_in servaddr;//�������󶨵ĵ�ַ  
	struct sockaddr_in listendAddr, connectedAddr, peerAddr;//�ֱ��ʾ�����ĵ�ַ�����ӵı��ص�ַ�����ӵĶԶ˵�ַ  
	int listendAddrLen, connectedAddrLen, peerLen;  
	char ipAddr[INET_ADDRSTRLEN];//������ʮ���Ƶĵ�ַ  
	listenfd = socket(AF_INET, SOCK_STREAM, 0);  
	memset(&servaddr, 0, sizeof(servaddr));  
  
	servaddr.sin_family = AF_INET;  
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);  
	servaddr.sin_port = htons(PORT);  
	  
	bind(listenfd, (struct sockaddr*)&servaddr, sizeof(servaddr));//�������˰󶨵�ַ  
  
	listen(listenfd, LISTENQ);  
	listendAddrLen = sizeof(listendAddr);  
	getsockname(listenfd, (struct sockaddr *)&listendAddr, &listendAddrLen);//��ȡ�����ĵ�ַ�Ͷ˿�  
	printf("listen address = %s:%d\n", inet_ntoa(listendAddr.sin_addr), ntohs(listendAddr.sin_port));  
  
	while(1) {  
		connfd = accept(listenfd, (struct sockaddr *)NULL, NULL);  
		connectedAddrLen = sizeof(connectedAddr);  
		getsockname(connfd, (struct sockaddr *)&connectedAddr, &connectedAddrLen);//��ȡconnfd��ʾ�������ϵı��ص�ַ  
		printf("connected server address = %s:%d\n", inet_ntoa(connectedAddr.sin_addr), ntohs(connectedAddr.sin_port));  
		getpeername(connfd, (struct sockaddr *)&peerAddr, &peerLen); //��ȡconnfd��ʾ�������ϵĶԶ˵�ַ  
		printf("connected peer address = %s:%d\n", inet_ntop(AF_INET, &peerAddr.sin_addr, ipAddr, sizeof(ipAddr)), ntohs(peerAddr.sin_port));  
	}  
	return 0;  
}  