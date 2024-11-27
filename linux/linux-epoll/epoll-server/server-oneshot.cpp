#include<sys/socket.h>  
#include<netinet/in.h>  
#include<arpa/inet.h>  
#include<assert.h>  
#include<stdio.h>  
#include<unistd.h>  
#include<errno.h>  
#include<string.h>  
#include<fcntl.h>  
#include<stdlib.h>  
#include<sys/epoll.h>  
#include<pthread.h>  
#include<iostream>  
#include<netinet/tcp.h>
#include <sys/types.h>  
#include <sys/stat.h>
#include <dirent.h>
#include <sys/sendfile.h> 

#define MYIP "172.16.226.131"
#define MAX_EVENT_NUMBER 20//最大事件连接数  

#ifndef MAX_PATH
#define MAX_PATH 260
#endif
#define DATA_SIZE 1024 * 8   //8kb
struct SRProtol{
	/*
		h: hello
		c: 检查文件路径是否存在  
		q: 请求文件数据
		s: 发送文件数据    此时  data字段有效
	*/
	char cmd;                 //控制码
	union{
		char filepath[MAX_PATH];  //文件路径
		char filename[MAX_PATH];  //文件名称
	};
	
	unsigned long long filelen;    // 最大文件长度限制4GB  //sizeof(unsigned long):4
	unsigned int randomIp;
	unsigned int randomPort;
	//unsigned int start_off;   
	//unsigned int data_size;
	//unsigned char data[DATA_SIZE];
}__attribute__ ((packed));

struct Response{
	char cmd;            //
	in_addr serverIp;
	uint32_t  serverPort;
	char savePath[MAX_PATH];
	int  sockfd;      //random socket
	char buf[MAX_PATH];  //data
}__attribute__ ((packed));

using namespace std;  
struct fds{//文件描述符结构体，用作传递给子线程的参数  
	int epollfd;  
	int sockfd;  
};  
int setnonblocking(int fd){//设置文件描述符为非阻塞  
	int old_option=fcntl(fd,F_GETFL);  
	int new_option=old_option|O_NONBLOCK;  
	fcntl(fd,F_SETFL,new_option);  
	return old_option;  
}  
void addfd(int epollfd,int fd,bool oneshot){//为文件描述符添加事件  
	epoll_event event;  
	event.data.fd=fd;  
	event.events=EPOLLIN|EPOLLET;  
	if(oneshot){//采用EPOLLONETSHOT事件  
		event.events|=EPOLLONESHOT;  
	}  
	epoll_ctl(epollfd,EPOLL_CTL_ADD,fd,&event);  
	setnonblocking(fd);  
}  
void reset_oneshot(int epollfd,int fd){//重置事件  
	epoll_event event;  
	event.data.fd=fd;  
	event.events=EPOLLIN|EPOLLET|EPOLLONESHOT;  
	epoll_ctl(epollfd,EPOLL_CTL_MOD,fd,&event);  
} 


 /* Read "n" bytes from a descriptor. */  
size_t readn(int fd, void *vptr, size_t n)  {       
	size_t nleft;  
	ssize_t nread;  
	char *ptr;  

	ptr = (char *)vptr;  
	nleft = n;  
	while (nleft > 0) {  
		if ( (nread = read(fd, ptr, nleft)) < 0) {  
			if (errno == EINTR)  
				nread = 0;  /* and call read() again */  
			else  
				return(-1);  
		} else if (nread == 0)  
			break;    /* EOF */  
		nleft -= nread;  
		ptr   += nread;  
	}  
	return(n - nleft);  /* return >= 0 */  
}
int createRandomSocket(Response * rs){
	int sock=socket(AF_INET,SOCK_STREAM,0);  
    if(sock<0)  
    {  
        perror("random socket");  
        return -1;
    }  
  
    struct sockaddr_in local;  
    local.sin_family=AF_INET;  
    local.sin_port=htons(0);  
    local.sin_addr.s_addr=inet_addr(MYIP);  
  
    socklen_t len=sizeof(local);  
  
   if(bind(sock,(struct sockaddr*)&local,len)<0)  
   {  
     perror("random bind");  
		return -1;
    }  
  
    if(listen(sock,1)<0)  
    {  
        perror("random listen");  
        return -1;
    }  
	sockaddr_in listendAddr;
	socklen_t listendAddrLen = sizeof(listendAddr);
	getsockname(sock, (struct sockaddr *)&listendAddr, &listendAddrLen);//获取监听的地址和端口  
	printf("[createRandomSocket]random listen address = %s:%d\n", inet_ntoa(listendAddr.sin_addr), ntohs(listendAddr.sin_port)); 
	rs->serverIp = listendAddr.sin_addr;
	rs->serverPort = listendAddr.sin_port;
	rs->sockfd = sock;
   return 0;   

}


