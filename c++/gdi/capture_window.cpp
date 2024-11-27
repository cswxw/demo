
// GDI_TestDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "GDI_Test.h"
#include "GDI_TestDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CGDI_TestDlg 对话框




CGDI_TestDlg::CGDI_TestDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CGDI_TestDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CGDI_TestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CGDI_TestDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON1, &CGDI_TestDlg::OnBnClickedButton1)
END_MESSAGE_MAP()


// CGDI_TestDlg 消息处理程序

BOOL CGDI_TestDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
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

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	::SetTimer(this->GetSafeHwnd(),1,1000,NULL); 


	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CGDI_TestDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CGDI_TestDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CGDI_TestDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}
#include <math.h>




void CGDI_TestDlg::OnBnClickedButton1()
{
	// TODO: 在此添加控件通知处理程序代码
 // TODO: Add extra validation here  
    HDC hDesktopDC = ::GetDC(NULL);  
    HDC hTmpDC = CreateCompatibleDC(hDesktopDC);  
    HBITMAP hBmp = CreateCompatibleBitmap(hDesktopDC, 351, 250);    //351x250, 示例数据  
    SelectObject(hTmpDC, hBmp);  
    BitBlt(hTmpDC, 0, 0, 351, 250, hDesktopDC, 0, 0, SRCCOPY);  
    DeleteObject(hTmpDC);  
  
    BITMAP bm;  
    PBITMAPINFO bmpInf;  
    if(GetObject(hBmp,sizeof(bm),&bm)==0)  
    {  
        ::ReleaseDC(NULL,hDesktopDC);  
        return ;  
    }  
  
    int nPaletteSize=0;  
    if(bm.bmBitsPixel<16)  
        nPaletteSize=(int)pow(2.0f,(int)bm.bmBitsPixel);  
  
    bmpInf=(PBITMAPINFO)LocalAlloc(LPTR,sizeof(BITMAPINFOHEADER)+  
        sizeof(RGBQUAD)*nPaletteSize+(bm.bmWidth+7)/8*bm.bmHeight*bm.bmBitsPixel);  
  
    BYTE* buf=((BYTE*)bmpInf) +   
        sizeof(BITMAPINFOHEADER)+  
        sizeof(RGBQUAD)*nPaletteSize;  
  
    //-----------------------------------------------  
    bmpInf->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);  
    bmpInf->bmiHeader.biWidth = bm.bmWidth;  
    bmpInf->bmiHeader.biHeight = bm.bmHeight;  
    bmpInf->bmiHeader.biPlanes = bm.bmPlanes;  
    bmpInf->bmiHeader.biBitCount = bm.bmBitsPixel;  
    bmpInf->bmiHeader.biCompression = BI_RGB;  
    bmpInf->bmiHeader.biSizeImage = (bm.bmWidth+7)/8*bm.bmHeight*bm.bmBitsPixel;  
    //-----------------------------------------------  
  
    if(!::GetDIBits(hDesktopDC,hBmp,0,(UINT)bm.bmHeight,buf,bmpInf,DIB_RGB_COLORS))  
    {  
        ::ReleaseDC(NULL,hDesktopDC);  
        LocalFree(bmpInf);  
        return ;  
    }  
  
    ::ReleaseDC(NULL,hDesktopDC);  
  
    CString sMsg;  
    sMsg.Format("BitsPixel:%d,width:%d,height:%d",  
        bm.bmBitsPixel,bm.bmWidth,bm.bmHeight);  
    //AfxMessageBox(sMsg);  
  
    //CClientDC dc(this);  
	HDC dc = ::GetWindowDC(this->GetSafeHwnd());
  
    int nOffset;  
    BYTE r,g,b;  
    int nWidth = bm.bmWidth*bm.bmBitsPixel/8;  
    nWidth = ((nWidth+3)/4)*4; //4字节对齐  
      
    if(bmpInf->bmiHeader.biBitCount == 8)  
    {         
        for(int i=0; i<bm.bmHeight; i++)  
        {  
            for(int j=0; j<bm.bmWidth; j++)  
            {  
                RGBQUAD rgbQ;  
                rgbQ = bmpInf->bmiColors[buf[i*nWidth+j]];  
                //dc.SetPixel(j,bm.bmHeight-i,RGB(rgbQ.rgbRed,rgbQ.rgbGreen,rgbQ.rgbBlue)); //测试显示  
				::SetPixel(dc,j,bm.bmHeight-i,RGB(rgbQ.rgbRed,rgbQ.rgbGreen,rgbQ.rgbBlue)); //测试显示  
            }  
        }  
    }  
    else if(bmpInf->bmiHeader.biBitCount == 16)  
    {  
        for(int i=0; i<bm.bmHeight; i++)  
        {  
            nOffset = i*nWidth;  
            for(int j=0; j<bm.bmWidth; j++)  
            {  
                b = buf[nOffset+j*2]&0x1F;  
                g = buf[nOffset+j*2]>>5;  
                g |= (buf[nOffset+j*2+1]&0x03)<<3;  
                r = (buf[nOffset+j*2+1]>>2)&0x1F;  
  
                r *= 8;  
                b *= 8;  
                g *= 8;  
                  
                //dc.SetPixel(j, bm.bmHeight-i, RGB(r,g,b)); //测试显示  
				::SetPixel(dc,j, bm.bmHeight-i, RGB(r,g,b)); //测试显示  
            }  
        }  
    }  
    else if(bmpInf->bmiHeader.biBitCount == 24)  
    {  
        for(int i=0; i<bm.bmHeight; i++)  
        {  
            nOffset = i*nWidth;  
            for(int j=0; j<bm.bmWidth; j++)  
            {  
                b = buf[nOffset+j*3];  
                g = buf[nOffset+j*3+1];  
                r = buf[nOffset+j*3+2];  
                  
                //dc.SetPixel(j, bm.bmHeight-i, RGB(r,g,b)); //测试显示  
				SetPixel(dc,j, bm.bmHeight-i, RGB(r,g,b)); //测试显示  
            }  
        }  
    }  
    else if(bmpInf->bmiHeader.biBitCount == 32)  
    {  
#if 0
		//从下往上 从左往右边
        for(int i=0; i<bm.bmHeight; i++)  
        {  
            nOffset = i*nWidth;  
            for(int j=0; j<bm.bmWidth; j++)  
            {  
                b = buf[nOffset+j*4];  
                g = buf[nOffset+j*4+1];  
                r = buf[nOffset+j*4+2];  
                  
                //dc.SetPixel(j, bm.bmHeight-i, RGB(r,g,b)); //测试显示  
				SetPixel(dc,j, bm.bmHeight-i, RGB(r,g,b)); //测试显示  
            }  
        }  
#else
		
        for(int i=bm.bmHeight; i>0; i--)  
        {    
			nOffset = i * nWidth;  
            for(int j=0; j<bm.bmWidth; j++)  
            {
				

                b = (BYTE)(buf[nOffset+j*4] * 0.11);  
                g = (BYTE)(buf[nOffset+j*4+1] * 0.59);  
                r = (BYTE)(buf[nOffset+j*4+2] * 0.3);  
				int temp = RGB(r,g,b);
				if(temp>128){
					//r=g=b=255;
				}
				else
					r=g=b=0;
				
                  
                //dc.SetPixel(j, bm.bmHeight-i, RGB(r,g,b)); //测试显示  
				SetPixel(dc,j, bm.bmHeight-i, RGB(r,g,b)); //测试显示
            }  
        }  
#endif
    }  
  
    DeleteObject(hBmp);  
    LocalFree(bmpInf);  
}

LRESULT CGDI_TestDlg::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	// TODO: 在此添加专用代码和/或调用基类
	
	switch(message){
		case WM_TIMER:
		{
			switch(wParam){
				case 1:{
					::KillTimer(this->GetSafeHwnd(),1);
					OnBnClickedButton1();
					::SetTimer(this->GetSafeHwnd(),1,500,NULL); 
				}
			}
			return 0;
		}
	}
	return CDialog::WindowProc(message, wParam, lParam);
}



