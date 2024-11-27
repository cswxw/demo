// test success

#include <windows.h>
#include <TlHelp32.h>
#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <tchar.h>

BOOL AdjustProcessPrivilege(HANDLE hProcess, LPCTSTR lpPrivilegeName, DWORD dwPrivilegeAttribute);

typedef struct _SYSTEM_HANDLE
{
 ULONG  uIdProcess;
 UCHAR  ObjectType;    // OB_TYPE_* (OB_TYPE_TYPE, etc.)
 UCHAR  Flags;         // HANDLE_FLAG_* (HANDLE_FLAG_INHERIT, etc.)
 USHORT  Handle;
 PVOID  pObject;
 ACCESS_MASK GrantedAccess;
} SYSTEM_HANDLE, *PSYSTEM_HANDLE;

 

typedef struct _SYSTEM_HANDLE_INFORMATION
{
 ULONG   uCount;
 SYSTEM_HANDLE aSH[];
} SYSTEM_HANDLE_INFORMATION, *PSYSTEM_HANDLE_INFORMATION;

/************************************************************************************************/

typedef enum _SYSTEMINFOCLASS
{
 SystemBasicInformation,             // 0x002C
 SystemProcessorInformation,         // 0x000C
 SystemPerformanceInformation,       // 0x0138
 SystemTimeInformation,              // 0x0020
 SystemPathInformation,              // not implemented
 SystemProcessInformation,           // 0x00C8+ per process
 SystemCallInformation,              // 0x0018 + (n * 0x0004)
 SystemConfigurationInformation,     // 0x0018
 SystemProcessorCounters,            // 0x0030 per cpu
 SystemGlobalFlag,                   // 0x0004 (fails if size != 4)
 SystemCallTimeInformation,          // not implemented
 SystemModuleInformation,            // 0x0004 + (n * 0x011C)
 SystemLockInformation,              // 0x0004 + (n * 0x0024)
 SystemStackTraceInformation,        // not implemented
 SystemPagedPoolInformation,         // checked build only
 SystemNonPagedPoolInformation,      // checked build only
 SystemHandleInformation,            // 0x0004  + (n * 0x0010)
 SystemObjectTypeInformation,        // 0x0038+ + (n * 0x0030+)
 SystemPageFileInformation,          // 0x0018+ per page file
 SystemVdmInstemulInformation,       // 0x0088
 SystemVdmBopInformation,            // invalid info class
 SystemCacheInformation,             // 0x0024
 SystemPoolTagInformation,           // 0x0004 + (n * 0x001C)
 SystemInterruptInformation,         // 0x0000, or 0x0018 per cpu
 SystemDpcInformation,               // 0x0014
 SystemFullMemoryInformation,        // checked build only
 SystemLoadDriver,                   // 0x0018, set mode only
 SystemUnloadDriver,                 // 0x0004, set mode only
 SystemTimeAdjustmentInformation,    // 0x000C, 0x0008 writeable
 SystemSummaryMemoryInformation,     // checked build only
 SystemNextEventIdInformation,       // checked build only
 SystemEventIdsInformation,          // checked build only
 SystemCrashDumpInformation,         // 0x0004
 SystemExceptionInformation,         // 0x0010
 SystemCrashDumpStateInformation,    // 0x0004
 SystemDebuggerInformation,          // 0x0002
 SystemContextSwitchInformation,     // 0x0030
 SystemRegistryQuotaInformation,     // 0x000C
 SystemAddDriver,                    // 0x0008, set mode only
 SystemPrioritySeparationInformation,// 0x0004, set mode only
 SystemPlugPlayBusInformation,       // not implemented
 SystemDockInformation,              // not implemented
 SystemPowerInfo,             // 0x0060 (XP only!)
 SystemProcessorSpeedInformation,    // 0x000C (XP only!)
 SystemTimeZoneInformation,          // 0x00AC
 SystemLookasideInformation,         // n * 0x0020
 SystemSetTimeSlipEvent,
 SystemCreateSession,    // set mode only
 SystemDeleteSession,    // set mode only
 SystemInvalidInfoClass1,   // invalid info class
 SystemRangeStartInformation,   // 0x0004 (fails if size != 4)
 SystemVerifierInformation,
 SystemAddVerifier,
 SystemSessionProcessesInformation, // checked build only
 MaxSystemInfoClass
} SYSTEMINFOCLASS, *PSYSTEMINFOCLASS;


typedef enum _OBJECT_INFORMATION_CLASS {
	ObjectBasicInformation = 0,
	ObjectNameInformation=1,
	ObjectTypeInformation = 2
} OBJECT_INFORMATION_CLASS;




