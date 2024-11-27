#include <sys/socket.h>  
#include <netinet/in.h>  
#include <arpa/inet.h>  
#include <assert.h>  
#include <stdio.h>  
#include <unistd.h>  
#include <stdlib.h>  
#include <errno.h>  
#include <string.h>  
#include <sys/types.h>  
#include <sys/stat.h>  
#include <fcntl.h>  
#include <sys/sendfile.h>  
#include <time.h>
//设置服务端socket地址重用
int socket_reuseaddr(int st)
{
    int on = 1;
    if (setsockopt(st, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) == -1)
    {
        printf("setsockopt reuseaddr failed ! error message :%s\n",
                strerror(errno));

        return -1;
    }
    return 0;
}
//设置非阻塞
int setnonblock(int st)
{
    if (st < 0)
    {
        printf("function setnonblock param not correct !\n");

        return -1;
    }
    int opts = fcntl(st, F_GETFL);
    if (opts < 0)
    {
        printf("func fcntl failed ! error message :%s\n", strerror(errno));
        return -1;
    }
    opts = opts | O_NONBLOCK;
    if (fcntl(st, F_SETFL, opts) < 0)
    {
        printf("func fcntl failed ! error message :%s\n", strerror(errno));
        return -1;
    }
    return opts;
}
int main( int argc, char* argv[] )  
{  
	if(argc <2) {
		printf("argv not enough .  [file_name]\n");
		return -1;
	}
	//unsigned long longnum=0,sum=0;  
	static char buf[1024];  
	memset(buf,'\0',sizeof(buf));  
	const char* ip = "192.168.109.1";  
	int port = 6789;  
	const char* file_name = argv[1];  
  
	int filefd = open( file_name, O_RDONLY );  
	assert( filefd > 0 );  
	struct stat stat_buf;  
	fstat( filefd, &stat_buf );  
		  
	//FILE *fp=fdopen(filefd,"r");  
		  
	struct sockaddr_in remote_addr;  
	bzero( &remote_addr, sizeof( remote_addr ) );  
	remote_addr.sin_family = AF_INET;  
	inet_pton( AF_INET, ip, &remote_addr.sin_addr );  
	remote_addr.sin_port = htons( port );  
  
	int sock = socket( PF_INET, SOCK_STREAM, 0 );  
	assert( sock >= 0 ); 
	
    /*将套接字绑定到服务器的网络地址上*/  
    if(connect(sock,(struct sockaddr *)&remote_addr,sizeof(struct sockaddr))<0)  
    {  
        perror("connect");  
        return 1;  
    }   
	else  
	{  
		time_t begintime=time(NULL);  
		  
		//while((fgets(buf,1024,fp))!=NULL){  
		//	num=send(connfd,buf,sizeof(buf),0);  
		//	sum+=num;  
		//	memset(buf,'\0',sizeof(buf));  
		//}  
		  
        sendfile( sock, filefd, NULL, stat_buf.st_size );  
		//time_t endtime=time(NULL);  
		//printf("sum:%ld\n",sum);  
		//printf("need time:%d\n",endtime-begintime);  
 
	}  
	close(filefd);
	close( sock );  
	return 0;  
}  
