#include "Header.h"
#include "Console.h"
#include "Interpreter.h"
#include "Actions.h"
#include "RecentFiles.h"

#include "Output.h"
#include "Application.h"
#include "String.h"
#include "Mutex.h"

LPCTSTR InterpreterPrompt = "\x1B[50m%s> \x1B[51m";


Interpreter::Interpreter()
{
    Initialised = false;
    Executing = false;
    LastPrompt = new String();
    dest = outNull;
    mutex = NULL;
}

void Interpreter::Begin(LPCTSTR Command)
{
    assert(!Initialised);

    Start(Command);
    SetPrompt(InterpreterPrompt);
    Evaluate("putChar '\\b'");
}

void Interpreter::Read(LPCTSTR Buffer, DWORD Size, bool Stdout)
{
    if (!Initialised) return;

    switch (dest)
    {
    case outScreen:
        output->Append(Buffer);
        break;

    case outPrompt:
        LastPrompt->Append(Buffer);
        break;

    case outBuffer:
        OutputBuffer->Append(Buffer);
        break;
    }
}

void Interpreter::Escape(ConsoleEscape Code, bool Stdout)
{
	if (!Initialised && Code == conBackspace)
		Initialised = true;
    else if (Code == conPromptBegin)
    {
        dest = outPrompt;
        LastPrompt->Reset();
    }
    else if (Code == conPromptEnd)
    {
        if (mutex != NULL)
            mutex->Unlock();
        else
        {
            dest = outScreen;
            app->RunningChanged(false);
        }
    }
	else if (Stdout)
		output->Escape(Code);
}

void Interpreter::Finished()
{
}

void Interpreter::AbortComputation()
{
    this->Exception();
}

LPCTSTR Interpreter::GetLastPrompt()
{
    return LastPrompt->Get();
}

void Interpreter::Evaluate(LPCTSTR Line)
{
    Write(Line, (DWORD) strlen(Line));
    Write("\n", 1);
}

String* Interpreter::GetType(LPCTSTR Expression)
{
    mutex = new Mutex();
    mutex->Lock();
    OutputBuffer = new String;
    dest = outBuffer;

    TCHAR Buffer[500];
    wsprintf(Buffer, ":type %s", Expression);
    Evaluate(Buffer);

    mutex->Lock();
    delete mutex;
    mutex = NULL;

    return OutputBuffer;
}

void Interpreter::Expression(LPCTSTR Expression)
{
    output->AppendLex(Expression);
    output->Append("\n");
    Evaluate(Expression);
}

bool Interpreter::Execute(Action* a)
{
	TCHAR Buffer[500];

	switch (a->Code)
	{
	case actLoad:
		{
			int n = 6;
			strcpy(Buffer, ":load ");
			for (int i = 0; a->Argument[i] != 0; i++)
			{
				if (a->Argument[i] == '\\')
				{
					Buffer[n++] = '\\';
					Buffer[n++] = '\\';
				}
				else
					Buffer[n++] = a->Argument[i];
			}
			Buffer[n] = 0;
			recentFiles->Add(a->Argument);
		}
		break;

    case actType:
        {
            String* s = GetType(a->Argument);
            if (IsError(s->Get()))
            {
                if (!ShowError(a->Orig, s->Get()))
                    output->AppendLex(a->Orig);
                output->Append("\n");
                output->SetForecolor(RED);
                output->Append(s->Get());
            }
            else
            {
                output->AppendLex(a->Orig);
                output->Append("\n");
                output->AppendLex(s->Get());
                output->Append("\n");
            }
            delete s;
        }
        return false;

	default:
		wsprintf(Buffer, ":%s %s", a->Command, a->Argument);
		break;
	}

    output->AppendLex(a->Orig);
    output->Append("\n");
	Evaluate(Buffer);
	return true;
}
