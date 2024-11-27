
#include "UIGroupBox.h"


//#include <UIlib.h>

CGroupBoxUI::CGroupBoxUI() :m_iFont(-1), m_bShowHtml(false), m_nBorderSize(1), m_pWideText(0)
{

}


CGroupBoxUI::~CGroupBoxUI()
{
#ifdef _UNICODE
	if (m_pWideText && m_pWideText != m_sText.GetData()) delete[] m_pWideText;
#else
	if (m_pWideText) delete[] m_pWideText;
#endif
}

///	@return LPCTSTR ���ؿؼ�����
///	@note	���������ؿؼ��࣬��ʽΪLPCTSTR
LPCTSTR CGroupBoxUI::GetClass() const
{
	return TEXT("GroupBoxUI");
}


/// @return LPVOID����
///	@note ��ȡ�ӿ�
LPVOID CGroupBoxUI::GetInterface(LPCTSTR pstrName)
{
	if (_tcscmp(pstrName, TEXT("GroupBox")) == 0)
	{
		return static_cast<CGroupBoxUI*>(this);
	}

	return CContainerUI::GetInterface(pstrName);
}


///	���ÿؼ�����
/// @param pstrname		�����õ��������ƣ�LPCTSTR����
/// @param pstrValue	�����õ�����ֵ��LPCTSTR����
/// @see				CControlUI::SetAttribute()
/// @note				���ػ��࣬���Ӳ��ֻ���û�е�����
void CGroupBoxUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
{
	if (_tcscmp(pstrName, _T("font")) == 0) SetFont(_ttoi(pstrValue));
	else if (_tcscmp(pstrName, _T("textcolor")) == 0)
	{
		if (*pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
		LPTSTR pstr = NULL;
		DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
		SetTextColor(clrColor);
	}
	else if (_tcscmp(pstrName, _T("showhtml")) == 0)
		SetShowHtml(_tcscmp(pstrValue, _T("true")) == 0);

	else CControlUI::SetAttribute(pstrName, pstrValue);
}

/// ���ÿؼ�������ɫ
/// @param	dwTextColor		�����õ�������ɫ
/// @note	����������ɫ��������ˢ��
void CGroupBoxUI::SetTextColor(DWORD dwTextColor)
{
	m_dwTextColor = dwTextColor;
	Invalidate();
}


/// ���ÿؼ�����
/// @param	iFont		�����õ������
/// @note	�������壬������ˢ��
void CGroupBoxUI::SetFont(int iFont)
{
	m_iFont = iFont;
	Invalidate();
}


void CGroupBoxUI::SetShowHtml(bool bShowHtml)
{
	if (m_bShowHtml == bShowHtml) return;

	m_bShowHtml = bShowHtml;
	Invalidate();
}


/// �ؼ�����
void CGroupBoxUI::PaintText(HDC hDC)
{
#ifdef _UNICODE
	m_pWideText = (LPWSTR)m_sText.GetData();
#else 
	const char * pstrText = m_sText.GetData();
	int iLen = _tcslen(pstrText);
	if (m_pWideText) delete[] m_pWideText;
	m_pWideText = new WCHAR[iLen + 1];
	::ZeroMemory(m_pWideText, (iLen + 1) * sizeof(WCHAR));
	::MultiByteToWideChar(CP_ACP, 0, pstrText, -1, (LPWSTR)m_pWideText, iLen);
#endif

	//���û������������ɫ������Ĭ������
	if (m_dwTextColor == 0)
	{
		m_dwTextColor = m_pManager->GetDefaultFontColor();
	}


	RECT rc;		//�������λ��
	rc = GetPos();

	rc.left = rc.left + m_cxyBorderRound.cx + GROUPBOX_TEXT_OFFSET;	//����ط�������Ӳ����ķ�ʽ����֪����ô��������Ӧ�÷ŵ�λ��

	HFONT hOldFont = (HFONT)::SelectObject(hDC, m_pManager->GetFont(m_iFont));
	Gdiplus::Graphics graphics(hDC);
	Gdiplus::Font font(hDC);
	graphics.SetTextRenderingHint(Gdiplus::TextRenderingHintSystemDefault);
	graphics.SetSmoothingMode(Gdiplus::SmoothingModeHighQuality);
	graphics.SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBicubic);
	Gdiplus::RectF rectF((Gdiplus::REAL)rc.left, (Gdiplus::REAL)rc.top, (Gdiplus::REAL)(rc.right - rc.left), (Gdiplus::REAL)(rc.bottom - rc.top));
	Gdiplus::SolidBrush brush(Gdiplus::Color(254, GetBValue(m_dwTextColor), GetGValue(m_dwTextColor), GetRValue(m_dwTextColor)));

	Gdiplus::StringFormat stringFormat = Gdiplus::StringFormat::GenericTypographic();
	Gdiplus::RectF bounds;
	graphics.MeasureString(m_pWideText, -1, &font, rectF, &stringFormat, &bounds);

	// MeasureString���ڼ����������һ����
	rc.bottom = rc.top + (long)bounds.Height + 1;		//�������Ǵ�UIRender.cpp��DrawText()�п������ģ���֪���������
	rc.right = rc.left + (long)bounds.Width + 1;

	m_iTextWidth = (int)bounds.Width;
	m_iTextHeigh = (int)bounds.Height;

	graphics.DrawString(m_pWideText, -1, &font, rectF, &stringFormat, &brush);

	::SelectObject(hDC, hOldFont);

}


