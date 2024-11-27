#ifndef __MY_MEMORY_H__
#define __MY_MEMORY_H__

#include <windows.h>


// MallocFast (not implemented)
#define	MallocFast		Malloc
#define	OBJECT_ALLOC__MAX_RETRY				30

#define	OBJECT_ALLOC_FAIL_SLEEP_TIME		150

// Return 'a' greater than min_value
#define	MORE(a, min_value)	((a) >= (min_value) ? (a) : (min_value))
// Return 'a' less than max_value
#define	LESS(a, max_value)	((a) <= (max_value) ? (a) : (max_value))
// Adjust value 'a' to be between b and c
#define	MAKESURE(a, b, c)		(((b) <= (c)) ? (MORE(LESS((a), (c)), (b))) : (MORE(LESS((a), (b)), (c))))
// Minimum value of a and b
#define	MIN(a, b)			((a) >= (b) ? (b) : (a))
// Maximum value of a and b
#define	MAX(a, b)			((a) >= (b) ? (a) : (b))

// Compare bool type values
#define	EQUAL_BOOL(a, b)	(((a) && (b)) || ((!(a)) && (!(b))))


// Compare a and b
#define COMPARE_RET(a, b)	(((a) == (b)) ? 0 : (((a) > (b)) ? 1 : -1))


// 64-bit integer type
typedef	unsigned long long	UINT64;
typedef signed long long	INT64;

typedef signed long long	time_64t;


/*  定义别名  */
#define WINDOWS_H
#ifndef	WINDOWS_H
typedef	unsigned int		UINT;
typedef	unsigned int		UINT32;
typedef	unsigned int		DWORD;
typedef	signed int			INT;
typedef	signed int			INT32;

typedef	int					UINT_PTR;
typedef	long				LONG_PTR;


typedef	unsigned int		BOOL;
#define	TRUE				1
#define	FALSE				0

#endif

// bool type



// 16bit integer type
typedef	unsigned short		WORD;
typedef	unsigned short		USHORT;
typedef	signed short		SHORT;

// 8bit integer type
typedef	unsigned char		BYTE;
typedef	unsigned char		UCHAR;

/************************************** List start ***********************************/ 
#define	INIT_NUM_RESERVED		32

// Macro
#define	LIST_DATA(o, i)		(((o) != NULL) ? ((o)->p[(i)]) : NULL)
#define	LIST_NUM(o)			(((o) != NULL) ? (o)->num_item : 0)

// Comparison function
typedef int (COMPARE)(void *p1, void *p2);


struct LIST
{
	//REF *ref;
	UINT num_item;   // 元素个数
	UINT num_reserved; //当前可以存放元素个数
	void **p;      //数据指针
	//LOCK *lock;
	COMPARE *cmp;
	bool sorted;
	UINT64 Param1;
};

// normal
LIST *NewList(COMPARE *cmp); //新建list
LIST *NewListFast(COMPARE *cmp);  // 新建list 和NewList相同  (no implement)
void ReleaseList(LIST *o);  //释放list
void Sort(LIST *o);    //排序 
void SortEx(LIST *o, COMPARE *cmp);//使用外部函数排序
void Add(LIST *o, void *p);
void Insert(LIST *o, void *p);//如何cmp 不为0，则按顺序插入数据
void InsertDistinct(LIST *o, void *p);  //插入元素 (Don't add if it already exists)
bool IsInList(LIST *o, void *p); //判断元素是否在列表中，直接比较
void AddDistinct(LIST *o, void *p); // Add an element to the list (Don't add if it already exists)
void SetSortFlag(LIST *o, bool sorted); // Setting the sort flag
bool Delete(LIST *o, void *p);//删除指定元素
void DeleteAll(LIST *o);  //删除所有元素
void * Search(LIST *o, void *target);//搜索元素

