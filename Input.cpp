#include "Header.h"
#include "Input.h"
#include "History.h"

TCHAR* Buffer = NULL;
HWND hInput;

void InputInit(HWND hInput)
{
	::hInput = hInput;

    //make it all protected
	//might want to enable SELCHANGE later, to reduce SETSEL calls
    SendMessage(hInput, EM_SETEVENTMASK, 0, ENM_KEYEVENTS | ENM_SELCHANGE);

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
