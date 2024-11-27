// lives.cpp : 定义控制台应用程序的入口点。
//

#include <windows.h>
#include <TlHelp32.h>
#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char exepath[MAX_PATH];

//声明需要用到的函数
typedef HMODULE(WINAPI *lpLoadLibraryA)(char* filename);
typedef FARPROC(WINAPI *lpGetProcAddress)(HMODULE hModule, char* funcName);
typedef UINT(
WINAPI*
lpWinExec)(
        IN LPCSTR lpCmdLine,
        IN UINT uCmdShow
        );

typedef DWORD(
WINAPI*
lpWaitForSingleObject)(
                    IN HANDLE hHandle,
                    IN DWORD dwMilliseconds
                    );


typedef HANDLE(
WINAPI*
lpOpenProcess)(
            IN DWORD dwDesiredAccess,
            IN BOOL bInheritHandle,
            IN DWORD dwProcessId
            );


typedef struct _thread_param
{
    lpLoadLibraryA loadFunc;
    lpGetProcAddress GetPFunc;
    DWORD PID;//被保护的进程的进程id
    //char mutex[8];//保证只被注入一次
    //保存所有参数
    //在被注入进程中需要调用的函数:LoadLibrary,GetProcAddress,WaitForSimpleObject,WinExec
    //因此需要参数:被保护的进程路径,kernel32.dll ,WaitForSingleObject,WinExec,
    //OpenProcess(这3个作为GetProcAddress的参数) 共5个参数
    char data[6][100]; 
}thread_param;

#if 0
DWORD WINAPI t1(){
	wchar_t Mutex[]= L"mutextest001";
    DWORD dwSize = 0;
    PSYSTEM_HANDLE_INFORMATION pmodule = NULL;
    POBJECT_NAME_INFORMATION pNameInfo;
    POBJECT_NAME_INFORMATION pNameType;
    PVOID pbuffer = NULL;
    NTSTATUS Status;
    int nIndex = 0;
    DWORD dwFlags = 0;
    char szType[128] = {0};
    char szName[512] = {0};
     
    if(ZwQuerySystemInformation==NULL)
    {
        printf("ZwQuerySystemInformation失败!\n");
        return 0;
    }
     
    pbuffer = VirtualAlloc(NULL, 0x1000, MEM_COMMIT, PAGE_READWRITE);
     
    if(pbuffer==NULL)
    {
        printf("pbuffer 0x1000失败!\n");
        return 0;
    }
     
    Status = ZwQuerySystemInformation(SystemHandleInformation, pbuffer, 0x1000, &dwSize);
     
    if(NT_SUCCESS(Status)==NULL)
    {
        if (STATUS_INFO_LENGTH_MISMATCH != Status)
        {
            printf("Status失败!\n");
            return 0;
        }
        else
        {
            // 这里大家可以保证程序的正确性使用循环分配稍好
            if (NULL != pbuffer)
            {
                VirtualFree(pbuffer, 0, MEM_RELEASE);
            }
             
            if (dwSize*2 > 0x4000000)  // MAXSIZE
            {
                printf("dwSize溢出!\n");
                return 0;
            }
             
            pbuffer = VirtualAlloc(NULL, dwSize*2, MEM_COMMIT, PAGE_READWRITE);
             
            if(pbuffer==NULL)
            {
                printf("pbuffer dwSize失败!\n");
                return 0;
            }
             
            Status = ZwQuerySystemInformation(SystemHandleInformation, pbuffer, dwSize*2, NULL);
             
            if(NT_SUCCESS(Status)==NULL)
            {
                printf("pbuffer dwSize失败!\n"); 
                return 0;
            }
        }
    }
    pmodule = (PSYSTEM_HANDLE_INFORMATION)((PULONG)pbuffer+1);
    dwSize = *((PULONG)pbuffer);
    for(nIndex = 0; nIndex < dwSize; nIndex++)
    {
         
        Status = NtQueryObject((HANDLE)pmodule[nIndex].Handle, ObjectNameInformation, szName, 512, &dwFlags);
         
        if (NT_SUCCESS(Status)==NULL)
        {
            printf("Status name失败!\n");
            return 0;
        }
        Status = NtQueryObject((HANDLE)pmodule[nIndex].Handle, ObjectTypeInformation, szType, 128, &dwFlags);
         
        if (NT_SUCCESS(Status)==NULL)
        {
            printf("Status type失败!\n");
            return 0;
        }
        pNameInfo = (POBJECT_NAME_INFORMATION)szName;
        pNameType = (POBJECT_NAME_INFORMATION)szType;
        GetDlgItem(IDC_EDIT1)->SetWindowText(szType);
        GetDlgItem(IDC_EDIT2)->SetWindowText(szName);
        if (0 == wcscmp((wchar_t *)pNameType->Name.Buffer, L"Mutant"))
        {
            if (wcsstr((wchar_t *)pNameInfo->Name.Buffer, Mutex))
            {
                CloseHandle((HANDLE)pmodule[nIndex].Handle);
                AfxMessageBox("已经可以再开启游戏!\n");
                break;
            }
        }
    } 
    if(nIndex == dwSize)
        printf("关闭互斥句柄失败!\n"); 
    if (NULL != pbuffer)
    {
        VirtualFree(pbuffer, 0, MEM_RELEASE);
    }
}