// int
void InsertInt(LIST *o, UINT i);
void InsertIntDistinct(LIST *o, UINT i);
void AddInt(LIST *o, UINT i);
void AddIntDistinct(LIST *o, UINT i);
void DelInt(LIST *o, UINT i);
void DelAllInt(LIST *o);
LIST *NewIntList(bool sorted);
void ReleaseIntList(LIST *o);
bool IsInList(LIST *o, void *p);

// int64
void InsertInt64(LIST *o, UINT64 i);
void InsertInt64Distinct(LIST *o, UINT64 i);
void AddInt64(LIST *o, UINT64 i);
void AddInt64Distinct(LIST *o, UINT64 i);
void DelInt64(LIST *o, UINT64 i);
LIST *NewInt64List(bool sorted);
void ReleaseInt64List(LIST *o);
bool IsInt64InList(LIST *o, UINT64 i);

// string
bool InsertStr(LIST *o, char *str);
void FreeStrList(LIST *o);

//array
void CopyToArray(LIST *o, void *p);
void *ToArrayEx(LIST *o, bool fast);
void *ToArray(LIST *o);

LIST *NewWindowList();
void FreeWindowList(LIST *o);
void AddWindow(LIST *o, HWND hWnd);
/************************************** List end ***********************************/ 

/************************************** Other start ***********************************/ 
void AbortExitEx(char *msg);
void *ZeroMallocFast(UINT size);
void *Malloc(UINT size);
void Free(void *p);
void SleepThread(UINT time);
UINT Cmp(void *p1, void *p2, UINT size);
USHORT Endian16(USHORT src);
UINT Endian32(UINT src);
UINT64 Endian64(UINT64 src);
USHORT Swap16(USHORT value);
UINT Swap32(UINT value);
UINT64 Swap64(UINT64 value);
void Zero(void *addr, UINT size);
void *ZeroMalloc(UINT size);
void ConvertPathW(wchar_t *path);
UINT64 SystemToUINT64(SYSTEMTIME *st);
time_t SystemToTime(SYSTEMTIME* st);
void UINT64ToSystem(SYSTEMTIME *st, UINT64 sec64);
UINT64 SafeTime64(UINT64 sec64);
void TimeToSystem(SYSTEMTIME *st, time_t t);
int CompareDirListByName(void *p1, void *p2);
bool GetEnv(char *name, char *data, UINT size);
bool GetEnvW(wchar_t *name, wchar_t *data, UINT size);
bool GetEnvW_ForWin32(wchar_t *name, wchar_t *data, UINT size,bool isUnicode);
void Win32Inc32(LONG *value);
void Win32Dec32(LONG *value);
int ComparePackName(void *p1, void *p2);
// Endian
bool CheckEndian();
bool IsBigEndian();
bool IsLittleEndian();
int CmpWindowList(void *p1, void *p2);
#pragma comment( lib,"winmm.lib" )
UINT Win32GetTick();
/************************************** Other end ***********************************/ 



/************************************** String start ***********************************/ 


// String token
struct UNI_TOKEN_LIST
{
	UINT NumTokens;
	wchar_t **Token;
};


// String token
struct TOKEN_LIST
{
	UINT NumTokens;
	char **Token;
};


#define	MAX_SIZE			512
#define	BUF_SIZE			512

#define	MIN(a, b)			((a) >= (b) ? (b) : (a))
#define	MAX(a, b)			((a) >= (b) ? (a) : (b))

