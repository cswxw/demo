#pragma once
#include "afxwin.h"


// CFloatWnd �Ի���

class CFloatWnd : public CDialog
{
	DECLARE_DYNAMIC(CFloatWnd)

public:
	CFloatWnd(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CFloatWnd();

// �Ի�������
	enum { IDD = IDD_DIALOG_FLOAT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	void CFloatWnd::OnUpdateTransparent(int iTransparent);
	CStatic m_Logo;
};
