/*
	typedef union epoll_data
	{
	  void *ptr;
	  int fd;
	  uint32_t u32;
	  uint64_t u64;
	} epoll_data_t;
	 
	struct epoll_event
	{
	  uint32_t events;   //Epoll events
	  epoll_data_t data;  //User data variable 
	};

*/


#if 1
#include <iostream>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
  
#define MAXLINE 10
#define OPEN_MAX 100
#define LISTENQ 20
#define SERV_PORT 8006
#define INFTIM 1000
  
//线程池任务队列结构体
 
struct task{
  int fd; //需要读写的文件描述符
 
  struct task *next; //下一个任务
 
};
  
//用于读写两个的两个方面传递参数
 
struct user_data{
  int fd;
  unsigned int n_size;
  char line[MAXLINE];
};
  
//线程的任务函数
 
void * readtask(void *args);
void * writetask(void *args);
  
  
//声明epoll_event结构体的变量,ev用于注册事件,数组用于回传要处理的事件
 
struct epoll_event ev,events[20];
int epfd;
pthread_mutex_t mutex;
pthread_cond_t cond1;
struct task *readhead=NULL,*readtail=NULL,*writehead=NULL;
  
void setnonblocking(int sock)
{
     int opts;
     opts=fcntl(sock,F_GETFL);
     if(opts<0)
     {
          perror("fcntl(sock,GETFL)");
          exit(1);
     }
    opts = opts|O_NONBLOCK;
     if(fcntl(sock,F_SETFL,opts)<0)
     {
          perror("fcntl(sock,SETFL,opts)");
          exit(1);
     }
}
  
int main()
{
     int i, maxi, listenfd, connfd, sockfd,nfds;
     pthread_t tid1,tid2;
     
     struct task *new_task=NULL;
     struct user_data *rdata=NULL;
     socklen_t clilen;
     
     pthread_mutex_init(&mutex,NULL);
     pthread_cond_init(&cond1,NULL);
     //初始化用于读线程池的线程
 
     pthread_create(&tid1,NULL,readtask,NULL);
     pthread_create(&tid2,NULL,readtask,NULL);
     
     //生成用于处理accept的epoll专用的文件描述符
 
     epfd=epoll_create(256);
  
     struct sockaddr_in clientaddr;
     struct sockaddr_in serveraddr;
     listenfd = socket(AF_INET, SOCK_STREAM, 0);
     //把socket设置为非阻塞方式
 
     setnonblocking(listenfd);
     //设置与要处理的事件相关的文件描述符
 
     ev.data.fd=listenfd;  //用户设置的数据
     //设置要处理的事件类型
 
     ev.events=EPOLLIN|EPOLLET;
     //注册epoll事件
 
     epoll_ctl(epfd,EPOLL_CTL_ADD,listenfd,&ev);
     
     bzero(&serveraddr, sizeof(serveraddr));
     serveraddr.sin_family = AF_INET;
     serveraddr.sin_port=htons(SERV_PORT);
     serveraddr.sin_addr.s_addr = INADDR_ANY;
     
     {
				int opt = 1;
				setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, (const void *)&opt, sizeof(opt));
     }
     
     	if(bind(listenfd,(sockaddr *)&serveraddr, sizeof(serveraddr)) != 0)
			{
				perror("bind sockfd_one");
				close(listenfd);		
				return -1;
			}
     listen(listenfd, LISTENQ);
     
     maxi = 0;
     for ( ; ; ) {
          //等待epoll事件的发生
 
          nfds=epoll_wait(epfd,events,20,5000);
          //处理所发生的所有事件
 				printf("epoll_wait rtn: %d\n", nfds);
        for(i=0;i<nfds;++i)
        {
               if(events[i].data.fd==listenfd)
               {
                    
                    connfd = accept(listenfd,(sockaddr *)&clientaddr, &clilen);
                    if(connfd<0){
                      perror("connfd<0");
                      exit(1);
                   }
                    setnonblocking(connfd);
                    
                    char *str = inet_ntoa(clientaddr.sin_addr);
                    std::cout<<"connec_ from >>"<<str<<std::endl;
 
                    //设置用于读操作的文件描述符
 
                    ev.data.fd=connfd;
                    //设置用于注测的读操作事件
 
                    ev.events=EPOLLIN|EPOLLET;
                    //注册ev
 
                   epoll_ctl(epfd,EPOLL_CTL_ADD,connfd,&ev);
               }
	            else if(events[i].events&EPOLLIN)
	            {
	                    //printf("reading!\n");
	 
	                    if ( (sockfd = events[i].data.fd) < 0){ printf("sockfd is %d\n",sockfd);continue;}
	                    new_task=new task();
	                    new_task->fd=sockfd;
	                    new_task->next=NULL;
	                    //添加新的读任务
	 
	                    pthread_mutex_lock(&mutex);
	                    if(readhead==NULL)
	                    {
	                      readhead=new_task;
	                      readtail=new_task;
	                    }
	                    else
	                    {
	                     readtail->next=new_task;
	                      readtail=new_task;
	                    }
	                   //唤醒所有等待cond1条件的线程
	 
	                    pthread_cond_broadcast(&cond1);
	                    pthread_mutex_unlock(&mutex);
	              }
	               else if(events[i].events&EPOLLOUT)
	               {
	                 	printf("[EPOLLOUT]\n");/*
	                 	//设置usedata为自定义数据结构
	              		rdata=(struct user_data *)events[i].data.ptr;
	                 	sockfd = rdata->fd;
	                 	write(sockfd, rdata->line, rdata->n_size);
	                 	delete rdata;
	                  //设置用于读操作的文件描述符
	                  ev.data.fd=sockfd;
	                  //设置用于注测的读操作事件
	               		ev.events=EPOLLIN|EPOLLET;
	                  //修改sockfd上要处理的事件为EPOLIN
	               		epoll_ctl(epfd,EPOLL_CTL_MOD,sockfd,&ev);
	               		*/
	               }
                              
          }
          
     }
}
 