UINT StrLen(char *str);
void Copy(void *dst, void *src, UINT size);
UINT HexTo4Bit(char c);
UINT HexToInt(char *str);
void ToHex64(char *str, UINT64 value);
char FourBitToHex(UINT value);
char ToLower(char c);
char ToUpper(char c);
int StrCmpi(char *str1, char *str2);
int StrCmp(char *str1, char *str2);
UINT StrCpy(char *dst, UINT size, char *src);
int CompareStr(void *p1, void *p2);
char *CopyStr(char *str);
UINT StrSize(char *str);
char *CopyFormat(char *fmt, ...);
wchar_t *CopyStrToUni(char *str);
UINT CalcStrToUni(char *str);
wchar_t *CopyUniStr(wchar_t *str);
UINT UniStrCpy(wchar_t *dst, UINT size, wchar_t *src);
UINT StrToUni(wchar_t *s, UINT size, char *str);
UINT UniToStr(char *str, UINT size, wchar_t *s);
UINT CalcUniToStr(wchar_t *s);
void UniFormat(wchar_t *buf, UINT size, wchar_t *fmt, ...);
int UniStrCmpi(wchar_t *str1, wchar_t *str2);
wchar_t UniToUpper(wchar_t c);
wchar_t *UniCopyStr(wchar_t *str);
wchar_t *CopyUniStr(wchar_t *str);
char *CopyUniToStr(wchar_t *unistr);
bool UniStartWith(wchar_t *str, wchar_t *key);
bool UniStartWith(wchar_t *str, wchar_t *key);
UINT UniStrCat(wchar_t *dst, UINT size, wchar_t *src);
void UniTrim(wchar_t *str);
void UniTrimLeft(wchar_t *str);
void UniTrimRight(wchar_t *str);

void Trim(char *str);
void TrimRight(char *str);
void TrimLeft(char *str);

UNI_TOKEN_LIST *UniParseToken(wchar_t *src, wchar_t *separator);
void UniFreeToken(UNI_TOKEN_LIST *tokens);
void FreeToken(TOKEN_LIST *tokens);
TOKEN_LIST *UniTokenListToTokenList(UNI_TOKEN_LIST *src);
UINT UniStrSize(wchar_t *str);
UINT UniStrLen(wchar_t *str);
UINT CalcUtf8ToUni(BYTE *u, UINT u_size);
UINT Utf8Len(BYTE *u, UINT size);
UINT UniToUtf8(BYTE *u, UINT size, wchar_t *s);
UINT GetUtf8Type(BYTE *s, UINT size, UINT offset);
UINT GetUniType(wchar_t c);
UINT Utf8ToUni(wchar_t *s, UINT size, BYTE *u, UINT u_size);
UINT CalcUniToUtf8(wchar_t *s);
bool IsEmptyStr(char *str);
/************************************** String end ***********************************/ 



/************************************** Base64 start ***********************************/ 

UINT Encode64(char *dst, char *src);
int B64_Encode(char *set, char *source, int len);
char B64_CodeToChar(BYTE c);

UINT Decode64(char *dst, char *src);
int B64_Decode(char *set, char *source, int len);
char B64_CharToCode(char c);


/************************************** Base64 end ***********************************/ 



/************************************** Buf start ***********************************/ 
#define	MEMORY_SLEEP_TIME		150
#define	MEMORY_MAX_RETRY		30
#define	INIT_BUF_SIZE			10240


// Buffer
struct BUF
{
	void *Buf;    //存放的数据地址
	UINT Size;       //存放数据大小 
	UINT SizeReserved;   //分配空间大小
	UINT Current;    //当前索引位置
};

BUF *NewBuf();
void FreeBuf(BUF *b);
void ClearBuf(BUF *b);
bool CompareBuf(BUF *b1, BUF *b2);
void WriteBuf(BUF *b, void *buf, UINT size);
UINT ReadBuf(BUF *b, void *buf, UINT size);
void AdjustBufSize(BUF *b, UINT new_size);
void SeekBuf(BUF *b, UINT offset, int mode);
void SeekBufToBegin(BUF *b);
void SeekBufToEnd(BUF *b);

//char
UCHAR ReadBufChar(BUF *b);
bool WriteBufChar(BUF *b, UCHAR uc);
//int
bool WriteBufInt(BUF *b, UINT value);
UINT ReadBufInt(BUF *b);
//int64
UINT64 ReadBufInt64(BUF *b);
bool WriteBufInt64(BUF *b, UINT64 value);
//string
bool WriteBufStr(BUF *b, char *str);
bool ReadBufStr(BUF *b, char *str, UINT size);
void AddBufStr(BUF *b, char *str);
void WriteBufLine(BUF *b, char *str);

