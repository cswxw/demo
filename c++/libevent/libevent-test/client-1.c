#include <stdio.h>
#include <stdlib.h>

#include <string.h>


#define EVENT_DEBUG_LOGGING_ENABLED

#include<event.h>  
#include<event2/bufferevent.h>  
#include<event2/buffer.h>  
#include<event2/util.h>  

#ifdef WIN32
#include <WinSock2.h>

#pragma comment(lib,"ws2_32.lib")
#pragma comment(lib,"event.lib")
#pragma comment(lib,"event_core.lib")
#pragma comment(lib,"event_extra.lib")
#else
#include<sys/types.h>  
#include<sys/socket.h>  
#include<netinet/in.h>  
#include<arpa/inet.h>  
#include<errno.h>  
#include<unistd.h> 
#define closesocket close
#endif


int tcp_connect_server(const char* server_ip, int port)
{
	int sockfd, status, save_errno;
	struct sockaddr_in server_addr;

	memset(&server_addr, 0, sizeof(server_addr));

	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(port);
#ifndef WIN32
	status = inet_aton(server_ip, &server_addr.sin_addr);
	if (status == 0) //the server_ip is not valid value  
	{
		//errno = EINVAL;
		return -1;
	}
#else
	server_addr.sin_addr.S_un.S_addr = inet_addr(server_ip);  //char * inet_ntoa(ULONG );
#endif

	sockfd = socket(PF_INET, SOCK_STREAM, 0);
	if (sockfd == -1)
		return sockfd;


	status = connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr));

	if (status == -1)
	{
		save_errno = errno;

		closesocket(sockfd);
		errno = save_errno; //the close may be error  
		return -1;
	}

	//evutil_make_socket_nonblocking(sockfd);

	return sockfd;
}

void socket_read_cb(int fd, short events, void *arg)
{
	char msg[1024];

	//为了简单起见，不考虑读一半数据的情况  
	int len = recv(fd, msg, sizeof(msg) - 1, 0);
	if (len <= 0)
	{
		perror("read fail ");
		exit(1);
	}

	msg[len] = '\0';

	printf("recv { %s } from server\n", msg);
}

void cmd_msg_cb(int fd, short events, void* arg)
{
	char msg[1024];
#ifndef WIN32
	int ret = read(fd, msg, sizeof(msg));
#else
	int ret = -1; 
	//TODO
#endif
	if (ret <= 0)
	{
		perror("read fail ");
		exit(1);
	}

	int sockfd = *((int*)arg);

	//把终端的消息发送给服务器端  
	//为了简单起见，不考虑写一半数据的情况  
	send(sockfd, msg, ret, 0);
}

int main(int argc, char** argv)
{
	int err;
	event_enable_debug_logging(EVENT_LOG_ERR);
#ifdef WIN32
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) { printf("WSAStartup failed\n"); return 1; };
#endif
#ifndef _DEBUG
	if (argc < 3)
	{
		printf("please input 2 parameter\n");
		return -1;
	}
	//两个参数依次是服务器端的IP地址、端口号  
	int sockfd = tcp_connect_server(argv[1], atoi(argv[2]));
#else
	//两个参数依次是服务器端的IP地址、端口号  
	int sockfd = tcp_connect_server("127.0.0.1", 9999);
#endif




	if (sockfd == -1)
	{
		perror("tcp_connect error ");
		return -1;
	}

	printf("connect to server successful\n");

	struct event_base* base = event_base_new();

	struct event *ev_sockfd = event_new(base, sockfd,
		EV_READ | EV_PERSIST,
		socket_read_cb, NULL);
	event_add(ev_sockfd, NULL);

	//监听终端输入事件  
#ifdef WIN32
	struct event* ev_cmd = event_new(base, _fileno(stdin), EV_READ | EV_PERSIST, cmd_msg_cb, (void*)&sockfd);
#else
	struct event* ev_cmd = event_new(base, STDIN_FILENO, EV_READ | EV_PERSIST, cmd_msg_cb, (void*)&sockfd);
#endif
	event_add(ev_cmd, NULL);

	event_base_dispatch(base);
#ifdef WIN32
	WSACleanup();
#endif
	printf("finished \n");
	return 0;
}