void CGroupBoxUI::PaintBorder(HDC hDC)
{
	RECT rc = GetPos();		//�����ʱ��λ��

	rc.top += (m_iTextHeigh * 1) / 2;		//��������Ƶ�Text�����²�

	int nSize = m_nBorderSize;
	//DWORD dwPenColor = m_dwBorderColor;
	Gdiplus::Graphics graphics(hDC);

	//const Gdiplus::Pen pen(Gdiplus::Color::Red, 1.0f);

	DWORD dwPenColor = GetAdjustColor(m_dwBorderColor);
	ASSERT(::GetObjectType(hDC) == OBJ_DC || ::GetObjectType(hDC) == OBJ_MEMDC);
	HPEN hPen = ::CreatePen(PS_SOLID | PS_INSIDEFRAME, nSize, RGB(GetBValue(dwPenColor), GetGValue(dwPenColor), GetRValue(dwPenColor)));
	HPEN hOldPen = (HPEN)::SelectObject(hDC, hPen);
	::SelectObject(hDC, ::GetStockObject(HOLLOW_BRUSH));

	//��λ�ĸ�Բ�ǵ�λ��
	RECT rcTopLeftCorner = { rc.left, rc.top, rc.left + 2 * m_cxyBorderRound.cx, rc.top + 2 * m_cxyBorderRound.cy };
	RECT rcTopRightCorner = { rc.right - 2 * m_cxyBorderRound.cx, rc.top, rc.right, rc.top + 2 * m_cxyBorderRound.cy };
	RECT rcBottomLeftCorner = { rc.left, rc.bottom - 2 * m_cxyBorderRound.cy, rc.left + 2 * m_cxyBorderRound.cx, rc.bottom };
	RECT rcBottomRightCorner = { rc.right - 2 * m_cxyBorderRound.cx, rc.bottom - 2 * m_cxyBorderRound.cy, rc.right, rc.bottom };

	//���ĸ�Բ��
	const Gdiplus::Pen pen(Gdiplus::Color(GetBValue(dwPenColor), GetGValue(dwPenColor), GetRValue(dwPenColor)), (float)m_nBorderSize);
	graphics.DrawArc(&pen, rcTopLeftCorner.left, rcTopLeftCorner.top, rcTopLeftCorner.right - rcTopLeftCorner.left, rcTopLeftCorner.bottom - rcTopLeftCorner.top, 180.0f, 90.0f);//����
	graphics.DrawArc(&pen, rcTopRightCorner.left, rcTopRightCorner.top, rcTopRightCorner.right - rcTopRightCorner.left, rcTopRightCorner.bottom - rcTopRightCorner.top, 270.0f, 90.0f);//����
	graphics.DrawArc(&pen, rcBottomLeftCorner.left, rcBottomLeftCorner.top, rcBottomLeftCorner.right - rcBottomLeftCorner.left, rcBottomLeftCorner.bottom - rcBottomLeftCorner.top, 90, 90);//����
	//���½�ƫ��һ�����ء�����
	graphics.DrawArc(&pen, rcBottomRightCorner.left-1, rcBottomRightCorner.top, rcBottomRightCorner.right - rcBottomRightCorner.left, rcBottomRightCorner.bottom - rcBottomRightCorner.top, 0, 90);//����


	//����----GDI
	MoveToEx(hDC, rc.left, rc.top + m_cxyBorderRound.cy, NULL);			//�����
	LineTo(hDC, rc.left, rc.bottom - m_cxyBorderRound.cy);

	MoveToEx(hDC, rc.left + m_cxyBorderRound.cx, rc.top, NULL);			//�ϵ�һ����
	LineTo(hDC, rc.left + m_cxyBorderRound.cx + GROUPBOX_TEXT_OFFSET - 5, rc.top);		//-5 ��Ϊ�˸�Text������߼��

	MoveToEx(hDC, rc.left + m_cxyBorderRound.cx + GROUPBOX_TEXT_OFFSET + m_iTextWidth + 5, rc.top, NULL);		//�ϵڶ����ߣ�+5��Ϊ�˸�Text�����ұ߼��
	LineTo(hDC, rc.right - m_cxyBorderRound.cx, rc.top);

	MoveToEx(hDC, rc.right, rc.top + m_cxyBorderRound.cy, NULL);		//�ұ���
	LineTo(hDC, rc.right, rc.bottom - m_cxyBorderRound.cy);

	MoveToEx(hDC, rc.left + m_cxyBorderRound.cx, rc.bottom, NULL);
	LineTo(hDC, rc.right - m_cxyBorderRound.cx, rc.bottom);


	//����GDI+    GDI+���� ��ƫ�ƣ����޸�width��height
	//graphics.DrawLine(&pen, rc.left, rc.top + m_cxyBorderRound.cy, rc.left, rc.bottom - m_cxyBorderRound.cy);
	//graphics.DrawLine(&pen, rc.left + m_cxyBorderRound.cx, rc.top, rc.left + m_cxyBorderRound.cx + GROUPBOX_TEXT_OFFSET - 5, rc.top);
	//graphics.DrawLine(&pen, rc.left + m_cxyBorderRound.cx + GROUPBOX_TEXT_OFFSET + m_iTextWidth + 5, rc.top, rc.right - m_cxyBorderRound.cx, rc.top);
	//graphics.DrawLine(&pen, rc.right, rc.top + m_cxyBorderRound.cy, rc.right, rc.bottom - m_cxyBorderRound.cy);		//�ұ�����ʾ������
	//graphics.DrawLine(&pen, rc.left + m_cxyBorderRound.cx, rc.bottom, rc.right - m_cxyBorderRound.cx, rc.bottom);	//�ױ���Ҳ��ʾ������



	//��ʼ��Բ�Ǿ��α߿�
	//RECT rcTopLeftCorner = { rc.left, rc.top, rc.left + 20, rc.top + 20 };	//���Ͻ�Բ��
	//RECT rcTopRightCorner = { rc.right - 2 * m_cxyBorderRound.cx, rc.top, rc.right, rc.top + 2 * m_cxyBorderRound.cy };
	//RECT rcBottomLeftCorner = { rc.left, rc.bottom - 2 * m_cxyBorderRound.cy, rc.left + 2 * m_cxyBorderRound.cx, rc.bottom };
	//RECT rcBottomRightCorner = { rc.right - 2 * m_cxyBorderRound.cx, rc.bottom - 2 * m_cxyBorderRound.cy, rc.right, rc.bottom };

	//�����Ͻ�Բ��

	/*MoveToEx(hDC, rc.left, rc.top, NULL);
	LineTo(hDC, rc.left, rc.bottom);
	MoveToEx(hDC, rc.left, rc.top, NULL);
	LineTo(hDC, rc.left, rc.top);
	MoveToEx(hDC, rc.left, rc.top, NULL);
	LineTo(hDC, rc.right, rc.top);
	MoveToEx(hDC, rc.right, rc.top, NULL);
	LineTo(hDC, rc.right, rc.bottom);
	MoveToEx(hDC, rc.left, rc.bottom, NULL);
	LineTo(hDC, rc.right, rc.bottom);*/
	::SelectObject(hDC, hOldPen);
	::DeleteObject(hPen);


}