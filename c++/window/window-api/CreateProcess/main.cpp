#include <windows.h>  
#include <stdio.h>
//test ok
int processToFile()
{
	PROCESS_INFORMATION pi;
	STARTUPINFO si = { sizeof(STARTUPINFO) };
	SECURITY_ATTRIBUTES sa = { sizeof(SECURITY_ATTRIBUTES),NULL,TRUE };
	HANDLE cmdOutput = CreateFile("output.txt",
		GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
		&sa, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (cmdOutput == INVALID_HANDLE_VALUE)
	{
		ExitProcess(0);
	}

	HANDLE cmdInput = CreateFile("input.txt",
		GENERIC_READ,
		FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
		&sa, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (cmdInput == INVALID_HANDLE_VALUE)
	{
		ExitProcess(0);
	}

	//si.hStdInput = cmdInput;
	si.hStdOutput = cmdOutput;
	si.dwFlags = STARTF_USESTDHANDLES;

	if (CreateProcess(NULL, "ping 8.8.8.8", NULL, NULL, TRUE, NULL, NULL, NULL, &si, &pi))
	{
		WaitForSingleObject(pi.hProcess, INFINITE);
		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);
	}

	CloseHandle(cmdOutput);
	CloseHandle(cmdInput);

	return 0;
}
//test ok
void processToPipe() {
	SECURITY_ATTRIBUTES sa;
	HANDLE hRead, hWrite;

	sa.nLength = sizeof(SECURITY_ATTRIBUTES);
	sa.lpSecurityDescriptor = NULL;
	sa.bInheritHandle = TRUE;
	if (!CreatePipe(&hRead, &hWrite, &sa, 0)) {
		printf("Error On CreatePipe()");
		return;
	}
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	si.cb = sizeof(STARTUPINFO);
	GetStartupInfo(&si);
	si.hStdError = hWrite;
	si.hStdOutput = hWrite;
	si.wShowWindow = SW_HIDE;
	si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
	if (!CreateProcess(NULL, TEXT("cmd /c dir /?")
		, NULL, NULL, TRUE, NULL, NULL, NULL, &si, &pi)) {
		printf("Error on CreateProcess() errCode:%d\n",GetLastError());
		return;
	}
	CloseHandle(hWrite);
	//WaitForSingleObject(pi.hProcess, INFINITE);
	char buffer[4096] = { 0 };
	DWORD bytesRead;
	while (true) {
		if (ReadFile(hRead, buffer, sizeof(buffer) -1, &bytesRead, NULL) == NULL)
			break;
		printf("[read]%s\n", buffer);
		//Sleep(200);
	}
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);
	CloseHandle(hRead);

}
int main() {
	processToPipe();
}