/************************************** Buf end ***********************************/

/************************************** File start ***********************************/



#if 0
HANDLE FileCreate(char *name);
HANDLE FileOpen(char *name, bool write_mode);
void FileClose(HANDLE h);
bool FileRead(HANDLE h, void *buf, UINT size);
bool FileWrite(HANDLE h, void *buf, UINT size);
UINT64 FileSize(HANDLE h);
#else
//unicode
// IO structure
struct IO
{
	char Name[MAX_SIZE];
	wchar_t NameW[MAX_SIZE];
	void *pData;
	bool WriteMode;
	bool HamMode;
	BUF *HamBuf;
	UINT64 SetUpdateTime, SetCreateTime;
	UINT64 GetUpdateTime, GetCreateTime, GetAccessTime;
};
typedef struct IO IO;

// File I/O data for Win32
typedef struct WIN32IO
{
	HANDLE hFile;
	bool WriteMode;
} WIN32IO;

// DIRENT structure
struct DIRENT
{
	bool Folder;				// Folder
	char *FileName;				// File name (ANSI)
	wchar_t *FileNameW;			// File name (Unicode)
	UINT64 FileSize;			// File size
	UINT64 CreateDate;			// Creation Date
	UINT64 UpdateDate;			// Updating date
};
typedef DIRENT DIRENT;

// DIRLIST structure

struct DIRLIST
{
	UINT NumFiles;				// Number of files
	struct DIRENT **File;			// File array
};


struct ENUM_DIR_WITH_SUB_DATA
{
	LIST *FileList;
};


IO *FileCreate(char *name);
IO *FileCreateW(wchar_t *name);
IO *FileCreateInnerW(wchar_t *name);
bool FileWrite(IO *o, void *buf, UINT size);

void *OSFileCreateW(wchar_t *name);
bool OSFileWrite(void *pData, void *buf, UINT size);
#endif


//win32 start
void *Win32MemoryAlloc(UINT size);
void *Win32MemoryReAlloc(void *addr, UINT size);
void Win32MemoryFree(void *addr);
//win32 file
void *Win32FileCreate(char *name);
void *Win32FileCreateW(wchar_t *name);
void *Win32FileOpen(char *name, bool write_mode, bool read_lock);
void *Win32FileOpenW(wchar_t *name, bool write_mode, bool read_lock);
bool Win32FileWrite(void *pData, void *buf, UINT size);
bool Win32FileRead(void *pData, void *buf, UINT size);
void Win32FileClose(void *pData, bool no_flush);
bool Win32FileDelete(char *name);
bool Win32FileDeleteW(wchar_t *name);
bool Win32FileSeek(void *pData, UINT mode, int offset);
bool Win32FileGetDate(void *pData, UINT64 *created_time, UINT64 *updated_time, UINT64 *accessed_date);
bool Win32FileSetDate(void *pData, UINT64 created_time, UINT64 updated_time);
void Win32FileFlush(void *pData);
UINT64 Win32FileSize(void *pData);
bool Win32IsFileExists(char *name);
bool Win32FileExistsW(wchar_t *name);
//win32 dir
bool Win32DeleteDir(char *name);
bool Win32DeleteDirW(wchar_t *name);
bool Win32MakeDir(char *name);
bool Win32MakeDirW(wchar_t *name);
void Win32GetCurrentDir(char *dir, UINT size);
void Win32GetCurrentDirW(wchar_t *dir, UINT size);
DIRLIST *Win32EnumDirEx(char *dirname, COMPARE *compare);
DIRLIST *Win32EnumDirExW(wchar_t *dirname, COMPARE *compare);
//win32 priority
void Win32SetHighPriority();
void Win32RestorePriority();
//win32 end

