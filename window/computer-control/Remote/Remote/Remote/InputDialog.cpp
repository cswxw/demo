// InputDialog.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "Remote.h"
#include "InputDialog.h"
#include "afxdialogex.h"


// CInputDialog �Ի���

IMPLEMENT_DYNAMIC(CInputDialog, CDialog)

CInputDialog::CInputDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CInputDialog::IDD, pParent)
	, m_str(_T(""))
{

}

CInputDialog::~CInputDialog()
{
}

void CInputDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_INPUT, m_str);
}


BEGIN_MESSAGE_MAP(CInputDialog, CDialog)
	ON_BN_CLICKED(IDOK, &CInputDialog::OnBnClickedOk)
END_MESSAGE_MAP()


// CInputDialog ��Ϣ�������


void CInputDialog::OnBnClickedOk()
{
	UpdateData(TRUE);
	if (m_str.IsEmpty()) {
		MessageBeep(0);
		return;   //���رնԻ���
	}
	CDialog::OnOK();
}
