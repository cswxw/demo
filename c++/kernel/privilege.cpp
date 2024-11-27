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





void test(){
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

}