#endif

DWORD WINAPI threadProc(LPVOID param)
{
    thread_param* tparam = (thread_param*)param;
    HMODULE hd=tparam->loadFunc(tparam->data[0]);//data的第一个参数是kernel32.dll
    lpWaitForSingleObject wfso=(lpWaitForSingleObject)tparam->GetPFunc(hd,tparam->data[2]);
    lpWinExec we=(lpWinExec)tparam->GetPFunc(hd,tparam->data[3]);
    lpOpenProcess op=(lpOpenProcess)tparam->GetPFunc(hd,tparam->data[4]);
    DWORD pid=tparam->PID;
    HANDLE hProcess=op(PROCESS_ALL_ACCESS,0,pid);  //lpOpenProcess

    wfso(hProcess,INFINITE);  //lpWaitForSingleObject

    we(tparam->data[1],SW_SHOW);//lpWinExec
    return 0;
}

DWORD codeInject(DWORD pid)
{
	BOOL  ret = 0;
    HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, 0, pid);
    if (hProcess==0||hProcess==INVALID_HANDLE_VALUE)
    {
        return 0;
    }
    thread_param param = { 0 };
    param.loadFunc = (lpLoadLibraryA)GetProcAddress(GetModuleHandleA("kernel32.dll"), "LoadLibraryA");
    param.GetPFunc = (lpGetProcAddress)GetProcAddress(GetModuleHandleA("kernel32.dll"), "GetProcAddress");
    memcpy(&param.data[0], "kernel32.dll", 13);// kernel32.dll
    GetModuleFileNameA(0,exepath,MAX_PATH);
    memcpy(&param.data[1], exepath, strlen(exepath)+1);//程序自身全路径
    memcpy(&param.data[2], "WaitForSingleObject", 20);
    memcpy(&param.data[3], "WinExec", 8);
    memcpy(&param.data[4],"OpenProcess",12);
	memcpy(&param.data[5],"OpenProcess",12);
    param.PID=GetCurrentProcessId();
    //DWORD codesize = (DWORD)codeInject - (DWORD)threadProc; //计算线程函数的代码大小
	//写入函数参数
    LPVOID database = VirtualAllocEx(hProcess, 0, sizeof(thread_param), MEM_COMMIT, PAGE_READWRITE);
    DWORD written;
    HANDLE hThread;
    if (database==0)
    {
        CloseHandle(hProcess);
        return 0;
    }
    ret = WriteProcessMemory(hProcess, database, &param, sizeof(thread_param), &written);
	if(ret == 0){
		printf("WriteProcessMemory data failed!\n");
		return 0;
	}

	//写入函数代码
    LPVOID codebase = VirtualAllocEx(hProcess, 0, 0x1000, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
    if (codebase == 0)
    {
        VirtualFreeEx(hProcess, database, sizeof(thread_param), MEM_FREE);
        CloseHandle(hProcess);
        return 0;
    }
    ret = WriteProcessMemory(hProcess, codebase, &threadProc, 0x1000, &written);
	if(ret == 0){
		printf("WriteProcessMemory code failed!\n");
		return 0;
	}
	//创建远程线程
	//如果调用成功,返回新线程句柄.
	//如果失败,返回NULL.
    if ((hThread=CreateRemoteThread(hProcess, 0, 0, (LPTHREAD_START_ROUTINE)codebase, database, 0, 0))== NULL)
    {
        VirtualFreeEx(hProcess, database, sizeof(thread_param), MEM_FREE);
        VirtualFreeEx(hProcess, codebase, 0x1000, MEM_FREE);
        CloseHandle(hThread);
        CloseHandle(hProcess);
        return 1;
    }
    VirtualFreeEx(hProcess, database, sizeof(thread_param), MEM_FREE);
    VirtualFreeEx(hProcess, codebase, 0x1000, MEM_FREE);
    CloseHandle(hProcess);
    return 0;
}

DWORD getTargetProcessId()
{

    PROCESSENTRY32W pe32;
    pe32.dwSize=sizeof(PROCESSENTRY32W);
    HANDLE snap=CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);
    DWORD result=Process32FirstW(snap,&pe32);
    if(result==0)
    {

        return 0;
    }
    do 
    {
        if (lstrcmpW(pe32.szExeFile,L"Test001.exe")==0)
        {
            CloseHandle(snap);
            return pe32.th32ProcessID;
        }
    } while (Process32NextW(snap,&pe32));
    CloseHandle(snap);
    return 0;
}

