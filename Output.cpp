/*
	OUTPUT
	Handle all outputting of everything
	Handle escape codes
	Handle buffering
*/
#include "Header.h"
#include "Output.h"
#include "Console.h"
#include "Application.h"
#include "Lexer.h"

const int OutputBufferSize = 100000;



/*

int RtfWindowTextLength()
{
    GETTEXTLENGTHEX gtl;
    gtl.codepage = CP_ACP;
    gtl.flags = GTL_DEFAULT;
    return (int) SendMessage(hRTF, EM_GETTEXTLENGTHEX, (WPARAM) &gtl, 0);
}

void WriteBuffer(LPCTSTR s, int Len)
{
    CHARRANGE cr;
    CHARFORMAT2 cf;
    Length = RtfWindowTextLength();

    cr.cpMin = max(OutputStart, Length + OutputPos);
    cr.cpMax = cr.cpMin + BufLen;
    SendMessage(hRTF, EM_EXSETSEL, 0, (LPARAM) &cr);

    cf.cbSize = sizeof(cf);
    cf.dwMask = CFM_COLOR | CFM_BACKCOLOR | CFM_BOLD | CFM_ITALIC | CFM_UNDERLINE;
    cf.dwEffects = 0;
    cf.crTextColor = BufFormat.ForeColor;
    cf.crBackColor = BufFormat.BackColor;
    cf.dwEffects = (BufFormat.Bold ? CFE_BOLD : 0) |
		   (BufFormat.Italic ? CFE_ITALIC : 0) |
		   (BufFormat.Underline ? CFE_UNDERLINE : 0);
    SendMessage(hRTF, EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM) &cf);
    // setcharformat seems to screw up the current selection!

    SendMessage(hRTF, EM_EXSETSEL, 0, (LPARAM) &cr);
    SendMessage(hRTF, EM_REPLACESEL, FALSE, (LPARAM) s);

    Length = RtfWindowTextLength();
    if (Length > MAXIMUM_BUFFER) {
	LPCSTR Blank = "";
	CHARRANGE cr;

	SendMessage(hRTF, EM_HIDESELECTION, TRUE, 0);

	cr.cpMin = 0;
	cr.cpMax = (Length - MAXIMUM_BUFFER) + (MAXIMUM_BUFFER / 4);
	SendMessage(hRTF, EM_EXSETSEL, 0, (LPARAM) &cr);

	SendMessage(hRTF, EM_REPLACESEL, FALSE, (LPARAM) Blank);

	cr.cpMin = -1;
	cr.cpMax = -1;
	SendMessage(hRTF, EM_EXSETSEL, 0, (LPARAM) &cr);

	SendMessage(hRTF, EM_HIDESELECTION, FALSE, 0);

	Length = RtfWindowTextLength();
    }
}

void FlushBuffer(BOOL Force)
{
    DWORD Res = WaitForSingleObject(hMutex, (Force ? INFINITE : 0));

    if (Res != WAIT_OBJECT_0)
	return; //you did not win

    if (BufLen != 0) {
	Buf[BufLen] = 0;
	Buf[BufLen+1] = 0;
	WriteBuffer(Buf, BufLen);
	OutputPos = BufPos - BufLen;
	BufPos = 0;
	BufLen = 0;
    }

    ReleaseMutex(hMutex);
}

void RtfWindowFlushBuffer()
{
    FlushBuffer(TRUE);
}

BOOL ParseEscapeCode(Format* f)
{
    int AnsiColor[8] = {BLACK, RED, GREEN, YELLOW, BLUE,
			MAGENTA, CYAN, WHITE};
    char* s;
    int i;

    EscBuf[EscBufPos] = 0;
    if (EscBuf[0] != '[')
	return FALSE;

    s = &EscBuf[1];
    for (i = 1; i <= EscBufPos; i++) {
	if (EscBuf[i] == ';')
	    EscBuf[i] = 0;

	if (EscBuf[i] == 0) {
	    int Val = atoi(s);
	    s = &EscBuf[i+1];

		if (Val == 50)
			app->ExecutionComplete();
		else if (Val == 0)
		*f = DefFormat;
	    else if (Val == 1)
		f->Bold = TRUE;
	    else if (Val == 4)
		f->Underline = TRUE;
	    else if (Val >= 30 && Val <= 37)
		f->ForeColor = AnsiColor[Val - 30];
	    else if (Val >= 40 && Val <= 47)
		f->BackColor = AnsiColor[Val - 40];
	}
    }
    return TRUE;
}

// need to copy from s to Buf
void AddToBuffer(LPCTSTR s)
{
    if (FormatChanged) {
	if (NowFormat.BackColor != BufFormat.BackColor ||
	    NowFormat.ForeColor != BufFormat.ForeColor ||
	    NowFormat.Bold      != BufFormat.Bold      ||
	    NowFormat.Underline != BufFormat.Underline ||
	    NowFormat.Italic    != BufFormat.Italic    )
	{
	    FlushBuffer(TRUE);
	    BufFormat = NowFormat;
	}
	FormatChanged = FALSE;
    }

    if (InEscBuf) {
	for (; *s != 0; s++) {
	    if (*s == 'm') {
		Format f = NowFormat;
		if (ParseEscapeCode(&f)) {
		    FormatChanged = TRUE;
		    NowFormat = f;
		}
		InEscBuf = FALSE;
		AddToBuffer(s+1);
		return;
	    } else if ((*s >= '0' && *s <= '9') ||
		(*s == ';') || (*s == '[')) {
		EscBuf[EscBufPos++] = *s;
		EscBufPos = min(EscBufPos, EscBufSize);
	    } else {
		InEscBuf = FALSE;
		AddToBuffer(EscBuf);
		break;
	    }
	}
    }

    for (; *s != 0; s++) {
	if (*s == '\b') {
	    if (BufPos == 0) {
		OutputPos--;
	    } else
		BufPos--;
	} else if (*s == 27) {
	    InEscBuf = TRUE;
	    EscBufPos = 0;
	    AddToBuffer(s+1);
	    return;
	} else {
	    if (BufLen >= BufSize)
		FlushBuffer(TRUE);
	    Buf[BufPos++] = *s;
	    BufLen = max(BufLen, BufPos);
	}
    }

    EnsureTimer();
}

void RtfWindowStartInput()
{
    CHARRANGE cr;
    CHARFORMAT cf;
    cf.cbSize = sizeof(cf);
    cf.dwMask = CFM_COLOR;
    cf.dwEffects = 0;
    cf.crTextColor = BLACK;

    cr.cpMin = Length;
    cr.cpMax = -1;
    SendMessage(hRTF, EM_EXSETSEL, 0, (LPARAM) &cr);

    SendMessage(hRTF, EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM) &cf);

    cr.cpMax = cr.cpMin;
    SendMessage(hRTF, EM_EXSETSEL, 0, (LPARAM) &cr);
}

int WinHugsColor(int Color)
{
    int PrevColor = NowFormat.ForeColor;
    FormatChanged = TRUE;
    NowFormat = DefFormat;
    NowFormat.ForeColor = Color;
    InEscBuf = FALSE;
    return PrevColor;
}



void OutputInit(HWND hRTF)
{
	::hRTF = hRTF;

    //make it all protected
	//might want to enable SELCHANGE later, to reduce SETSEL calls
    // SendMessage(hRTF, EM_SETEVENTMASK, 0,
	// ENM_PROTECTED | ENM_LINK | ENM_KEYEVENTS | ENM_SELCHANGE);

    // Allow them 1 million characters
    // the system will sort out overflows later
    SendMessage(hRTF, EM_LIMITTEXT, 1000000, 0);

    // Default formatting information
    BufFormat = DefFormat;
    NowFormat = DefFormat;
	FormatChanged = true;

	hMutex = CreateMutex(NULL, FALSE, NULL);

	CHARFORMAT cf;
    cf.cbSize = sizeof(CHARFORMAT);
    cf.dwMask = CFM_FACE | CFM_SIZE;
	strcpy(cf.szFaceName, "Courier New");
	cf.yHeight = 200;
    SendMessage(hRTF, EM_SETCHARFORMAT, SCF_ALL, (LPARAM) &cf);
}

void OutputColor(int Color)
{
    FormatChanged = true;
    NowFormat = DefFormat;
    NowFormat.ForeColor = Color;
    InEscBuf = false;
}

void OutputBold(bool Bold)
{
	CHARFORMAT cf;
    cf.cbSize = sizeof(cf);
	cf.dwMask = CFM_BOLD;
	cf.dwEffects = (Bold ? CFE_BOLD : 0);

    SendMessage(hRTF, EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM) &cf);
}

void OutputAppend(LPCTSTR Text)
{
	RtfWindowPutS(Text);
	//	SendMessage(hRTF, EM_SETSEL, -1, -1);
	//  SendMessage(hRTF, EM_REPLACESEL, FALSE, (LPARAM) Text);
}

void OutputCopy(HWND hCopy)
{
    int Length = GetWindowTextLength(hCopy);
    LPTSTR Buffer = new TCHAR[Length+2];
    GetWindowText(hCopy, Buffer, Length+1);

    CHARFORMAT cf;
    cf.cbSize = sizeof(cf);
    cf.dwMask = CFM_BOLD | CFM_COLOR;

    TCHAR Buf2[2];
    Buf2[1] = 0;

    for (int i = 0; i < Length; i++)
    {
        Buf2[0] = Buffer[i];
        SendMessage(hCopy, EM_SETSEL, i, i+1);
        SendMessage(hCopy, EM_GETCHARFORMAT, SCF_SELECTION, (LPARAM) &cf);
        SendMessage(hRTF, EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM) &cf);
        SendMessage(hRTF, EM_REPLACESEL, FALSE, (LPARAM) Buf2);
    }
}
*/

