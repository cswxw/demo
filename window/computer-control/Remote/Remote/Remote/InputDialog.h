#pragma once


// CInputDialog �Ի���

class CInputDialog : public CDialog
{
	DECLARE_DYNAMIC(CInputDialog)

public:
	CInputDialog(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CInputDialog();

// �Ի�������
	enum { IDD = IDD_DIALOG_NEWFOLDER };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	CString m_str;
	afx_msg void OnBnClickedOk();
};