typedef enum _FILE_INFORMATION_CLASS {
 FileDirectoryInformation=1,
 FileFullDirectoryInformation,
 FileBothDirectoryInformation,
 FileBasicInformation,
 FileStandardInformation,
 FileInternalInformation,
 FileEaInformation,
 FileAccessInformation,
 FileNameInformation,
 FileRenameInformation,
 FileLinkInformation,
 FileNamesInformation,
 FileDispositionInformation,
 FilePositionInformation,
 FileFullEaInformation,
 FileModeInformation,
 FileAlignmentInformation,
 FileAllInformation,
 FileAllocationInformation,
 FileEndOfFileInformation,
 FileAlternateNameInformation,
 FileStreamInformation,
 FilePipeInformation,
 FilePipeLocalInformation,
 FilePipeRemoteInformation,
 FileMailslotQueryInformation,
 FileMailslotSetInformation,
 FileCompressionInformation,
 FileCopyOnWriteInformation,
 FileCompletionInformation,
 FileMoveClusterInformation,
 FileQuotaInformation,
 FileReparsePointInformation,
 FileNetworkOpenInformation,
 FileObjectIdInformation,
 FileTrackingInformation,
 FileOleDirectoryInformation,
 FileContentIndexInformation,
 FileInheritContentIndexInformation,
 FileOleInformation,
 FileMaximumInformation
} FILE_INFORMATION_CLASS, *PFILE_INFORMATION_CLASS;

typedef struct _IO_STATUS_BLOCK {
union {
NTSTATUS Status;
PVOID Pointer;
} DUMMYUNIONNAME;

ULONG_PTR Information;
} IO_STATUS_BLOCK, *PIO_STATUS_BLOCK;


typedef struct _FILE_NAME_INFORMATION {
 ULONG  FileNameLength;
 WCHAR  FileName[1];
} FILE_NAME_INFORMATION, *PFILE_NAME_INFORMATION;
typedef struct _NM_INFO
{
 HANDLE  hFile;
 FILE_NAME_INFORMATION Info;
 WCHAR Name[MAX_PATH];
} NM_INFO, *PNM_INFO;


typedef NTSTATUS (WINAPI *ZWQUERYSYSTEMINFORMATION)(unsigned long, PVOID, ULONG, PULONG);


NTSTATUS
ZwQuerySystemInformation(
 IN SYSTEMINFOCLASS SystemInformationClass,
 OUT PVOID SystemInformation,
 IN ULONG SystemInformationLength,
 OUT PULONG ReturnLength OPTIONAL
);


typedef NTSTATUS (WINAPI *ZWQUERYINFORMATIONFILE)(HANDLE, PIO_STATUS_BLOCK, PVOID, ULONG,
FILE_INFORMATION_CLASS);


NTSTATUS
ZwQueryInformationFile(
 IN HANDLE  FileHandle,
 OUT PIO_STATUS_BLOCK  IoStatusBlock,
  OUT PVOID  FileInformation,
 IN ULONG  Length,
 IN FILE_INFORMATION_CLASS  FileInformationClass
);
typedef struct _UNICODE_STRING {
	USHORT Length;   //有效字符串的长度（字节数）
	USHORT MaximumLength; //字符串的最大长度（字节数）
	PWSTR Buffer; //指向字符串的指针
}UNICODE_STRING,*PUNICODE_STRING;

typedef struct _OBJECT_NAME_INFORMATION {
  UNICODE_STRING Name;
} OBJECT_NAME_INFORMATION, *POBJECT_NAME_INFORMATION;


typedef long(__stdcall*PNtQueryObject)(HANDLE ObjectHandle,ULONG ObjectInformationClass,PVOID ObjectInformation,ULONG ObjectInformationLength,PULONG ReturnLength);


HANDLE hHeap;
/************************************************************************************************/

DWORD WINAPI GetFileNameThread(PVOID lpParameter)
{
 PNM_INFO NmInfo = (PNM_INFO)lpParameter;
 IO_STATUS_BLOCK IoStatus;
 HMODULE hNtDLL = LoadLibrary(_T("NTDLL.DLL"));
 if ( !hNtDLL )
 {
  return FALSE;
 }
 ZWQUERYINFORMATIONFILE ZwQueryInformationFile = (ZWQUERYINFORMATIONFILE)GetProcAddress(hNtDLL, _T("ZwQueryInformationFile"));
 if( ZwQueryInformationFile == NULL)
 {
  return FALSE;
 }
 ZwQueryInformationFile(NmInfo->hFile, &IoStatus, &NmInfo->Info, sizeof(NM_INFO) - sizeof(HANDLE), FileNameInformation);
 return 0;
}

