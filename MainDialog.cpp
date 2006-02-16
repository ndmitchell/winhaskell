#include "Header.h"
#include "Wrapper.h"
#include "Output.h"
#include "Input.h"
#include "History.h"
#include "Toolbar.h"
#include "RecentFiles.h"
#include "Registry.h"
#include "Lexer.h"

const int ToolbarHeight = 46;

bool MultilineText = false;


void SetStatusBar(LPCTSTR Text)
{
	SetDlgItemText(G_hWnd, ID_STATUS, Text);
}

void ExecuteStage(es Stage)
{
	if (Stage != esCompiling)
	{
		bool Enter = (Stage == esRunning);

		TBBUTTONINFO tbi;
		tbi.cbSize = sizeof(tbi);
		tbi.dwMask = TBIF_STATE;

		tbi.fsState = (Enter ? TBSTATE_ENABLED : 0);
		SendDlgItemMessage(G_hWnd, ID_TOOLBAR, TB_SETBUTTONINFO, ID_STOP, (LPARAM) &tbi);

		tbi.fsState = (Enter ? 0 : TBSTATE_ENABLED);
		SendDlgItemMessage(G_hWnd, ID_TOOLBAR, TB_SETBUTTONINFO, ID_RUN, (LPARAM) &tbi);
	}
	switch (Stage)
	{
	case esCompiling:
		SetStatusBar("Compiling...");
		break;

	case esRunning:
		SetStatusBar("Running...");
		break;

	case esFinished:
		SetStatusBar("Finished");
		break;
	}
}

HANDLE hProc = NULL;

void ActiveProcess(HANDLE hProcess)
{
	hProc = hProcess;
}

void ExecutionComplete()
{
	ExecuteStage(esFinished);
}

void MainDialogResize()
{
    HWND hItem;
	int Height;
	RECT rcWnd, rc;

	HWND hRtfInput = GetDlgItem(G_hWnd, rtfInput);
	HWND hRtfOutput = GetDlgItem(G_hWnd, rtfOutput);

	GetClientRect(G_hWnd, &rcWnd);
	Height = rcWnd.bottom;

	hItem = GetDlgItem(G_hWnd, ID_STATUS);
	GetClientRect(hItem, &rc);
    MoveWindow(hItem, 0, Height - rc.bottom,
		rcWnd.right, rc.bottom, TRUE);
	Height -= rc.bottom + 3;

	int txtHeight = (MultilineText ? 100 : 24);
	MoveWindow(hRtfInput, 0, Height - txtHeight,
		rcWnd.right - 28, txtHeight, TRUE);
	MoveWindow(GetDlgItem(G_hWnd, ID_EXPAND), rcWnd.right - 25,
		Height - 25 + 5, 25, 25, TRUE);
	Height -= txtHeight + 3;

	MoveWindow(hRtfOutput, 0, ToolbarHeight,
		rcWnd.right, Height - ToolbarHeight, TRUE);
}

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


void MainDialogInit()
{
    RegistryReadWindowPos(G_hWnd);

	ToolbarInit();
	RecentFilesInit();
	MainDialogInitArrows();
    CreateStatusWindow(WS_CHILD | WS_VISIBLE, "Welcome to WinHaskell", G_hWnd, ID_STATUS);
	MainDialogResize();

	SendMessage(G_hWnd, WM_SETICON, ICON_BIG, (LPARAM) LoadIcon(G_hInstance, MAKEINTRESOURCE(1)));

	ExecuteStage(esFinished);

	OutputInit(GetDlgItem(G_hWnd, rtfOutput));
	OutputColor(GREEN);
	OutputAppend("-- Welcome to WinHaskell, (C) Neil Mitchell 2005-2006\n");
	OutputColor(BLACK);

	InputInit(GetDlgItem(G_hWnd, rtfInput));

	InitWrapper();
}

void FireCommand()
{
    LPCTSTR Command = InputGet();

	int Len = (int) strlen(Command);
	LPTSTR Buffer = new TCHAR[Len+2];
	strcpy(Buffer, Command);
	strcat(Buffer, "\n");

	OutputCopy(GetDlgItem(G_hWnd, rtfInput));
    OutputAppend("\n");

	ExecuteStage(esRunning);

	HistoryAdd(Command);

	TCHAR Verb[500], Argument[500];
	if (ParseCommand(Command, Verb, Argument))
	{
		if (stricmp(Verb, "load") == 0 || stricmp(Verb, "l") == 0)
            RecentFilesAdd(Argument);
	}

	RunCommand(Command);
}


void MainDialogFireCommand()
{
	LPCTSTR Buffer = InputGet();
	if (Buffer == NULL)
        InputSet(":main");

	FireCommand();
}

void FlipExpand()
{
	MultilineText = !MultilineText;
	ShowWindow(GetDlgItem(G_hWnd, ID_ENTER), (MultilineText ? SW_SHOW : SW_HIDE));

    TBBUTTONINFO tbi;
    tbi.cbSize = sizeof(tbi);
	tbi.dwMask = TBIF_IMAGE;
	tbi.iImage = (MultilineText ? 1 : 0);
	SendDlgItemMessage(G_hWnd, ID_EXPAND, TB_SETBUTTONINFO, ID_EXPAND, (LPARAM) &tbi);

	MainDialogResize();

}

void MainDialogCommand(int ID, WPARAM wParam, LPARAM lParam)
{
	switch (ID)
	{
	case ID_RUN:
		MainDialogFireCommand();
		break;

	case ID_STOP:
		AbortExecution();
		break;

    case rtfInput:
        if (HIWORD(wParam) == EN_CHANGE)
            InputChanged();
        break;

	case ID_EXPAND:
		FlipExpand();
		break;

	case IDCANCEL:
		EndDialog(G_hWnd, 0);
		break;
	}
}

BOOL MainDialogNotify(LPNMHDR nmhdr)
{
	if (nmhdr->idFrom == rtfInput)
		return InputNotify(nmhdr);
	else if (nmhdr->idFrom == ID_TOOLBAR || nmhdr->code == TTN_GETDISPINFO)
		return ToolbarNotify(nmhdr);
	return FALSE;
}

void MainDialogDropFiles(HDROP hDrop)
{
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
}

INT_PTR CALLBACK MainDialogProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_INITDIALOG:
		G_hWnd = hWnd;
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

	case WM_TIMER:
	    OutputTimer();
	    break;

	case WM_CLOSE:
        RegistryWriteWindowPos(hWnd);
		PostQuitMessage(0);
		break;
	}
    return FALSE;
}


void ShowMainDialog()
{
    CreateDialog(G_hInstance, MAKEINTRESOURCE(DLG_MAIN), NULL, &MainDialogProc);
}