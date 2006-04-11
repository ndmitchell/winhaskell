#include "Header.h"
#include "Input.h"
#include "History.h"
#include "Lexer.h"
#include "Commands.h"
#include "Completion.h"
#include "Application.h"

int CharWidth;

const int MaxInputSize = 500;

const bool ShowAutoComplete = true;

// The active completion objects
Completion* CompCommand = NULL;
Completion* CompFile = NULL;
Completion* CompCode = NULL;

Completion* Active = NULL;
LexItem CompItem;


INT_PTR CALLBACK InputDialogProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);


Input::Input(HWND hParent)
{
	hWnd = CreateDialog(hInst, MAKEINTRESOURCE(CTL_INPUT), hParent, InputDialogProc);
    hRTF = GetDlgItem(hWnd, rtfInput);

    //make it all protected
	//might want to enable SELCHANGE later, to reduce SETSEL calls
    SendMessage(hRTF, EM_SETEVENTMASK, 0, ENM_KEYEVENTS | ENM_SELCHANGE | ENM_CHANGE);

	CHARFORMAT cf;
    cf.cbSize = sizeof(CHARFORMAT);
    cf.dwMask = CFM_FACE | CFM_SIZE;
	strcpy(cf.szFaceName, "Courier New");
	cf.yHeight = 200;
    SendMessage(hRTF, EM_SETCHARFORMAT, SCF_ALL, (LPARAM) &cf);

    // Give lots of space for buffer manips
    SendMessage(hRTF, EM_LIMITTEXT, MaxInputSize - 50, 0);

    HDC hDC = GetDC(hRTF);
    SIZE sz;
    BOOL b = GetTextExtentPoint(hDC, "WINHASKELL", 10, &sz);
    CharWidth = (b ? sz.cx / 10 : 10);
    ReleaseDC(hRTF, hDC);
}

void Input::Get(LPTSTR Buffer)
{
	GetWindowText(hRTF, Buffer, MaxInputSize);
}

void Input::Set(LPCTSTR Buffer)
{
    SetWindowText(hRTF, Buffer);
}

void Input::SelAll()
{
    SendMessage(hRTF, EM_SETSEL, 0, -1);
}

void InputChanged(HWND hInput)
{
    CHARRANGE cr;
    SendMessage(hInput, EM_EXGETSEL, 0, (LPARAM) &cr);

    TCHAR Buffer[MaxInputSize];
    GetWindowText(hInput, Buffer, MaxInputSize);
    int Length = GetWindowTextLength(hInput);

    LexItem Items[250];
    int LexItems = GetLexemes(Buffer, Items, 250);

    CHARFORMAT cf;
    cf.cbSize = sizeof(cf);
    cf.dwMask = CFM_COLOR | CFM_BOLD;
    cf.dwEffects = 0;

    //First do syntax colouring
    for (int i = 0; i < LexItems; i++)
    {
        SendMessage(hInput, EM_SETSEL, Items[i].Start, Items[i].End);

        cf.dwEffects = 0;
        if (Items[i].Lex == LexKeyword)
            cf.crTextColor = BLUE;
        else if (Items[i].Lex == LexOperator)
            cf.crTextColor = RED;
        else if (Items[i].Lex == LexString)
            cf.crTextColor = CYAN;
        else
        {
            cf.crTextColor = BLACK;
            if (Items[i].Lex == LexCommand)
                cf.dwEffects = CFE_BOLD;
        }
        SendMessage(hInput, EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM) &cf);
    }

    //Now make all mismatched brackets bold
    MismatchedBrackets(Buffer);
    cf.dwEffects = CFE_BOLD;
	cf.crTextColor = BRIGHTRED;
    for (int i = 0; i < Length; i++)
    {
        if (Buffer[i] == 0)
        {
            SendMessage(hInput, EM_SETSEL, i, i+1);
            SendMessage(hInput, EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM) &cf);
        }
    }

    //Now give command help if appropriate
    bool ShowComplete = false;
    Action* c = GetCommand(Buffer);
    if (c != NULL)
    {
        SetStatusBar(c->Help);

        int Pos = 0;
        if (LexItems > 0 && Items[0].Lex == LexCommand &&
            cr.cpMin >= Items[0].Start && cr.cpMax <= Items[0].End)
        {
            ShowComplete = true;

            if (CompCommand == NULL)
            {
                CompCommand = new Completion(NULL);
                CommandsCompletion(CompCommand);
            }

            RECT rc;
            GetWindowRect(hInput, &rc);
            POINT pt = {rc.left + (CharWidth * Items[0].Start), rc.top};

            CompCommand->Move(pt);
            if (ShowAutoComplete)
                CompCommand->Show();

            Buffer[Items[0].End] = 0;
            CompCommand->SetCurrent(&Buffer[Items[0].Start]);
            Active = CompCommand;
            CompItem = Items[0];
        }
    }
    else
        SetStatusBar("Type an expression now");

    if (CompCommand != NULL && !ShowComplete)
    {
        CompCommand->Hide();
        Active = NULL;
    }

    SendMessage(hInput, EM_EXSETSEL, 0, (LPARAM) &cr);
}

void CompletionFinish(HWND hInput)
{
    TCHAR Buffer[100];
    Active->GetCurrent(Buffer);

    SendMessage(hInput, EM_SETSEL, CompItem.Start, CompItem.End);
    SendMessage(hInput, EM_REPLACESEL, FALSE, (LPARAM) Buffer);

    Active->Hide();
    Active = NULL;
}

BOOL InputNotify(HWND hWnd, LPNMHDR nmhdr)
{
	bool Cancel = false;

	if (nmhdr->code == EN_MSGFILTER)
	{
		MSGFILTER* mf = (MSGFILTER*) nmhdr;
		if (mf->msg == WM_KEYDOWN)
		{
			if (mf->wParam == VK_RETURN)
			{
				Cancel = true;
                if (Active == NULL)
				    app->DefaultCommand();
                else
                    CompletionFinish(nmhdr->hwndFrom);
			}
			else if (mf->wParam == VK_UP || mf->wParam == VK_DOWN)
			{
				Cancel = true;
                int Dir = (mf->wParam == VK_UP ? -1 : +1);
                if (Active == NULL)
                {
                    SetWindowText(nmhdr->hwndFrom, app->history->Get(Dir));
                    app->input->SelAll();
                }
                else
                    Active->SetCurrentDelta(Dir);
			}
		}
        else if (mf->msg == WM_CHAR && mf->wParam == VK_TAB)
        {
            if (mf->wParam == VK_TAB && Active != NULL)
            {
                Cancel = true;
                CompletionFinish(nmhdr->hwndFrom);
            }
        }
	}

	if (Cancel)
	{
        SetWindowLong(hWnd, DWL_MSGRESULT, 1);
		return TRUE;
	}
	return FALSE;
}


INT_PTR CALLBACK InputDialogProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch(uMsg)
    {
    case WM_SIZE:
        {
            RECT rc;
            GetClientRect(hWnd, &rc);
            MoveWindow(GetDlgItem(hWnd, rtfInput), 0, 0, rc.right, rc.bottom, TRUE);
        }
        break;

    case WM_COMMAND:
        if (HIWORD(wParam) == EN_CHANGE)
            InputChanged((HWND) lParam);
        break;

    case WM_NOTIFY:
        return InputNotify(hWnd, (LPNMHDR) lParam);
        break;
    }
    return FALSE;
}

