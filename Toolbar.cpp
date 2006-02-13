#include "Header.h"
#include "RecentFiles.h"

HWND hToolbar;

int Buttons[] = {
    // -1 is a separator, 0 is the end
    ID_OPEN, -1,
	ID_RUN, ID_STOP, -1,
	ID_OPTIONS,
	0
};
LPCTSTR ButtonMsg[] = {
    "Load", NULL, "Run", "Stop", NULL, "Options"
};

void EnableButton(int id, bool Enable)
{
    TBBUTTONINFO tbi;
    tbi.cbSize = sizeof(tbi);
    tbi.dwMask = TBIF_STATE;
    tbi.fsState = (Enable ? TBSTATE_ENABLED : 0);
	SendMessage(hToolbar, TB_SETBUTTONINFO, id, (LPARAM) &tbi);
}

void ToolbarInit()
{
	int i;
    int AnyButtons = 0, RealButtons = 0;

    for (AnyButtons = 0; Buttons[AnyButtons] != 0; AnyButtons++)
		; // no code required

    TBBUTTON* TbButtons = new TBBUTTON[AnyButtons];
    for (i = 0; i < AnyButtons; i++)
	{
		if (Buttons[i] == -1)
		{
			TbButtons[i].iBitmap = 0;
			TbButtons[i].fsStyle = BTNS_SEP;
			TbButtons[i].idCommand = 0;
		}
		else
		{
			TbButtons[i].iBitmap = RealButtons;
			RealButtons++;
			TbButtons[i].idCommand = Buttons[i];
			TbButtons[i].fsStyle = (Buttons[i] == ID_OPEN || Buttons[i] == ID_OPTIONS ? TBSTYLE_DROPDOWN : TBSTYLE_BUTTON);
		}
		TbButtons[i].fsState = TBSTATE_ENABLED;
		TbButtons[i].dwData = (DWORD_PTR) NULL;
		TbButtons[i].iString = (INT_PTR) NULL; //ButtonMsg[i];
    }

    hToolbar = CreateWindowEx(
		0,
		TOOLBARCLASSNAME, NULL,
		TBSTYLE_TOOLTIPS | WS_CHILD | WS_VISIBLE | CCS_NODIVIDER | TBSTYLE_FLAT | TBSTYLE_LIST,
		// TBSTYLE_TOOLTIPS | WS_CHILD | WS_VISIBLE | TBSTYLE_WRAPABLE | /*CCS_NORESIZE |*/ CCS_NODIVIDER | TBSTYLE_FLAT,
		0, 0, 600, 40,
		G_hWnd, (HMENU) (int) ID_TOOLBAR, G_hInstance, NULL);

	SendMessage(hToolbar, TB_SETEXTENDEDSTYLE, 0, TBSTYLE_EX_DRAWDDARROWS);

    // create the image list
	HIMAGELIST hImgList = ImageList_Create(32, 32, ILC_COLOR24 | ILC_MASK, RealButtons, RealButtons);
	HBITMAP hBmp = LoadBitmap(G_hInstance, MAKEINTRESOURCE(BMP_TOOLBAR));
    ImageList_AddMasked(hImgList, hBmp, RGB(255,0,255));
    DeleteObject(hBmp);
    SendMessage(hToolbar, TB_SETIMAGELIST, 0, (LPARAM) hImgList);

    // create the image list
    hImgList = ImageList_Create(32, 32, ILC_COLOR24 | ILC_MASK, RealButtons, RealButtons);
	hBmp = LoadBitmap(G_hInstance, MAKEINTRESOURCE(BMP_TOOLBAR_DIS));
    ImageList_AddMasked(hImgList, hBmp, RGB(255,0,255));
    DeleteObject(hBmp);
	SendMessage(hToolbar, TB_SETDISABLEDIMAGELIST, 0, (LPARAM) hImgList);

    // setup the toolbar properties
    SendMessage(hToolbar, TB_SETBUTTONSIZE, 0, MAKELONG(42,42));
    SendMessage(hToolbar, TB_SETBITMAPSIZE, 0, MAKELONG(32,32));
    SendMessage(hToolbar, TB_ADDBUTTONS, AnyButtons, (LPARAM) TbButtons);
    delete[] TbButtons;

	MoveWindow(hToolbar, 0, 0, 600, 80, TRUE);
}

BOOL ToolbarNotify(LPNMHDR nmhdr)
{
	if (nmhdr->code == TBN_DROPDOWN)
	{
		LPNMTOOLBAR nmtoolbar = (LPNMTOOLBAR) nmhdr;
        HMENU hItem;
		if (nmtoolbar->iItem == ID_OPEN)
            hItem = RecentFilesMenu();
        else if (nmtoolbar->iItem == ID_OPTIONS)
        {
            hItem = GetSubMenu(LoadMenu(G_hInstance, MAKEINTRESOURCE(MNU_COMPILER)), 0);
            CheckMenuRadioItem(hItem, 0, 0, 0, MF_BYPOSITION);
        }

        RECT rc;
		SendMessage(hToolbar, TB_GETRECT, nmtoolbar->iItem, (LPARAM) &rc);

		POINT pt;
		pt.x = rc.left;
		pt.y = rc.bottom;
		ClientToScreen(hToolbar, &pt);

		TrackPopupMenu(hItem,
			TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_VERTICAL,
			pt.x, pt.y, 0, G_hWnd, NULL);
	}
	return FALSE;
}