void ConbinePathW(wchar_t *dst, UINT size, wchar_t *dirname, wchar_t *filename);
void NormalizePathW(wchar_t *dst, UINT size, wchar_t *src);
void FreeDir(DIRLIST *d);
void GetHomeDirW(wchar_t *path, UINT size);
TOKEN_LIST *EnumDirWithSubDirs(char *dirname, bool isAddDir);
UNI_TOKEN_LIST *EnumDirWithSubDirsW(wchar_t *dirname,bool isAddDir);
void EnumDirWithSubDirsMain(ENUM_DIR_WITH_SUB_DATA *d, wchar_t *dirname,bool isAddDir);
DIRLIST *EnumDirExW(wchar_t *dirname, COMPARE *compare);
bool MsIsDirectory(char *name);
bool MsIsDirectoryW(wchar_t *name);
bool MsUniIsDirectory(wchar_t *name,bool isUnicode);
bool MsFileDelete(char *name);
bool MsUniFileDelete(wchar_t *name,bool isUnicode);
bool MsDirectoryDelete(char *name);
bool MsUniDirectoryDelete(wchar_t *name);
bool MsMakeDir(char *name);
bool MsUniMakeDir(wchar_t *name);
void MsMakeDirEx(char *name);
void MsMakeDirExW(wchar_t *name);
void MsUniMakeDirEx(wchar_t *name,bool isUnicode);

/************************************** File end ***********************************/


/************************************** stack start ***********************************/
// Stack
struct SK
{
	//REF *ref;
	UINT num_item;   //当前入栈个数
	UINT num_reserved;  //栈总个数
	void **p;       //存放数据
	//LOCK *lock;
	bool no_compact;  //当 栈过大,入栈个数过小 时，true 不压缩栈大小   false 压缩栈大小
};  //s->p[s->num_item - 1]; 指向最顶层元素，留一个空的位置

SK *NewSk();
SK *NewSkEx(bool no_compact);
void ReleaseSk(SK *s);
void CleanupSk(SK *s);
void *Pop(SK *s);
void Push(SK *s, void *p);

/************************************** stack end ***********************************/


/************************************** lock start ***********************************/
// Lock object
struct LOCK
{
	void *pData;
	BOOL Ready;

#ifdef	_DEBUG
	char *FileName;
	UINT Line;
	UINT ThreadId;
#endif	// _DEBUG
};

LOCK *Win32NewLock();
bool Win32Lock(LOCK *lock);
void Win32Unlock(LOCK *lock);
void Win32UnlockEx(LOCK *lock, bool inner);
void Win32DeleteLock(LOCK *lock);
LOCK *NewLock();
void DeleteLock(LOCK *lock);
LOCK *NewLockMain();

/************************************** lock end ***********************************/

/************************************** event start ***********************************/
// Event object
struct EVENT
{
	//REF *ref;
	void *pData;
};

void Win32InitEvent(EVENT *event);
void Win32SetEvent(EVENT *event);
void Win32ResetEvent(EVENT *event);
bool Win32WaitEvent(EVENT *event, UINT timeout);
void Win32FreeEvent(EVENT *event);

/************************************** event end ***********************************/


/************************************** pack start ***********************************/

#define	MAX_VALUE_SIZE			(384 * 1024 * 1024)	// Maximum Data size that can be stored in a single VALUE
#define	MAX_VALUE_NUM			262144	// Maximum VALUE number that can be stored in a single ELEMENT
#define	MAX_ELEMENT_NAME_LEN	63		// The length of the name that can be attached to the ELEMENT
#define	MAX_ELEMENT_NUM			262144	// Maximum ELEMENT number that can be stored in a single PACK
#define	MAX_PACK_SIZE			(512 * 1024 * 1024)	// Maximum size of a serialized PACK


// PACK object
struct PACK
{
	LIST *elements;			// Element list
};

// VALUE object
struct VALUE
{
	UINT Size;				// Size
	UINT IntValue;			// Integer value
	void *Data;				// Data
	char *Str;				// ANSI string
	wchar_t *UniStr;		// Unicode strings
	UINT64 Int64Value;		// 64 bit integer type
};

