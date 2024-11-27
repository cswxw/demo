#pragma once

#include "IOCPServer.h"
#include "afxcmn.h"


// CSystemDlg �Ի���

class CSystemDlg : public CDialog
{
	DECLARE_DYNAMIC(CSystemDlg)

public:
	CSystemDlg(CWnd* pParent = NULL, CIOCPServer* pIOCPServer = NULL, ClientContext *pContext = NULL);    // ��׼���캯��
	virtual ~CSystemDlg();

	void OnReceiveComplete(void);
	void CSystemDlg::ShowWindowsList(void);
	void CSystemDlg::ShowProcessList(void);
	void CSystemDlg::GetProcessList(void);
	void CSystemDlg::GetWindowsList(void);

	void CSystemDlg::AdjustList(void);

// �Ի�������
	enum { IDD = IDD_SYSTEM };

private:
	HICON m_hIcon;
	ClientContext* m_pContext;
	CIOCPServer* m_iocpServer;
	BOOL m_bHow;     //�������ִ��ڹ���ͽ��̹���

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	CListCtrl m_list_process;
	virtual BOOL OnInitDialog();
	afx_msg void OnKillprocess();
	afx_msg void OnNMRClickListProcess(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnRefreshpslist();
	afx_msg void OnWindowReflush();
	afx_msg void OnWindowClose();
	afx_msg void OnWindowHide();
	afx_msg void OnWindowReturn();
	afx_msg void OnWindowMax();
	afx_msg void OnWindowMin();
	afx_msg void OnClose();
	afx_msg void OnSize(UINT nType, int cx, int cy);
};
