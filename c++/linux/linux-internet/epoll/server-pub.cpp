//�����̷����
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

#define MAXSOCKET 20

int main(int arg, char *args[])
{
    if (arg < 2)
    {
        printf("please print one param!\n");
        return -1;
    }
    //create server socket
    int listen_st = server_socket(atoi(args[1]));
    if (listen_st < 0)
    {
        return -1;
    }
    /*
     * ����epoll_event�ṹ�����ev������ev����ע���¼���
     * ����events���ڻش���Ҫ������¼�
     */
    struct epoll_event ev, events[100];
    //�������ڴ���accept��epollר���ļ�������
    int epfd = epoll_create(MAXSOCKET);
    //��socket���óɷ�������ʽ
    setnonblock(listen_st);
    //������Ҫ�ŵ�epoll������ļ�������
    ev.data.fd = listen_st;
    //��������ļ���������Ҫepoll��ص��¼�
    /*
     * EPOLLIN�����ļ����������¼�
     *accept,recv���Ƕ��¼�
     */
    ev.events = EPOLLIN | EPOLLERR | EPOLLHUP;
    /*
     * ע��epoll�¼�
     * ����epoll_ctl��&ev������ʾ��Ҫepoll���ӵ�listen_st���socket�е�һЩ�¼�
     */
    epoll_ctl(epfd, EPOLL_CTL_ADD, listen_st, &ev);

    while (1)
    {
        /*
         * �ȴ�epoll���е�socket�����¼�������һ������Ϊ������
         * events���������������epoll_event���͵�����
         * ���epoll���е�һ�����߶��socket�����¼���
         * epoll_wait�ͻ᷵�أ�����events�д���˷����¼���socket�����socket���������¼�
         * ����ǿ��һ�㣬epoll�ش�ŵ���һ����socket������һ����socket�¼�
         * һ��socket�����ж���¼���epoll_wait���ص�������Ϣ��socket����Ŀ
         * ���epoll_wait�����¼����������ĳ������ȴû�д���ǰsocket�������¼�
         * ��ôepoll_wait�������ٴ�����������ֱ�ӷ��أ�����events����ľ��Ǹղ��Ǹ�socketû�б�������¼�
         */
        int nfds = epoll_wait(epfd, events, MAXSOCKET, -1);
        if (nfds == -1)
        {
            printf("epoll_wait failed ! error message :%s \n", strerror(errno));
            break;
        }
        int i = 0;
        for (; i < nfds; i++)
        {
            if (events[i].data.fd < 0)
                continue;
            if (events[i].data.fd == listen_st)
            {
                //���տͻ���socket
                int client_st = server_accept(listen_st);
                /*
                 * ��⵽һ���û���socket���ӵ�������listen_st�󶨵Ķ˿�
                 *
                 */
                if (client_st < 0)
                {
                    continue;
                }
                //���ÿͻ���socket������
                setnonblock(client_st);
                //���ͻ���socket���뵽epoll����
                struct epoll_event client_ev;
                client_ev.data.fd = client_st;
                client_ev.events = EPOLLIN | EPOLLERR | EPOLLHUP;
                epoll_ctl(epfd, EPOLL_CTL_ADD, client_st, &client_ev);
                /*
                 * ע�ͣ���epoll����listen_st���������socket����Ϣ��ʱ��
                 * ֻ���������Կͻ��˵�������Ϣ
                 * recv,sendʹ�õĶ��ǿͻ��˵�socket��������listen_st������Ϣ��
                 */
                continue;
            }
            //�ͻ������¼�����
            if (events[i].events & EPOLLIN)
            {
                //��ʾ��������ߵ�client_st���յ���Ϣ
                if (socket_recv(events[i].data.fd) < 0)
                {
                    close_socket(events[i].data.fd);
                    //�������ݳ�����߿ͻ����Ѿ��ر�
                    events[i].data.fd = -1;
                    /*����continue����Ϊ�ͻ���socket�Ѿ����ر��ˣ�
                     * �������socket���ܻ����������¼��������ִ���������¼���
                     * �������socket�Ѿ������ó�-1
                     * ���Ժ����close_socket()�������ᱨ��
                     */
                    continue;
                }
                /*
                 * �˴�����continue����Ϊÿ��socket�������ж���¼�ͬʱ���͵���������
                 * ��Ҳ�����������if������if-else��ԭ��
                 */

            }
            //�ͻ������¼�����
            if (events[i].events & EPOLLERR)
            {
                printf("EPOLLERR\n");
                //���س����¼����ر�socket������epoll�أ����ر�socket����events[i].data.fd=-1,epoll���Զ�����socket�ӳ������
                close_socket(events[i].data.fd);
                events[i].data.fd = -1;
                continue;
            }
            //�ͻ������¼�����
            if (events[i].events & EPOLLHUP)
            {
                printf("EPOLLHUP\n");
                //���ع����¼����ر�socket������epoll��
                close_socket(events[i].data.fd);
                events[i].data.fd = -1;
                continue;
            }
        }
    }
    //close epoll
    close(epfd);
    //close server socket
    close_socket(listen_st);
    return 0;
}