/*�ͻ���*/  
#define PORT 6563  
#include<stdio.h>  
#include<sys/socket.h>  
#include<netinet/in.h>  
#include<unistd.h>  
#include<string.h>  
#include<arpa/inet.h>  
  
int main(int argc, char **argv) {  
	struct sockaddr_in servaddr;//�������˵�ַ  
	struct sockaddr_in clientAddr;//�ͻ��˵�ַ  
	int sockfd;   
	int clientAddrLen = sizeof(clientAddr);  
	char ipAddress[INET_ADDRSTRLEN];//������ʮ���Ƶ�ip��ַ  
	  
	if(argc < 2) {  
		printf("parameter error");  
	}  
  
	sockfd = socket(AF_INET, SOCK_STREAM, 0);  
	memset(&servaddr, 0, sizeof(servaddr));  
	servaddr.sin_family = AF_INET;  
	servaddr.sin_port = htons(PORT);    
	if(inet_pton(AF_INET, argv[1], &servaddr.sin_addr) <= 0) {  
		printf("server address error\n");//��ַ�������Ϸ�  
	}  
  
	connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr));//��������˷�����������  
	  
	getsockname(sockfd, (struct sockaddr*)&clientAddr, &clientAddrLen);//��ȡsockfd��ʾ�������ϵı��ص�ַ  
   
	printf("client:client ddress = %s:%d\n", inet_ntop(AF_INET, &clientAddr.sin_addr, ipAddress, sizeof(ipAddress)), ntohs(clientAddr.sin_port));  
	return 0;  
}  