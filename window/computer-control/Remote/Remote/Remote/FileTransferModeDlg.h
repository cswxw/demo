#pragma once


// CFileTransferModeDlg �Ի���

class CFileTransferModeDlg : public CDialog
{
	DECLARE_DYNAMIC(CFileTransferModeDlg)

public:
	CFileTransferModeDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CFileTransferModeDlg();

	CString  m_strFileName;

// �Ի�������
	enum { IDD = IDD_TRANSFERMODE_DLG};

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	afx_msg	void OnEndDialog(UINT id);
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOverwrite();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
};
