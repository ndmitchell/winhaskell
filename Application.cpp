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

void Application::ExecutionComplete()
{
    toolbar->RunningChanged(false);
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

void SetStatusBar(LPCTSTR Text)
{
    SetDlgItemText(app->hWnd, ID_STATUS, Text);
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

/*
void MainDialogInitArrows()
{
    // create the image list
    HIMAGELIST hImgList = ImageList_Create(16, 16, ILC_COLOR24 | ILC_MASK, 3, 3);
	HBITMAP hBmp = LoadBitmap(G_hInstance, MAKEINTRESOURCE(BMP_ARROWS));
    ImageList_AddMasked(hImgList, hBmp, RGB(255,0,255));
    DeleteObject(hBmp);

	TBBUTTON tb;
	tb.iBitmap = 0;
	tb.idCommand = ID_EXPAND;
	tb.fsStyle = TBSTYLE_BUTTON;
	tb.fsState = TBSTATE_ENABLED;
	tb.dwData = (DWORD_PTR) NULL;
	tb.iString = (INT_PTR) NULL;

    HWND hToolbar = CreateWindowEx(
		0,
		TOOLBARCLASSNAME, NULL,
		TBSTYLE_TOOLTIPS | WS_CHILD | WS_VISIBLE | CCS_NODIVIDER | TBSTYLE_FLAT | CCS_NORESIZE,
		0, 0, 25, 25,
		G_hWnd, (HMENU) (int) ID_EXPAND, G_hInstance, NULL);

	SendMessage(hToolbar, TB_SETIMAGELIST, 0, (LPARAM) hImgList);
    SendMessage(hToolbar, TB_SETBUTTONSIZE, 0, MAKELONG(20,20));
    SendMessage(hToolbar, TB_SETBITMAPSIZE, 0, MAKELONG(16,16));
    SendMessage(hToolbar, TB_ADDBUTTONS, 1, (LPARAM) &tb);
}
*/

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

void FireCommand()
{
    /*
    TCHAR Command[MaxInputSize];
    InputGet(Command);

    HWND G_hWnd = app->hWnd;

	OutputCopy(GetDlgItem(G_hWnd, rtfInput));
    OutputAppend("\n");

	//ExecuteStage(esRunning);

	HistoryAdd(Command);

	TCHAR Verb[500], Argument[500];
	if (ParseCommand(Command, Verb, Argument))
	{
		if (stricmp(Verb, "load") == 0 || stricmp(Verb, "l") == 0)
            RecentFilesAdd(Argument);
	}

	//Wrap->RunCommand(Command);
    */
}


void MainDialogFireCommand()
{
    /*
    TCHAR Buffer[500];
	InputGet(Buffer);
	if (Buffer[0] == 0)
        InputSet(":main");

	FireCommand();
    */
}

void FlipExpand()
{
    /*
	MultilineText = !MultilineText;
	ShowWindow(GetDlgItem(G_hWnd, ID_ENTER), (MultilineText ? SW_SHOW : SW_HIDE));

    TBBUTTONINFO tbi;
    tbi.cbSize = sizeof(tbi);
	tbi.dwMask = TBIF_IMAGE;
	tbi.iImage = (MultilineText ? 1 : 0);
	SendDlgItemMessage(G_hWnd, ID_EXPAND, TB_SETBUTTONINFO, ID_EXPAND, (LPARAM) &tbi);

	MainDialogResize();
    */
}

void MainDialogPickOpenFile()
{
    /*
	TCHAR Buffer[MAX_PATH];
	Buffer[0] = 0;

	OPENFILENAME ofn = {0};
    ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hInstance = G_hInstance;
    ofn.hwndOwner = G_hWnd;
    ofn.lpstrFilter = "Haskell Files (*.hs;*.lhs)\0*.hs;*.lhs\0All Files (*.*)\0*.*\0";
    ofn.nFilterIndex = 1;
    ofn.lpstrFile= Buffer;
    ofn.nMaxFile = MAX_PATH;
    ofn.lpstrFileTitle = NULL;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_EXPLORER;
    ofn.lpfnHook = NULL;
    ofn.lpstrInitialDir = NULL;
    if (!GetOpenFileName(&ofn))
		return;

	TCHAR Buffer2[MAX_PATH+100];
	wsprintf(Buffer2, ":load \"%s\"", Buffer);
	InputSet(Buffer2);
	FireCommand();
    */
}

void MainDialogCommand(int ID, WPARAM wParam, LPARAM lParam)
{
    /*
	switch (ID)
	{
	case ID_RUN:
		MainDialogFireCommand();
		break;

	case ID_STOP:
		Wrap->AbortExecution();
		break;

	case ID_OPEN:
		MainDialogPickOpenFile();
		break;

    case rtfInput:
        if (HIWORD(wParam) == EN_CHANGE)
            InputChanged();
        break;

	case ID_EXPAND:
		FlipExpand();

        ToolbarReparent();

		break;

	case IDCANCEL:
		EndDialog(G_hWnd, 0);
		break;
	}*/
}

BOOL MainDialogNotify(LPNMHDR nmhdr)
{
    /*
	if (nmhdr->idFrom == rtfInput)
		return InputNotify(nmhdr);
	else if (nmhdr->idFrom == ID_TOOLBAR || nmhdr->code == TTN_GETDISPINFO)
		return ToolbarNotify(NULL, nmhdr);
	return FALSE;
    */
    return FALSE;
}

void MainDialogDropFiles(HDROP hDrop)
{
    /*
    char Command[MAX_PATH], File[MAX_PATH];

    DragQueryFile(hDrop, 0, File, MAX_PATH);
    DragFinish(hDrop);

    //Move the current directory
    //Happens automatically if they use the open dialog
    //If they directly invoke :load then not necessary
    //SetWorkingDir(File);

    wsprintf(Command, ":load \"%s\"", File);
    InputSet(Command);
    FireCommand();
    */
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

	case WM_COMMAND:
		MainDialogCommand(LOWORD(wParam), wParam, lParam);
		break;

	case WM_NOTIFY:
	    return MainDialogNotify((LPNMHDR) lParam);
	    break;

	case WM_USER:
		app->TimerId = SetTimer(NULL, 0, (UINT) wParam, TimerFunc);
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