void* RandomSocketRecv(void* arg){
	if(arg ==  NULL) return 0;
	Response * pRes = (Response*) arg;
	int confd;

	struct timeval timeout = {6,0};  
	if (setsockopt(pRes->sockfd, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(struct timeval)) != 0)  
	{  
		printf("[RandomSocketRecv]set accept timeout failed\n");
	} 
	struct sockaddr_in client_address;  
	socklen_t client_addrlength=sizeof(client_address);  
    if((confd = accept(pRes->sockfd, (struct sockaddr*)&client_address, &client_addrlength)) == -1){  
        printf("[RandomSocketRecv]accept socket error: %s(errno: %d)",strerror(errno),errno);
		close(pRes->sockfd);
        return 0;
    }
	printf("[RandomSocketRecv] client ip:%s port :%d\n",inet_ntoa(client_address.sin_addr), ntohs(client_address.sin_port));
	char buf[1024*8];
	int fp = open(pRes->savePath,O_WRONLY|O_CREAT,S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH);
	if(fp < -1){
		perror("[RandomSocketRecv]open file error");

		close(confd);
		close(pRes->sockfd);
		return 0;
	}
	int len=0;
	while(1){
		len = recv(confd, buf, sizeof(buf),0);
		if(len == 0){
			break;
		}else if(len < -1){
			break;
		}else{
			write(fp,buf,len);
		}

	}
	
	printf("[RandomSocketRecv] recv finished  .recv ret:%d\n",len);
	close(fp);
	close(confd);
	close(pRes->sockfd);
	free(arg);
	return 0;
}


int isrfile(char *path)
{
	struct stat buf;
	int cc;
	int euid,egid;
	cc=stat(path,&buf);
	if(!cc) {
		//S_IFMT是一个掩码，它的值是017000（注意这里用的是八进制）用来过滤出前四位表示的文件类型
		if((buf.st_mode & S_IFMT) != S_IFREG) return 0;
		euid=geteuid(); //返回有效用户的ID
		egid=getegid();  //取得执行目前进程有效组识别码
		if(euid==0) {
			if(buf.st_mode & S_IRUSR || buf.st_mode & S_IRGRP ||
			   buf.st_mode & S_IROTH)
				 return 1;
			else return 0;
		}
		if((buf.st_mode & S_IROTH)!=0) return 1;   //S_IROTH group has read permission
		if((buf.st_gid == egid) && ((buf.st_mode & S_IRGRP)!=0))   //S_IRGRP group has read permission 
				return 1;
		if((buf.st_uid == euid) && ((buf.st_mode & S_IRUSR)!=0))   //S_IRUSR owner has read permission
				return 1;

	}
	return cc;
}
void* RandomSocketSend(void* arg){
	SRProtol* sp = (SRProtol*)arg;
	//unsigned long longnum=0,sum=0;  
	static char buf[1024];  
	memset(buf,'\0',sizeof(buf));  
 
	const char* file_name = sp->filepath;  
	printf("[%s] open file %s\n",__FUNCTION__,file_name);
	int filefd = open( file_name, O_RDONLY );  
	assert( filefd > 0 );  
	struct stat stat_buf;  
	fstat( filefd, &stat_buf );  
	printf("[send] file size:%ld\n",stat_buf.st_size);
	//FILE *fp=fdopen(filefd,"r");  
		  
	struct sockaddr_in remote_addr;  
	bzero( &remote_addr, sizeof( remote_addr ) );  
	remote_addr.sin_family = AF_INET;  
	
	remote_addr.sin_addr = *(in_addr*)&sp->randomIp;
	remote_addr.sin_port = sp->randomPort;
  
	printf("[send] random ip:%s  port:%d\n",inet_ntoa(remote_addr.sin_addr), ntohs(remote_addr.sin_port));
  
	int sock = socket( AF_INET, SOCK_STREAM, 0 );  
	assert( sock >= 0 ); 
	//sleep(1);

    /*将套接字绑定到服务器的网络地址上*/  
    if(connect(sock,(struct sockaddr *)&remote_addr,sizeof(struct sockaddr))<0)  
    {  
        perror("connect");  
        return 0;  
    }   
	else  
	{	
		char buff[1024];
		//time_t begintime=time(NULL);  
		printf("[send] start sendfile\n");
		
		
		while(1){
			
			int ret = read(filefd, buff,sizeof(buff));
			if( ret == 0){
				printf("[send] read file EOF\n");
				break;
			}else if(ret < 0){
				printf("[send] read file error. errCode :%d %s\n",errno,strerror(errno));
				break;
			}else{
				//printf("[info] read size:%d\n",ret);
				int send_len = send(sock, buff,ret,0);
				if(send_len < 0){
					printf("[send] send error. errCode :%d %s\n",errno,strerror(errno));
					break;
				}else if(send_len == 0){
					printf("[send] send len == 0.\n");
					break;
				}
			}
			
			
			/*
			int ret = sendfile(sock, filefd, NULL, stat_buf.st_size); //stat_buf.st_size
			if(ret == -1){
				printf("[send] sendfile error. errCode :%d %s\n",errno,strerror(errno));
				break;
			}else if(ret == 0){
				break;
			}
			*/
			
			

			
		}
		printf("[send] end sendfile\n");

	}  
	printf("[send] finished. errCode :%d %s\n",errno,strerror(errno));
	close(sock);
	close(filefd);
	
	
	free(arg);
	return 0;
}

