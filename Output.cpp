/*
	OUTPUT
	Handle all outputting of everything
	Handle escape codes
	Handle buffering
*/
#include "Header.h"
#include "Output.h"

#define MAXIMUM_BUFFER   100000

HWND hRTF;

struct Format
{
    int ForeColor;
    int BackColor;
    bool Bold;
    bool Italic;
    bool Underline;
};

bool FormatChanged = false;
Format DefFormat = {0x000000, 0xFFFFFF, false, false, false};
Format BufFormat;
Format NowFormat;

const int BufSize = 995;
char Buf[1000];
int BufPos = 0; // where to write out in the buffer
int BufLen = 0; // how much of the buffer is useful
int OutputPos = 0; // how much to delete of the existing thing
bool IsTimer = false;

// buffer to hold an escape character
bool InEscBuf = false;
const int EscBufSize = 100;
char EscBuf[100];
int EscBufPos = 0;

DWORD Length = 0;
DWORD OutputStart;

HANDLE hMutex;


void EnsureTimer()
{
    if (!IsTimer) {
		IsTimer = TRUE;
		SetTimer(GetParent(hRTF), 666, 100, NULL);
    }
}

void DestTimer()
{
    KillTimer(GetParent(hRTF), 666);
    IsTimer = FALSE;
}


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
			ExecutionComplete();
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

void OutputTimer()
{
    // if you are doing useful work, why die?
    if (BufLen == 0)
	DestTimer();
    FlushBuffer(FALSE);
}

void RtfWindowPutS(LPCTSTR s)
{
    AddToBuffer(s);
}

void RtfEchoCommand(LPCTSTR s)
{
    RtfWindowPutS(s);
    RtfWindowPutS("\n");
}

void RtfWindowStartOutput()
{
    RtfWindowPutS("\n");
    RtfWindowFlushBuffer();
    BufFormat = DefFormat;
    NowFormat = DefFormat;
    OutputStart = RtfWindowTextLength();
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