void GetFileName(HANDLE hFile, PCHAR TheName)
{
 HANDLE   hThread;
 PNM_INFO Info = (PNM_INFO)HeapAlloc(hHeap, 0, sizeof(NM_INFO));
 Info->hFile = hFile;
 hThread = CreateThread(NULL, 0, GetFileNameThread, Info, 0, NULL);
 if (WaitForSingleObject(hThread, INFINITE) == WAIT_TIMEOUT) TerminateThread(hThread, 0);
 CloseHandle(hThread);
 memset(TheName, 0, MAX_PATH);
 WideCharToMultiByte(CP_ACP, 0, Info->Info.FileName, Info->Info.FileNameLength >> 1, TheName, MAX_PATH, NULL, NULL);
 HeapFree(hHeap, 0, Info);
}

#define STATUS_INFO_LENGTH_MISMATCH 0xC0000004
    #define NT_SUCCESS(Status) (((NTSTATUS)(Status)) >= 0)  
PVOID GetInfoTable(IN ULONG ATableType)
{
 ULONG    mSize = 0x8000;
 PVOID    mPtr;
 NTSTATUS status;
 do
 {
  mPtr = HeapAlloc(hHeap, 0, mSize);
  if (!mPtr) return NULL;
  memset(mPtr, 0, mSize);
  
  HMODULE hNtDLL = LoadLibrary(_T("NTDLL.DLL"));
  if ( !hNtDLL )
  {
   return FALSE;
  }
  
  ZWQUERYSYSTEMINFORMATION ZwQuerySystemInformation = (ZWQUERYSYSTEMINFORMATION)GetProcAddress(hNtDLL, _T("ZwQuerySystemInformation"));
  if( ZwQuerySystemInformation == NULL)
  {
   return FALSE;
  }
 
  status = ZwQuerySystemInformation(ATableType, mPtr, mSize, NULL);
  
  if (status == STATUS_INFO_LENGTH_MISMATCH)
  {
   HeapFree(hHeap, 0, mPtr);
   mSize = mSize * 2;
  }
 } while (status == STATUS_INFO_LENGTH_MISMATCH);
 if (NT_SUCCESS(status)) return mPtr;
 HeapFree(hHeap, 0, mPtr);
 return NULL;
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

void MyCloseHandle(DWORD pid)
{
 //获取进程中的句柄
 PSYSTEM_HANDLE_INFORMATION Info;
 ULONG r;
 CHAR Name[MAX_PATH];
 HANDLE hProcess, hFile;
 hHeap = GetProcessHeap();
 Info = (PSYSTEM_HANDLE_INFORMATION)GetInfoTable(SystemHandleInformation);
 if (Info)
 {
  for (r = 0; r < Info->uCount; r++)
  {
   if (Info->aSH[r].uIdProcess == pid)
   {
    hProcess = OpenProcess(PROCESS_DUP_HANDLE, FALSE, Info->aSH[r].uIdProcess);
    if (hProcess)
    {
     if (DuplicateHandle(hProcess, (HANDLE)Info->aSH[r].Handle, GetCurrentProcess(), &hFile, 0, FALSE, DUPLICATE_SAME_ACCESS))
     {
      GetFileName(hFile, Name);
	  printf("Name:%s\n",Name);
      if(strstr(Name, "mutextest001") != NULL)
      {
       printf("PID=%d FileHandle %d FileName=%s\n", Info->aSH[r].uIdProcess, Info->aSH[r].Handle, Name);
      }
      
      CloseHandle(hFile);
     }
     CloseHandle(hProcess);
    }
   }
  }
  HeapFree(hHeap, 0, Info);
 }
 printf("Duplicate Finish.\n");
}




DWORD WINAPI t1(){
	wchar_t Mutex[]= L"mutextest001";
	DWORD dwSize = 0;
	SYSTEM_HANDLE* pmodule = NULL;
	POBJECT_NAME_INFORMATION pNameInfo;
	POBJECT_NAME_INFORMATION pNameType;
	PVOID pbuffer = NULL;
	NTSTATUS Status;
	int nIndex = 0;
	DWORD dwFlags = 0;
	char szType[128] = {0};
	char szName[512] = {0};
     

  HMODULE hNtDLL = LoadLibrary(_T("NTDLL.DLL"));
  if ( !hNtDLL )
  {
   return FALSE;
  }
  PNtQueryObject NtQueryObject = (PNtQueryObject)GetProcAddress(hNtDLL,_T("NtQueryObject"));
    if(NtQueryObject==NULL)
    {
        printf("NtQueryObject失败!\n");
        return 0;
    }
	ZWQUERYSYSTEMINFORMATION ZwQuerySystemInformation = (ZWQUERYSYSTEMINFORMATION)GetProcAddress(hNtDLL, _T("ZwQuerySystemInformation"));

    if(ZwQuerySystemInformation==NULL)
    {
        printf("ZwQuerySystemInformation失败!\n");
        return 0;
    }
#if 0

   //调用函数，获取进程信息
    Status = ZwQuerySystemInformation(
                                                    SystemHandleInformation,   //获取进程信息,宏定义为5
                                                    NULL,
                                                    0,
                                                    &dwSize  //返回的长度，即为我们需要申请的缓冲区的长度
                                                    );
    if(!dwSize)
    {
            printf("ZwQuerySystemInformation error!\n");  
            return Status;
    }
    printf("retLength =  %u\n",dwSize);
    //申请空间
    pbuffer = malloc(dwSize);
    if(!pbuffer)
    {
            printf("malloc error!\n");  
            return -1; 
    }
    Status = ZwQuerySystemInformation(
                                    SystemHandleInformation,   //获取进程信息,宏定义为5
                                    pbuffer,
                                    dwSize,
                                    &dwSize
                                    );
    if(NT_SUCCESS(Status)!=NULL/*STATUS_INFO_LENGTH_MISMATCH == nStatus*/)
    {
		free(pbuffer);
		printf("ZwQuerySystemInformation failed %d\n",Status);
		return -1;
	}
#else
    //MEM_COMMIT 为指定地址空间提交物理内存。这个函数初始化内在为零
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
#endif
    pmodule = (SYSTEM_HANDLE*)((PULONG)pbuffer+1); //PSYSTEM_HANDLE_INFORMATION
    dwSize = *((PULONG)pbuffer);
    for(nIndex = 0; nIndex < dwSize; nIndex++)
    {
         
        Status = NtQueryObject((HANDLE)pmodule[nIndex].Handle, ObjectNameInformation, szName, 512, &dwFlags);
         
        if (NT_SUCCESS(Status)==NULL)
        {
            printf("Status name失败!\n");
            continue;
        }
        Status = NtQueryObject((HANDLE)pmodule[nIndex].Handle, ObjectTypeInformation, szType, 128, &dwFlags);
         
        if (NT_SUCCESS(Status)==NULL)
        {
            printf("Status type失败!\n");
            //return 0;
			continue;
        }
        pNameInfo = (POBJECT_NAME_INFORMATION)szName;
        pNameType = (POBJECT_NAME_INFORMATION)szType;
        //GetDlgItem(IDC_EDIT1)->SetWindowText(szType);
        //GetDlgItem(IDC_EDIT2)->SetWindowText(szName);
		printf("Type: %ls\n",pNameType->Name.Buffer);
		printf("Name: %ls\n",pNameInfo->Name.Buffer);
		
       if (0 == wcscmp((wchar_t *)pNameType->Name.Buffer, L"Mutant"))
        {
            if (wcsstr((wchar_t *)pNameInfo->Name.Buffer, Mutex))
            {
                CloseHandle((HANDLE)pmodule[nIndex].Handle);
                printf("close ok!\n");
                break;
            }
        }

    } 
    if(nIndex == dwSize)
        printf("关闭互斥句柄失败!\n"); 
#if 0
	free(pbuffer);
#else
    if (NULL != pbuffer)
    {
        VirtualFree(pbuffer, 0, MEM_RELEASE);
    }
#endif
}



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


int create()
{
    HANDLE m_hMutex = CreateMutex(NULL,TRUE,"mutextest001");
    DWORD dwRet = GetLastError();
    if (m_hMutex)
    {
        if (ERROR_ALREADY_EXISTS == dwRet)
        {
            printf("Mutex already exist!\n");
            CloseHandle(m_hMutex);
            return 0;
        }
    }
    else
    {
        printf("CreateMutex failed!\n");
        CloseHandle(m_hMutex);
        return 0;
    }
	return 1;
}

int main(){
	if(AdjustProcessPrivilege((HANDLE)NULL, SE_DEBUG_NAME, SE_PRIVILEGE_ENABLED) == FALSE ||
		AdjustProcessPrivilege((HANDLE)NULL, SE_TAKE_OWNERSHIP_NAME, SE_PRIVILEGE_ENABLED) == FALSE ||
		AdjustProcessPrivilege((HANDLE)NULL, SE_SECURITY_NAME, SE_PRIVILEGE_ENABLED) == FALSE ||
		AdjustProcessPrivilege((HANDLE)NULL, SE_AUDIT_NAME, SE_PRIVILEGE_ENABLED) == FALSE)
	{
		printf(("Adjust process privilege failed!/n"));
	}
	printf("第一次创建\n");
	if(create() == 1){
		printf("create success\n");
	}else{
		printf("create failed\n");
	}
	//MyCloseHandle(GetCurrentProcessId());
	t1();
	
	printf("第二次创建\n");

	if(create() == 1){
		printf("create success\n");
	}else{
		printf("create failed\n");
	}

	return 0;
}

