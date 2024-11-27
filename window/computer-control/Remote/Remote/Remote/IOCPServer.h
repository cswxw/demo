
#pragma once
#include "CpuUseage.h"
#include "Buffer.h"

#include <winsock2.h>
#pragma comment(lib,"ws2_32.lib")




enum IOType 
{
	IOInitialize,
	IORead,
	IOWrite,
	IOIdle
};


#define	NC_CLIENT_CONNECT		0x0001
#define	NC_CLIENT_DISCONNECT	0x0002
#define	NC_TRANSMIT				0x0003
#define	NC_RECEIVE				0x0004
#define NC_RECEIVE_COMPLETE		0x0005 // ��������

#define HDR_SIZE	13
#define FLAG_SIZE	5
#define HUERISTIC_VALUE 2

#define SIO_KEEPALIVE_VALS    _WSAIOW(IOC_VENDOR,4)

struct tcp_keepalive {  //�׽��ֵı������
	ULONG onoff;
	ULONG keepalivetime;
	ULONG keepaliveinterval;
};


struct ClientContext     //����Ŀͻ����±�����
{
	 SOCKET				m_Socket;                   // �׽���(��һ���źŽ���ͨ��ʹ�õ�)
	 CBuffer		    m_CompressionBuffer;	    // ���յ���ѹ��������
	 CBuffer			m_DeCompressionBuffer;	    // ��ѹ�������
	 CBuffer			m_ResendWriteBuffer;	    // �ϴη��͵����ݰ�������ʧ��ʱ�ط�ʱ�� Ҳ���Ǳ��ݵ�����
	 CBuffer			m_WriteBuffer;              // �򱻿ض˷��͵�����
	 WSABUF				m_wsaInBuffer;              // ����û�а취��ΪIOCPϵ�еĺ�����Ҫ��ʹ��WSABUF
	 BYTE				m_byInBuffer[8192];
	 BOOL				m_bIsMainSocket;            // ����socket �û�

	 LONG				m_nMsgIn;
	 LONG				m_nMsgOut;
	 WSABUF				m_wsaOutBuffer;
	 int				m_Dialog[2]; // �ŶԻ����б��ã���һ��int�������Ƿ���Dlg���ڶ�����CDialog�ĵ�ַ�Ǹ�Dlg
};

typedef void (CALLBACK* NOTIFYPROC)(LPVOID, ClientContext*, UINT nCode);

typedef CList<ClientContext*, ClientContext* > ContextList;                

class CMainFrame;




#include "Mapper.h"
class CIOCPServer
{
public:
	NOTIFYPROC					m_pNotifyProc;
	CMainFrame*					m_pFrame;                    //���Ի���


	LONG					    m_nCurrentThreads;
	LONG					    m_nBusyThreads;
	bool					    m_bTimeToKill;
	UINT					    m_nMaxConnections;          // ���������

	CIOCPServer();
	virtual ~CIOCPServer();         //�׽��ֿ�ĵ�Clearup

	void CIOCPServer::Shutdown();   //��Դ���ͷ� �׽��ֵĹر�

	bool CIOCPServer::Initialize(NOTIFYPROC pNotifyProc, CMainFrame* pFrame, int nMaxConnections, int nPort);
	static unsigned __stdcall ListenThreadProc(LPVOID lpVoid);   //ע�������ж����̻߳ص�ʱһ��Ҫʹ������Static 
	static unsigned __stdcall ThreadPoolFunc(LPVOID WorkContext);
	bool CIOCPServer::InitializeIOCP(void);                      //IOCP��ɶ˿ڵĴ���
	void CIOCPServer::OnAccept();								 //���������źű����ǵļ����̲߳���
	BOOL CIOCPServer::AssociateSocketWithCompletionPort(SOCKET socket, HANDLE hCompletionPort, DWORD dwCompletionKey);   //�������ǽ��û������±����ĵ��������Key
	static CRITICAL_SECTION	m_cs;
	ClientContext*  CIOCPServer::AllocateContext();
	void CIOCPServer::MoveToFreePool(ClientContext *pContext);
	void CIOCPServer::RemoveStaleClient(ClientContext* pContext, BOOL bGraceful);     //��ɶ˿�ʧ�� �Ƴ��ÿͻ����±�����

	void CIOCPServer::PostRecv(ClientContext* pContext);

	void CIOCPServer::Send(ClientContext* pContext, LPBYTE lpData, UINT nSize);
protected:
	WSAEVENT				m_hEvent;
	SOCKET					m_sockListen;   
	HANDLE					m_hThread;                  //�����̵߳��߳̾��
	HANDLE					m_hKillEvent;               //Ҫ������߳��˳����¼�����     �ڹ��캯����Ҫ��ס����  
	bool                    m_bInit;
	HANDLE					m_hCompletionPort;          //��ɶ˿�

	//�̳߳�
	LONG					m_nThreadPoolMin;
	LONG					m_nThreadPoolMax;
	LONG					m_nCPULowThreadshold;
	LONG					m_nCPUHighThreadshold;

	CCpuUsage				m_cpu;
	LONG				    m_nWorkerCnt;               //�����Լ�����ɶ˿�Ͷ�ݵ��̸߳�����Ԥ��ֵ


	LONG					m_nKeepLiveTime;            //�������ʹ��

	//�ڴ��
	ContextList				m_listContexts;             //���������ͷָ��      
	ContextList				m_listFreePool;
	
	
	UINT					m_nSendKbps;                // ���ͼ�ʱ�ٶ�
	UINT					m_nRecvKbps;                // ���ܼ�ʱ�ٶ�
	BYTE				    m_bPacketFlag[5];           // ���ݰ���ͷ�� Ҳ����Shine
	
	


	BEGIN_IO_MSG_MAP()
		IO_MESSAGE_HANDLER(IOInitialize, OnClientInitializing)
		IO_MESSAGE_HANDLER(IORead, OnClientReading)                   //���ض��������Ʒ������ݶ���������Ӧ����
		IO_MESSAGE_HANDLER(IOWrite, OnClientWriting)                  //���ض��򱻿ض˷�������������õ���Ӧ
	END_IO_MSG_MAP()

	bool OnClientInitializing	(ClientContext* pContext, DWORD dwSize = 0);
	bool OnClientReading		(ClientContext* pContext, DWORD dwSize = 0);
	bool OnClientWriting		(ClientContext* pContext, DWORD dwSize = 0);



	

};




//���������ﻹ������һ���������� ���� ���ߵ����ݵ�ͬ��

class CLock
{
public:
	CLock(CRITICAL_SECTION& cs, const CString& strFunc)
	{
		m_strFunc = strFunc;
		m_pcs = &cs;
		Lock();
	}
	~CLock()
	{
		Unlock();

	}

	void Unlock()
	{
		LeaveCriticalSection(m_pcs);
		TRACE(_T("LC %d %s\n") , GetCurrentThreadId() , m_strFunc);
	}

	void Lock()
	{
		TRACE(_T("EC %d %s\n") , GetCurrentThreadId(), m_strFunc);
		EnterCriticalSection(m_pcs);
	}


protected:
	CRITICAL_SECTION*	m_pcs;
	CString				m_strFunc;

};






class OVERLAPPEDPLUS 
{
public:
	OVERLAPPED			m_ol;
	IOType				m_ioType;

	OVERLAPPEDPLUS(IOType ioType) {
		ZeroMemory(this, sizeof(OVERLAPPEDPLUS));
		m_ioType = ioType;
	}
};




