#pragma once

#include "IOCPServer.h"
#include "afxcmn.h"

// CServerDlg �Ի���

class CServerDlg : public CDialog
{
	DECLARE_DYNAMIC(CServerDlg)

public:
	CServerDlg(CWnd* pParent = NULL, CIOCPServer* pIOCPServer = NULL, ClientContext *pContext = NULL);   // ��׼���캯��
	virtual ~CServerDlg();


	int CServerDlg::ShowServiceList(void);

// �Ի�������
	enum { IDD = IDD_SERVERDLG };

	ClientContext* m_pContext;
	CIOCPServer* m_iocpServer;

	void OnReceiveComplete(void);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	CListCtrl m_list;
	virtual BOOL OnInitDialog();
};
