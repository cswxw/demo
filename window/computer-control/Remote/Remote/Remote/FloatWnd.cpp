// FloatWnd.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "Remote.h"
#include "FloatWnd.h"
#include "afxdialogex.h"


// CFloatWnd �Ի���

IMPLEMENT_DYNAMIC(CFloatWnd, CDialog)

CFloatWnd::CFloatWnd(CWnd* pParent /*=NULL*/)
	: CDialog(CFloatWnd::IDD, pParent)
{

}

CFloatWnd::~CFloatWnd()
{
}

void CFloatWnd::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LOGO, m_Logo);
}


BEGIN_MESSAGE_MAP(CFloatWnd, CDialog)
END_MESSAGE_MAP()


// CFloatWnd ��Ϣ�������


BOOL CFloatWnd::OnInitDialog()
{
	CDialog::OnInitDialog();

	CBitmap m_Bitmap;
	HBITMAP hBitmap = m_Logo.GetBitmap();
	ASSERT(hBitmap);

	m_Bitmap.Attach(hBitmap);
	BITMAP bmp;
	m_Bitmap.GetBitmap(&bmp);

	int nX = bmp.bmWidth;
	int nY = bmp.bmHeight;

	
	m_Logo.MoveWindow(0,0,nX,nY);
	CenterWindow();

	m_Bitmap.Detach();

	//����WS_EX_LAYERED��չ����
	SetWindowLong(m_hWnd,GWL_EXSTYLE,GetWindowLong(this->GetSafeHwnd(),GWL_EXSTYLE)^0x80000);


	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}



void CFloatWnd::OnUpdateTransparent(int iTransparent)
{
    HINSTANCE hInst = LoadLibrary("User32.DLL");
	if(hInst)
	{
		typedef BOOL (WINAPI *SLWA)(HWND,COLORREF,BYTE,DWORD);
		SLWA pFun = NULL;
		//ȡ��SetLayeredWindowAttributes����ָ�� 
		pFun = (SLWA)GetProcAddress(hInst,"SetLayeredWindowAttributes");
		if(pFun)
		{
			pFun(m_hWnd,255,255,0);
		}
		FreeLibrary(hInst); 
	}
}