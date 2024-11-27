//网络编程客户端
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>//htons()函数头文件
#include <netinet/in.h>//inet_addr()头文件
#include <fcntl.h>
#include <sys/epoll.h>
#include "pub.h"

int main(int arg,char *args[])
{
    if(arg<2)
    {
        printf("please print two param !\n");
    }
    //端口号
    int port=atoi(args[2]);
    //服务端IP地址
    char ipaddr[30]={0};
    strcpy(ipaddr,args[1]);
    //connect server
    int st=connect_server(ipaddr,port);
    //send message
    //发送消息--
    socket_send(st);
    //close socket
    close(st);
    return 0;
}