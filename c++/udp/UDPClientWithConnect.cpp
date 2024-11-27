/*
解释一个带有connect的UDP的好处。由于UDP是不可靠传输，如果我发了数据出去，对方其实服务器是关闭的，这时会有什么结果呢？对于刚才的UDPclient1，也就是不带connect的，客户端程序会卡在recvfrom这里，因为对方是关闭的，它永远也收不到来自对方的回包。但是对于UDPclient2，也就是带有connect，我们其实可以收到一个错误，并设置errno（errno：111，connection refused）。


*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>

#define PORT 1234

#define MAXDATASIZE 100

int main(int argc, char *argv[])
{
    int sockfd, num;
    char buf[MAXDATASIZE];
    struct hostent *he;
    struct sockaddr_in server, peer;
    if(argc != 3)
    {
        printf("Usage: %s <IP address> <message>\n", argv[0]);
        exit(1);
    }

    if((sockfd=socket(AF_INET, SOCK_DGRAM, 0)) == -1)
    {
        printf("socket() error\n");
        exit(1);
    }
    bzero(&server, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);

    server.sin_addr.s_addr = inet_addr(argv[1]);
    //server.sin_addr.s_addr = inet_addr(argv[1]);
    if(connect(sockfd, (struct sockaddr *)&server, sizeof(server)) == -1)
    {
        printf("connect() error.\n");
        exit(1);
    }
    
    send(sockfd, argv[2], strlen(argv[2]), 0);

    while(1)
    {
        if((num = recv(sockfd, buf, MAXDATASIZE, 0)) == -1)
        {
            printf("recv() error.\n");
            exit(1);
        }
        
        buf[num] = '\0';
        printf("Server Message: %s.\n", buf);
        break;
    }
    close(sockfd);
}