/*

处理接收到的数据
return :
	1:   继续保持连接
	0:   关闭连接
	-1:  发送错误，关闭连接
*/
int handleMessage(int sockfd,SRProtol * st){
	printf("***handleMessage start\n");
	Response *rp = (Response*)calloc(sizeof(Response),1);
	struct sockaddr_in peerAddr;
	socklen_t peerLen = sizeof(sockaddr_in);
	SRProtol *sp=NULL;
	char buff[MAX_PATH]={0};
	int fp ;
	pthread_t thread;  
	rp->cmd=st->cmd;
	switch(st->cmd){
		case 'h':
			strcpy(rp->buf,"h ok");
			send(sockfd,(const char*)rp,sizeof(Response),0);
			break;
		case 's':
			printf("[info]prepare recv file\n");
			if(createRandomSocket(rp) < 0){
				printf("program except");
				exit(-1);
			}
			sprintf(rp->savePath, "/home/adcsw/code/epoll-server/file/%s",st->filename);
			send(sockfd,(const char*)rp,sizeof(Response),0);  // send random serverip and port
			
			pthread_create(&thread,NULL, RandomSocketRecv,(void*)rp);//调用工作者线程处理数据  
			break;
		case 'e':
			printf("[info]finished recv file\n");
			break;
		case 'f':
			
			//fp = open(buff,O_WRONLY|O_CREAT,S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH);
			//lseek(fp,st->start_off,SEEK_SET);
			//write(fp,st->data,st->data_size);
			//close(fp);
			break;
		case 'c':   // 检查文件
			if(isrfile(st->filepath) == 1){
				strcpy(rp->buf,"ok");
			}else{
				strcpy(rp->buf,"no");
			}
			send(sockfd,(const char*)rp,sizeof(Response),0);
			break;
		case 'r':
			printf("[info]prepare send file\n");
			sp = (SRProtol*)calloc(sizeof(SRProtol),1);
			memcpy(sp,st,sizeof(SRProtol));
			
			//更新远程ip字段
			getpeername(sockfd, (struct sockaddr *)&peerAddr, &peerLen);
			sp->randomIp = *(unsigned int*)&peerAddr.sin_addr;
			
			pthread_create(&thread,NULL, RandomSocketSend,(void*)sp);//调用工作者线程处理数据  
			break;
		case 'q':
			break;
		default:
			break;
	}
	
	printf("***handleMessage finished\n");
	return 1;
}



void* worker(void* arg){//工作者线程(子线程)接收socket上的数据并重置事件  
	int sockfd=((fds*)arg)->sockfd;  
	int epollfd=((fds*)arg)->epollfd;//事件表描述符从arg参数(结构体fds)得来  
	//cout<<"***start new thread to receive data on fd:"<<sockfd<<endl;  
	SRProtol st;

	while(1){  
		int ret = 0;
		memset(&st,'\0',sizeof(SRProtol));//缓冲区置空  
		ret=recv(sockfd,(char*)&st,sizeof(SRProtol),0);//接收数据  
		//ret = readn(sockfd,buf,sizeof(SRProtol));
		if(ret==0){//关闭连接  
			close(sockfd);  
			cout<<"close "<<sockfd<<endl;  
			break;  
		}  
		else if(ret<0){  
			if(errno == EAGAIN||errno == EWOULDBLOCK||errno == EINTR){//并非网络出错，而是可以再次注册事件  
				reset_oneshot(epollfd,sockfd);  
				cout<<"reset epollfd"<<endl;  
				break;  
			}
		}  
		else{
			printf("[worker]  recv data recv ret:%d\n",ret);
			if(ret == sizeof(SRProtol)){
				//handle message
				if(handleMessage(sockfd,&st)<=0){
					close(sockfd);  
					cout<<"handleMessage to close "<<sockfd<<endl; 
					break;
				}
			}

		}  
	}  
	//cout<<"***thread exit on fd:"<<sockfd << endl << endl;    
	return NULL;  
}  

