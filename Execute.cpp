#include "Header.h"

DWORD main2(LPCTSTR InputFile);

BOOL ExecuteCommand(LPTSTR Console, LPCTSTR CurrentDir);
BOOL CreateChildProcess();
BOOL ExecuteCommand2();

HANDLE Process;

HANDLE hChildStdinRd, hChildStdinWr, hChildStdinWrDup, 
   hChildStdoutRd, hChildStdoutWr, hChildStdoutRdDup, 
   hInputFile; 
 



void ErrorBox(LPCTSTR Msg)
{
	MessageBox(G_hWnd, Msg, "WinHaskell: Error", MB_ICONERROR);
}

// Create a temporary haskell file
void CreateTemporaryHaskell(LPTSTR TempFileDir, LPTSTR TempFileName)
{
	GetTempPath(MAX_PATH, TempFileDir);

	GetTempFileName(TempFileDir, "HBC", 0, TempFileName);
	strcpy(strrchr(TempFileName, '.'), ".hs");
}

// Write out a sequence of commands such that main
// will display the goods
void WriteHaskell(LPCTSTR TempFileName, LPCTSTR Command)
{
	HANDLE hFile = CreateFile(TempFileName, GENERIC_WRITE, 0,
		NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

	if (hFile == INVALID_HANDLE_VALUE)
	{
		ErrorBox("Could not create file");
		return;
	}

	DWORD Dword;
	WriteFile(hFile, "main = print $ ", 15, &Dword, NULL);
	WriteFile(hFile, Command, (DWORD) strlen(Command), &Dword, NULL);
	CloseHandle(hFile);
}

DWORD WINAPI ThreadProc(LPVOID lpParameter)
{
	LPTSTR Command = (LPTSTR) lpParameter;

	// First create the temporary file
	TCHAR TempFileName[MAX_PATH], TempFileDir[MAX_PATH];
	CreateTemporaryHaskell(TempFileDir, TempFileName);

	// Write out the Haskell
	WriteHaskell(TempFileName, Command);
	free(Command);

	TCHAR Console[MAX_PATH*3];
	strcpy(Console, "yhi ");
	strcat(Console, &strrchr(TempFileName,'\\')[1]);

	ExecuteStage(esCompiling);
	if (!ExecuteCommand(Console, TempFileDir))
	{
		ExecuteStage(esFinished);
		return 0; //failed to compile
	}

	strcpy(Console, "yhc Main");
	ExecuteStage(esRunning);
	ExecuteCommand(Console, TempFileDir);
	ExecuteStage(esFinished);

	return 0;
}

void Execute(LPCTSTR Command)
{
	// DWORD ThreadId;
	//	CreateThread(NULL, 0, &ThreadProc, strdup(Command), 0, &ThreadId);

	ExecuteCommand2();

}

// See msdn.microsoft.com/library/en-us/dllproc/base/creating_a_child_process_with_redirected_input_and_output.asp
HANDLE CreateStream(BOOL Write, HANDLE* hUseless)
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

DWORD WINAPI ReadStdout(HANDLE hStdout) 
{ 
   DWORD dwRead; 
   CHAR chBuf[101]; 

   while(true)
   { 
      if( !ReadFile(hStdout, chBuf, 1, &dwRead, 
         NULL) || dwRead == 0) break; 
	  chBuf[dwRead] = 0;
//	  AppendText(chBuf);
   } 
   return 0;
}

BOOL ExecuteCommand(LPTSTR Console, LPCTSTR CurrentDir)
{
	HANDLE hStdin2, hStdout2;
	HANDLE hStdin = CreateStream(FALSE, &hStdin2);
	HANDLE hStdout = CreateStream(TRUE, &hStdout2);

	PROCESS_INFORMATION pi = {0}; 
	STARTUPINFO si = {0};
	si.cb = sizeof(STARTUPINFO); 
	si.hStdInput = hStdin;
	si.hStdOutput = hStdout;
	si.hStdError = hStdout;
	si.dwFlags = STARTF_USESTDHANDLES;
 
	if (CreateProcess(NULL, Console,
		NULL, NULL, TRUE, CREATE_NO_WINDOW, NULL, CurrentDir, &si, &pi) == 0)
		return FALSE;
	ActiveProcess(pi.hProcess);

//	CloseHandle(hStdin2);
	CloseHandle(hStdin);
	CloseHandle(hStdout);
	
	CreateThread(NULL, 0, ReadStdout, hStdout2, 0, NULL);
	return true;

	DWORD ExitCode;
	GetExitCodeProcess(pi.hProcess, &ExitCode);
	ActiveProcess(NULL);
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);

	return (ExitCode == 0);
}

