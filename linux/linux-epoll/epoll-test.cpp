




#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/epoll.h>

/* 最大缓存区大小 */
#define MAX_BUFFER_SIZE 5 + 1
/* epoll最大监听数 */
#define MAX_EPOLL_EVENTS 20
/* LT模式 */
#define EPOLL_LT 0
/* ET模式 */
#define EPOLL_ET 1
/* 文件描述符设置阻塞 */
#define FD_BLOCK 0
/* 文件描述符设置非阻塞 */
#define FD_NONBLOCK 1

/* 设置文件为非阻塞 */
int set_nonblock(int fd)
{
    int old_flags = fcntl(fd, F_GETFL);
    fcntl(fd, F_SETFL, old_flags | O_NONBLOCK);
    return old_flags;
}

/* 注册文件描述符到epoll，并设置其事件为EPOLLIN(可读事件) */
void addfd_to_epoll(int epoll_fd, int fd, int epoll_type, int block_type)
{
    struct epoll_event ep_event;
    ep_event.data.fd = fd;
    ep_event.events = EPOLLIN; //表示关联的fd可以进行读操作了。

    /* 如果是ET模式，设置EPOLLET */
    if (epoll_type == EPOLL_ET)
        ep_event.events |= EPOLLET;

    /* 设置是否阻塞 */
    if (block_type == FD_NONBLOCK)
        set_nonblock(fd);

    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &ep_event);
}

/* LT处理流程 */
void epoll_lt(int sockfd)
{
    char buffer[MAX_BUFFER_SIZE];
    int ret;

    memset(buffer, 0, MAX_BUFFER_SIZE);
    printf("开始recv()...\n");
    ret = recv(sockfd, buffer, MAX_BUFFER_SIZE, 0);
    printf("ret = %d\n", ret);
    if (ret > 0)
        printf("收到消息:%s, 共%d个字节\n", buffer, ret);
    else
    {
        if (ret == 0)
            printf("客户端主动关闭！！！\n");
        close(sockfd);
    }

    printf("LT处理结束！！！\n");
}

/* 带循环的ET处理流程 */
void epoll_et_loop(int sockfd)
{
    char buffer[MAX_BUFFER_SIZE];
    int ret;

    printf("带循环的ET读取数据开始...\n");
    while (1)
    {
        memset(buffer, 0, MAX_BUFFER_SIZE);
        ret = recv(sockfd, buffer, MAX_BUFFER_SIZE - 1, 0);
        if (ret == -1)
        {
            if (errno == EAGAIN || errno == EWOULDBLOCK) //对非阻塞socket而言，EAGAIN不是一种错误。在VxWorks和Windows上，EAGAIN的名字叫做EWOULDBLOCK。
            {
                printf("循环读完所有数据！！！  errno = %d \n", errno); //#define	EAGAIN		// #define	EWOULDBLOCK	EAGAIN
                break;
            }
            close(sockfd);
            break;
        }
        else if (ret == 0)
        {
            printf("客户端主动关闭请求！！！\n");
            close(sockfd);
            break;
        }
        else
            printf("收到消息:%s, 共%d个字节\n", buffer, ret);
    }
    printf("带循环的ET处理结束！！！\n");
}


/* 不带循环的ET处理流程，比epoll_et_loop少了一个while循环 */
void epoll_et_nonloop(int sockfd)
{
    char buffer[MAX_BUFFER_SIZE];
    int ret;

    printf("不带循环的ET模式开始读取数据...\n");
    memset(buffer, 0, MAX_BUFFER_SIZE);
    ret = recv(sockfd, buffer, MAX_BUFFER_SIZE - 1 , 0);
    if (ret > 0)
    {
        printf("收到消息:%s, 共%d个字节\n", buffer, ret);
    }
    else
    {
        if (ret == 0)
            printf("客户端主动关闭连接！！！\n");
        close(sockfd);
    }

    printf("不带循环的ET模式处理结束！！！\n");
}

/* 处理epoll的返回结果 */
void epoll_process(int epollfd, struct epoll_event *events, int number, int sockfd, int epoll_type, int block_type)
{
    struct sockaddr_in client_addr;
    socklen_t client_addrlen;
    int newfd, connfd;
    int i;

    for (i = 0; i < number; i++)
    {
        newfd = events[i].data.fd;
        if (newfd == sockfd)
        {
            printf("=================================新一轮accept()===================================\n");
            printf("accept()开始...\n");

            /* 休眠3秒，模拟一个繁忙的服务器，不能立即处理accept连接 */
            printf("开始休眠3秒...\n");
            sleep(3);
            printf("休眠3秒结束！！！\n");

            client_addrlen = sizeof(client_addr);
            connfd = accept(sockfd, (struct sockaddr *)&client_addr, &client_addrlen);
            printf("connfd = %d\n", connfd);

            /* 注册已链接的socket到epoll，并设置是LT还是ET，是阻塞还是非阻塞 */
            addfd_to_epoll(epollfd, connfd, epoll_type, block_type);
            printf("accept()结束！！！\n");
        }
        else if (events[i].events & EPOLLIN)
        {
            /* 可读事件处理流程 */

            if (epoll_type == EPOLL_LT)    
            {
                printf("============================>水平触发开始...\n");
                epoll_lt(newfd);
            }
            else if (epoll_type == EPOLL_ET)
            {
                printf("============================>边缘触发开始...\n");

                /* 带循环的ET模式 */
                epoll_et_loop(newfd);

                /* 不带循环的ET模式 */
                //epoll_et_nonloop(newfd);
            }
        }
        else
            printf("其他事件发生...\n");
    }
}

