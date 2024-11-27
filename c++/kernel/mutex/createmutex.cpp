#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
 
int main(int argc, char* argv[])
{
    HANDLE m_hMutex = CreateMutex(NULL,TRUE,L"mutextest001");
    DWORD dwRet = GetLastError();
    if (m_hMutex)
    {
        if (ERROR_ALREADY_EXISTS == dwRet)
        {
            printf("程序已经在运行中了,程序退出!\n");
            CloseHandle(m_hMutex);
            return 0;
        }
    }
    else
    {
        printf("创建互斥量错误,程序退出!\n");
        CloseHandle(m_hMutex);
        return 0;
    }
    getchar();
    CloseHandle(m_hMutex);
    return 0;
}