// ELEMENT object
struct ELEMENT
{
	char name[MAX_ELEMENT_NAME_LEN + 1];	// Element name
	UINT num_value;			// Number of values (>=1)
	UINT type;				// Type
	VALUE **values;			// List of pointers to the value
};

// Type of VALUE
#define	VALUE_INT			10		// Integer type
#define	VALUE_DATA			11		// Data type
#define	VALUE_STR			12		// ANSI string type
#define	VALUE_UNISTR		13		// Unicode string type
#define	VALUE_INT64			14		// 64 bit integer type

//pack
BUF *PackToBuf(PACK *p);
PACK *BufToPack(BUF *b);
PACK *NewPack();
bool ReadPack(BUF *b, PACK *p);
void WritePack(BUF *b, PACK *p);
void FreePack(PACK *p);
TOKEN_LIST *GetPackElementNames(PACK *p);

//pack cmp
bool PackCmpStr(PACK *p, char *name, char *str);

//pack get buf
BUF *PackGetBuf(PACK *p, char *name);
BUF *PackGetBufEx(PACK *p, char *name, UINT index);
//pack get data
bool PackGetData(PACK *p, char *name, void *data);
bool PackGetDataEx(PACK *p, char *name, void *data, UINT index);
bool PackGetData2(PACK *p, char *name, void *data, UINT size);
bool PackGetDataEx2(PACK *p, char *name, void *data, UINT size, UINT index);
UINT PackGetDataSize(PACK *p, char *name);
UINT PackGetDataSizeEx(PACK *p, char *name, UINT index);
//pack get int64
UINT64 PackGetInt64(PACK *p, char *name);
UINT64 PackGetInt64Ex(PACK *p, char *name, UINT index);
//pack get bool
bool PackGetBool(PACK *p, char *name);
bool PackGetBoolEx(PACK *p, char *name, UINT index);
//pack get int
UINT PackGetInt(PACK *p, char *name);
UINT PackGetIntEx(PACK *p, char *name, UINT index);
//pack get unicode str
bool PackGetUniStr(PACK *p, char *name, wchar_t *unistr, UINT size);
bool PackGetUniStrEx(PACK *p, char *name, wchar_t *unistr, UINT size, UINT index);
//pack get str
bool PackGetStr(PACK *p, char *name, char *str, UINT size);
bool PackGetStrEx(PACK *p, char *name, char *str, UINT size, UINT index);

UINT PackGetIndexCount(PACK *p, char *name);
UINT PackGetNum(PACK *p, char *name);

//pack add bool
void PackAddBool(PACK *p, char *name, bool b);
void PackAddBoolEx(PACK *p, char *name, bool b, UINT index, UINT total);
//pack add buf
void PackAddBuf(PACK *p, char *name, BUF *b);
void PackAddBufEx(PACK *p, char *name, BUF *b, UINT index, UINT total);
//pack add data
void PackAddData(PACK *p, char *name, void *data, UINT size);
void PackAddDataEx(PACK *p, char *name, void *data, UINT size, UINT index, UINT total);
//pack add int64
void PackAddInt64(PACK *p, char *name, UINT64 i);
void PackAddInt64Ex(PACK *p, char *name, UINT64 i, UINT index, UINT total);
//pack add int
void PackAddInt(PACK *p, char *name, UINT i);
void PackAddIntEx(PACK *p, char *name, UINT i, UINT index, UINT total);
//pack add unicode str
void PackAddUniStr(PACK *p, char *name, wchar_t *unistr);
void PackAddUniStrEx(PACK *p, char *name, wchar_t *unistr, UINT index, UINT total);
//pack add str
void PackAddStr(PACK *p, char *name, char *str);
void PackAddStrEx(PACK *p, char *name, char *str, UINT index, UINT total);

void PackAddNum(PACK *p, char *name, UINT num);

