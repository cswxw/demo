
// Remote.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������
#include "IniFile.h"

// CRemoteApp:
// �йش����ʵ�֣������ Remote.cpp
//

class CRemoteApp : public CWinApp
{
public:
	CRemoteApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��
	CIniFile m_IniFile;                //�ó�Ա�ǽ������ǵ�ini�����ļ��Ķ�д
	DECLARE_MESSAGE_MAP()
};

extern CRemoteApp theApp;