//设置服务端socket地址重用
int socket_reuseaddr(int st)
{
    int on = 1;
    if (setsockopt(st, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) == -1)
    {
        printf("setsockopt reuseaddr failed ! error message :%s\n",
                strerror(errno));
        //close socket
        close(st);
        return -1;
    }
    return 0;
}
int setRecvTimeout(int sockfd, int time){
	 //超时时间 
    struct timeval timeout;  
    timeout.tv_sec = time;  
    timeout.tv_usec = 0;  
      
    socklen_t len = sizeof(timeout);  
    if (setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &timeout, len) == -1) {  
		printf("setsockopt SO_RCVTIMEO failed ! error message :%s\n",strerror(errno)); 
        close(sockfd);
        return -1;  
    } 
	return 0;
}

//将sockaddr_in转化成IP地址
int sockaddr_toa(const struct sockaddr_in * addr, char * ipaddr)
{
    if (addr == NULL || ipaddr == NULL)
    {
        return -1;
    }
    unsigned char *p = (unsigned char *) &(addr->sin_addr.s_addr);
    sprintf(ipaddr, "%u.%u.%u.%u", p[0], p[1], p[2], p[3]);
    return 0;
}
int main(int argc,char* argv[]){  

	int port=6789;  
	int ret=0;  
	struct sockaddr_in address;  
	bzero(&address,sizeof(address));  
	address.sin_family=AF_INET;  
	inet_pton(AF_INET,MYIP,&address.sin_addr);  //第一个参数af是地址簇，第二个参数*src是来源地址，第三个参数* dst接收转换后的数据
	address.sin_port=htons(port);  
	
	int listenfd=socket(PF_INET,SOCK_STREAM,0);  
	 //reuseaddr
    if (socket_reuseaddr(listenfd) < 0){
        return -1;
    }
	//recvTimeout
    //if (setRecvTimeout(listenfd,5) < 0){
    //    return -1;
    //}
	
	int  keepIdle = 3;
	int  keepInterval = 3;
	int  keepCount = 2;
	/*开始首次KeepAlive探测前的TCP空闭时间 */
	//setsockopt(listenfd, SOL_TCP, TCP_KEEPIDLE, (void *)&keepIdle, sizeof(keepIdle)); 
	/* 两次KeepAlive探测间的时间间隔  */
	//setsockopt(listenfd, SOL_TCP,TCP_KEEPINTVL, (void *)&keepInterval, sizeof(keepInterval));
	/* 判定断开前的KeepAlive探测次数*/
	//setsockopt(listenfd,SOL_TCP, TCP_KEEPCNT, (void *)&keepCount, sizeof(keepCount));
	
	assert(listenfd>=0);  
	ret=bind(listenfd,(struct sockaddr*)&address,sizeof(address));  
	printf("%d:%s\n",errno,strerror(errno));
	assert(ret!=-1);  
	ret=listen(listenfd,5);  
	assert(ret!=-1);  
	epoll_event events[MAX_EVENT_NUMBER];  
	int epollfd=epoll_create(MAX_EVENT_NUMBER);  
	assert(epollfd!=-1);  
	addfd(epollfd,listenfd,false);//不能将监听端口listenfd设置为EPOLLONESHOT否则会丢失客户连接  
	
	printf("[debug]sizeof(SRProtol):%d\n",sizeof(SRProtol));
	printf("[debug]sizeof(Response):%d\n",sizeof(Response));
	
	
	while(1){  
		int ret=epoll_wait(epollfd,events,MAX_EVENT_NUMBER,-1);//等待事件发生  
		if(ret<0){  
			cout<<"epoll error"<<endl;  
			break;  
		}  
		for(int i=0;i<ret;i++){  
			int sockfd=events[i].data.fd;  
			if(sockfd==listenfd){//监听端口  
				struct sockaddr_in client_address;  
				socklen_t client_addrlength=sizeof(client_address);  
				int connfd=accept(listenfd,(struct sockaddr*)&client_address,&client_addrlength);  
				addfd(epollfd,connfd,true);//新的客户连接置为EPOLLONESHOT事件  
				char ipaddr[20] = { 0 };
				sockaddr_toa(&client_address, ipaddr);
				printf("accept by %s\n", ipaddr);
			}  
			else if(events[i].events&EPOLLIN){//客户端有数据发送的事件发生  
				pthread_t thread;  
				fds fds_for_new_worker;  
				fds_for_new_worker.epollfd=epollfd;  
				fds_for_new_worker.sockfd=sockfd;  
				pthread_create(&thread,NULL, worker,(void*)&fds_for_new_worker);//调用工作者线程处理数据  
			}  
			else{  
				cout<<"something wrong"<<endl;  
			}  
		}  
	}  
	close(listenfd); 
	close(epollfd);
	return 0;  
}  
