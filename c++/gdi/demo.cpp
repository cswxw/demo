#include <windows.h>
#include <string.h>
#include <locale.h>
#include <tchar.h>


static TCHAR szAppName[] = TEXT("GDI Test");
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
     
     hWnd = CreateWindow(szAppName,            // window class name
                          szAppName,           // window caption
                          WS_OVERLAPPEDWINDOW, // window style
                          CW_USEDEFAULT,       // initial x position
                          CW_USEDEFAULT,       // initial y position
                          400,                 // initial x size
                          300,                 // initial y size
                          NULL,                // parent window handle
                          NULL,                // window menu handle
                          hInstance,           // program instance handle
                          NULL);               // creation parameters
     
     ShowWindow(hWnd, iCmdShow);
     UpdateWindow(hWnd);
     
     while (GetMessage(&msg, NULL, 0, 0))
     {
          TranslateMessage(&msg);
          DispatchMessage(&msg);
     }

     return msg.wParam;
}

//绘制指定属性的直线
static void DrawLine(HDC hDC, int x0, int y0, int x1, int y1, int style, int width, COLORREF color)
{
	HPEN hPen = CreatePen(style, width, color);
	HPEN hOldPen = (HPEN)SelectObject(hDC, hPen);

	MoveToEx(hDC, x0, y0, NULL);
	LineTo(hDC, x1, y1);

	SelectObject(hDC, hOldPen);
	DeleteObject(hPen);
}

//绘制实心圆
static void DrawCircle(HDC hDC, int x, int y, int len, COLORREF color)
{
	HBRUSH hBrush = CreateSolidBrush(color);
	HBRUSH hOldBrush = (HBRUSH)SelectObject(hDC, hBrush);

	HPEN hPen = CreatePen(PS_SOLID, 1, color);
	HPEN hOldPen = (HPEN)SelectObject(hDC, hPen);

	Ellipse(hDC, x-len/2, y-len/2, x+len/2, y+len/2);

	SelectObject(hDC, hOldBrush);
	DeleteObject(hPen);

	SelectObject(hDC, hOldPen);
	DeleteObject(hOldBrush);
}

//绘制填充矩形
static void DrawRect(HDC hDC, int left, int top, int width, int height, int style, COLORREF color)
{
	HBRUSH hBrush = CreateHatchBrush(style, color);
	HBRUSH hOldBrush = (HBRUSH)SelectObject(hDC, hBrush);

	Rectangle(hDC, left, top, left+width, top+height);
	
	SelectObject(hDC, hOldBrush);
	DeleteObject(hOldBrush);
}

//绘制位图填充矩形
static void DrawBmpRect(HDC hDC, int left, int top, int width, int height, LPCTSTR file)
{
	HBITMAP hBitmap = (HBITMAP)LoadImage(NULL, file, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE|LR_CREATEDIBSECTION);
	HBRUSH hBrush = CreatePatternBrush(hBitmap);
	HBRUSH hOldBrush = (HBRUSH)SelectObject(hDC, hBrush);

	Rectangle(hDC, left, top, left+width, top+height);

	SelectObject(hDC, hOldBrush);
	DeleteObject(hOldBrush);
	DeleteObject(hBitmap);
}


