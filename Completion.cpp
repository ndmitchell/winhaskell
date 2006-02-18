#include "Header.h"
#include "Completion.h"

HWND hComplete;

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
    DestroyWindow(hComplete);
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

