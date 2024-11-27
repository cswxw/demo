
#define CRTDBG_MAP_ALLOC  
#include <stdlib.h>  
#include <crtdbg.h>  



#include <stdio.h>
#include <string.h>
#include <locale.h>
#include "../WG_M_LIB/MyMemory.h"
#pragma comment(lib,"../Debug/WG_M_LIB.lib")

void testIntList();
void testStrList();
void testDir();
void testSK();
void testPack();
void testBuf();
void testCurDir();
int main(){
	//检查程序内存泄露
	// 在入口函数一开始添加以下代码  
	_CrtDumpMemoryLeaks();  
	//在你运行程序的时候，会在第num次申请空间时断点。
	//_CrtSetBreakAlloc(198);   //num就是刚刚检测出来的内存泄露的地方大括号内的数字  
    _CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );  


	setlocale(LC_ALL, "chs");
	//testIntList();
	//testStrList();
	//testDir();
	//testSK();
	//testPack();
	//testBuf();
	testCurDir();




	getchar();
	return 1;
}
void testIntList(){
	LIST *o;
	UINT i=0;
	o = NewIntList(true);

	//插入数据
	for(i=0;i<10;i++){
		AddInt(o,i);
	}

	//插入不重复数据
	for(i=0;i<10;i++){
		AddInt(o,i);
	}
	Sort(o); // 排序
	for (i = 0;i < LIST_NUM(o);i++)
	{
		int *q = (int*)LIST_DATA(o, i);
		printf("%d: %d\n",i,*q);
	}
	i = 1;
	printf("Search:%x\n" ,Search(o, &i));



	ReleaseIntList(o);
}

void testStrList(){
	LIST *o;
	UINT i=0;
	o = NewList(CompareStr);

	for(i =0;i< 10;i++){
		char *s = CopyFormat("%d,this is a test!",10-i);
		Add(o, s);
	}

	//show
	for (i = 0;i < LIST_NUM(o);i++)
	{
		char *p = (char*)LIST_DATA(o, i);
		printf("%s\n",p);
	}
	// Sort
	Sort(o);

	//show
	for (i = 0;i < LIST_NUM(o);i++)
	{
		char *p = (char*)LIST_DATA(o, i);
		printf("%s\n",p);
	}

	FreeStrList(o);
}


void testDir(){
	wchar_t  dirname[]= L"D:\\swap\\backup\\";
	UINT i=0;


	DIRLIST* dir = Win32EnumDirExW(dirname, CompareDirListByName);

	
	// Files
	for (i = 0;i < dir->NumFiles;i++)
	{
		DIRENT *e = dir->File[i];
		wchar_t tmp[MAX_SIZE];

		ConbinePathW(tmp, sizeof(tmp), dirname, e->FileNameW);

		
		if (e->Folder == false)
		{
			wprintf(L"file: %ws \n", tmp);
		}else{
			wprintf(L"dir: %ws \n",  tmp);
		}
	}
	FreeDir(dir);


	{
		char dir[] = "e:";
		UINT i = 0;
		TOKEN_LIST * token_list = EnumDirWithSubDirs(dir,true);
		for (i = 0;i < token_list->NumTokens;i++){
			char s[MAX_SIZE];
			StrCpy(s, sizeof(s), token_list->Token[i]);
			Trim(s);
			if(MsIsDirectory(s)){
				printf("dir:%s\n",s);
			}else{
				printf("file:%s\n",s);
			}

		}
		FreeToken(token_list);
	}

}


void testSK(){
	SK* sk = NewSk();

	UINT i,total;
	total = 100;
	for(i =0;i< total;i++){
		char *s = CopyFormat("%d,this is a test!",i);
		Push(sk,s);
	}
	for(i =0;i< total;i++){
		char *s = (char *)Pop(sk);
		puts(s);
		free(s);
		
	}
	ReleaseSk(sk);	
}

void testPack(){
	char password[20]="123456";
	char out[20]={0};
	BUF *buf = NewBuf();
	PACK *p = NewPack();
	char *data= NULL;
	UINT size=0;
	//add
	PackAddBool(p, "is_accept_empty_password", true);
	PackAddStr(p, "method", "login");
	PackAddData(p, "data", password, sizeof(password));
	PackAddBuf(p, "Buffer", buf);
	PackAddInt(p, "error", 12);
	
	//get
	printf("bool: %s\n",PackGetBool(p, "is_accept_empty_password") == true?"true":"false");
	PackGetStr(p, "method", out, sizeof(out));
	printf("str: %s\n",out);
	size = PackGetDataSize(p, "data");
	data = (char*)ZeroMalloc(size + 8);
	PackGetData(p, "data", data);
	printf("data: %s\n",data);
	Free(data);
	printf("int: %d\n",PackGetInt(p,"error"));

	FreePack(p);
	FreeBuf(buf);
}

void testBuf(){
	//SendAll(s, b->Buf, b->Size, s->SecureMode);
	char str[]="this is a test!";
	char data[20]={0};
	BUF *b = NewBuf();
	WriteBuf(b, str, StrLen(str));
	SeekBuf(b, 0, 0);

	ReadBuf(b, data, sizeof(data));
	printf("data:%s\n",data);

	FreeBuf(b);
}

void testCurDir(){
	char *path = "D:\\systemp\\t2";
	char *path2 = "D:\\systemp\\t2\\1";
	char *path3 = "D:\\systemp\\t2\\1\\2";
	//char *path4 = "D:\\s\\t2\\2\\3\\4\\5\\6\\7\\8\\9\\10";
	//MsMakeDirEx(path4);

	Win32EnumDirEx(path,NULL);


	SetCurrentDirectoryA(path);
	printf(" %d \n",Win32IsFileExists("1.txt"));
	SetCurrentDirectoryA(path2);
	printf(" %d \n",Win32IsFileExists("2.txt"));

	SetCurrentDirectoryA(path3);
	printf(" %d \n",Win32IsFileExists("3.txt"));

	printf(" %d \n",Win32IsFileExists("4.txt"));
}