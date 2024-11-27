#include "LogLoginTime.h"
#include "main.h"



HWND g_hwnd = NULL;  //���崰������
DWORD WINAPI CreateLogLoginTime(LPVOID lpParam)
{
	__dbg_printf("[window] start CreateLogLoginTime ");
	WNDCLASS wndclass;    //���崰����ṹ�����
	
	MSG msg;        //������Ϣ�ṹ�����
	HINSTANCE hinstance = GetModuleHandle(0);
	wndclass.style = CS_HREDRAW || CS_VREDRAW; //�ı䴰�ڴ�С���ػ�
	wndclass.lpfnWndProc = WindowProc;  //���ں���ΪWindowProc()
	wndclass.cbClsExtra = 0; //����������չ
	wndclass.cbWndExtra = 0;       //����ʵ������չ
	wndclass.hInstance = hinstance;     //ע�ᴰ����ʵ�����
	wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);          //ʹ�ù���ͷ
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);      //ָ������ͷ����ʽ
	wndclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);               //���ڱ���Ϊ��ɫ
	wndclass.lpszMenuName = NULL;           //����Ĭ���޲˵�
	wndclass.lpszClassName = "MyLogSystemTimeServiceClass";         //��������

	if (!RegisterClass(&wndclass))         //��Ϊע����ĺ���
	{
		return false;    //ע�᲻�ɹ�,����false
	}

	//�������ڵĺ���
	//��һ��Ӧ�ó���ͨ��CreateWindowEx��������CreateWindow�������󴴽�����ʱ���ʹ���Ϣ��(����Ϣ�ں�������֮ǰ����)��
	g_hwnd = CreateWindow("MyLogSystemTimeServiceClass",     //��������Ϊ"Windows���ڴ���"
		"MyLogSystemTimeServiceTitle",   //���ڵ�����Ϊ��Windows���ڴ������������ڱ�������ʾ�Ĵ�������
		WS_OVERLAPPEDWINDOW, //�ص�ʽ����
		CW_USEDEFAULT, CW_USEDEFAULT,    //�������Ͻ�����Ļ�ϵ�Ĭ��λ��
		CW_USEDEFAULT, CW_USEDEFAULT,    //���ڵĿ�Ⱥ͸߶�
		NULL,      //�����޸��ര��
		NULL,      //���������˵�
		hinstance,        //�����˴��ڵ�ʵ�����
		NULL        //�˴����޴�������
	);

	ShowWindow(g_hwnd, SW_HIDE); //��ʾ����
	UpdateWindow(g_hwnd);          //���ϵĸ��´��ڵĿͻ���
	while (GetMessage(&msg, NULL, 0, 0))    //������Ϣ
	{
		TranslateMessage(&msg);              //������Ϣת��
		DispatchMessage(&msg);               //������Ϣ�����ں���
	}
	__dbg_printf("[window] end CreateLogLoginTime ");
	return msg.wParam;       //�����˳�ֵ

}



LRESULT CALLBACK WindowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)               //���ں���

{
	switch (message)
	{
	case WM_CREATE:
		
		if (WTSRegisterSessionNotification(hwnd, NOTIFY_FOR_THIS_SESSION)) {
			__dbg_printf("[window] WTSRegisterSessionNotification success");
		}
		else {
			__dbg_printf("[window] WTSRegisterSessionNotification failed");
		}
		break;
	case WM_PAINT:             //�ػ����ڿͻ�����Ϣ����
		HDC hdc; //�����豸���
		PAINTSTRUCT ps;            //�����ͼ�ṹ�����
		hdc = BeginPaint(hwnd, &ps);  //��ȡҪ�ػ��Ĵ��ڵ��豸��������
		//TextOut(hdc, 15, 20, "Windows���ڴ����Ĺ���!", 22);                //��������ڵ��ı�
		EndPaint(hwnd, &ps);               //�����ػ�
		return 0;
		//�û�ͨ������رճ���ť����Ϣ��������һ����ϢWM_CLOSE��Ȼ��������Ϣ������ȡ��WM_CLOSE������DestroyWindow()��
		//��Ϣ��������WM_DESTROY��Ӧ�ó����ٴ�ȡ�ߣ�������PostQuitMessage()�����յõ�WM_QUIT��ʹ��Ϣѭ���˳��������˳���
	case WM_CLOSE:
		break;
	case WM_DESTROY:               //����������Ϣ����
		__dbg_printf("[window] WM_DESTROY");
		WTSUnRegisterSessionNotification(hwnd);
		PostQuitMessage(0);               //�����˳�������ϢWM_QUIT
		return 0;
	case WM_WTSSESSION_CHANGE: {
		switch (wParam)
		{
		case WTS_CONSOLE_CONNECT:
			__dbg_printf(TEXT("[info] WTS_CONSOLE_CONNECT"));
			break;
		case WTS_CONSOLE_DISCONNECT:
			__dbg_printf(TEXT("[info] WTS_CONSOLE_CONNECT"));
			break;
		case WTS_REMOTE_CONNECT:
			__dbg_printf(TEXT("[info] WTS_REMOTE_CONNECT"));
			break;
		case WTS_REMOTE_DISCONNECT:
			__dbg_printf(TEXT("[info] WTS_REMOTE_DISCONNECT"));
			break;
		case WTS_SESSION_LOGON:
			__dbg_printf(TEXT("[info] WTS_SESSION_LOGON"));
			break;
		case WTS_SESSION_LOGOFF:
			__dbg_printf(TEXT("[info] WTS_SESSION_LOGOFF"));
			break;
		case WTS_SESSION_LOCK:
			logTime("WTS_SESSION_LOCK");
			__dbg_printf(TEXT("[info] WTS_SESSION_LOCK"));
			break;
		case WTS_SESSION_UNLOCK:
			logTime("WTS_SESSION_UNLOCK");
			__dbg_printf(TEXT("[info] WTS_SESSION_UNLOCK"));
			break;
		case WTS_SESSION_REMOTE_CONTROL:
			__dbg_printf(TEXT("[info] WTS_SESSION_REMOTE_CONTROL"));
			break;
		default:
			__dbg_printf(TEXT("[info] default"));
			break;
		}
		}
	}
	return DefWindowProc(hwnd, message, wParam, lParam);

}