/* 出错处理 */
void err_exit(char *msg)
{
    perror(msg);
    exit(1);
}

/* 创建socket */
int create_socket(const char *ip, const int port_number)
{
    struct sockaddr_in server_addr;
    int sockfd, reuse = 1;

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port_number);

    if (inet_pton(PF_INET, ip, &server_addr.sin_addr) == -1)
        err_exit("inet_pton() error");

    if ((sockfd = socket(PF_INET, SOCK_STREAM, 0)) == -1)
        err_exit("socket() error");

    /* 设置复用socket地址 */
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) == -1)
        err_exit("setsockopt() error");

    if (bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
        err_exit("bind() error");

    if (listen(sockfd, 5) == -1)
        err_exit("listen() error");

    return sockfd;
}

/* main函数 */
int main(int argc, const char *argv[])
{
    if (argc < 3)
    {
        fprintf(stderr, "usage:%s ip_address port_number\n", argv[0]);
        exit(1);
    }

    int sockfd, epollfd, number;

    sockfd = create_socket(argv[1], atoi(argv[2]));
    struct epoll_event events[MAX_EPOLL_EVENTS];

    /* linux内核2.6.27版的新函数，和epoll_create(int size)一样的功能，并去掉了无用的size参数 */
    if ((epollfd = epoll_create1(0)) == -1)
        err_exit("epoll_create1() error");

    /* 以下设置是针对监听的sockfd，当epoll_wait返回时，必定有事件发生，
     * 所以这里我们忽略罕见的情况外设置阻塞IO没意义，我们设置为非阻塞IO */

    /* sockfd：非阻塞的LT模式 */
    addfd_to_epoll(epollfd, sockfd, EPOLL_LT, FD_NONBLOCK);

    /* sockfd：非阻塞的ET模式 */
    //addfd_to_epoll(epollfd, sockfd, EPOLL_ET, FD_NONBLOCK); //高并发时，会出现客户端连接不上的问题

   
    while (1)
    {
        number = epoll_wait(epollfd, events, MAX_EPOLL_EVENTS, -1);
        if (number == -1)
            err_exit("epoll_wait() error");
        else
        {
            /* 以下的LT，ET，以及是否阻塞都是是针对accept()函数返回的文件描述符，即函数里面的connfd */

            /* connfd:阻塞的LT模式 */
            //epoll_process(epollfd, events, number, sockfd, EPOLL_LT, FD_BLOCK);  //处理代码一次接收不完，水平触发一直触发，直到数据全部读取完毕

            /* connfd:非阻塞的LT模式 */
            //epoll_process(epollfd, events, number, sockfd, EPOLL_LT, FD_NONBLOCK);//处理代码一次接收不完，水平触发一直触发，直到数据全部读取完毕

            /* connfd:阻塞的ET模式 */
            //epoll_process(epollfd, events, number, sockfd, EPOLL_ET, FD_BLOCK);  //[warn][error]//[warn]不使用循环读取数据,读不完的数据会在下一次触发进行读取。[error]使用循环读取数据，用另一个客户端去连接，服务器不能受理这个新的客户端！

            /* connfd:非阻塞的ET模式 */
            epoll_process(epollfd, events, number, sockfd, EPOLL_ET, FD_NONBLOCK); //[warn][ok]//[warn]不使用循环读取数据,读不完的数据会在下一次触发进行读取。[ok] 使用循环读取数据，根据EAGAIN和EWOULDBLOCK来判断数据全部读取完毕了
        }
    }

    close(sockfd);
    return 0;
}
/*
1.对于监听的sockfd，最好使用水平触发模式，边缘触发模式会导致高并发情况下，有的客户端会连接不上。如果非要使用边缘触发，网上有的方案是用while来循环accept()。
2.对于读写的connfd，水平触发模式下，阻塞和非阻塞效果都一样，不过为了防止特殊情况，还是建议设置非阻塞。
3.对于读写的connfd，边缘触发模式下，必须使用非阻塞IO，并要一次性全部读写完数据。
*/


