#include <stdio.h>
#include <stdlib.h>

#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>

/**
 *  创建一个SocketPair，通过返回的两个fd可以进行进程间通信
 *  @param family : 套接字对间使用的协议族，可以是AF_INET或AF_LOCAL
 *  @param type : 套接字类型
 *  @param protocol : 协议类型
 *  @param fd[2] : 将要创建的Socketpair两端的文件描述符
 */
int
Socketpair(int family, int type, int protocol, int fd[2])
{
    int32_t listener = -1;
    int32_t connector = -1;
    int32_t acceptor = -1;
    struct sockaddr_in listen_addr;
    struct sockaddr_in connect_addr;
    unsigned int size;

    if (protocol || 
            (family != AF_INET && family != AF_LOCAL)) {
        fprintf(stderr, "EAFNOSUPPORT\n");
        return -1;
    }
    if (!fd) {
        fprintf(stderr, "EINVAL\n");
        return -1;
    }

    /*创建listener，监听本地的换回地址，端口由内核分配*/
    listener = socket(AF_INET, type, 0);
    if (listener < 0)
        return -1;
    memset(&listen_addr, 0, sizeof(listen_addr));
    listen_addr.sin_family = AF_INET;
    listen_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    listen_addr.sin_port = 0;    /* kernel chooses port.    */
    if (bind(listener, (struct sockaddr *) &listen_addr, sizeof(listen_addr))
            == -1)
        goto fail;
    if (listen(listener, 1) == -1)
        goto fail;

    /*创建connector, 连接到listener, 作为Socketpair的一端*/
    connector = socket(AF_INET, type, 0);
    if (connector < 0)
        goto fail;
    /* We want to find out the port number to connect to.  */
    size = sizeof(connect_addr);
    if (getsockname(listener, (struct sockaddr *) &connect_addr, &size) == -1)
        goto fail;
    if (size != sizeof(connect_addr))
        goto fail;
    if (connect(connector, (struct sockaddr *) &connect_addr,
                sizeof(connect_addr)) == -1)
        goto fail;

    size = sizeof(listen_addr);
    /*调用accept函数接受connector的连接，将返回的文件描述符作为Socketpair的另一端*/
    acceptor = accept(listener, (struct sockaddr *) &listen_addr, &size);
    if (acceptor < 0)
        goto fail;
    if (size != sizeof(listen_addr))
        goto fail;
    close(listener);
    
    /**
     * 至此，我们已经创建了两个连接在一起的文件描述符，
     * 通过向其中任意一个发送数据，都会“转发”到另一个，即可以实现进程间的通信
     */
     
    /* Now check we are talking to ourself by matching port and host on the
       two sockets.     */
    if (getsockname(connector, (struct sockaddr *) &connect_addr, &size) == -1)
        goto fail;
    if (size != sizeof(connect_addr)
            || listen_addr.sin_family != connect_addr.sin_family
            || listen_addr.sin_addr.s_addr != connect_addr.sin_addr.s_addr
            || listen_addr.sin_port != connect_addr.sin_port)
        goto fail;
    fd[0] = connector;
    fd[1] = acceptor;

    return 0;

fail:
    if (listener != -1)
        close(listener);
    if (connector != -1)
        close(connector);
    if (acceptor != -1)
        close(acceptor);

    return -1;
}




#include <sys/types.h>
#include <sys/socket.h>

#include <stdlib.h>
#include <stdio.h>

int Socketpair(int, int, int, int[]);

int main ()
{
    int fds[2];

    int r = Socketpair(AF_INET, SOCK_STREAM, 0, fds);
    if (r < 0) {
        perror( "socketpair()" );
        exit( 1 );
    }

    if(fork()) {
        /*  Parent process: echo client */
        int val = 0;
        close( fds[1] );
        while ( 1 ) {
            sleep(1);
            ++val;
            printf( "Sending data: %d\n", val );
            write( fds[0], &val, sizeof(val) );
            read( fds[0], &val, sizeof(val) );
            printf( "Data received: %d\n", val );
        }
    }
    else {
        /*  Child process: echo server */
        int val;
        close( fds[0] );
        while ( 1 ) {
            read( fds[1], &val, sizeof(val) );
            ++val;
            write( fds[1], &val, sizeof(val) );
        }
    }
}



