#include "Header.h"
#include "Application.h"
#include "Toolbar.h"

#include "Output.h"
#include "Input.h"
#include "Console.h"
#include "Interpreter.h"
#include "Hugs.h"
#include "History.h"
#include "Lexer.h"
#include "RecentFiles.h"
#include "Command.h"



void ShowMainDialog();


Application::Application()
{
    app = this;
    ShowMainDialog();

    recentFiles = new RecentFiles();
    history = new History();
    interpreter = (Interpreter*) StartHugs();
}

void Application::DefaultCommand()
{
    FireCommand(toolbar->DefaultCommand(), 0);
}

void Application::RunningChanged(bool Running)
{
    toolbar->RunningChanged(Running);
    if (!Running)
		ShowPrompt();
}

void Application::ShowPrompt()
{
    output->FormatReset();
    output->SetForecolor(GREEN);
    output->Append(interpreter->GetLastPrompt());
}

void Application::Warning(LPCTSTR Text)
{
	OutputFormat of;
	output->FormatGet(&of);
	output->SetForecolor(RED);
	output->Append("Warning: ");
	output->Append(Text);
	output->Append("\n");
	output->FormatSet(&of);
}

void Application::AddTimer(Console* c, UINT Elapse)
{
	timeout = c;
	PostMessage(hWnd, WM_USER, Elapse, (LPARAM) c);
}

void Application::DelTimer(Console* c)
{
	//Not implemented
	//KillTimer(NULL, (UINT) c);
}

void Application::StatusBar(LPCTSTR Text)
{
    SetWindowText(hStatus, Text);
}


void MainDialogResize()
{
	int Height;
	RECT rcWnd, rc;

    HWND G_hWnd = app->hWnd;

	GetClientRect(G_hWnd, &rcWnd);
	Height = rcWnd.bottom;

    int ToolbarHeight = toolbar->Height();
    MoveWindow(toolbar->hWnd, 0, 0, rcWnd.right, ToolbarHeight, TRUE);

    GetClientRect(app->hStatus, &rc);
    MoveWindow(app->hStatus, 0, Height - rc.bottom,
		rcWnd.right, rc.bottom, TRUE);
	Height -= rc.bottom + 3;

	int txtHeight = (false ? 100 : 24);
	MoveWindow(input->hWnd, 0, Height - txtHeight,
		rcWnd.right, txtHeight, TRUE);
	//MoveWindow(GetDlgItem(G_hWnd, ID_EXPAND), rcWnd.right - 25,
	//	Height - 25 + 5, 25, 25, TRUE);
	Height -= txtHeight + 3;

    MoveWindow(output->hWnd, 0, ToolbarHeight,
		rcWnd.right, Height - ToolbarHeight, TRUE);
}

void MainDialogInit()
{
    HWND G_hWnd = app->hWnd;
//    RegistryReadWindowPos(G_hWnd);

    toolbar = new Toolbar(app->hWnd);
    input = new Input(app->hWnd);
    output = new Output(app->hWnd);


    //RecentFilesInit();
	//MainDialogInitArrows();
    app->hStatus = CreateStatusWindow(WS_CHILD | WS_VISIBLE, "Welcome to WinHaskell", app->hWnd, 0);
	MainDialogResize();

	SendMessage(G_hWnd, WM_SETICON, ICON_BIG, (LPARAM) LoadIcon(hInst, MAKEINTRESOURCE(1)));

    output->SetForecolor(GREEN);
    output->Append("-- Welcome to WinHaskell, (C) Neil Mitchell 2005-2006\n");
    output->SetForecolor(BLACK);
}

void MainDialogDropFiles(HDROP hDrop)
{
    char File[MAX_PATH];
    DragQueryFile(hDrop, 0, File, MAX_PATH);
    DragFinish(hDrop);
    app->LoadFile(File);
}

VOID CALLBACK TimerFunc(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime)
{
	if (app->timeout->Tick())
		KillTimer(NULL, idEvent);
}


INT_PTR CALLBACK MainDialogProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_INITDIALOG:
        app->hWnd = hWnd;
	    MainDialogInit();
	    break;

	case WM_SIZE:
		MainDialogResize();
		break;

	case WM_DROPFILES:
	    MainDialogDropFiles((HDROP) wParam);
	    break;

	case WM_USER:
		app->TimerId = (UINT) SetTimer(NULL, 0, (UINT) wParam, TimerFunc);
		break;

	case WM_CLOSE:
        //RegistryWriteWindowPos(hWnd);
		PostQuitMessage(0);
		break;
	}
    return FALSE;
}


void ShowMainDialog()
{
    CreateDialog(hInst, MAKEINTRESOURCE(DLG_MAIN), NULL, &MainDialogProc);
}
