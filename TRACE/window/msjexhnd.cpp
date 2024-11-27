//==========================================
// Matt Pietrek
// Microsoft Systems Journal, April 1997
// FILE: MSJEXHND.CPP
//==========================================
#include "stdafx.h"
#include <tchar.h>
#include "msjexhnd.h"
/*
首先设置工程为Release编译，然后选中工程右键>属性，在c/c++>OutputFiles>Assembler Output选择Assembly, Machine Code and Source(/FAcs).
	这个选项将为每个源文件（*.cpp）生成机器码、汇编码和源代码的对应表，可以在“Release”目录下找到和查看这些文件
然后在Linker>Debugging>Generate Map File选择Yes(/MAP)，这个选项将生成编译后的函数地址和函数名的对应表。

错误eip 对应section范围，算出函数偏移到obj查找对应代码。
*/
//============================== Global Variables =============================
//
// Declare the static variables of the MSJExceptionHandler class
//
TCHAR MSJExceptionHandler::m_szLogFileName[MAX_PATH];
LPTOP_LEVEL_EXCEPTION_FILTER MSJExceptionHandler::m_previousFilter;
HANDLE MSJExceptionHandler::m_hReportFile;
MSJExceptionHandler g_MSJExceptionHandler;  // Declare global instance of class
//============================== Class Methods =============================
//=============
// Constructor 
//=============
#if 0
#include <DbgHelp.h>
#pragma comment(lib,"Dbghelp.lib")

void MSJExceptionHandler::printStack(void)
{
	unsigned int   i;
	void         * stack[100];
	unsigned short frames;
	SYMBOL_INFO  * symbol;
	HANDLE         process;

	process = GetCurrentProcess();

	SymInitialize(process, NULL, TRUE);

	frames = CaptureStackBackTrace(0, 100, stack, NULL);
	symbol = (SYMBOL_INFO *)calloc(sizeof(SYMBOL_INFO) + 256 * sizeof(char), 1);
	symbol->MaxNameLen = 255;
	symbol->SizeOfStruct = sizeof(SYMBOL_INFO);

	for (i = 0; i < frames; i++)
	{
		SymFromAddr(process, (DWORD64)(stack[i]), 0, symbol);

		_tprintf(_T("%i: %s - 0x%0X\n"), frames - i - 1, symbol->Name, symbol->Address);
	}

	free(symbol);
}
#endif

