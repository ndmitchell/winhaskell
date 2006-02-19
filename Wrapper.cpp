#include "Header.h"
#include "Wrapper.h"
#include "Output.h"
#include "WrapHugs.h"
#include <signal.h>

Wrapper* WrapperInit()
{
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

    return new WrapHugs();
}