void * readtask(void *args)
{
    
   int fd=-1;
   unsigned int n;
   //用于把读出来的数据传递出去
 
   struct user_data *data = NULL;
   while(1){
         
        pthread_mutex_lock(&mutex);
        {
	        //等待到任务队列不为空
	 
	        while(readhead==NULL)
	             pthread_cond_wait(&cond1,&mutex);
	         
	        fd=readhead->fd;
	        //从任务队列取出一个读任务
	 
	        struct task *tmp=readhead;
	        readhead = readhead->next;
	        delete tmp;
        }
        pthread_mutex_unlock(&mutex);
        
        
        
        data = new user_data();
        data->fd=fd;
         
 
        char recvBuf[1024] = {0};
        int ret = 999;
        int rs = 1;
 
        while(rs)
        {
            ret = recv(fd,recvBuf,sizeof(recvBuf) -1 ,0);// 接受客户端消息
 
            if(ret < 0)
            {
                //由于是非阻塞的模式,所以当errno为EAGAIN时,表示当前缓冲区已无数据可//读在这里就当作是该次事件已处理过。
 
                if(errno == EAGAIN)
                {
                    printf("EAGAIN\n");
                    break;
                }
                else{
                    printf("recv error!\n");
                    close(fd);
                    break;
                }
            }
            else if(ret == 0)
            {
            	printf("client socket has close!\n");
            	if (epoll_ctl(epfd, EPOLL_CTL_DEL,fd, 0) < 0)//加入epoll事件集合
              {
                  printf("[%s] epoll_ctl error\n",__func__);
              }
              close(fd);
              break;
            }
            if(ret > 0 ){
            	recvBuf[ret] = '\0';
            	printf("[recv] {%s}\n",recvBuf);
            }
        }
        //发送数据
        {
            char buf[1000] = {0};
            sprintf(buf,"HTTP/1.0 200 OK\r\nContent-type: text/plain\r\n\r\n%s","Hello world!\n");
            send(fd,buf,strlen(buf),0);
            //close(fd);
       }
   }
}

#else

#include <sys/socket.h>  
#include <sys/wait.h>  
#include <netinet/in.h>  
#include <netinet/tcp.h>  
#include <sys/epoll.h>  
#include <sys/sendfile.h>  
#include <sys/stat.h>  
#include <unistd.h>  
#include <stdio.h>  
#include <stdlib.h>  
#include <string.h>  
#include <strings.h>  
#include <fcntl.h>  
#include <errno.h>   
  