char* getScreen(long left,long top,long right,long bottom, int *sumSize){
 
	HWND DeskWnd=::GetDesktopWindow();//获取桌面窗口句柄
	RECT DeskRC;
	DeskRC.left = left;
	DeskRC.top = top;
	DeskRC.right = right;
	DeskRC.bottom = bottom;
 
//	::GetClientRect(DeskWnd,&DeskRC);//获取窗口大小
	HDC DeskDC=GetDC(DeskWnd);//获取窗口DC
	HBITMAP DeskBmp=::CreateCompatibleBitmap(DeskDC,DeskRC.right-DeskRC.left,DeskRC.bottom-DeskRC.top);//兼容位图
	HDC memDC=::CreateCompatibleDC(DeskDC);//兼容DC
	SelectObject(memDC,DeskBmp);//把兼容位图选入兼容DC中
	BitBlt(memDC,0,0,right-DeskRC.left,DeskRC.bottom-DeskRC.top,DeskDC,DeskRC.left,DeskRC.top,SRCCOPY);//拷贝DC
 
	BITMAP bmInfo;
	DWORD bmDataSize;
	char *bmData;//位图数据
	GetObject(DeskBmp,sizeof(BITMAP),&bmInfo);//根据位图句柄，获取位图信息
	bmDataSize=bmInfo.bmWidthBytes*bmInfo.bmHeight;//计算位图数据大小
	bmData=new char[bmDataSize];//分配数据
 
	BITMAPFILEHEADER bfh;//位图文件头
	bfh.bfType=0x4d42;
	bfh.bfSize=bmDataSize+54;
	bfh.bfReserved1=0;
	bfh.bfReserved2=0;
	bfh.bfOffBits=54;
 
	BITMAPINFOHEADER bih;//位图信息头
	bih.biSize=40;
	bih.biWidth=bmInfo.bmWidth;
	bih.biHeight=bmInfo.bmHeight;
	bih.biPlanes=1;
	bih.biBitCount=24;
	bih.biCompression=BI_RGB;
	bih.biSizeImage=bmDataSize;
	bih.biXPelsPerMeter=0;
	bih.biYPelsPerMeter=0;
	bih.biClrUsed=0;
	bih.biClrImportant=0;
 
	::GetDIBits(DeskDC,DeskBmp,0,bmInfo.bmHeight,bmData,(BITMAPINFO *)&bih,DIB_RGB_COLORS);//获取位图数据部分
 
    *sumSize = sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER)+bmDataSize;
    char * stream = new char[*sumSize];
	memcpy(stream,&bfh,sizeof(BITMAPFILEHEADER));
	memcpy(stream+sizeof(BITMAPFILEHEADER),&bih,sizeof(BITMAPINFOHEADER));
	memcpy(stream+sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER),bmData,bmDataSize);
 
	HANDLE hFile=CreateFileA("d:\\Screen.bmp",GENERIC_WRITE,0,NULL,CREATE_ALWAYS,
	FILE_ATTRIBUTE_NORMAL,0);//创建文件
	DWORD dwSize;
	WriteFile(hFile,(void *)stream,*sumSize,&dwSize,0);//写入位图文件头
	::CloseHandle(hFile);//关闭文件句柄
 
 
 
	return stream;
 
}


//保存图片数据到文件
void ScreenSnap(HBITMAP hBitmap,char *bmpPath,HDC dc)
{
	BITMAP bmInfo;
	DWORD bmDataSize;
	char *bmData;//位图数据
	GetObject(hBitmap,sizeof(BITMAP),&bmInfo);//根据位图句柄，获取位图信息
	bmDataSize=bmInfo.bmWidthBytes*bmInfo.bmHeight;//计算位图数据大小
	bmData=new char[bmDataSize];//分配数据

	//位图文件头
	BITMAPFILEHEADER bfh;
	bfh.bfType=0x4d42; // Specifies the file type, must be BM. 
	bfh.bfSize=bmDataSize+ 54;  //Specifies the size, in bytes, of the bitmap file. 
	bfh.bfReserved1=0;  //Reserved; must be zero. 
	bfh.bfReserved2=0; //Reserved; must be zero. 
	bfh.bfOffBits= 54;  //Specifies the offset, in bytes, from the beginning of the BITMAPFILEHEADER structure to the bitmap bits.
	
	int totalHeader = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER); // 14+ 40

	//位图信息头
	BITMAPINFOHEADER bih;
	bih.biSize=40;   //Specifies the number of bytes required by the structure. 
	bih.biWidth=bmInfo.bmWidth;  //Specifies the width of the bitmap, in pixels. 
	bih.biHeight=bmInfo.bmHeight;  //Specifies the height of the bitmap, in pixels. ......
	bih.biPlanes=1;  //Specifies the number of planes for the target device. This value must be set to 1.
	
	bih.biBitCount=24; //Specifies the number of bits-per-pixel. 
					   //The bitmap has a maximum of 2^24 colors,
	bih.biCompression=BI_RGB;  //BI_RGB   An uncompressed format.
	bih.biSizeImage=bmDataSize; //Specifies the size, in bytes, of the image. This may be set to zero for BI_RGB bitmaps. 
	bih.biXPelsPerMeter=0; //指定位图的目标设备的水平分辨率（以每像素为单位）。 应用程序可以使用此值从资源组中选择与当前设备的特征最匹配的位图。
	bih.biYPelsPerMeter=0; //指定位图的目标设备的垂直分辨率（以每像素为单位）。
	bih.biClrUsed=0;  //Specifies the number of color indexes in the color table that are actually used by the bitmap.
	bih.biClrImportant=0; //Specifies the number of color indexes that are required for displaying the bitmap. If this value is zero, all colors are required. 
