#include "Header.h"
#include "Console.h"
#include "Interpreter.h"
#include "Hugs.h"
#include "Application.h"


void Hugs::SetPrompt(LPCTSTR Command)
{
    int n = (int) strlen(Command);
    LPTSTR Buffer = new TCHAR[n + 15];
    strcpy(Buffer, ":set -p\"");
    strcat(Buffer, Command);
    strcat(Buffer, "\"");
    Evaluate(Buffer);
    delete[] Buffer;
}

Hugs* StartHugs()
{
    LPCTSTR Command = "C:\\Program Files\\WinHugs\\Hugs.exe";
    return new Hugs(Command);
}

bool Hugs::IsError(LPCTSTR Result)
{
    return (strncmp(Result, "ERROR ", 6) == 0);
}

void Hugs::ErrorHints(LPCTSTR Input, LPCTSTR Result)
{
    LPCTSTR UndefVar = "ERROR - Undefined variable \"";
    int iUndefVar = (int) strlen(UndefVar);

    if (strncmp(Result, UndefVar, iUndefVar) == 0)
    {
        LPTSTR Name = (LPTSTR) &Result[iUndefVar];
        LPTSTR NameEnd = strchr(Name, '\"');
        NameEnd[0] = 0;

        LPTSTR Pos = strstr(Input, Name);
        NameEnd[0] = '\"';

        app->AddError(Pos - Input, NameEnd - Name, "Undefined variable");
    }
}

