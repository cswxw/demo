#include "LogLoginTime.h"
#include "main.h"



HWND g_hwnd = NULL;  //定义窗口类句柄
DWORD WINAPI CreateLogLoginTime(LPVOID lpParam)
{
	__dbg_printf("[window] start CreateLogLoginTime ");
	WNDCLASS wndclass;    //定义窗口类结构体变量
	
	MSG msg;        //定义消息结构体变量
	HINSTANCE hinstance = GetModuleHandle(0);
	wndclass.style = CS_HREDRAW || CS_VREDRAW; //改变窗口大小则重绘
	wndclass.lpfnWndProc = WindowProc;  //窗口函数为WindowProc()
	wndclass.cbClsExtra = 0; //窗口类无扩展
	wndclass.cbWndExtra = 0;       //窗口实例无扩展
	wndclass.hInstance = hinstance;     //注册窗口类实例句柄
	wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);          //使用光标箭头
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);      //指定鼠标箭头的样式
	wndclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);               //窗口背景为黑色
	wndclass.lpszMenuName = NULL;           //窗口默认无菜单
	wndclass.lpszClassName = "MyLogSystemTimeServiceClass";         //窗口类名

	if (!RegisterClass(&wndclass))         //此为注册类的函数
	{
		return false;    //注册不成功,返回false
	}

	//创建窗口的函数
	//当一个应用程序通过CreateWindowEx函数或者CreateWindow函数请求创建窗口时发送此消息，(此消息在函数返回之前发送)。
	g_hwnd = CreateWindow("MyLogSystemTimeServiceClass",     //窗口类名为"Windows窗口创建"
		"MyLogSystemTimeServiceTitle",   //窗口的名称为“Windows窗口创建”，即窗口标题栏显示的窗口名称
		WS_OVERLAPPEDWINDOW, //重叠式窗口
		CW_USEDEFAULT, CW_USEDEFAULT,    //窗口左上角在屏幕上的默认位置
		CW_USEDEFAULT, CW_USEDEFAULT,    //窗口的宽度和高度
		NULL,      //窗口无父类窗口
		NULL,      //窗口无主菜单
		hinstance,        //创建此窗口的实例句柄
		NULL        //此窗口无创建参数
	);

	ShowWindow(g_hwnd, SW_HIDE); //显示窗口
	UpdateWindow(g_hwnd);          //不断的更新窗口的客户区
	while (GetMessage(&msg, NULL, 0, 0))    //捕获消息
	{
		TranslateMessage(&msg);              //键盘消息转换
		DispatchMessage(&msg);               //派送消息给窗口函数
	}
	__dbg_printf("[window] end CreateLogLoginTime ");
	return msg.wParam;       //返回退出值

}



LRESULT CALLBACK WindowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)               //窗口函数

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
	case WM_PAINT:             //重画窗口客户区消息处理
		HDC hdc; //定义设备句柄
		PAINTSTRUCT ps;            //定义绘图结构体变量
		hdc = BeginPaint(hwnd, &ps);  //获取要重画的窗口的设备描述表句柄
		//TextOut(hdc, 15, 20, "Windows窗口创建的过程!", 22);                //输出到窗口的文本
		EndPaint(hwnd, &ps);               //结束重画
		return 0;
		//用户通过点击关闭程序按钮后，消息队列增加一条消息WM_CLOSE，然后程序从消息队列中取走WM_CLOSE，调用DestroyWindow()，
		//消息队列增加WM_DESTROY，应用程序再次取走，并调用PostQuitMessage()，最终得到WM_QUIT而使消息循环退出，程序退出。
	case WM_CLOSE:
		break;
	case WM_DESTROY:               //撤销窗口消息处理
		__dbg_printf("[window] WM_DESTROY");
		WTSUnRegisterSessionNotification(hwnd);
		PostQuitMessage(0);               //产生退出程序消息WM_QUIT
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