// Output class

INT_PTR CALLBACK OutputDialogProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_SIZE:
        {
            RECT rc;
            GetClientRect(hWnd, &rc);
			MoveWindow(output->hRTF, 0, 0, rc.right, rc.bottom, TRUE);
        }
        break;
    }
    return FALSE;
}

Output::Output(HWND hParent)
{
	Length = 0;
    hWnd = CreateDialog(hInst, MAKEINTRESOURCE(CTL_OUTPUT), hParent, OutputDialogProc);
    hRTF = GetDlgItem(hWnd, rtfOutput);

    SendMessage(hRTF, EM_LIMITTEXT, 1000000, 0);

	CHARFORMAT cf;
    cf.cbSize = sizeof(CHARFORMAT);
    cf.dwMask = CFM_FACE | CFM_SIZE | CFM_COLOR;
	strcpy(cf.szFaceName, "Courier New");
	cf.yHeight = 200;
    SendMessage(hRTF, EM_SETCHARFORMAT, SCF_ALL, (LPARAM) &cf);
}

Output::~Output()
{
}

void Output::AppendLexeme(Lexeme l, LPCTSTR Str)
{
    switch (l)
    {
    case LexKeyword:
        SetForecolor(BLUE);
        break;

    case LexOperator: case LexError:
        SetForecolor(RED);
        break;

    case LexString:
        SetForecolor(CYAN);
        break;

    default:
        SetForecolor(BLACK);
    }
    if (l == LexCommand)
        SetBold(true);
    else if (l == LexError)
        SetUnderline(true);

    Append(Str);

    if (l == LexCommand)
        SetBold(false);
    else if (l == LexError)
        SetUnderline(false);
}

