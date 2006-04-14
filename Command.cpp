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

void Application::CommandRun(LPCTSTR Command)
{
	input->Set(Command);
	CommandRun();
}

void Application::CommandRun()
{
    TCHAR Buffer[500];
    input->Get(Buffer);
	input->SelAll();

	output->FormatReset();
    output->AppendLex(Buffer);
    output->Append("\n");
    history->Add(Buffer);

	Action a(Buffer);
	
	switch (a.Code)
	{
	case actUnknown: case actBlank: case actShell:
		Warning("I don't know how to execute this command...");
		return;

	case actExpression:
		interpreter->Evaluate(Buffer);
		break;

	case actQuit:
		SendMessage(app->hWnd, WM_CLOSE, 0, 0);
		return;

	default:
		if (!interpreter->Execute(&a))
        {
            output->FormatReset();
            output->SetForecolor(GREEN);
            output->Append(interpreter->GetLastPrompt());
			return;
        }
	}

    app->RunningChanged(true);
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
		CommandRun();
        break;

    case cmdStop:
        interpreter->AbortComputation();
        break;

	case cmdOpen:
		CommandOpen();
		break;

    default:
		Warning("Command not handled, give me mon£y to implement this");
    }
}

HMENU Application::QueryCommand(Command c)
{
    if (c == cmdRecent)
        return recentFiles->GetMenu();
    else
        return NULL;
}
