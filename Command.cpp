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
#include "Actions.h"
#include "Profile.h"

void Application::CommandRun(LPCTSTR Command)
{
	input->Set(Command);
	CommandRun();
}

void Application::CommandRun()
{
    TCHAR Buffer[500];
    input->Get(Buffer);

	if (Buffer[0] == 0)
	{
		strcpy(Buffer, ":main");
		input->Set(Buffer);
	}

	input->SelAll();

	output->FormatReset();
    history->Add(Buffer);

	Action a(Buffer);

	bool Success = true;
	
	switch (a.Code)
	{
	case actUnknown: case actBlank: case actShell:
		Warning("I don't know how to execute this command...");
		Success = false;
		break;

	case actExpression:
		Success = interpreter->Expression(Buffer);
		break;

	case ackRunProfile:
		output->AppendLex(a.Orig);
		output->Append("\n");
		Success = ProfileRun(a.Argument);
		break;

	case actQuit:
		SendMessage(app->hWnd, WM_CLOSE, 0, 0);
		return;

	default:
		Success = interpreter->Execute(&a);
	}

	if (Success)
		app->RunningChanged(true);
	else
		app->ShowPrompt();
}

void Application::LoadFile(LPCTSTR File)
{
	TCHAR Buffer[MAX_PATH+100];
	wsprintf(Buffer, ":load \"%s\"", File);
	CommandRun(Buffer);
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
        Running = 0;
		CommandRun();
        break;

    case cmdStop:
        if (Running == 0)
            interpreter->AbortComputation();
        else
            ProfileAbort();
        break;

	case cmdOpen:
		CommandOpen();
		break;

	case cmdRunProfile:
        Running = 1;
		CommandRun(":main-profile");
		break;

    default:
		Warning("Command not handled, give me mon£y to implement this");
        ShowPrompt();
    }
}

HMENU Application::QueryCommand(Command c)
{
    if (c == cmdRecent)
        return recentFiles->GetMenu();
    else
        return NULL;
}
