#include "Header.h"
#include "Application.h"
#include "Toolbar.h"

#include "Icon.h"
#include "Command.h"


/////////////////////////////////////////////////////////////////////
// Data loading section
#define TOOLBAR_BEGIN void Toolbar_Begin(Toolbar* toolbar){Toolbar_Data td;
#define TOOLBAR_END }

#define TAB_BEGIN(a,b) Tab_Begin(&td,a,b);
#define TAB_END Tab_End(&td, toolbar);

#define BUTTON(a,b,c) Add_Button(&(td.Data[td.Count++]),a,b,c);
#define SEPARATOR Add_Separator(&(td.Data[td.Count++]));
#define DROPPER(a,b,c) Add_Dropper(&(td.Data[td.Count++]),a,b,c);

struct Toolbar_Data
{
    LPCTSTR Name;
    bool HasRunStop;
    TBBUTTON Data[20];
    int Count;
};

void Tab_Begin(Toolbar_Data* td, LPCTSTR Name, bool HasRunStop)
{
    td->Count = 0;
    td->Name = Name;
    td->HasRunStop = HasRunStop;
}

void Add_Button(TBBUTTON* tb, LPCTSTR Name, Command c, Icon i)
{
    tb->iString = (INT_PTR) Name;
    tb->idCommand = c;
    tb->iBitmap = i;
    tb->fsStyle = TBSTYLE_BUTTON;
    tb->fsState = TBSTATE_ENABLED;
    tb->dwData = NULL;
}

void Add_Dropper(TBBUTTON* tb, LPCTSTR Name, Command c, Icon i)
{
    Add_Button(tb, Name, c, i);
    tb->fsStyle = BTNS_WHOLEDROPDOWN;
}

void Add_Separator(TBBUTTON* tb)
{
    Add_Button(tb, NULL, (Command) 0, (Icon) 0);
    tb->fsStyle = TBSTYLE_SEP;
}

void Tab_End(Toolbar_Data* td, Toolbar* toolbar)
{
    // Create the toolbar
    HWND hToolbar = CreateWindowEx(
		WS_EX_TRANSPARENT,
		TOOLBARCLASSNAME, NULL,
        WS_CHILD | TBSTYLE_TOOLTIPS | TBSTYLE_FLAT |  TBSTYLE_TRANSPARENT | CCS_NODIVIDER | CCS_NOPARENTALIGN | TBSTYLE_LIST | CCS_NOMOVEY | CCS_NORESIZE,
		20, 40, 600, 50,
        toolbar->hTab /*toolbar->hWnd*/, (HMENU) (int) 0, hInst, NULL);

	SendMessage(hToolbar, TB_SETEXTENDEDSTYLE, 0, TBSTYLE_EX_DRAWDDARROWS);

    SendMessage(hToolbar, TB_SETIMAGELIST, 0, (LPARAM) toolbar->hNormalIml);
    SendMessage(hToolbar, TB_SETDISABLEDIMAGELIST, 0, (LPARAM) toolbar->hDisableIml);

    SendMessage(hToolbar, TB_SETBITMAPSIZE, 0, MAKELONG(32,32));
    SendMessage(hToolbar, TB_ADDBUTTONS, td->Count, (LPARAM) td->Data);

    // Now resize each button appropriately
    HDC hDC = GetDC(hToolbar);
    SIZE sz;
    TBBUTTONINFO tbi;
    tbi.cbSize = sizeof(tbi);
    tbi.dwMask = TBIF_SIZE;
    for (int i = 0; i < td->Count; i++)
    {
        if (td->Data[i].fsStyle != TBSTYLE_SEP)
        {
            LPCTSTR str = (LPCTSTR) td->Data[i].iString;
            GetTextExtentPoint(hDC, str, (int) strlen(str), &sz);
            tbi.cx = (WORD) sz.cx + 40;
            SendMessage(hToolbar, TB_SETBUTTONINFO, td->Data[i].idCommand, (LPARAM) &tbi);
        }
    }
    ReleaseDC(hToolbar, hDC);


    // Set up the tabs
    TCITEM tci;
    tci.mask = TCIF_TEXT;
    tci.pszText = (LPTSTR) td->Name;
    TabCtrl_InsertItem(toolbar->hTab, toolbar->TabCount, &tci);

    if (toolbar->TabCount == 0)
        ShowWindow(hToolbar, SW_SHOW);

    // Move the data to the toolbar
    toolbar->Tabs[toolbar->TabCount].HasRunStop = td->HasRunStop;
    toolbar->Tabs[toolbar->TabCount].hToolbar = hToolbar;
    toolbar->TabCount++;
}


#include "ToolbarData.h"

INT_PTR CALLBACK ToolbarDialogProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

