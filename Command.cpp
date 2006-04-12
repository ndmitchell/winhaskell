#include "Header.h"
#include "Application.h"
#include "Input.h"
#include "Output.h"
#include "History.h"
#include "Console.h"
#include "Interpreter.h"
#include "Toolbar.h"
#include "Command.h"
#include "RecentFiles.h"


void Application::CommandRun()
{
    TCHAR Buffer[500];
    input->Get(Buffer);
	output->FormatReset();
    output->Append(Buffer);
    output->Append("\n");
    input->SelAll();
    history->Add(Buffer);
    interpreter->Evaluate(Buffer);
    toolbar->RunningChanged(true);
}

void Application::LoadFile(LPCTSTR File)
{
	TCHAR Buffer[MAX_PATH+100];
	wsprintf(Buffer, ":load \"%s\"", File);
	input->Set(Buffer);
	CommandRun();
}

void Application::CommandOpen()
{
	TCHAR Buffer[MAX_PATH];
	Buffer[0] = 0;

	OPENFILENAME ofn = {0};
    ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hInstance = hInst;
	ofn.hwndOwner = app->hWnd;
    ofn.lpstrFilter = "Haskell Files (*.hs;*.lhs)\0*.hs;*.lhs\0All Files (*.*)\0*.*\0";
    ofn.nFilterIndex = 1;
    ofn.lpstrFile= Buffer;
    ofn.nMaxFile = MAX_PATH;
    ofn.lpstrFileTitle = NULL;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_EXPLORER;
    ofn.lpfnHook = NULL;
    ofn.lpstrInitialDir = NULL;
    if (!GetOpenFileName(&ofn))
		return;

	LoadFile(Buffer);
}


void Application::FireCommand(Command c, int Param)
{
    switch (c)
    {
    case cmdRun:
		CommandRun();
        break;

    case cmdStop:
        interpreter->AbortComputation();
        break;

	case cmdOpen:
		CommandOpen();
		break;

    default:
		OutputFormat of;
		output->FormatGet(&of);
		output->SetForecolor(RED);
		output->Append("\nWarning: Command not handled, give me mon£y to implement this\n");
		output->FormatSet(&of);
    }
}

HMENU Application::QueryCommand(Command c)
{
    if (c == cmdRecent)
        return recentFiles->GetMenu();
    else
        return NULL;
}