//element
ELEMENT *NewElement(char *name, UINT type, UINT num_value, VALUE **values);
ELEMENT *ReadElement(BUF *b);
void WriteElement(BUF *b, ELEMENT *e);
bool AddElement(PACK *p, ELEMENT *e);
ELEMENT *GetElement(PACK *p, char *name, UINT type);
void DelElement(PACK *p, char *name);
void FreeElement(ELEMENT *e);
//value
VALUE *NewIntValue(UINT i);
VALUE *NewInt64Value(UINT64 i);
VALUE *NewDataValue(void *data, UINT size);
VALUE *NewStrValue(char *str);
VALUE *NewUniStrValue(wchar_t *str);
VALUE *ReadValue(BUF *b, UINT type);
void WriteValue(BUF *b, VALUE *v, UINT type);
void FreeValue(VALUE *v, UINT type);

//data
UINT GetDataValueSize(ELEMENT *e, UINT index);
void *GetDataValue(ELEMENT *e, UINT index);
//int
UINT GetIntValue(ELEMENT *e, UINT index);
//int64
UINT64 GetInt64Value(ELEMENT *e, UINT index);
//unicode str
wchar_t *GetUniStrValue(ELEMENT *e, UINT index);
//str
char *GetStrValue(ELEMENT *e, UINT index);


/************************************** pack end ***********************************/


/************************************** mfc start ***********************************/
// Child window enumeration
typedef struct ENUM_CHILD_WINDOW_PARAM
{
	LIST *o;
	bool no_recursion;
	bool include_ipcontrol;
} ENUM_CHILD_WINDOW_PARAM;

typedef struct SEARCH_WINDOW_PARAM
{
	wchar_t *caption;
	HWND hWndFound;
} SEARCH_WINDOW_PARAM;


LIST *EnumAllChildWindow(HWND hWnd);
LIST *EnumAllChildWindowEx(HWND hWnd, bool no_recursion, bool include_ipcontrol, bool no_self);
BOOL CALLBACK EnumChildWindowProc(HWND hWnd, LPARAM lParam);
LIST *EnumAllWindow();
LIST *EnumAllWindowEx(bool no_recursion, bool include_ipcontrol);
LIST *EnumAllTopWindow();
BOOL CALLBACK EnumTopWindowProc(HWND hWnd, LPARAM lParam);
HWND SearchWindow(char *caption);
HWND SearchWindowW(wchar_t *caption);
BOOL CALLBACK SearchWindowEnumProcW(HWND hWnd, LPARAM lParam);
char *GetTextA(HWND hWnd, UINT id);
wchar_t *GetTextW(HWND hWnd, UINT id);
HWND DlgItem(HWND hWnd, UINT id);
void Center(HWND hWnd);


//list start
#include <CommCtrl.h>

void LvInsert(HWND hWnd, UINT id, UINT num_str , ...);
UINT LvNum(HWND hWnd, UINT id);
UINT LvGetSelected(HWND hWnd, UINT id);
void LvDeleteItem(HWND hWnd, UINT id, UINT index);
void LvSelect(HWND hWnd, UINT id, UINT index);
bool LvIsSelected(HWND hWnd, UINT id);
void LvRename(HWND hWnd, UINT id, UINT pos);
void LvSetItemA(HWND hWnd, UINT id, UINT index, UINT pos, char *str);
char *LvGetStrA(HWND hWnd, UINT id, UINT index, UINT pos);
//list end
/************************************** mfc end ***********************************/



/************************************** socket start ***********************************/
#if 1
#include <winsock.h>
#pragma comment(lib,"Ws2_32.lib")

#ifndef SOCKET
	#ifdef WIN32
		#define SOCKET unsigned int
	#else
		#define SOCKET int
	#endif
#endif
// Constant of the return value
#define	SOCK_LATER	(0xffffffff)	// In blocking
#ifndef WIN32
	#define	closesocket(s)		close(s)
#endif
// IP address
struct IP
{
	UCHAR addr[4];					// IPv4 address, (meaning that 223.255.255.254 = IPv6)
	UCHAR ipv6_addr[16];			// IPv6 address
	UINT ipv6_scope_id;				// IPv6 scope ID
};

