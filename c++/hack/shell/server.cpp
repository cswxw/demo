#include <winsock2.h>
#include <mstcpip.h>
#include <windows.h>
#include <stdio.h>

 
#pragma comment(lib,"Ws2_32.lib")  
#pragma comment(lib,"Mswsock.lib")  

fd_set socketSet;

DWORD WINAPI RemoteReadThread(LPVOID lparam){
	char buf[4096];
	SOCKET sListen = (SOCKET)lparam;
     
    FD_ZERO(&socketSet);  
    FD_SET(sListen,&socketSet);  

    fd_set    readSet;  
    FD_ZERO(&readSet);  

	fd_set    writeSet;  
    FD_ZERO(&readSet);

	fd_set    exceptfds;  
    FD_ZERO(&exceptfds);

	while(1){
		readSet = socketSet;   
		//writeSet = socketSet;   
		int   nRetAll = select(0,&readSet,NULL,NULL,NULL/*&time*/);//�������ó�ʱ��selectΪ����  
        if(nRetAll >0 )   //-1
		{  
			//�Ƿ���ڿͻ��˵���������  
			if(FD_ISSET(sListen ,&readSet))//��readset�л᷵���Ѿ����ù�listen���׽��֡�  
			{  
				
				if(socketSet.fd_count < FD_SETSIZE)  
				{  
					sockaddr_in addrRemote;  
					int nAddrLen = sizeof(addrRemote);  
					SOCKET sClient = accept(sListen,(sockaddr*)&addrRemote,&nAddrLen); 
					if(sClient!=INVALID_SOCKET)  
					{ 
						FD_SET(sClient,&socketSet);  
						printf("\n���յ����ӣ�(%s) %d\n",inet_ntoa(addrRemote.sin_addr),sClient);  
					}
				}  
				else  
				{  
					printf("���������Ѵ����ޣ�\n");  
					//continue;  
				}   
			}  
			
			
			for(int i=1;i<socketSet.fd_count;i++)  
			{  
				if(FD_ISSET(socketSet.fd_array[i], &readSet)  )  
				{  
					//����recv���������ݡ� 
					int nRecv = recv(socketSet.fd_array[i],buf,sizeof(buf),0);  
					if(nRecv > 0)  
					{  
						buf[nRecv] = 0;  
						printf("%s", buf);  
					}else{
						printf("ɾ��socket %d\n",socketSet.fd_array[i]);
						closesocket(socketSet.fd_array[i]);
						FD_CLR(socketSet.fd_array[i],&socketSet);
					}
				}  
 
				//if(FD_ISSET(socketSet.fd_array[i] , &exceptfds)  )
				//{ 
					
				//}  
			}  
			
		}  
        else if(nRetAll == 0)  
        {  
			printf("time out!\n");  
        }  
        else  
        {  
            printf("select error!%d\n",WSAGetLastError());  
            //Sleep(5000);  
            break;  
        }  

	}
	return 0;
}

    void safe_flush(FILE *fp)
    {
    	int ch;
    	while ((ch = fgetc(fp)) != EOF && ch != '\n');
    }
int main(){

    WSAData wsaData;  
    if(0 != WSAStartup(MAKEWORD(2,2),&wsaData))  
    {  
        printf("��ʼ��ʧ��!%d\n",WSAGetLastError());  
        Sleep(5000);  
        return -1;  
    }
	USHORT nport = 1234;  
	SOCKET sListen = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);  

	u_long ul = 1;  
	ioctlsocket(sListen,FIONBIO,&ul);  

	sockaddr_in sin;  
	sin.sin_family = AF_INET;  
	sin.sin_port = htons(nport);  
	sin.sin_addr.S_un.S_addr = ADDR_ANY;  


	if(SOCKET_ERROR == bind(sListen,(sockaddr*)&sin,sizeof(sin)))  
	{  
		printf("bind failed!%d\n",WSAGetLastError());  
		return -1;  
	}	
	listen(sListen,5);  

	DWORD threadId;
	HANDLE m_hThreadWrite = CreateThread(NULL, 0, RemoteReadThread, (LPVOID)sListen, 0, &threadId); // �����߳�
	
	while(1){
		char buff[1024];
		
		scanf("%[^\n]",buff);
		
		if(strcmp(buff,"quit") == 0){
			break;
		}
		if(socketSet.fd_count > 1){
			strcat(buff,"\r\n");
			send(socketSet.fd_array[1],buff,strlen(buff),0);
		}else{
			printf("no client !\n");
		}
		safe_flush(stdin);
		

	}


    closesocket(sListen);  
    WSACleanup();  
	return 0;
}