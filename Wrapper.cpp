#include "Header.h"
#include "Wrapper.h"
#include "Output.h"
//#include "Events.h"
#include <signal.h>

LPCTSTR Prompt = "\x1B[0;32m%s>\x1B[0m \x1B[50m";

bool Running = true;
bool Initialising = true;

HANDLE hStdin, hStdout, hStderr;
HANDLE hProcess;
DWORD hProcessId;

// Pre definitions
HANDLE CreateStream(bool Write, HANDLE* hUseless);
DWORD WINAPI ReadStdErr(HANDLE hStderr);
DWORD WINAPI ReadStdOut(LPVOID Param);

// Needs a hidden Console, or can't send Ctrl-C at it
LPCTSTR ConsoleTitle = "WINHASKELL HIDDEN CONSOLE";

void CreateHiddenConsole()
{
	char Buffer[100];
	wsprintf(Buffer, "%s - %i", ConsoleTitle, GetTickCount());

	AllocConsole();
	SetConsoleTitle(Buffer);

	/*
	//Has the potential to work, but doesn't quite
	RECT rc;
	BOOL Res = GetWindowRect(G_hWnd, &rc);

	SMALL_RECT src;
	src.Top = 0; //rc.top;
	src.Left = 0; //rc.left;
	src.Right = 0; //rc.right; //src.Left+100;
	src.Bottom = 0; //rc.bottom; //src.Top+100;
	BOOL Res2 = SetConsoleWindowInfo(GetStdHandle(STD_OUTPUT_HANDLE), TRUE, &src);
	*/

	//Bound the wait at 1 second, in case something goes wrong
	for (int i = 0; i < 10; i++)
	{
		Sleep(100);
		HWND hWnd = FindWindow(NULL, Buffer);
		if (hWnd != NULL)
		{
			ShowWindow(hWnd, SW_HIDE);
			break;
		}
	}
}

bool InitWrapper()
{
	CreateHiddenConsole();

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
		return false;

	hProcess = pi.hProcess;
	hProcessId = pi.dwProcessId;

	CloseHandle(tStdin);
	CloseHandle(tStdout);
	CloseHandle(tStderr);
	
	DWORD dword;
	CreateThread(NULL, 0, ReadStdOut, NULL, 0, &dword);
	CreateThread(NULL, 0, ReadStdErr, NULL, 0, &dword);

	TCHAR Buffer[100] = ":set -p\"";
	strcat(Buffer, Prompt);
	strcat(Buffer, "\"\n");
	WriteFile(hStdin, Buffer, (DWORD) strlen(Buffer), &dword, NULL);

	LPCTSTR InitMsg = "putChar '\001'\n";
	WriteFile(hStdin, InitMsg, (DWORD) strlen(InitMsg), &dword, NULL);

	return true;
}

void InitDone()
{
	Initialising = false;
	OutputAppend(" Loaded\n");
}


void TermWrapper()
{
}

void AbortExecution()
{
	BOOL b = GenerateConsoleCtrlEvent(CTRL_BREAK_EVENT, (DWORD) hProcessId);
	DWORD d = GetLastError();
}

void RunCommand(LPCTSTR Command)
{
	DWORD dword;
	WriteFile(hStdin, Command, (DWORD) strlen(Command), &dword, NULL);
	WriteFile(hStdin, "\n", 1, &dword, NULL);
}

DWORD WINAPI ReadStdErr(HANDLE hStderr)
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
	return 0;
}

DWORD WINAPI ReadStdOut(LPVOID Param) 
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
	return 0;
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