/*
int GetDIBits(  HDC hdc,           // handle to DC
  HBITMAP hbmp,      // handle to bitmap
  UINT uStartScan,   // first scan line to set
  UINT cScanLines,   // number of scan lines to copy
  LPVOID lpvBits,    // array for bitmap bits
  LPBITMAPINFO lpbi, // bitmap data buffer
  UINT uUsage        // RGB or palette index);
					//DIB_RGB_COLORS The color table should consist of literal red, green, blue (RGB) values. 
*/
	::GetDIBits(dc,hBitmap,0,bmInfo.bmHeight,bmData,(BITMAPINFO *)&bih,DIB_RGB_COLORS);//获取位图数据部分
			
	DWORD dwSize;
	HANDLE hFile=CreateFileA(bmpPath,GENERIC_WRITE,0,NULL,CREATE_ALWAYS,
		  FILE_ATTRIBUTE_NORMAL,0);//创建文件
	WriteFile(hFile,(void *)&bfh,sizeof(BITMAPFILEHEADER),&dwSize,0);//写入位图文件头
	WriteFile(hFile,(void *)&bih,sizeof(BITMAPINFOHEADER),&dwSize,0);//写入位图信息头
	WriteFile(hFile,(void *)bmData,bmDataSize,&dwSize,0);//写入位图数据
	::CloseHandle(hFile);//关闭文件句柄
	free(bmData);
}
//截全屏
void captureFullWin(){
	 HWND DeskWnd=::GetDesktopWindow();//获取桌面窗口句柄
	 RECT DeskRC;
	 ::GetClientRect(DeskWnd,&DeskRC);//获取窗口大小
	 
	 HDC DeskDC=GetDC(DeskWnd);//获取窗口DC
	 HDC memDC=::CreateCompatibleDC(DeskDC);//兼容DC

	 HBITMAP DeskBmp=::CreateCompatibleBitmap(DeskDC,DeskRC.right,DeskRC.bottom);//兼容位图
	 
	 SelectObject(memDC,DeskBmp);//把兼容位图选入兼容DC中

	/*
	BOOL BitBlt(
	  HDC hdcDest, // handle to destination DC
	  int nXDest,  // x-coord of destination upper-left corner
	  int nYDest,  // y-coord of destination upper-left corner
	  int nWidth,  // width of destination rectangle
	  int nHeight, // height of destination rectangle
	  HDC hdcSrc,  // handle to source DC
	  int nXSrc,   // x-coordinate of source upper-left corner
	  int nYSrc,   // y-coordinate of source upper-left corner
	  DWORD dwRop  // raster operation code
	);
	*/
	BitBlt(memDC,0,0,DeskRC.right,DeskRC.bottom,DeskDC,0,0,SRCCOPY);//拷贝DC
	
	ScreenSnap(DeskBmp,"d:\\Screen.bmp",DeskDC);
}