/////////////////////////////////////////////////////////////////////
// General section
Toolbar::Toolbar(HWND hParent)
{
    hWnd = CreateDialog(hInst, MAKEINTRESOURCE(CTL_TOOLBAR), hParent, ToolbarDialogProc);
    hTab = GetDlgItem(hWnd, TAB_LIST);
    SetWindowLongPtr(hWnd, DWL_USER, (LONG) this);

	hNormalIml = ImageList_Create(32, 32, ILC_COLOR24 | ILC_MASK, 0, 0);
	HBITMAP hBmp = LoadBitmap(hInst, MAKEINTRESOURCE(BMP_TOOLBAR));
    ImageList_AddMasked(hNormalIml, hBmp, RGB(255,0,255));
    DeleteObject(hBmp);

    // create the image list
    hDisableIml = ImageList_Create(32, 32, ILC_COLOR24 | ILC_MASK, 0, 0);
	hBmp = LoadBitmap(hInst, MAKEINTRESOURCE(BMP_TOOLBAR_DIS));
    ImageList_AddMasked(hDisableIml, hBmp, RGB(255,0,255));
    DeleteObject(hBmp);

    TabCount = 0;
    Toolbar_Begin(this);
    RunningChanged(false);
};

void Toolbar::RunningChanged(bool Running)
{
    TBBUTTONINFO tbi;
    tbi.cbSize = sizeof(tbi);
    tbi.dwMask = TBIF_STATE;
    for (int i = 0; i < TabCount; i++)
    {
        if (Tabs[i].HasRunStop)
        {
            int Enable  =  Running ? cmdStop : cmdRun;
            int Disable = !Running ? cmdStop : cmdRun;

            tbi.fsState = 0;
            SendMessage(Tabs[i].hToolbar, TB_SETBUTTONINFO, Disable, (LPARAM) &tbi);
            tbi.fsState = TBSTATE_ENABLED;
            SendMessage(Tabs[i].hToolbar, TB_SETBUTTONINFO, Enable, (LPARAM) &tbi);
        }
    }
}

Command Toolbar::DefaultCommand()
{
    int i = TabCtrl_GetCurSel(hTab);
    if (Tabs[i].HasRunStop)
        return cmdRun; // should give the correct runner
    else
        return cmdRun;
}

void ToolbarResize(Toolbar* toolbar)
{
    RECT rc;
    GetClientRect(toolbar->hWnd, &rc);
    MoveWindow(toolbar->hTab, 5, 5, rc.right - 10, rc.bottom - 10, TRUE);

    int i = TabCtrl_GetCurSel(toolbar->hTab);
    MoveWindow(toolbar->Tabs[i].hToolbar, 5, 25, rc.right - 20, 42, TRUE);
}

BOOL ToolbarNotify(Toolbar* toolbar, LPNMHDR nmhdr)
{
    if (nmhdr->code == TCN_SELCHANGE || nmhdr->code == TCN_SELCHANGING)
    {
        int i = TabCtrl_GetCurSel(nmhdr->hwndFrom);
        ShowWindow(toolbar->Tabs[i].hToolbar, (nmhdr->code == TCN_SELCHANGE ? SW_SHOW : SW_HIDE));
        ToolbarResize(toolbar);
    }
    else if (nmhdr->code == TBN_DROPDOWN)
    {
		LPNMTOOLBAR nmtoolbar = (LPNMTOOLBAR) nmhdr;
        HMENU hMenu = app->QueryCommand((Command) nmtoolbar->iItem);
        if (hMenu == NULL)
            return FALSE;

        RECT rc;
        SendMessage(nmhdr->hwndFrom, TB_GETRECT, nmtoolbar->iItem, (LPARAM) &rc);

		POINT pt;
		pt.x = rc.left;
		pt.y = rc.bottom;
		ClientToScreen(nmhdr->hwndFrom, &pt);

		TrackPopupMenu(hMenu,
			TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_VERTICAL,
            pt.x, pt.y, 0, toolbar->hWnd, NULL);
    }
    else if (nmhdr->code == TTN_GETDISPINFO)
    {
        /*
        LPTOOLTIPTEXT tt = (LPTOOLTIPTEXT) nmhdr;
        tt->hinst = NULL;
        for (int i = 0; Buttons[i] != 0; i++)
        {
            if (Buttons[i] == nmhdr->idFrom)
                strcpy(tt->szText, ButtonMsg[i]);
        }
        */
    }
    else if (nmhdr->code == NM_CLICK)
    {
        DWORD Pos = GetMessagePos();
        int Hot = SendMessage(nmhdr->hwndFrom, TB_GETHOTITEM, 0, 0);
        if (Hot != -1)
        {
            TBBUTTONINFO tbi;
            tbi.cbSize = sizeof(tbi);
            tbi.dwMask = TBIF_BYINDEX | TBIF_COMMAND | TBIF_STYLE;

            int Tab = TabCtrl_GetCurSel(toolbar->hTab);
            SendMessage(toolbar->Tabs[Tab].hToolbar, TB_GETBUTTONINFO, Hot, (LPARAM) &tbi);

            if (nmhdr->hwndFrom == toolbar->Tabs[Tab].hToolbar &&
                tbi.fsStyle == TBSTYLE_BUTTON)
            {
                app->FireCommand((Command) tbi.idCommand, 0);
            }
        }
    }
	return FALSE;
}

INT_PTR CALLBACK ToolbarDialogProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch(uMsg)
    {
    case WM_NOTIFY:
        return ToolbarNotify((Toolbar*) GetWindowLong(hWnd, DWL_USER), (LPNMHDR) lParam);
        break;

    case WM_COMMAND:
        app->FireCommand((Command) LOWORD(wParam), 0);
        break;

    case WM_SIZE:
        ToolbarResize((Toolbar*) GetWindowLong(hWnd, DWL_USER));
        break;
    }
    return FALSE;
}
