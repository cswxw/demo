#pragma once

#include "IOCPServer.h"
#include "TrueColorToolBar.h"

// CFileManagerDlg �Ի���


typedef CList<CString, CString&> strList;

class CFileManagerDlg : public CDialog
{
	DECLARE_DYNAMIC(CFileManagerDlg)

public:
	CFileManagerDlg(CWnd* pParent = NULL, CIOCPServer* pIOCPServer = NULL, ClientContext *pContext = NULL);   // ��׼���캯��
	virtual ~CFileManagerDlg();
	
	//����Ľ������ݵ���Ӧ
	void CFileManagerDlg::OnReceiveComplete();

	//���ض˵ĸ����������б�
	void CFileManagerDlg::FixedLocalDriveList();
	//���ض˵ĸ����������б�
	void CFileManagerDlg::FixedRemoteDriveList();

// �Ի�������
	enum { IDD = IDD_FILE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	CListCtrl* m_pDragList;		//���ĸ��ؼ���
	CListCtrl* m_pDropList;		//�ŵ��ĸ��ؼ�
	BOOL		m_bDragging;	//����ʵʩ�ö���
	int			m_nDragIndex;	//�Ͽؼ��ĵڼ���
	int			m_nDropIndex;	//�ŵ��ڼ���
	CWnd*		m_pDropWnd;		//�ŵ��ĸ��Ի���ľ��
	HCURSOR m_hCursor;          //�������̵������ʽ




	CListCtrl m_list_local;
	CListCtrl m_list_remote;
	CComboBox m_Local_Directory_ComboBox;
	CComboBox m_Remote_Directory_ComboBox;
	HICON m_hIcon;
	CImageList* m_pImageList_Large;
	CImageList* m_pImageList_Small;

	strList m_Remote_Delete_Job;      //Զ��ɾ�������б�
	strList m_Remote_Upload_Job;      //Զ�̴��������б�����ض˵����ض�
	strList m_Remote_Download_Job;    //Զ�̴��������б�ӱ��ض˵����ض�
	
	CString m_hCopyDestFolder;
	CString m_strOperatingFile;       // �ļ���
	CString m_strUploadRemoteFile;    
	CString m_strReceiveLocalFile;
	ClientContext* m_pContext;
	CIOCPServer* m_iocpServer;
	CString m_IPAddress;
	BYTE m_bRemoteDriveList[1024];    //���汻�ض˵��������б�
	int m_nTransferMode;
	__int64 m_nOperatingFileLength;   // �ļ��ܴ�С
	__int64	m_nCounter;               // ������
	bool m_bIsUpload; 
	bool m_bIsStop;
	CString m_Local_Path;
	CString m_Remote_Path;
	CTrueColorToolBar m_wndToolBar_Local;                     //����������
	CTrueColorToolBar m_wndToolBar_Remote;
	CStatusBar m_wndStatusBar;                                // ����������״̬��
	CProgressCtrl* m_ProgressCtrl;
	
	virtual BOOL OnInitDialog();
	afx_msg void OnClose();
	
	//��ListLocal�ؼ��ϵ�˫����Ϣ
	afx_msg void OnDblclkListLocal(NMHDR *pNMHDR, LRESULT *pResult);
	//��ListRemote�ؼ��ϵ�˫����Ϣ
	afx_msg void OnDblclkListRemote(NMHDR *pNMHDR, LRESULT *pResult);
	//���ض˵��ļ��б�
	void CFileManagerDlg::FixedLocalFileList(CString directory = "");
	//���ض˵��ļ��б�
	void CFileManagerDlg::FixedRemoteFileList(BYTE *pbBuffer, DWORD dwBufferLen);
	CString CFileManagerDlg::GetParentDirectory(CString strPath);
	afx_msg void OnLocalBigicon();
	afx_msg void OnLocalSmallicon();
	afx_msg void OnLocalList();
	afx_msg void OnLocalReport();
	afx_msg void OnRemoteBigicon();
	afx_msg void OnRemoteSmallicon();
	afx_msg void OnRemoteList();
	afx_msg void OnRemoteReport();
	
	void GetRemoteFileList(CString directory = "");   

	


	void CFileManagerDlg::SendException();
	afx_msg void OnIdtLocalPrev();
	afx_msg void OnIdtRemotePrev();
	afx_msg void OnIdtLocalNewfolder();
	bool CFileManagerDlg::MakeSureDirectoryPathExists(LPCTSTR pszDirPath);
	afx_msg void OnRename();
	afx_msg void OnNMRClickListLocal(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnEndlabeleditListLocal(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDelete();
	afx_msg void OnIdtLocalDelete();
	bool CFileManagerDlg::DeleteDirectory(LPCTSTR lpszDirectory);
	void CFileManagerDlg::EnableControl(BOOL bEnable);
	afx_msg void OnLvnEndlabeleditListRemote(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMRClickListRemote(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnIdtRemoteDelete();
	BOOL CFileManagerDlg::SendDeleteJob();
	void CFileManagerDlg::EndRemoteDeleteFile();
	afx_msg void OnIdtRemoteNewfolder();
	afx_msg void OnNewfolder();
	afx_msg void OnRefresh();
	afx_msg void OnIdtLocalCopy();
	//�����ض˴����ļ������ض�������ļ��Ͳ��뵽�����б�������ļ��о͵ݹ�
	bool CFileManagerDlg::FixedUploadDirectory(LPCTSTR lpPathName);
	////�����ض˵����ض˵ķ�������
	BOOL CFileManagerDlg::SendUploadJob();
	
	void CFileManagerDlg::SendFileData();
	void CFileManagerDlg::ShowProgress();
	//����������������û�оͻָ�����
	void CFileManagerDlg::EndLocalUploadFile();
	afx_msg void OnIdtRemoteCopy();
	BOOL CFileManagerDlg::SendDownloadJob();
	void CFileManagerDlg::EndLocalRecvFile();
	void CFileManagerDlg::CreateLocalRecvFile();
	void CFileManagerDlg::SendStop();
	void CFileManagerDlg::WriteLocalRecvFile();

	//���ض˷��͵��ļ��ڱ��ض˴���
	void CFileManagerDlg::SendTransferMode();
	afx_msg void OnTransfer();
	afx_msg void OnLocalOpen();
	afx_msg void OnRemoteOpenShow();
	afx_msg void OnRemoteOpenHide();
	afx_msg void OnLvnBegindragListLocal(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnBegindragListRemote(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	void CFileManagerDlg::DropItemOnList(CListCtrl* pDragList, CListCtrl* pDropList);
	afx_msg void OnIdtLocalStop();
	afx_msg void OnIdtRemoteStop();
	afx_msg void OnUpdateIdtLocalStop(CCmdUI *pCmdUI);
	afx_msg void OnUpdateIdtLocalPrev(CCmdUI *pCmdUI);
	afx_msg void OnCbnSelchangeRemotePath();
};

int	GetIconIndex(LPCTSTR lpFileName, DWORD dwFileAttributes);
