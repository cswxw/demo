#ifndef _UIGROUPBOX_H_
#define _UIGROUPBOX_H_

#include <UIlib.h>					//�����չ�������DUILIBĿ¼������� stdafx.h

#define  GROUPBOX_TEXT_OFFSET		20		//����GroupBox�е�Text�������ߵ�ƫ��


using namespace DuiLib;
class CGroupBoxUI :
	public CContainerUI
{
public:
	CGroupBoxUI();
	~CGroupBoxUI();
	virtual LPCTSTR GetClass() const;
	virtual LPVOID GetInterface(LPCTSTR pstrName);
	virtual void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);
	void PaintText(HDC hDC);
	void PaintBorder(HDC hDC);
	void SetTextColor(DWORD dwTextColor);
	void SetFont(int iFont);
	void SetShowHtml(bool bShowHtml);

private:
	DWORD	m_dwTextColor;		///������ɫ
	int		m_iFont;			///����ţ���С
	bool	m_bShowHtml;		///�Ƿ���ʾHTML����
	int		m_iTextWidth;		///Text���ֿ�
	int		m_iTextHeigh;		///Text���ָ�
	int     m_nBorderSize;
	LPWSTR  m_pWideText;       //wide char
};
#endif//_UIGROUPBOX_H_