void Output::AppendLex(LexList* ll)
{
    SelEnd();
    for (LexList* i = ll; i != NULL; i = i->Next)
        AppendLexeme(i->Lex, i->Str);
    FormatReset();
}

void Output::AppendLex(LPCTSTR Text)
{
    Lexer lex(Text);
    SelEnd();
    for (LexToken* lt = lex.Next(); lt != NULL; lt = lex.Next())
        AppendLexeme(lt->Lex, lt->Str);
    FormatReset();
}

void Output::Append(LPCTSTR Text)
{
	SelEnd();
	SendMessage(hRTF, EM_REPLACESEL, FALSE, (LPARAM) Text);

	/*

    Length = RtfWindowTextLength();
    if (Length > MAXIMUM_BUFFER) {
	LPCSTR Blank = "";
	CHARRANGE cr;

	SendMessage(hRTF, EM_HIDESELECTION, TRUE, 0);

	cr.cpMin = 0;
	cr.cpMax = (Length - MAXIMUM_BUFFER) + (MAXIMUM_BUFFER / 4);
	SendMessage(hRTF, EM_EXSETSEL, 0, (LPARAM) &cr);

	SendMessage(hRTF, EM_REPLACESEL, FALSE, (LPARAM) Blank);

	cr.cpMin = -1;
	cr.cpMax = -1;
	SendMessage(hRTF, EM_EXSETSEL, 0, (LPARAM) &cr);

	SendMessage(hRTF, EM_HIDESELECTION, FALSE, 0);

	Length = RtfWindowTextLength();
    }*/
}

