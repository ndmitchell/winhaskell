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
    Evaluate("putChar '\\001'");
}

void Interpreter::Read(LPCTSTR Buffer, DWORD Size, bool Stdout)
{
    if (!Initialised)
    {
        for (DWORD i = 0; i < Size; i++)
        {
            if (Buffer[i] == '\001')
            {
                Initialised = true;
                output->Append(&Buffer[i+1]);
                break;
            }
        }
    }
    else
        output->Append(Buffer);
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


