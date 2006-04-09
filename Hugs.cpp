#include "Header.h"
#include "Console.h"
#include "Interpreter.h"
#include "Hugs.h"

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