HANDLE MyHStdIn, MyHStdIn2;

DWORD WINAPI ReadStdIn(LPVOID lpParameter)
{
    DWORD state1, state2;

   /* int Result1 = GetNamedPipeHandleState(MyHStdIn2, &state1, NULL, NULL, NULL, NULL, 0);
    Sleep(5000);

    int Result2 = GetNamedPipeHandleState(MyHStdIn2, &state2, NULL, NULL, NULL, NULL, 0);
	*/

    DWORD dword;
    char* Buffer = "repeat 'a' !! 1000000\n";
    WriteFile(MyHStdIn2, Buffer, strlen(Buffer), &dword, NULL);

    Sleep(1000);

    //Result1 = GetNamedPipeHandleState(MyHStdIn2, &state1, NULL, NULL, NULL, NULL, 0);


    return 0;

    /*

	HANDLE hStdin = (HANDLE) lpParameter;
	if (WaitForSingleObject(MyHStdIn, INFINITE) == WAIT_FAILED)
		MessageBox(NULL, "Failed to wait", "", 0);
	else
		MessageBox(NULL, "Waiting for standard in", "", 0);

	DWORD dword;
	while(true)
	{
		int i = GetTickCount();
		WaitForSingleObject(MyHStdIn2, INFINITE);
		int j = GetTickCount() - i;

		if (j > 1000)
			MessageBeep(0);
	}
	return 0;
	*/
}


BOOL ExecuteCommand2()
{
	HANDLE hStdin2, hStdout2;
	HANDLE hStdin = CreateStream(FALSE, &hStdin2);
	HANDLE hStdout = CreateStream(TRUE, &hStdout2);

	PROCESS_INFORMATION pi = {0}; 
	STARTUPINFO si = {0};
	si.cb = sizeof(STARTUPINFO); 
	si.hStdInput = hStdin;
	si.hStdOutput = hStdout;
	si.hStdError = hStdout;
	si.dwFlags = STARTF_USESTDHANDLES;
 
	if (CreateProcess(NULL, "ghci", //"\"c:\\program files\\winhugs\\hugs.exe\"",
		NULL, NULL, TRUE, CREATE_NO_WINDOW, NULL, NULL, &si, &pi) == 0)
		return FALSE;
	ActiveProcess(pi.hProcess);

	Process = pi.hProcess;

	CloseHandle(hStdin);
	CloseHandle(hStdout);

	DWORD dword;
	MyHStdIn = hStdin;
	MyHStdIn2 = hStdin2;
	CreateThread(NULL, 0, ReadStdIn, hStdin, 0, &dword);

	//CreateThread(NULL, 0, ReadStdout, hStdout2, 0, &dword);
	ReadStdout(hStdout2);

	return 0;

	DWORD ExitCode;
	GetExitCodeProcess(pi.hProcess, &ExitCode);
	ActiveProcess(NULL);
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);

	return (ExitCode == 0);
}

