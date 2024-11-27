//     
// a simple echo server using epoll in linux    
//     
// 2009-11-05    
// 2013-03-22:修改了几个问题，1是/n格式问题，2是去掉了原代码不小心加上的ET模式;  
// 本来只是简单的示意程序，决定还是加上 recv/send时的buffer偏移  
// by sparkling    
//     
#include <sys/socket.h>    
#include <sys/epoll.h>    
#include <netinet/in.h>    
#include <arpa/inet.h>    
#include <fcntl.h>    
#include <unistd.h>    
#include <stdio.h>    
#include <errno.h>    
#include <iostream>    
#include <stdlib.h>
#include <string.h>
using namespace std;    
#define MAX_EVENTS 500    
struct myevent_s    
{    
    int fd;    
    void (*call_back)(int fd, int events, void *arg);    
    int events;    
    void *arg;    
    int status; // 1: in epoll wait list, 0 not in    
    char buff[128]; // recv data buffer    
    int len, s_offset;    
    long last_active; // last active time    
};    
// set event    
void EventSet(myevent_s *ev, int fd, void (*call_back)(int, int, void*), void *arg)    
{    
    ev->fd = fd;    
    ev->call_back = call_back;    
    ev->events = 0;    
    ev->arg = arg;    
    ev->status = 0;  
    bzero(ev->buff, sizeof(ev->buff));  
    ev->s_offset = 0;    
    ev->len = 0;  
    ev->last_active = time(NULL);    
}    
// add/mod an event to epoll    
void EventAdd(int epollFd, int events, myevent_s *ev)    
{    
    struct epoll_event epv = {0, {0}};    
    int op;    
    epv.data.ptr = ev;    
    epv.events = ev->events = events;    
    if(ev->status == 1){    
        op = EPOLL_CTL_MOD;    
    }    
    else{    
        op = EPOLL_CTL_ADD;    
        ev->status = 1;    
    }    
/*
2. int epoll_ctl(int epfd, int op, int fd, struct epoll_event *event);
epoll的事件注册函数，它不同与select()是在监听事件时告诉内核要监听什么类型的事件，而是在这里先注册要监听的事件类型。第一个参数是epoll_create()的返回值，第二个参数表示动作，用三个宏来表示：
EPOLL_CTL_ADD：注册新的fd到epfd中；
EPOLL_CTL_MOD：修改已经注册的fd的监听事件；
EPOLL_CTL_DEL：从epfd中删除一个fd；
第三个参数是需要监听的fd，第四个参数是告诉内核需要监听什么事,struct epoll_event结构如下：
typedef union epoll_data {
    void *ptr;
    int fd;
    __uint32_t u32;
    __uint64_t u64;
} epoll_data_t;

struct epoll_event {
    __uint32_t events; // Epoll events 
    epoll_data_t data; // User data variable 
};

events可以是以下几个宏的集合：
EPOLLIN ：表示对应的文件描述符可以读（包括对端SOCKET正常关闭）；
EPOLLOUT：表示对应的文件描述符可以写；
EPOLLPRI：表示对应的文件描述符有紧急的数据可读（这里应该表示有带外数据到来）；
EPOLLERR：表示对应的文件描述符发生错误；
EPOLLHUP：表示对应的文件描述符被挂断；
EPOLLET： 将EPOLL设为边缘触发(Edge Triggered)模式，这是相对于水平触发(Level Triggered)来说的。
EPOLLONESHOT：只监听一次事件，当监听完这次事件之后，如果还需要继续监听这个socket的话，需要再次把这个socket加入到EPOLL队列里


*/


    if(epoll_ctl(epollFd, op, ev->fd, &epv) < 0)    
        printf("Event Add failed[fd=%d], evnets[%d]\n", ev->fd, events);    
    else    
        printf("Event Add OK[fd=%d], op=%d, evnets[%0X]\n", ev->fd, op, events);    
}    
#define P(...) printf("\t%s : %s \n", #__VA_ARGS__, __VA_ARGS__);
#define PI(...) printf("\t%s : %d \n", #__VA_ARGS__, __VA_ARGS__);
void show(const char *fuc,myevent_s * t){
    printf("\t%s\n",fuc);
    PI(t->fd);
    P(t->buff);
    PI(t->len);
    PI(t->s_offset);
}



