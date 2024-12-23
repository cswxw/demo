#include <iostream>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <arpa/inet.h>  
#define BUFSIZE  1024*8
int main(int argc, char *argv[])  
{  
	int server_sockfd;//服务器端套接字  
	int client_sockfd;//客户端套接字  
	int len;  
	struct sockaddr_in my_addr;   //服务器网络地址结构体  
	struct sockaddr_in remote_addr; //客户端网络地址结构体  
	int sin_size;  
	char buf[BUFSIZE+1];  //数据传送的缓冲区  
	memset(&my_addr,0,sizeof(my_addr)); //数据初始化--清零  
	my_addr.sin_family=AF_INET; //设置为IP通信  
	my_addr.sin_addr.s_addr=INADDR_ANY;//服务器IP地址--允许连接到所有本地地址上  
	my_addr.sin_port=htons(6789); //服务器端口号  

	/*创建服务器端套接字--IPv4协议，面向连接通信，TCP协议*/  
	if((server_sockfd=socket(PF_INET,SOCK_STREAM,0))<0)  
	{    
		perror("socket");  
		return 1;  
	}  

	/*将套接字绑定到服务器的网络地址上*/  
	if (bind(server_sockfd,(struct sockaddr *)&my_addr,sizeof(struct sockaddr))<0)  
	{  
		perror("bind");  
		return 1;  
	}  

	/*监听连接请求--监听队列长度为5*/  
	listen(server_sockfd,5);  

	sin_size=sizeof(struct sockaddr_in);  

	/*等待客户端连接请求到达*/  
	if((client_sockfd=accept(server_sockfd,(struct sockaddr *)&remote_addr,(socklen_t*)&sin_size)) < 0)  
	{  
		perror("accept");  
		return 1;  
	}  
	printf("accept client %s\n",inet_ntoa(remote_addr.sin_addr));  

	struct sockaddr_in listendAddr, connectedAddr, peerAddr;//分别表示监听的地址，连接的本地地址，连接的对端地址 
	socklen_t  connectedAddrLen, peerLen;  
	char ipAddr[INET_ADDRSTRLEN];//保存点分十进制的地址 
	getsockname(server_sockfd, (struct sockaddr *)&connectedAddr, &connectedAddrLen);//获取connfd表示的连接上的本地地址  
	printf("connected server address = %s:%d\n", inet_ntoa(connectedAddr.sin_addr), ntohs(connectedAddr.sin_port));  
	getpeername(client_sockfd, (struct sockaddr *)&peerAddr, &peerLen); //获取connfd表示的连接上的对端地址  
	printf("connected peer address = %s:%d\n", inet_ntop(AF_INET, &peerAddr.sin_addr, ipAddr, sizeof(ipAddr)), ntohs(peerAddr.sin_port));



	int fp= open("1.txt",O_WRONLY | O_CREAT ,S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH);
	printf("start recv\n");
	/*接收客户端的数据并将其发送给客户端--recv返回接收到的字节数，send返回发送的字节数*/ 
	while(1)  
	{  
		len=recv(client_sockfd,buf, BUFSIZE,0);
		if(len > 0){
			buf[len]='\0';
			printf("[recv len]:%d\n",len);
			write(fp,buf,len);
		}else if(len == 0){
			break;
		}else{
			if(errno == EAGAIN||errno == EWOULDBLOCK||errno == EINTR){
				perror("len < 0");
				continue;
			}
			break;
		}
	}  
	close(fp);
	printf("recv ret:%d\n", len);
	close(client_sockfd);  
	close(server_sockfd);  
	return 0;  
}  


