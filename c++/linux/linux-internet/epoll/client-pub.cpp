//�����̿ͻ���
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>//htons()����ͷ�ļ�
#include <netinet/in.h>//inet_addr()ͷ�ļ�
#include <fcntl.h>
#include <sys/epoll.h>
#include "pub.h"

int main(int arg,char *args[])
{
    if(arg<2)
    {
        printf("please print two param !\n");
    }
    //�˿ں�
    int port=atoi(args[2]);
    //�����IP��ַ
    char ipaddr[30]={0};
    strcpy(ipaddr,args[1]);
    //connect server
    int st=connect_server(ipaddr,port);
    //send message
    //������Ϣ--
    socket_send(st);
    //close socket
    close(st);
    return 0;
}