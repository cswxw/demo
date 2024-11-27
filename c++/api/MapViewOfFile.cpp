void * test(wchar_t *path){
	HANDLE hFile;
	// Create a file
	hFile = CreateFileW(path, GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL,
		NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		hFile = CreateFileW(path, GENERIC_READ | GENERIC_WRITE,
			FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_HIDDEN,
			NULL);
		if (hFile == INVALID_HANDLE_VALUE)
		{
			return NULL;
		}
	}
   //创建一个文件映射内核对象;
    HANDLE hMapFile = CreateFileMappingW( hFile,
        NULL,
        PAGE_READWRITE,
        0,
        0,
        NULL ); 
    if(hMapFile == NULL)
    {
        return NULL;
    }

    //将文件数据映射到进程的地址空间;
    char * buf = (char *)MapViewOfFile( hMapFile, 
        FILE_MAP_ALL_ACCESS,
        0,
        0, 
        0); 

	//关闭句柄;
    //UnmapViewOfFile(buf); 
    //CloseHandle(hMapFile); 
    //CloseHandle(hFile); 
	return buf;
}

