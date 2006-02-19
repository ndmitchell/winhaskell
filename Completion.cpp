#include "Header.h"
#include "Completion.h"

INT_PTR CALLBACK CompletionProc(HWND hwndDlg,
    UINT uMsg,
    WPARAM wParam,
    LPARAM lParam
)
{
    return FALSE;
}

Completion::Completion(CompletionCallback Callback)
{
    this->Callback = Callback;
    hWnd = CreateDialog(G_hInstance,
        MAKEINTRESOURCE(DLG_COMPLETION),
        G_hWnd, CompletionProc);
    hLst = GetDlgItem(hWnd, LST_COMPLETION);
}

Completion::~Completion()
{
    DestroyWindow(hWnd);
}

void Completion::Hide()
{
    ShowWindow(hWnd, SW_HIDE);
}

void Completion::Show()
{
    ShowWindow(hWnd, SW_SHOWNOACTIVATE);
}

void Completion::Add(LPCTSTR Str)
{
    SendMessage(hLst, LB_ADDSTRING, 0, (LPARAM) Str);
}

void Completion::SetCurrent(LPCTSTR Str)
{
    int Index = (int) SendMessage(hLst, LB_FINDSTRING, -1, (LPARAM) Str);
    if (Index != LB_ERR)
    {
        SendMessage(hLst, LB_SETTOPINDEX, Index - 3, 0);
        SendMessage(hLst, LB_SETCURSEL, Index, 0);
    }
}

void Completion::SetCurrentDelta(int Delta)
{
    int Index = (int) SendMessage(hLst, LB_GETCURSEL, 0, 0);
    if (Index == -1) return;
    int Max = (int) SendMessage(hLst, LB_GETCOUNT, 0, 0);

    Index += Delta;
    if (Index >= Max || Index < 0) return;
    SendMessage(hLst, LB_SETCURSEL, Index, 0);
}

void Completion::GetCurrent(LPTSTR Buffer)
{
    int Index = (int) SendMessage(hLst, LB_GETCURSEL, 0, 0);
    SendMessage(hLst, LB_GETTEXT, Index, (LPARAM) Buffer);
}

void Completion::Move(POINT pt)
{
    RECT rc;
    GetWindowRect(hWnd, &rc);
    SetWindowPos(hWnd, NULL, pt.x, pt.y + rc.top - rc.bottom, 0, 0,
        SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
}

