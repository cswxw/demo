//ͷ�ļ� pub.h
#ifndef _vsucess

#define _vsucess

#ifdef __cplusplus
extern "C"
{

#endif
//����������socket
int server_socket(int port);

//���÷�����
int setnonblock(int st);

//���տͻ���socket
int server_accept(int st);

//�ر�socket
int close_socket(int st);

//������Ϣ
int socket_recv(int st);

//���ӷ�����
int connect_server(char *ipaddr,int port);

//������Ϣ
int socket_send(int st);

//��sockaddr_inת����IP��ַ
int sockaddr_toa(const struct sockaddr_in * addr, char * ipaddr);

#ifdef __cplusplus
}
#endif

#endif