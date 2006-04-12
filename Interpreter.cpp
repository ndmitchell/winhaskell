#include "Header.h"
#include "Console.h"
#include "Interpreter.h"

#include "Output.h"
#include "Application.h"

LPCTSTR InterpreterPrompt = "\x1B[0;32m%s>\x1B[0m \x1B[50m";


Interpreter::Interpreter()
{
    Initialised = false;
    Executing = false;
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
    if (Initialised)
        output->Append(Buffer);
}

void Interpreter::Escape(ConsoleEscape Code, bool Stdout)
{
	if (!Initialised && Code == conBackspace)
		Initialised = true;
	else if (Code == conComplete)
		app->ExecutionComplete();
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

void Interpreter::Evaluate(LPCTSTR Line)
{
    Write(Line, (DWORD) strlen(Line));
    Write("\n", 1);
}