// delete an event from epoll    
void EventDel(int epollFd, myevent_s *ev)    
{    
    struct epoll_event epv = {0, {0}};    
    if(ev->status != 1) return;    
    epv.data.ptr = ev;    
    ev->status = 0;  
    epoll_ctl(epollFd, EPOLL_CTL_DEL, ev->fd, &epv);    
}    
int g_epollFd;    
myevent_s g_Events[MAX_EVENTS+1]; // g_Events[MAX_EVENTS] is used by listen fd    
void RecvData(int fd, int events, void *arg);    
void SendData(int fd, int events, void *arg);    
// accept new connections from clients    
void AcceptConn(int fd, int events, void *arg)    
{    
    struct sockaddr_in sin;    
    socklen_t len = sizeof(struct sockaddr_in);    
    int nfd, i;    
    // accept    
    if((nfd = accept(fd, (struct sockaddr*)&sin, &len)) == -1)    
    {    
        if(errno != EAGAIN && errno != EINTR)    
        {    
        }  
        printf("%s: accept, %d", __func__, errno);    
        return;    
    }    
    do    
    {    
        for(i = 0; i < MAX_EVENTS; i++)    
        {    
            if(g_Events[i].status == 0)    
            {    
                break;    
            }    
        }    
        if(i == MAX_EVENTS)    
        {    
            printf("%s:max connection limit[%d].", __func__, MAX_EVENTS);    
            break;    
        }    
        // set nonblocking  
        int iret = 0;  
        if((iret = fcntl(nfd, F_SETFL, O_NONBLOCK)) < 0)  
        {  
            printf("%s: fcntl nonblocking failed:%d", __func__, iret);  
            break;  
        }  
        // add a read event for receive data    
        EventSet(&g_Events[i], nfd, RecvData, &g_Events[i]);    
        EventAdd(g_epollFd, EPOLLIN, &g_Events[i]);    
    }while(0);    
    printf("new conn[%s:%d][time:%d], pos[%d]\n", inet_ntoa(sin.sin_addr),  
            ntohs(sin.sin_port), g_Events[i].last_active, i);    
}    
// receive data    
void RecvData(int fd, int events, void *arg)    
{    
    struct myevent_s t2;
    struct myevent_s *ev = (struct myevent_s*)arg;    
    int len;    
    // receive data  
    len = recv(fd, ev->buff+ev->len, sizeof(ev->buff)-1-ev->len, 0);      
    EventDel(g_epollFd, ev);  
    if(len > 0)  
    {  
        ev->len += len;  
        ev->buff[len] = '\0';    
        printf("C[%d]:%s\n", fd, ev->buff);    
        t2=*ev;
        // change to send event    
        EventSet(ev, fd, SendData, ev);    
        ev->len = t2.len;
        strcpy(ev->buff, t2.buff);
        EventAdd(g_epollFd, EPOLLOUT, ev);    
        show(__FUNCTION__,ev);
    }    
    else if(len == 0)    
    {    
        close(ev->fd);    
        printf("[fd=%d] pos[%d], closed gracefully.\n", fd, ev-g_Events);    
    }    
    else    
    {    
        close(ev->fd);    
        printf("recv[fd=%d] error[%d]:%s\n", fd, errno, strerror(errno));    
    }    
}    
// send data    
void SendData(int fd, int events, void *arg)    
{    
    struct myevent_s *ev = (struct myevent_s*)arg;    
    int len;    

    show(__FUNCTION__,ev);

    // send data    
    len = send(fd, ev->buff + ev->s_offset, ev->len - ev->s_offset, 0);  
    if(len > 0)    
    {  
        printf("send[fd=%d], [%d<->%d]%s\n", fd, len, ev->len, ev->buff);  
        ev->s_offset += len;  
        if(ev->s_offset == ev->len)  
        {  
            // change to receive event  
            EventDel(g_epollFd, ev);    
            EventSet(ev, fd, RecvData, ev);    
            EventAdd(g_epollFd, EPOLLIN, ev);    
        }  
    }    
    else if(len < 0 )   
    {    
        printf("[failed] send[fd=%d] error[%d] %s\n", fd, errno, strerror(errno));    
        close(ev->fd);    
        EventDel(g_epollFd, ev);    
    }else {
        printf("%s: len == 0 \n", __FUNCTION__);
        close(ev->fd);
        EventDel(g_epollFd, ev);
    }    
}    
void InitListenSocket(int epollFd, short port)    
{    
    int listenFd = socket(AF_INET, SOCK_STREAM, 0);    
    fcntl(listenFd, F_SETFL, O_NONBLOCK); // set non-blocking    
    printf("server listen fd=%d\n", listenFd);    
    EventSet(&g_Events[MAX_EVENTS], listenFd, AcceptConn, &g_Events[MAX_EVENTS]);    
    // add listen socket    
    EventAdd(epollFd, EPOLLIN, &g_Events[MAX_EVENTS]);    
    // bind & listen    
    sockaddr_in sin;    
    bzero(&sin, sizeof(sin));    
    sin.sin_family = AF_INET;    
    sin.sin_addr.s_addr = INADDR_ANY;    
    sin.sin_port = htons(port);    
    bind(listenFd, (const sockaddr*)&sin, sizeof(sin));    
    listen(listenFd, 5);    
}    
int main(int argc, char **argv)    
{    
    unsigned short port = 6666; // default port    
    if(argc == 2){    
        port = atoi(argv[1]);    
    }    
/*
 int epoll_create(int size);
创建一个epoll的句柄，size用来告诉内核这个监听的数目一共有多大。这个参数不同于select()中的第一个参数，给出最大监听的fd+1的值。需要注意的是，当创建好epoll句柄后，它就是会占用一个fd值，在linux下如果查看/proc/进程id/fd/，是能够看到这个fd的，所以在使用完epoll后，必须调用close()关闭，否则可能导致fd被耗尽。
*/
    // create epoll    
    g_epollFd = epoll_create(MAX_EVENTS);    
    
    if(g_epollFd <= 0) printf("create epoll failed.%d\n", g_epollFd);    
    // create & bind listen socket, and add to epoll, set non-blocking    
    InitListenSocket(g_epollFd, port);    
    // event loop    
    struct epoll_event events[MAX_EVENTS];    
    printf("server running:port[%d]\n", port);    
    int checkPos = 0;    
    while(1){    
        // a simple timeout check here, every time 100, better to use a mini-heap, and add timer event    
        long now = time(NULL);    
        for(int i = 0; i < MAX_EVENTS; i++, checkPos++) // doesn't check listen fd    
        {    
            if(checkPos == MAX_EVENTS) checkPos = 0; // recycle    
            if(g_Events[checkPos].status != 1) continue;    
            long duration = now - g_Events[checkPos].last_active;    
            if(duration >= 60) // 60s timeout    
            {    
                close(g_Events[checkPos].fd);    
                printf("[fd=%d] timeout[%d--%d].\n", g_Events[checkPos].fd, g_Events[checkPos].last_active, now);    
                EventDel(g_epollFd, &g_Events[checkPos]);    
            }    
        }    
        // wait for events to happen    
/*
3. int epoll_wait(int epfd, struct epoll_event * events, int maxevents, int timeout);
等待事件的产生，类似于select()调用。参数events用来从内核得到事件的集合，maxevents告之内核这个events有多大，这个 maxevents的值不能大于创建epoll_create()时的size，参数timeout是超时时间（毫秒，0会立即返回，-1将不确定，也有说法说是永久阻塞）。该函数返回需要处理的事件数目，如返回0表示已超时。

*/
        int fds = epoll_wait(g_epollFd, events, MAX_EVENTS, 1000);    
        if(fds < 0){    
            printf("epoll_wait error, exit\n");    
            break;    
        }    
        for(int i = 0; i < fds; i++){    
            myevent_s *ev = (struct myevent_s*)events[i].data.ptr;    
            if((events[i].events&EPOLLIN)&&(ev->events&EPOLLIN)) // read event    
            {    
                ev->call_back(ev->fd, events[i].events, ev->arg);    
            }    
            if((events[i].events&EPOLLOUT)&&(ev->events&EPOLLOUT)) // write event    
            {    
                ev->call_back(ev->fd, events[i].events, ev->arg);    
            }    
        }    
    }    
    // free resource    
    return 0;    
}     