/*

#define BUFSIZE 4096 
 
BOOL CreateChildProcess(VOID); 
VOID WriteToPipe(VOID); 
VOID ReadFromPipe(VOID); 
VOID ErrorExit(LPTSTR); 
VOID ErrMsg(LPTSTR, BOOL); 
 
DWORD main2(LPCTSTR InputFile) 
{ 
   SECURITY_ATTRIBUTES saAttr; 
   BOOL fSuccess; 
 
// Set the bInheritHandle flag so pipe handles are inherited. 
 
   saAttr.nLength = sizeof(SECURITY_ATTRIBUTES); 
   saAttr.bInheritHandle = TRUE; 
   saAttr.lpSecurityDescriptor = NULL; 
 
// Get the handle to the current STDOUT. 
 
   hStdout = GetStdHandle(STD_OUTPUT_HANDLE); 
 
// Create a pipe for the child process's STDOUT. 
 
   if (! CreatePipe(&hChildStdoutRd, &hChildStdoutWr, &saAttr, 0)) 
      ErrorExit("Stdout pipe creation failed\n"); 
 
// Create noninheritable read handle and close the inheritable read 
// handle. 

    fSuccess = DuplicateHandle(GetCurrentProcess(), hChildStdoutRd,
        GetCurrentProcess(), &hChildStdoutRdDup , 0,
        FALSE,
        DUPLICATE_SAME_ACCESS);
    if( !fSuccess )
        ErrorExit("DuplicateHandle failed");
    CloseHandle(hChildStdoutRd);

// Create a pipe for the child process's STDIN. 
	return 0;
 
   if (! CreatePipe(&hChildStdinRd, &hChildStdinWr, &saAttr, 0)) 
      ErrorExit("Stdin pipe creation failed\n"); 
 
// Duplicate the write handle to the pipe so it is not inherited. 
 
   fSuccess = DuplicateHandle(GetCurrentProcess(), hChildStdinWr, 
      GetCurrentProcess(), &hChildStdinWrDup, 0, 
      FALSE,                  // not inherited 
      DUPLICATE_SAME_ACCESS); 
   if (! fSuccess) 
      ErrorExit("DuplicateHandle failed"); 
 
   CloseHandle(hChildStdinWr); 
 
// Now create the child process. 
   return 0;
   
   fSuccess = CreateChildProcess();
   if (! fSuccess) 
      ErrorExit("Create process failed");

   return 0;

// Get a handle to the parent's input file. 
 
   hInputFile = CreateFile(InputFile, GENERIC_READ, 0, NULL, 
      OPEN_EXISTING, FILE_ATTRIBUTE_READONLY, NULL); 
 
   if (hInputFile == INVALID_HANDLE_VALUE) 
      ErrorExit("CreateFile failed\n"); 
 
// Write to pipe that is the standard input for a child process. 
 
   WriteToPipe(); 
 
// Read from pipe that is the standard output for child process. 
 
   ReadFromPipe(); 
 
   return 0; 
} 
 
BOOL CreateChildProcess() 
{ 
   PROCESS_INFORMATION piProcInfo; 
   STARTUPINFO siStartInfo;
   BOOL bFuncRetn = FALSE; 
 
// Set up members of the PROCESS_INFORMATION structure. 
 
   ZeroMemory( &piProcInfo, sizeof(PROCESS_INFORMATION) );
 
// Set up members of the STARTUPINFO structure. 
 
   ZeroMemory( &siStartInfo, sizeof(STARTUPINFO) );
   siStartInfo.cb = sizeof(STARTUPINFO); 
   siStartInfo.hStdError = hChildStdoutWr;
   siStartInfo.hStdOutput = hChildStdoutWr;
   siStartInfo.hStdInput = hChildStdinRd;
   siStartInfo.dwFlags |= STARTF_USESTDHANDLES;
 
// Create the child process. 
    
   bFuncRetn = CreateProcess(NULL, 
      "yhcc",       // command line 
      NULL,          // process security attributes 
      NULL,          // primary thread security attributes 
      TRUE,          // handles are inherited 
      0,             // creation flags 
      NULL,          // use parent's environment 
      NULL,          // use parent's current directory 
      &siStartInfo,  // STARTUPINFO pointer 
      &piProcInfo);  // receives PROCESS_INFORMATION 
   
   if (bFuncRetn == 0) 
      ErrorExit("CreateProcess failed");
   else 
   {
      CloseHandle(piProcInfo.hProcess);
      CloseHandle(piProcInfo.hThread);
      return bFuncRetn;
   }
}
 
VOID WriteToPipe(VOID) 
{ 
   DWORD dwRead, dwWritten; 
   CHAR chBuf[BUFSIZE]; 
 
// Read from a file and write its contents to a pipe. 
 
   for (;;) 
   { 
      if (! ReadFile(hInputFile, chBuf, BUFSIZE, &dwRead, NULL) || 
         dwRead == 0) break; 
      if (! WriteFile(hChildStdinWrDup, chBuf, dwRead, 
         &dwWritten, NULL)) break; 
   } 
 
// Close the pipe handle so the child process stops reading. 
 
   if (! CloseHandle(hChildStdinWrDup)) 
      ErrorExit("Close pipe failed"); 
} 
 
VOID ReadFromPipe(VOID) 
{ 
   DWORD dwRead, dwWritten; 
   CHAR chBuf[BUFSIZE]; 

// Close the write end of the pipe before reading from the 
// read end of the pipe. 
 
   if (!CloseHandle(hChildStdoutWr)) 
      ErrorExit("CloseHandle failed"); 
 
// Read output from the child process, and write to parent's STDOUT. 
 
   for (;;) 
   { 
      if( !ReadFile( hChildStdoutRdDup, chBuf, BUFSIZE, &dwRead, 
         NULL) || dwRead == 0) break; 
	  chBuf[dwRead] = 0;
	  SetDlgItemText(G_hWnd, rtf, chBuf);
      //if (! WriteFile(hStdout, chBuf, dwRead, &dwWritten, NULL)) 
      //   break; 
   } 
} 
 
VOID ErrorExit (LPTSTR lpszMessage) 
{ 
	ErrorBox(lpszMessage);
}

*/
