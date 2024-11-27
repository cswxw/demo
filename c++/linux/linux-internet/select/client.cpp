/*============================================================================= 
#     FileName: tcpcliselect.c 
#         Desc: send data to server and receive data from server 
#       Author: Licaibiao 
#   LastChange: 2017-02-12  
=============================================================================*/  
#include<stdio.h>    
#include<sys/types.h>    
#include<sys/socket.h>    
#include<unistd.h>    
#include<stdlib.h>    
#include<errno.h>    
#include<arpa/inet.h>    
#include<netinet/in.h>    
#include<string.h>    
#include<signal.h>  
#define MAXLINE   1024  
#include <errno.h>
#define LISTENLEN 10  
#define SERV_PORT 6666  

int max(int a, int b)  
{  
    return a>b ? a : b;  
}  

void str_cli(FILE *fp, int sockfd)  
{  
    int         maxfdp1, stdineof;  
    fd_set      rset;  
    char        buf[MAXLINE];  
    int     n;  

    stdineof = 0;  
    for ( ; ; )   
    { 
        FD_ZERO(&rset);  
        int ret=0;
        if (stdineof == 0)  
            FD_SET(fileno(fp), &rset);  
        FD_SET(sockfd, &rset);  
        maxfdp1 = max(fileno(fp), sockfd) + 1; 
        //maxfdp1 = 3;
        ret = select(maxfdp1, &rset, NULL, NULL, NULL);  
        if(ret <0 ){
            perror("select");
            return;
        }

        if (FD_ISSET(sockfd, &rset))   
        {     
            printf("[info] socket signed\n");
            if ( (n = read(sockfd, buf, MAXLINE)) == 0) /* socket is readable */  
            {  
                if (stdineof == 1)  {
                    printf("[info] normal termination\n");
                    return;     /* normal termination */  
                }
                else  {
                    //perror("server close");
                    printf("str_cli: server terminated prematurely : %d \n",errno);  
                    return ;
                }
            } else if( n <0 ){ perror("read return -1");  return; }
            const char *tip= "[recv] :";
            write(fileno(stdout), tip, strlen(tip));  
            
            write(fileno(stdout), buf, n);  
        }  

        if (FD_ISSET(fileno(fp), &rset))  /* input is readable */  
        {    
            if ( (n = read(fileno(fp), buf, MAXLINE)) == 0)   
            {  
                printf("[info] input shutdown\n");
                stdineof = 1;  
                shutdown(sockfd, SHUT_WR);  /* send FIN */  
                FD_CLR(fileno(fp), &rset);  
                continue;  
            }  
            buf[n] = '\0';
            printf("[info] : write to socket  %s", buf);
            write(sockfd, buf, n);
            printf("[info] write return\n")  ;
        }  
    }  
}  

int main(int argc, char **argv)  
{  
    int sockfd;  
    struct sockaddr_in  servaddr;  

    sockfd = socket(AF_INET, SOCK_STREAM, 0);  

    bzero(&servaddr, sizeof(servaddr));  
    servaddr.sin_family = AF_INET;  
    servaddr.sin_port = htons(SERV_PORT);  
    inet_pton(AF_INET, "127.0.0.1", &servaddr.sin_addr);  

    connect(sockfd, (struct sockaddr*) &servaddr, sizeof(servaddr));  

    str_cli(stdin, sockfd);     /* do it all */  

    exit(0);  
}  