#define MAX_EVENTS 10  
#define PORT 8006
  
//设置socket连接为非阻塞模式  
void setnonblocking(int sockfd) {  
    int opts;  
  
    opts = fcntl(sockfd, F_GETFL);  
    if(opts < 0) {  
        perror("fcntl(F_GETFL)\n");  
        exit(1);  
    }  
    opts = (opts | O_NONBLOCK);  
    if(fcntl(sockfd, F_SETFL, opts) < 0) {  
        perror("fcntl(F_SETFL)\n");  
        exit(1);  
    }  
}  
  
int main(){  
    struct epoll_event ev, events[MAX_EVENTS];  
    socklen_t addrlen;
    int listenfd, conn_sock, nfds, epfd, fd, i, nread, n;  
    struct sockaddr_in local, remote;  
    char buf[BUFSIZ];  
  
    //创建listen socket  
    if( (listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {  
        perror("sockfd\n");  
        exit(1);  
    }  
    setnonblocking(listenfd);  
    bzero(&local, sizeof(local));  
    local.sin_family = AF_INET;  
    local.sin_addr.s_addr = htonl(INADDR_ANY);;  
    local.sin_port = htons(PORT);  
    
     {
				int opt = 1;
				setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, (const void *)&opt, sizeof(opt));
     }
     
    if( bind(listenfd, (struct sockaddr *) &local, sizeof(local)) < 0) {  
        perror("bind\n");  
        exit(1);  
    }  
    listen(listenfd, 20);  
  
    epfd = epoll_create(MAX_EVENTS);  
    if (epfd == -1) {  
        perror("epoll_create");  
        exit(EXIT_FAILURE);  
    }  
  
    ev.events = EPOLLIN;  
    ev.data.fd = listenfd;  
    if (epoll_ctl(epfd, EPOLL_CTL_ADD, listenfd, &ev) == -1) {  
        perror("epoll_ctl: listen_sock");  
        exit(EXIT_FAILURE);  
    }  
  
    for (;;) {  
        nfds = epoll_wait(epfd, events, MAX_EVENTS, -1);  
        if (nfds == -1) {  
            perror("epoll_pwait");  
            exit(EXIT_FAILURE);  
        }  
  
        for (i = 0; i < nfds; ++i) {  
            fd = events[i].data.fd;  
            if (fd == listenfd) {  
                if((conn_sock = accept(listenfd,(struct sockaddr *) &remote,   
                                (socklen_t *)&addrlen)) > 0) {  
                    setnonblocking(conn_sock);  
                    ev.events = EPOLLIN | EPOLLET;  
                    ev.data.fd = conn_sock;  
                    if (epoll_ctl(epfd, EPOLL_CTL_ADD, conn_sock,  
                                &ev) == -1) {  
                        perror("epoll_ctl: add");  
                        exit(EXIT_FAILURE);  
                    }  
                }  
                if (conn_sock == -1) {  
                    if (errno != EAGAIN && errno != ECONNABORTED   
                            && errno != EPROTO && errno != EINTR)   
                        perror("accept");  
                }  
                continue;  
            }    
            if (events[i].events & EPOLLIN) {  
                n = 0;  
                while ((nread = read(fd, buf + n, BUFSIZ-1)) > 0) {  
                    n += nread;  
                }
                if (nread == -1 && errno != EAGAIN) {  
                    perror("read error");  
                } 
                ev.data.fd = fd;  
                ev.events = events[i].events | EPOLLOUT;  
                if (epoll_ctl(epfd, EPOLL_CTL_MOD, fd, &ev) == -1) {  
                    perror("epoll_ctl: mod");  
                }  
            }  
            if (events[i].events & EPOLLOUT) {  
                sprintf(buf, "HTTP/1.1 200 OK\r\nContent-Length: %d\r\n\r\nHello World", 11);  
                int nwrite, data_size = strlen(buf);  
                n = data_size;  
                while (n > 0) {  
                    nwrite = write(fd, buf + data_size - n, n);  
                    if (nwrite < n) {  
                        if (nwrite == -1 && errno != EAGAIN) {  
                            perror("write error");  
                        }
                        break;
                    }  
                    n -= nwrite;  
                } 
                close(fd);  
            }  
        }  
    }  
  
    return 0;  
} 


#endif



