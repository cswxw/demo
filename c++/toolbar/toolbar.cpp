#include <windows.h>
#include <commctrl.h>
#include <tchar.h>

#pragma comment(lib, "comctl32.lib")

// Windows XP sytle button
#pragma comment(linker,"\"/manifestdependency:type='win32' "\
						"name='Microsoft.Windows.Common-Controls' "\
						"version='6.0.0.0' processorArchitecture='*' "\
						"publicKeyToken='6595b64144ccf1df' language='*'\"")

#define IDC_TOOLBAR   1001
#define IDC_STATUSBAR 1002
//#define PIC_RESOURCE_USED

#ifdef PIC_RESOURCE_USED
#define IDB_NEW   110
#define IDB_OPEN  111
#define IDB_SAVE  112
#endif
#define ID_FOPEN  1111
#define ID_FCLOSE 1112
#define ID_FSAVE  1113

static TCHAR szAppName[] = TEXT("toolbar");
static LRESULT CALLBACK WndProc (HWND, UINT, WPARAM, LPARAM);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)
{
     HWND     hWnd;
     MSG      msg;
     WNDCLASS wndclass;

     wndclass.style         = CS_HREDRAW | CS_VREDRAW;
     wndclass.lpfnWndProc   = WndProc;
     wndclass.cbClsExtra    = 0;
     wndclass.cbWndExtra    = 0;
     wndclass.hInstance     = hInstance;
     wndclass.hIcon         = LoadIcon(NULL, IDI_APPLICATION);
     wndclass.hCursor       = LoadCursor(NULL, IDC_ARROW);
     wndclass.hbrBackground = (HBRUSH) GetStockObject(WHITE_BRUSH);
     wndclass.lpszMenuName  = NULL;
     wndclass.lpszClassName = szAppName;

     if (!RegisterClass(&wndclass))
     {
          MessageBox (NULL, TEXT("This program requires Windows NT!"), szAppName, MB_ICONERROR);
          return 0;
     }
     //初始化公共空间
     INITCOMMONCONTROLSEX icc;
     icc.dwSize = sizeof(INITCOMMONCONTROLSEX);
     icc.dwICC = ICC_BAR_CLASSES;
     InitCommonControlsEx(&icc);
     hWnd = CreateWindow(szAppName,                  // window class name
                          szAppName,                 // window caption
                          WS_OVERLAPPEDWINDOW,        // window style
                          CW_USEDEFAULT,              // initial x position
                          CW_USEDEFAULT,              // initial y position
                          400,              // initial x size
                          300,              // initial y size
                          NULL,                       // parent window handle
                          NULL,                       // window menu handle
                          hInstance,                  // program instance handle
                          NULL);                     // creation parameters
     
     ShowWindow(hWnd, iCmdShow);
     UpdateWindow(hWnd);
     
     while (GetMessage(&msg, NULL, 0, 0))
     {
          TranslateMessage(&msg);
          DispatchMessage(&msg);
     }

     return msg.wParam;
}

HWND CreateToolbar(HWND hParentWnd)
{
    HWND hWndTB;
    TBBUTTON tbb[3];
    HIMAGELIST hImageList,hHotImageList,hDisableImageList;
    HBITMAP hBitmap;

    HINSTANCE hInst = GetModuleHandle(NULL);
    //创建Toolbar控件
    hWndTB = CreateWindowEx(0, TOOLBARCLASSNAME,TEXT(""),
							WS_CHILD|WS_VISIBLE|WS_BORDER|TBSTYLE_LIST|TBSTYLE_AUTOSIZE|TBSTYLE_TOOLTIPS,
							0,0,0,0,
							hParentWnd,
							(HMENU)IDC_TOOLBAR,
							hInst,
							NULL);
    if(!hWndTB)
    {
        return NULL;
    }
    SendMessage(hWndTB, TB_BUTTONSTRUCTSIZE, (WPARAM)sizeof(TBBUTTON), 0);
    //下面创建三组24x24像素大小的位图图像列表，用于工具栏图标
    hImageList = ImageList_Create(24,24,ILC_COLOR24|ILC_MASK,3,1);
#ifdef PIC_RESOURCE_USED
    hBitmap = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_COLOR3));