void Output::SelEnd()
{
	SendMessage(hRTF, EM_SETSEL, -1, -1);
}

void Output::SetCharFormat(CHARFORMAT* cf, DWORD Mask)
{
	cf->cbSize = sizeof(CHARFORMAT);
	cf->dwMask = Mask;
	SelEnd();
	SendMessage(hRTF, EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM) cf);
}

void Output::SetBold(bool Bold)
{
	CHARFORMAT cf;
	cf.dwEffects = (Bold ? CFE_BOLD : 0);
	SetCharFormat(&cf, CFM_BOLD);
}

void Output::SetUnderline(bool Underline)
{
	CHARFORMAT cf;
	cf.dwEffects = (Underline ? CFE_UNDERLINE : 0);
	SetCharFormat(&cf, CFM_UNDERLINE);
}

void Output::SetForecolor(int Color)
{
	CHARFORMAT cf;
	cf.dwEffects = 0;
	cf.crTextColor = Color;
	SetCharFormat(&cf, CFM_COLOR);
}

void Output::SetBackcolor(int Color)
{
	CHARFORMAT2 cf2;
	cf2.cbSize = sizeof(cf2);
	cf2.dwMask = CFM_BACKCOLOR;
	cf2.dwEffects = 0;
	cf2.crBackColor = Color;
	SelEnd();
	SendMessage(hRTF, EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM) &cf2);
}

void Output::Escape(ConsoleEscape Code)
{
    int AnsiColor[8] = {
		BLACK, RED, GREEN, YELLOW, BLUE,
		MAGENTA, CYAN, WHITE};

	if (Code == conBold)
		SetBold(true);
	else if (Code == conUnderline)
		SetUnderline(true);
	else if (Code >= conForeground && Code <= conForeground+8)
		SetForecolor(AnsiColor[Code-conForeground]);
	else if (Code >= conBackground && Code <= conBackground+8)
		SetBackcolor(AnsiColor[Code-conBackground]);
	else if (Code == conReset)
		FormatReset();
	else if (Code == conBackspace)
		Rewind();
}

void Output::FormatReset()
{
	OutputFormat of;
	of.ForeColor = BLACK;
	of.BackColor = WHITE;
	of.Bold = false;
	of.Underline = false;
	FormatSet(&of);
}

void Output::FormatSet(OutputFormat* of)
{
	CHARFORMAT2 cf2;
	cf2.cbSize = sizeof(cf2);
	cf2.dwEffects = (of->Bold ? CFE_BOLD : 0) | (of->Underline ? CFE_UNDERLINE : 0);
	cf2.dwMask = CFM_BOLD | CFM_UNDERLINE | CFM_COLOR | CFM_BACKCOLOR;
	cf2.crBackColor = of->BackColor;
	cf2.crTextColor = of->ForeColor;
	SelEnd();
	SendMessage(hRTF, EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM) &cf2);
}

void Output::FormatGet(OutputFormat* of)
{
	CHARFORMAT2 cf2;
	cf2.cbSize = sizeof(cf2);
	cf2.dwMask = CFM_BOLD | CFM_UNDERLINE | CFM_COLOR | CFM_BACKCOLOR;
	SelEnd();
	SendMessage(hRTF, EM_GETCHARFORMAT, SCF_SELECTION, (LPARAM) &cf2);
	of->ForeColor = cf2.crTextColor;
	of->BackColor = cf2.crBackColor;
	of->Bold = (cf2.dwEffects & CFM_BOLD ? true : false);
	of->Underline = (cf2.dwEffects & CFM_UNDERLINE ? true : false);
}
