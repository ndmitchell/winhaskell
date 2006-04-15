#include "Header.h"
#include "Console.h"
#include "Application.h"
#include "Mutex.h"
#include "Output.h"


HANDLE CreateStreamConsole(bool Write, HANDLE* hUseless);

bool ConsoleInitDone;

void ConsoleInit()
{
    if (ConsoleInitDone)
        return;
    ConsoleInitDone = true;

    // Needs a hidden Console, or can't send Ctrl-C at it
    LPCTSTR ConsoleTitle = "WINHASKELL HIDDEN CONSOLE";

	char Buffer[100];
	wsprintf(Buffer, "%s - %i", ConsoleTitle, GetTickCount());

	AllocConsole();
	SetConsoleTitle(Buffer);

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



//Marshal back to C++
DWORD WINAPI ConsoleReadStdOut(LPVOID Param) 
{
    ((Console*) Param)->Internal_ReadHandle(true);
    return 0;
}

DWORD WINAPI ConsoleReadStdErr(LPVOID Param) 
{
    ((Console*) Param)->Internal_ReadHandle(false);
    return 0;
}

Console::Console()
{
	mutex = new Mutex();
	BufSize = 0;
    ConsoleInit();
    Running = false;
}

Console::~Console()
{
    delete mutex;
    if (Running)
        Abort();
}

void Console::Start(LPCTSTR Command)
{
    assert(!Running);

    Running = true;
	BufSize = 0;

    HANDLE tStdin = CreateStreamConsole(false, &hStdin);
	HANDLE tStdout = CreateStreamConsole(true, &hStdout);
	HANDLE tStderr = CreateStreamConsole(true, &hStderr);

	PROCESS_INFORMATION pi = {0};
	STARTUPINFO si = {0};
	si.cb = sizeof(STARTUPINFO); 
	si.hStdInput = tStdin;
	si.hStdOutput = tStdout;
	si.hStdError = tStdout; // Was previously stderr
	si.dwFlags = STARTF_USESTDHANDLES;
    
    LPSTR Command2 = strdup(Command);

	if (CreateProcess(NULL, Command2,
		NULL, NULL, TRUE, CREATE_NEW_PROCESS_GROUP, NULL, NULL, &si, &pi) == 0)
		return; //failure

    free(Command2);

	hProcess = pi.hProcess;
	hProcessId = pi.dwProcessId;

	CloseHandle(tStdin);
	CloseHandle(tStdout);
	CloseHandle(tStderr);
	
	DWORD dword;
    CreateThread(NULL, 0, ConsoleReadStdOut, this, 0, &dword);
	//CreateThread(NULL, 0, ConsoleReadStdErr, this, 0, &dword);
}

void Console::Exception()
{
	GenerateConsoleCtrlEvent(CTRL_BREAK_EVENT, (DWORD) hProcessId);
}

void Console::Abort()
{
}

void Console::Write(LPCTSTR Buffer, DWORD Size)
{
    DWORD dword;
	WriteFile(hStdin, Buffer, Size, &dword, NULL);
    assert(dword == Size);
}

void Console::FlushBuffer()
{
	if (BufSize != 0)
	{
		Buffer[BufSize] = 0;
		Read(Buffer, BufSize, BufStdout);
		BufSize = 0;
	}
}

void Console::Read(LPCTSTR Buffer, DWORD Size, bool Stdout)
{
	output->Append(Buffer);
}

void Console::Escape(ConsoleEscape Code, bool Stdout)
{
	output->Escape(Code);
}

bool Console::Tick()
{
	if (!mutex->LockImmediate())
		return false;
	else
	{
		FlushBuffer();
		mutex->Unlock();
		return true;
	}
}

void Console::ProcessEscape()
{
	//first check its a valid/recognised escape character
	assert(Buffer[0] == ConsoleEscapeChar);
	int State = 0;

	for (DWORD i = 0; State != -1 && i < BufSize; i++)
	{
		char c = Buffer[i];
		switch (State)
		{
		case 0:
			State = (c == ConsoleEscapeChar ? 1 : -1);
			break;

		case 1:
			State = (c == '[' ? 2 : -1);
			break;

		case 2:
			State = (c >= '0' && c <= '9' ? 3 : -1);
			break;

		case 3:
			if (c == 'm')
				State = 10;
			else if (c == ';')
				State = 2;
			else if (c >= '0' && c <= '9')
				State = 3;
			else
				State = -1;
			break;

		default:
			State = -1;
		}
	}

	if (State == -1)
	{
		FlushBuffer();
		return;
	}

	//valid, now process it
	for (DWORD i = 0; i < BufSize; i++)
	{
		if (Buffer[i] == ';' || Buffer[i] == '[')
			Buffer[i] = 0;
	}
	for (DWORD i = 0; i < BufSize; i++)
	{
		if (Buffer[i] == 0)
			Escape((ConsoleEscape) atoi(&Buffer[i+1]), BufStdout);
	}
	BufSize = 0;
}

void Console::Internal_ReadHandle(bool Stdout)
{
	DWORD dword;
    CHAR chBuf[2] = {0, 0};
    HANDLE h = (Stdout ? hStdout : hStderr);

	while(true)
	{ 
		if(!ReadFile(h, chBuf, 1, &dword,  NULL) || dword == 0)
			break;

        {
            Lock l(mutex);

		    if (chBuf[0] == '\b')
		    {
			    FlushBuffer();
			    Escape(conBackspace, Stdout);
		    }
		    else
		    {
			    if ((BufSize != 0 && Stdout != BufStdout) ||
				    (BufSize >= ConsoleBufferSize) ||
				    (BufSize != 0 && chBuf[0] == ConsoleEscapeChar))
			    {
				    app->DelTimer(this);
				    FlushBuffer();
			    }
			    bool AddTimer = (BufSize == 0);
			    BufStdout = Stdout;
			    Buffer[BufSize++] = chBuf[0];

			    if (chBuf[0] == 'm' && Buffer[0] == ConsoleEscapeChar)
				    ProcessEscape();

			    if (AddTimer)
				    app->AddTimer(this, ConsoleTimeout);
		    }
        }
	}
	{
		Lock l(mutex);
		FlushBuffer();
		Running = false;
	}
	Finished();
}



// See msdn.microsoft.com/library/en-us/dllproc/base/creating_a_child_process_with_redirected_input_and_output.asp

HANDLE CreateStreamConsole(bool Write, HANDLE* hUseless)
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
