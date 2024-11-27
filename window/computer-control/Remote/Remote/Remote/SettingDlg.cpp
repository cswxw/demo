// SettingDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "Remote.h"
#include "SettingDlg.h"
#include "afxdialogex.h"


// CSettingDlg �Ի���

IMPLEMENT_DYNAMIC(CSettingDlg, CDialogEx)

CSettingDlg::CSettingDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CSettingDlg::IDD, pParent)
	, m_nListernProt(0)
	, m_nMax_Connect(0)
{

}

CSettingDlg::~CSettingDlg()
{
}

void CSettingDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_PORT, m_nListernProt);
	DDX_Text(pDX, IDC_EDIT_MAX, m_nMax_Connect);
}


BEGIN_MESSAGE_MAP(CSettingDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &CSettingDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CSettingDlg ��Ϣ�������


void CSettingDlg::OnBnClickedOk()
{
	UpdateData(TRUE);
	((CRemoteApp *)AfxGetApp())->m_IniFile.SetInt("Settings", "ListenPort", m_nListernProt);      
	//��ini�ļ���д��ֵ
	((CRemoteApp *)AfxGetApp())->m_IniFile.SetInt("Settings", "MaxConnection", m_nMax_Connect);
	MessageBox("���óɹ����������������Ч��");


	CDialogEx::OnOK();
}