// Type of socket
#define	SOCK_TCP				1
#define	SOCK_UDP				2
#define	SOCK_INPROC				3
#define	SOCK_RUDP_LISTEN		5
#define	SOCK_REVERSE_LISTEN		6


// Constant
#define	MD5_SIZE	16
#define	SHA1_SIZE	20
#define	SHA256_SIZE	32


// Memory size that can be passed to the kernel at a time
#define	MAX_SEND_BUF_MEM_SIZE				(10 * 1024 * 1024)

// Socket
struct SOCK
{
	LOCK *lock;					// Lock
	LOCK *ssl_lock;				// Lock related to the SSL
	LOCK *disconnect_lock;		// Disconnection lock
	SOCKET socket;				// Socket number
	//SSL *ssl;					// SSL object

	UINT Type;					// Type of socket
	bool Connected;				// Connecting flag
	bool ServerMode;			// Server mode
	bool AsyncMode;				// Asynchronous mode
	bool SecureMode;			// SSL communication mode
	bool ListenMode;			// In listening
	BUF *SendBuf;				// Transmission buffer
	bool IpClientAdded;			// Whether it has been added to the list IP_CLIENT
	bool LocalOnly;				// Only local
	bool EnableConditionalAccept;	// Conditional Accept is Enabled
	IP RemoteIP;				// IP address of the remote host
	IP LocalIP;					// IP address of the local host
	char *RemoteHostname;		// Remote host name
	UINT RemotePort;			// Port number of the remote side
	UINT LocalPort;				// Port number of the local side
	UINT64 SendSize;			// Total size of the sent data
	UINT64 RecvSize;			// Total size of received data
	UINT64 SendNum;				// Number of sent data blocks
	UINT64 RecvNum;				// Number of received data blocks

	char *CipherName;			// Cipher algorithm name
	char *WaitToUseCipher;		// Set the algorithm name to want to use
	bool IgnoreRecvErr;			// Whether the RecvFrom error is ignorable
	bool IgnoreSendErr;			// Whether the SendTo error is ignorable
	UINT TimeOut;				// Time-out value

	bool CancelAccept;			// Cancel flag of the Accept
	bool AcceptCanceled;		// Flag which shows canceling of the Accept
	bool WriteBlocked;			// Previous write is blocked
	bool NoNeedToRead;			// Is not required to read
	bool Disconnecting;			// Disconnecting
	bool UdpBroadcast;			// UDP broadcast mode
	void *Param;				// Any parameters
	bool IPv6;					// IPv6
	bool IsRawSocket;			// Whether it is a raw socket
	UINT RawSocketIPProtocol;	// IP protocol number if it's a raw socket
	
};


UINT SecureRecv(SOCK *sock, void *data, UINT size);
void Disconnect(SOCK *sock);
UINT Recv(SOCK *sock, void *data, UINT size, bool secure);
bool RecvAll(SOCK *sock, void *data, UINT size, bool secure);
PACK *RecvPack(SOCK *s);
void HashSha1(void *dst, void *src, UINT size);
PACK *RecvPackWithHash(SOCK *s);
void SendAdd(SOCK *sock, void *data, UINT size);
UINT SecureSend(SOCK *sock, void *data, UINT size);
UINT Send(SOCK *sock, void *data, UINT size, bool secure);
bool SendAll(SOCK *sock, void *data, UINT size, bool secure);
bool SendNow(SOCK *sock, int secure);
bool SendPack(SOCK *s, PACK *p);
SOCK *NewSock();
void CleanupSock(SOCK *s);
void ReleaseSock(SOCK *s);
UINT IPToUINT(IP *ip);
void SetIP6(IP *ip, UCHAR *value);
bool IsIP6(IP *ip);
bool IsIP4(IP *ip);
bool IsSameIPVer(IP *ip1, IP *ip2);
void ZeroIP6(IP *ip);
void ZeroIP4(IP *ip);
void UINTToIP(IP *ip, UINT value);


#endif
/************************************** socket end ***********************************/

#endif