#if 0

BOOL EnablePriv()

{

	HANDLE hToken;

	if (OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES, &hToken) )

	{

		  TOKEN_PRIVILEGES tkp;

		  LookupPrivilegeValue( NULL,SE_DEBUG_NAME,&tkp.Privileges[0].Luid );//修改进程权限

		  tkp.PrivilegeCount=1;

		  tkp.Privileges[0].Attributes=SE_PRIVILEGE_ENABLED;

		  AdjustTokenPrivileges( hToken,FALSE,&tkp,sizeof tkp,NULL,NULL );//通知系统修改进程权限

		  return( (GetLastError()==ERROR_SUCCESS) );

	}

      return TRUE;

}
void AdjustPrivileges()    
{    
        BOOL retn;    
        HANDLE hToken;    
        retn = OpenProcessToken(GetCurrentProcess(),TOKEN_ADJUST_PRIVILEGES,&hToken);    
        if(retn != TRUE)    
        {    
                    printf("获取令牌句柄失败!\n");
                    return;    
        }    
       
        TOKEN_PRIVILEGES tp; //新特权结构体    
        LUID Luid;    
        retn = LookupPrivilegeValue(NULL,SE_DEBUG_NAME,&Luid);    
     
        if(retn != TRUE)    
        {    
			printf("获取Luid失败\n");
                    return;    
        }    
                //给TP和TP里的LUID结构体赋值    
        tp.PrivilegeCount = 1;    
        tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;    
        tp.Privileges[0].Luid = Luid;    
          
        AdjustTokenPrivileges(hToken,FALSE,&tp,sizeof(TOKEN_PRIVILEGES),NULL,NULL);    
        if(GetLastError() != ERROR_SUCCESS)    
        {    
			printf("修改特权不完全或失败!\n");
        }    
       else  
       {    
		   printf("修改成功!\n");
       }    
}
#endif

BOOL AdjustProcessPrivilege(HANDLE hProcess, LPCTSTR lpPrivilegeName, DWORD dwPrivilegeAttribute)
{
 BOOL bRetValue = FALSE;
 HANDLE hProcessToken;
 
 //如果hProcess是NULL，说明调用者想要调整当前进程的权限，使用GetCurrentProcess获得的进程句柄无需关闭
 HANDLE hOpenProcess = (hProcess != NULL) ? hProcess : GetCurrentProcess();
 //打开进程令牌，期望的权限为可以调整权限和查询，得到进程令牌句柄
 if(OpenProcessToken(hOpenProcess, TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hProcessToken) != FALSE)
 {
  LUID stPrivilegeLuid;
  //通过权限名称，查找指定的权限的LUID值
  if(LookupPrivilegeValue(NULL, lpPrivilegeName, &stPrivilegeLuid) != FALSE)
  {
   //设置新的权限
   TOKEN_PRIVILEGES stNewTokenPrivilege;
   stNewTokenPrivilege.PrivilegeCount = 1;
   stNewTokenPrivilege.Privileges[0].Luid = stPrivilegeLuid;
   stNewTokenPrivilege.Privileges[0].Attributes = dwPrivilegeAttribute;
   //调整权限
   if(AdjustTokenPrivileges(hProcessToken, FALSE, &stNewTokenPrivilege, sizeof(stNewTokenPrivilege), NULL, NULL) != FALSE)
   {
    bRetValue = TRUE;
   }
  }
  //关闭进程令牌句柄
  CloseHandle(hProcessToken);
 }
 return bRetValue;
}





int wmain(int argc, WCHAR* argv[])
{
    setlocale(LC_ALL,"chs");
    DWORD pid;
	//AdjustPrivileges();  //ignore
	//if(EnablePriv() == FALSE){
	//	printf("EnablePriv failed!\n");  //ignore
	//}
	if(AdjustProcessPrivilege((HANDLE)NULL, SE_DEBUG_NAME, SE_PRIVILEGE_ENABLED) == FALSE ||
	  AdjustProcessPrivilege((HANDLE)NULL, SE_TAKE_OWNERSHIP_NAME, SE_PRIVILEGE_ENABLED) == FALSE ||
	  AdjustProcessPrivilege((HANDLE)NULL, SE_SECURITY_NAME, SE_PRIVILEGE_ENABLED) == FALSE ||
	  AdjustProcessPrivilege((HANDLE)NULL, SE_AUDIT_NAME, SE_PRIVILEGE_ENABLED) == FALSE)
	 {
	  printf(("Adjust process privilege failed!/n"));
	 }
    pid=getTargetProcessId();
    if (pid==0)
    {
        printf("getTargetProcessId error code is %d\n",GetLastError());
        return 0;
    }
    codeInject(pid);
	getchar();
    return 0;
}