#else
    hBitmap = (HBITMAP)LoadImage(NULL, TEXT("color24x3.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE|LR_CREATEDIBSECTION); //加载一组图片
#endif
    ImageList_AddMasked(hImageList, hBitmap, RGB(255,255,255));
    DeleteObject (hBitmap);
    SendMessage(hWndTB,TB_SETIMAGELIST,0,(LPARAM)hImageList); //正常显示时的图像列表

    hHotImageList = ImageList_Create(24,24,ILC_COLOR24|ILC_MASK,3,1);
#ifdef PIC_RESOURCE_USED
    hBitmap = LoadBitmap(hInst,MAKEINTRESOURCE(IDB_GREEN3));
#else
    hBitmap = (HBITMAP)LoadImage(NULL, TEXT("green24x3.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE|LR_CREATEDIBSECTION);
#endif
    ImageList_AddMasked(hHotImageList,hBitmap, RGB(255,255,255));
    DeleteObject (hBitmap);
    SendMessage(hWndTB,TB_SETHOTIMAGELIST,0,(LPARAM)hHotImageList); //鼠标悬浮时的图像列表

    hDisableImageList = ImageList_Create(24,24,ILC_COLOR24|ILC_MASK,3,1);
#ifdef PIC_RESOURCE_USED
    hBitmap = LoadBitmap(hInst,MAKEINTRESOURCE(IDB_GRAY3));
#else
    hBitmap = (HBITMAP)LoadImage(NULL, TEXT("gray24x3.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE|LR_CREATEDIBSECTION);
#endif
    ImageList_AddMasked(hDisableImageList,hBitmap, RGB(255,255,255));
    DeleteObject (hBitmap);
    SendMessage(hWndTB,TB_SETDISABLEDIMAGELIST,0,(LPARAM)hDisableImageList); //当工具栏button失能是的图像列表

    ZeroMemory(tbb, sizeof(tbb));
    tbb[0].iBitmap =MAKELONG(0,0) ;
    tbb[0].fsState = TBSTATE_ENABLED;
    tbb[0].fsStyle = TBSTYLE_BUTTON|BTNS_AUTOSIZE;
    tbb[0].idCommand = ID_FOPEN; 
    tbb[0].iString = (INT_PTR)TEXT("打开");
    tbb[1].iBitmap =MAKELONG(1,0);
    tbb[1].fsState = TBSTATE_ENABLED;
    tbb[1].fsStyle = TBSTYLE_BUTTON|BTNS_AUTOSIZE;
    tbb[1].idCommand = ID_FCLOSE; 
    tbb[1].iString = (INT_PTR)TEXT("关闭");
    tbb[2].iBitmap =MAKELONG(2,0);
    tbb[2].fsState = TBSTATE_ENABLED;
    tbb[2].fsStyle = TBSTYLE_BUTTON|BTNS_AUTOSIZE;
    tbb[2].idCommand = ID_FSAVE;
    tbb[2].iString = (INT_PTR)TEXT("保存");
    SendMessage(hWndTB, TB_ADDBUTTONS, sizeof(tbb)/sizeof(TBBUTTON), (LPARAM)&tbb); //配置工具栏按钮信息
    SendMessage(hWndTB,WM_SIZE,0,0);

    return hWndTB;
}

HWND CreateStatusBar(HWND hParentWnd)
{
#define PANEL_NUM 3
    int array[PANEL_NUM]={120,120*2,-1};
    HINSTANCE hInst = GetModuleHandle(NULL);
    //创建Statusbar控件
    HWND hWndStatus = CreateWindowEx(0, STATUSCLASSNAME, TEXT(""), WS_CHILD|WS_BORDER|WS_VISIBLE, 0, 0, 0, 0, hParentWnd, (HMENU)IDC_STATUSBAR, hInst, NULL);
    if (hWndStatus)
    {
        SendMessage(hWndStatus,SB_SETPARTS,(WPARAM)PANEL_NUM,(LPARAM)array); //设置面板个数
        SendMessage(hWndStatus,SB_SETTEXT,(LPARAM)1,(WPARAM)TEXT("panel-1")); //设置第二个面板内容
        SendMessage(hWndStatus,SB_SETTEXT,(LPARAM)2,(WPARAM)TEXT("panel-2")); //设置第三个面板内容
    }
#undef PANEL_NUM

    return hWndStatus;
}

static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    HDC         hDC;
    PAINTSTRUCT ps;
    static HWND hToolbar;
    static HWND hStatusbar;

    switch (message)
    {
    case WM_CREATE:
        hToolbar = CreateToolbar(hWnd);
        hStatusbar = CreateStatusBar(hWnd);
        return 0;
	
    case WM_COMMAND:
    {
        int wmId    = LOWORD(wParam);
        int wmEvent = HIWORD(wParam);

        // 分析菜单选择:
        switch (wmId)
        {
        case ID_FOPEN:
            SendMessage(hToolbar, TB_ENABLEBUTTON, (WPARAM)ID_FOPEN, (LPARAM)MAKELONG(FALSE,0));
            SendMessage(hToolbar, TB_ENABLEBUTTON, (WPARAM)ID_FSAVE, (LPARAM)MAKELONG(TRUE,0));
            break;
        case ID_FSAVE:
            SendMessage(hToolbar, TB_ENABLEBUTTON, (WPARAM)ID_FSAVE, (LPARAM)MAKELONG(FALSE,0));
            SendMessage(hToolbar, TB_ENABLEBUTTON, (WPARAM)ID_FOPEN, (LPARAM)MAKELONG(TRUE,0));
            break;
        case ID_FCLOSE:
            MessageBox(hWnd, TEXT("click!"), TEXT("hint"), MB_OK);
            break;
        }
    }
    return 0;

    case WM_NOTIFY:
    {
        LPNMHDR lpnmhdr=(LPNMHDR)lParam;
        LPTOOLTIPTEXT lpttext;

        if(lpnmhdr->code==TTN_GETDISPINFO)  
        {
            //处理鼠标在工具栏上悬浮移动时的文本提示
            lpttext=(LPTOOLTIPTEXT)lParam;
            switch(lpttext->hdr.idFrom)
            {
            case ID_FOPEN:
                lpttext->lpszText=TEXT("打开文件");
                break;

            case ID_FCLOSE:
                lpttext->lpszText=TEXT("关闭文件");
                break;

            case ID_FSAVE:
                lpttext->lpszText=TEXT("保存为文件");
                break;
            }
        }
    }
    return 0;

    case WM_SIZE:
    {
        SendMessage(hToolbar, TB_AUTOSIZE, 0, 0);
        SendMessage(hStatusbar, WM_SIZE, 0, 0); 
    }
    return 0;

    case WM_MOUSEMOVE:
    {
        TCHAR szBuf[MAX_PATH];
        _stprintf(szBuf,TEXT("Mouse(%d,%d)"),LOWORD(lParam),HIWORD(lParam));
        SendMessage(hStatusbar, SB_SETTEXT, 0, (LPARAM)(LPSTR)szBuf);
    }
    return 0;

    case WM_PAINT:
        hDC = BeginPaint(hWnd, &ps);
        ;
        EndPaint(hWnd, &ps);
        return 0;

    case WM_DESTROY:
        PostQuitMessage(0);
        return 0 ;
    }
    return DefWindowProc (hWnd, message, wParam, lParam);
}