static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	HDC         hDC;
	PAINTSTRUCT ps;

	switch (message)
	{
	case WM_CREATE:
		captureFullWin();
		return 0;

	case WM_PAINT:
#if 1
		{
			hDC = BeginPaint(hWnd, &ps);
			for (int i=10; i<50; i+=4)
			{
				SetPixel(hDC, i, 10, RGB(0, 0, 0)); //绘制像素点
			}
                        //绘制不同模式的直线
			DrawLine(hDC, 120, 30, 200, 30, PS_SOLID, 2, RGB(0,0,0));
			DrawLine(hDC, 120, 50, 200, 50, PS_DASH, 1, RGB(100,0,200));
			DrawLine(hDC, 120, 70, 200, 70, PS_DASHDOT, 1, RGB(100,250,100));
                        //绘制弧线、弦割线、饼图
			Arc(hDC, 10, 30, 40, 50, 40, 30, 10, 40);
			Chord(hDC, 10, 60, 40, 80, 40, 60, 10, 70);
			Pie(hDC, 10, 90, 40, 110, 40, 90, 10, 100);

			POINT pt[4] = {{90,130},{60,40},{140,150},{160,80}};
                        //绘制椭圆、矩形
			Ellipse(hDC,pt[0].x, pt[0].y, pt[1].x, pt[1].y);
			Rectangle(hDC, pt[2].x, pt[2].y, pt[3].x, pt[3].y);

                        //绘制贝塞尔曲线
			PolyBezier(hDC, pt, 4);
                        //标出贝塞尔曲线的四个锚点
			DrawCircle(hDC, pt[0].x, pt[0].y, 8, RGB(0,255,0));
			DrawCircle(hDC, pt[1].x, pt[1].y, 8, RGB(0,0,255));
			DrawCircle(hDC, pt[2].x, pt[2].y, 8, RGB(0,0,0));
			DrawCircle(hDC, pt[3].x, pt[3].y, 8, RGB(255,0,0));
                        //绘制圆
			DrawCircle(hDC, 100, 180, 60, RGB(0, 250, 250));
                        //绘制不同填充模式的矩形
			DrawRect(hDC, 220, 20, 60, 40, HS_BDIAGONAL, RGB(255,0,0));
			DrawRect(hDC, 220, 80, 60, 40, HS_CROSS, RGB(0,255,0));
			DrawRect(hDC, 290, 20, 60, 40, HS_DIAGCROSS, RGB(0,0,255));
			DrawRect(hDC, 290, 80, 60, 40, HS_VERTICAL, RGB(0,0,0));
                        //绘制位图
			DrawBmpRect(hDC, 180, 140, 180, 100, TEXT("chenggong.bmp"));
                        //绘制文本
			TextOut(hDC, 20, 220, TEXT("GDI画图输出测试程序"), 11);
			//EndPaint(hWnd, &ps);
		}
		{
			RECT rect = {10, 30, 100, 50};
			TCHAR str[] = TEXT("English and 中文");
 
			//hDC = BeginPaint(hWnd, &ps);
			TextOut(hDC, 10, 10, str, _tcslen(str));
 
			SetTextColor(hDC, RGB(255,0,0));
			DrawText(hDC, str, -1, &rect, DT_LEFT|DT_VCENTER);
 
			SetTextColor(hDC, RGB(0,255,0));
			INT dx[] = {8,8,8,8,16,8,8,8,16,8,8,8,10};
			ExtTextOut(hDC, 10, 50, 0, &rect, str, _tcslen(str), dx);
 
			SetTextColor(hDC, RGB(0,0,255));
			rect.right = 110;
			rect.top = 70;
			rect.bottom = 82;
			ExtTextOut(hDC, 10, rect.top, ETO_CLIPPED, &rect, str, _tcslen(str), dx);
			HFONT hFont = CreateFont(96,         // nHeight, 所创建字体的字符高度
						0,           // nWidth,       字体的字符平均宽度
						200,          // nEscapement,  字符输出方向与水平向右的方向所成角度,以0.1度为单位
						0,             // nOrientation, 字符与基线的角度，以0.1度为单位
						FW_BOLD,        // nWeight,      字符颜色的深浅度
						TRUE,            // bItalic,      斜体属性标志(FALSE:正常字体，TRUE:斜体)
						FALSE,            // bUnderline,   下划线属性标志(FALSE:无下划线，TRUE:有下划线)
						FALSE,             // cStrikeOut,   删除线属性标志(FALSE:无删除线，TRUE:有删除线)
						ANSI_CHARSET,       // nCharSet,        字符集标识0:ANSI字符集，1:系统缺省字符集
						OUT_DEFAULT_PRECIS,  // nOutPrecision,   输出精度
						CLIP_DEFAULT_PRECIS, // nClipPrecision,  剪切精度
						DEFAULT_QUALITY,      // nQuality,        输出品质
						DEFAULT_PITCH|FF_SWISS, // nPitchAndFamily, 字符间距
						TEXT("Arial"));          // lpszFacename,    现有系统TrueType字体名称
			HFONT hOldFont = (HFONT)SelectObject(hDC, hFont);
			SetBkMode(hDC, TRANSPARENT);
			SetTextColor(hDC, RGB(0x00, 0xFF, 0xFF));
			TextOut(hDC, 0, 150, TEXT("创建Font"), 6);
			DeleteObject(hFont);
			EndPaint(hWnd, &ps);
		}		
#endif
		return 0;

	case WM_DESTROY:
		PostQuitMessage(0);
		return 0 ;
	}

	return DefWindowProc (hWnd, message, wParam, lParam);
}


