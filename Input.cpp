#include "Header.h"
#include "Input.h"
#include "History.h"
#include "Lexer.h"

TCHAR* Buffer = NULL;
HWND hInput;

void InputInit(HWND hInput)
{
	::hInput = hInput;

    //make it all protected
	//might want to enable SELCHANGE later, to reduce SETSEL calls
    SendMessage(hInput, EM_SETEVENTMASK, 0, ENM_KEYEVENTS | ENM_SELCHANGE | ENM_CHANGE);

	CHARFORMAT cf;
    cf.cbSize = sizeof(CHARFORMAT);
    cf.dwMask = CFM_FACE | CFM_SIZE;
	strcpy(cf.szFaceName, "Courier New");
	cf.yHeight = 200;
    SendMessage(hInput, EM_SETCHARFORMAT, SCF_ALL, (LPARAM) &cf);
}

//Do not make anyone responsible for deleting the input buffer
//Return NULL if empty
LPCTSTR InputGet()
{
	if (Buffer != NULL)
		delete[] Buffer;

	int Size = GetWindowTextLength(hInput);
	Buffer = new TCHAR[Size+3];
	if (Size == 0)
		return NULL;
	else
		GetWindowText(hInput, Buffer, Size+1);
	return Buffer;
}

void InputChanged()
{
    CHARRANGE cr;
    SendMessage(hInput, EM_EXGETSEL, 0, (LPARAM) &cr);

    int Length = GetWindowTextLength(hInput);
    TCHAR* Buffer = new TCHAR[Length+2];
    GetWindowText(hInput, Buffer, Length+1);

    CHARFORMAT cf;
    cf.cbSize = sizeof(cf);
    cf.dwMask = CFM_COLOR | CFM_BOLD;
    cf.dwEffects = 0;

    //First do syntax colouring
    int Pos = 0;
    while (Buffer[Pos] != 0)
    {
        int LastPos = Pos;
        Lexeme l = GetLexeme(Buffer, &Pos);
        SendMessage(hInput, EM_SETSEL, LastPos, Pos);

        if (l != LexSpace)
        {
            if (l == LexKeyword)
                cf.crTextColor = BLUE;
            else if (l == LexOperator)
                cf.crTextColor = RED;
            else if (l == LexString)
                cf.crTextColor = CYAN;
            else
                cf.crTextColor = BLACK;
            SendMessage(hInput, EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM) &cf);
        }
    }

    //Now make all mismatched brackets bold
    MismatchedBrackets(Buffer);
    for (int i = 0; i < Length; i++)
    {
        cf.dwMask = CFM_BOLD;
        cf.dwEffects = CFE_BOLD;
        if (Buffer[i] == 0)
        {
            SendMessage(hInput, EM_SETSEL, i, i+1);
            SendMessage(hInput, EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM) &cf);
        }
    }

    delete[] Buffer;
    SendMessage(hInput, EM_EXSETSEL, 0, (LPARAM) &cr);
}

BOOL InputNotify(NMHDR* nmhdr)
{
	if (nmhdr->code == EN_MSGFILTER)
	{
		MSGFILTER* mf = (MSGFILTER*) nmhdr;
		if (mf->msg == WM_KEYDOWN)
		{
			bool Cancel = false;
			if (mf->wParam == VK_RETURN)
			{
				Cancel = true;
				MainDialogFireCommand();
			}
			else if (mf->wParam == VK_UP || mf->wParam == VK_DOWN)
			{
				Cancel = true;
				SetWindowText(hInput,
					HistoryGet(mf->wParam == VK_UP ? -1 : +1));
			}

			if (Cancel)
			{
				SetWindowLong(G_hWnd, DWL_MSGRESULT, 1);
				return TRUE;
			}
		}
	}
	return FALSE;
}
