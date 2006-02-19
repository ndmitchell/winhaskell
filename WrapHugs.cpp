#include "Header.h"
#include "Wrapper.h"
#include "Output.h"
#include "WrapHugs.h"
#include <signal.h>

LPCTSTR Prompt = "\x1B[0;32m%s>\x1B[0m \x1B[50m";

HANDLE CreateStream(bool Write, HANDLE* hUseless);


//Marshal back to C++
DWORD WINAPI WrapHugsReadStdOut(LPVOID Param) 
{
    ((WrapHugs*) Param)->ReadStdOut();
    return 0;
}

DWORD WINAPI WrapHugsReadStdErr(LPVOID Param) 
{
    ((WrapHugs*) Param)->ReadStdErr();
    return 0;
}

WrapHugs::WrapHugs()
{
    Initialising = true;

	OutputAppend("Loading Hugs... ");

	HANDLE tStdin = CreateStream(false, &hStdin);
	HANDLE tStdout = CreateStream(true, &hStdout);
	HANDLE tStderr = CreateStream(true, &hStderr);

	PROCESS_INFORMATION pi = {0};
	STARTUPINFO si = {0};
	si.cb = sizeof(STARTUPINFO); 
	si.hStdInput = tStdin;
	si.hStdOutput = tStdout;
	si.hStdError = tStderr;
	si.dwFlags = STARTF_USESTDHANDLES;

	if (CreateProcess(NULL, "c:\\program files\\winhugs\\hugs.exe",
		NULL, NULL, TRUE, CREATE_NEW_PROCESS_GROUP, NULL, "c:\\program files\\winhugs", &si, &pi) == 0)
		return; //failure

	hProcess = pi.hProcess;
	hProcessId = pi.dwProcessId;

	CloseHandle(tStdin);
	CloseHandle(tStdout);
	CloseHandle(tStderr);
	
	DWORD dword;
	CreateThread(NULL, 0, WrapHugsReadStdOut, this, 0, &dword);
	CreateThread(NULL, 0, WrapHugsReadStdErr, this, 0, &dword);

	TCHAR Buffer[100] = ":set -p\"";
	strcat(Buffer, Prompt);
	strcat(Buffer, "\"\n");
	WriteFile(hStdin, Buffer, (DWORD) strlen(Buffer), &dword, NULL);

	LPCTSTR InitMsg = "putChar '\001'\n";
	WriteFile(hStdin, InitMsg, (DWORD) strlen(InitMsg), &dword, NULL);
}

void WrapHugs::InitDone()
{
	Initialising = false;
	OutputAppend(" Loaded\n");
}


WrapHugs::~WrapHugs()
{
}

void WrapHugs::AbortExecution()
{
	GenerateConsoleCtrlEvent(CTRL_BREAK_EVENT, (DWORD) hProcessId);
}

void WrapHugs::RunCommand(LPCTSTR Command)
{
	DWORD dword;
	WriteFile(hStdin, Command, (DWORD) strlen(Command), &dword, NULL);
	WriteFile(hStdin, "\n", 1, &dword, NULL);
}

void WrapHugs::ReadStdErr()
{
	DWORD dwRead; 
	CHAR chBuf[101]; 

	while(true)
	{ 
		if(!ReadFile(hStderr, chBuf, 1, &dwRead,  NULL) || dwRead == 0)
			break; 
		chBuf[dwRead] = 0;
		OutputColor(RED);
		OutputAppend(chBuf);
		OutputColor(BLACK);
	} 
}

void WrapHugs::ReadStdOut() 
{
	DWORD dwRead; 
	CHAR chBuf[101]; 

	while(true)
	{ 
		if(!ReadFile(hStdout, chBuf, 1, &dwRead,  NULL) || dwRead == 0)
			break;
		chBuf[dwRead] = 0;

		if (Initialising)
		{
			if (chBuf[0] == 1)
			{
				Initialising = false;
				InitDone();
			}
		}
		else if (chBuf[0] != '\r')
			OutputAppend(chBuf);
	} 
}


// See msdn.microsoft.com/library/en-us/dllproc/base/creating_a_child_process_with_redirected_input_and_output.asp

HANDLE CreateStream(bool Write, HANDLE* hUseless)
{
	SECURITY_ATTRIBUTES saAttr; 
	saAttr.nLength = sizeof(SECURITY_ATTRIBUTES); 
	saAttr.bInheritHandle = TRUE; 
	saAttr.lpSecurityDescriptor = NULL;

	HANDLE hRead, hWrite;
	CreatePipe(&hRead, &hWrite, &saAttr, 0);

	if (!Write)
	{
		HANDLE hSwap = hWrite;
		hWrite = hRead;
		hRead = hSwap;
	}

	HANDLE hProcess = GetCurrentProcess();
	HANDLE hDupe;
    DuplicateHandle(hProcess, hRead, hProcess, &hDupe,
        0, FALSE, DUPLICATE_SAME_ACCESS);
    CloseHandle(hRead);
	*hUseless = hDupe;

	return hWrite;
}