MSJExceptionHandler::MSJExceptionHandler()
{
	SetErrorMode(SEM_NOGPFAULTERRORBOX);
	// Install the unhandled exception filter function
	m_previousFilter = SetUnhandledExceptionFilter(MSJUnhandledExceptionFilter);
	// Figure out what the report file will be named, and store it away
	GetModuleFileName(0, m_szLogFileName, MAX_PATH);
	PTSTR pszDot = _tcsrchr(m_szLogFileName, _T('.'));
	if (pszDot)
	{
		pszDot++;   // Advance past the '.'
		if (_tcslen(pszDot) >= 3)
			_tcscpy(pszDot, _T("RPT"));   // "RPT" -> "Report"
	}
}
//============
// Destructor 
//============
MSJExceptionHandler::~MSJExceptionHandler()
{
	SetUnhandledExceptionFilter(m_previousFilter);
}
//==============================================================
// Lets user change the name of the report file to be generated 
//==============================================================
void MSJExceptionHandler::SetLogFileName(PTSTR pszLogFileName)
{
	_tcscpy(m_szLogFileName, pszLogFileName);
}
//===========================================================
// Entry point where control comes on an unhandled exception 
//===========================================================
LONG WINAPI MSJExceptionHandler::MSJUnhandledExceptionFilter(
	PEXCEPTION_POINTERS pExceptionInfo)
{
	m_hReportFile = CreateFile(m_szLogFileName,
		GENERIC_WRITE,
		0,
		0,
		OPEN_ALWAYS,
		FILE_FLAG_WRITE_THROUGH,
		0);
	if (m_hReportFile)
	{
		SetFilePointer(m_hReportFile, 0, 0, FILE_END);
		GenerateExceptionReport(pExceptionInfo);
		CloseHandle(m_hReportFile);
		m_hReportFile = 0;
	}

	if (m_previousFilter)
		return m_previousFilter(pExceptionInfo);
	else
		return EXCEPTION_CONTINUE_SEARCH;
}
//===========================================================================
// Open the report file, and write the desired information to it.  Called by 
// MSJUnhandledExceptionFilter                                               
//===========================================================================
void MSJExceptionHandler::GenerateExceptionReport(
	PEXCEPTION_POINTERS pExceptionInfo)
{
	// Start out with a banner
	_tprintf(_T("//=====================================================\r\n"));
	SYSTEMTIME tm;
	GetLocalTime(&tm);
	_tprintf(_T("%4d年%2d月%2d日 %2d:%2d:%2d\r\n"),
		tm.wYear, tm.wMonth, tm.wDay,
		tm.wHour, tm.wMinute, tm.wSecond);
	_tprintf(_T("//=====================================================\r\n"));
	PEXCEPTION_RECORD pExceptionRecord = pExceptionInfo->ExceptionRecord;
	// First print information about the type of fault
	_tprintf(_T("Exception code: %08X %s\r\n"),
		pExceptionRecord->ExceptionCode,
		GetExceptionString(pExceptionRecord->ExceptionCode));
	// Now print information about where the fault occured
	TCHAR szFaultingModule[MAX_PATH];
	DWORD section, offset;
	GetLogicalAddress(pExceptionRecord->ExceptionAddress,
		szFaultingModule,
		sizeof(szFaultingModule),
		section, offset);
	_tprintf(_T("Fault address:  %08X %02X:%08X %s\r\n"),
		pExceptionRecord->ExceptionAddress,
		section, offset, szFaultingModule);
	PCONTEXT pCtx = pExceptionInfo->ContextRecord;
	// Show the registers
#ifdef _M_IX86  // Intel Only!
	_tprintf(_T("\r\nRegisters:\r\n"));
	_tprintf(_T("EAX:%08X\r\nEBX:%08X\r\nECX:%08X\r\nEDX:%08X\r\nESI:%08X\r\nEDI:%08X\r\n"),
		pCtx->Eax, pCtx->Ebx, pCtx->Ecx, pCtx->Edx, pCtx->Esi, pCtx->Edi);
	_tprintf(_T("CS:EIP:%04X:%08X\r\n"), pCtx->SegCs, pCtx->Eip);
	_tprintf(_T("SS:ESP:%04X:%08X  EBP:%08X\r\n"),
		pCtx->SegSs, pCtx->Esp, pCtx->Ebp);
	_tprintf(_T("DS:%04X  ES:%04X  FS:%04X  GS:%04X\r\n"),
		pCtx->SegDs, pCtx->SegEs, pCtx->SegFs, pCtx->SegGs);
	_tprintf(_T("Flags:%08X\r\n"), pCtx->EFlags);
	// Walk the stack using x86 specific code
	IntelStackWalk(pCtx);
#endif
	_tprintf(_T("\r\n"));
}
//======================================================================
// Given an exception code, returns a pointer to a static string with a 
// description of the exception                                         
//======================================================================
LPTSTR MSJExceptionHandler::GetExceptionString(DWORD dwCode)
{
#define EXCEPTION( x ) case EXCEPTION_##x: return _T(#x);
	switch (dwCode)
	{
		EXCEPTION(ACCESS_VIOLATION)
			EXCEPTION(DATATYPE_MISALIGNMENT)
			EXCEPTION(BREAKPOINT)
			EXCEPTION(SINGLE_STEP)
			EXCEPTION(ARRAY_BOUNDS_EXCEEDED)
			EXCEPTION(FLT_DENORMAL_OPERAND)
			EXCEPTION(FLT_DIVIDE_BY_ZERO)
			EXCEPTION(FLT_INEXACT_RESULT)
			EXCEPTION(FLT_INVALID_OPERATION)
			EXCEPTION(FLT_OVERFLOW)
			EXCEPTION(FLT_STACK_CHECK)
			EXCEPTION(FLT_UNDERFLOW)
			EXCEPTION(INT_DIVIDE_BY_ZERO)
			EXCEPTION(INT_OVERFLOW)
			EXCEPTION(PRIV_INSTRUCTION)
			EXCEPTION(IN_PAGE_ERROR)
			EXCEPTION(ILLEGAL_INSTRUCTION)
			EXCEPTION(NONCONTINUABLE_EXCEPTION)
			EXCEPTION(STACK_OVERFLOW)
			EXCEPTION(INVALID_DISPOSITION)
			EXCEPTION(GUARD_PAGE)
			EXCEPTION(INVALID_HANDLE)
	}
	// If not one of the "known" exceptions, try to get the string
	// from NTDLL.DLL's message table.
	static TCHAR szBuffer[512] = { 0 };
	FormatMessage(FORMAT_MESSAGE_IGNORE_INSERTS | FORMAT_MESSAGE_FROM_HMODULE,
		GetModuleHandle(_T("NTDLL.DLL")),
		dwCode, 0, szBuffer, sizeof(szBuffer), 0);
	return szBuffer;
}
//==============================================================================
// Given a linear address, locates the module, section, and offset containing  
// that address.                                                               
//                                                                             
// Note: the szModule paramater buffer is an output buffer of length specified 
// by the len parameter (in characters!)                                       
//==============================================================================
BOOL MSJExceptionHandler::GetLogicalAddress(
	PVOID addr, PTSTR szModule, DWORD len, DWORD& section, DWORD& offset)
{
	MEMORY_BASIC_INFORMATION mbi;
	if (!VirtualQuery(addr, &mbi, sizeof(mbi)))
		return FALSE;
	DWORD hMod = (DWORD)mbi.AllocationBase;
	if (!GetModuleFileName((HMODULE)hMod, szModule, len))
		return FALSE;
	// Point to the DOS header in memory
	PIMAGE_DOS_HEADER pDosHdr = (PIMAGE_DOS_HEADER)hMod;
	// From the DOS header, find the NT (PE) header
	PIMAGE_NT_HEADERS pNtHdr = (PIMAGE_NT_HEADERS)(hMod + pDosHdr->e_lfanew);
	PIMAGE_SECTION_HEADER pSection = IMAGE_FIRST_SECTION(pNtHdr);
	DWORD rva = (DWORD)addr - hMod; // RVA is offset from module load address
	// Iterate through the section table, looking for the one that encompasses
	// the linear address.
	for (unsigned i = 0;
		i < pNtHdr->FileHeader.NumberOfSections;
		i++, pSection++)
	{
		DWORD sectionStart = pSection->VirtualAddress;
		DWORD sectionEnd = sectionStart
			+ max(pSection->SizeOfRawData, pSection->Misc.VirtualSize);
		// Is the address in this section???
		if ((rva >= sectionStart) && (rva <= sectionEnd))
		{
			// Yes, address is in the section.  Calculate section and offset,
			// and store in the "section" & "offset" params, which were
			// passed by reference.
			section = i + 1;
			offset = rva - sectionStart;
			return TRUE;
		}
	}
	return FALSE;   // Should never get here!
}
//============================================================
// Walks the stack, and writes the results to the report file 
//============================================================
void MSJExceptionHandler::IntelStackWalk(PCONTEXT pContext)
{
	_tprintf(_T("\r\nCall stack:\r\n"));
	_tprintf(_T("Address   Frame     Logical addr  Module\r\n"));
	DWORD pc = pContext->Eip;
	PDWORD pFrame, pPrevFrame;

	pFrame = (PDWORD)pContext->Ebp;
	do
	{
		TCHAR szModule[MAX_PATH] = _T("");
		DWORD section = 0, offset = 0;
		GetLogicalAddress((PVOID)pc, szModule, sizeof(szModule), section, offset);
		_tprintf(_T("%08X  %08X  %04X:%08X %s\r\n"),
			pc, pFrame, section, offset, szModule);
		pc = pFrame[1];
		pPrevFrame = pFrame;
		pFrame = (PDWORD)pFrame[0]; // precede to next higher frame on stack
		if ((DWORD)pFrame & 3)    // Frame pointer must be aligned on a
			break;                  // DWORD boundary.  Bail if not so.
		if (pFrame <= pPrevFrame)
			break;
		// Can two DWORDs be read from the supposed frame address?          
		if (IsBadWritePtr(pFrame, sizeof(PVOID) * 2))
			break;
	} while (1);
}
//============================================================================
// Helper function that writes to the report file, and allows the user to use 
// printf style formating                                                     
//============================================================================
int __cdecl MSJExceptionHandler::_tprintf(const TCHAR * format, ...)
{
	TCHAR szBuff[1024];
	int retValue;
	DWORD cbWritten;
	va_list argptr;

	va_start(argptr, format);
	retValue = wvsprintf(szBuff, format, argptr);
	va_end(argptr);
	WriteFile(m_hReportFile, szBuff, retValue * sizeof(TCHAR), &cbWritten, 0);
	return retValue;
}
