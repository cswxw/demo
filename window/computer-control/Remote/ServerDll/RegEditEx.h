#pragma once


int  
ReadRegEx(HKEY MainKey,LPCTSTR SubKey,LPCTSTR Vname,DWORD Type,
		  char *szData,LPBYTE szBytes,DWORD lbSize,int Mode);


//ȥ���ַ�������ǰ��Ŀո�
char *DelSpace(char *szData);
