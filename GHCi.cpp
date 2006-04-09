#include "Header.h"
#include "Console.h"
#include "Interpreter.h"
#include "GHCi.h"

void GHCi::SetPrompt(LPCTSTR Command)
{
    int n = (int) strlen(Command);
    LPTSTR Buffer = new TCHAR[n + 15];
    strcpy(Buffer, ":set prompt \"");
    strcat(Buffer, Command);
    strcat(Buffer, "\"");

    Evaluate(Buffer);
    delete[] Buffer;
}

GHCi* StartGHCi()
{
    LPCTSTR Command = "C:\\ghc\\ghc-6.4.2.20060329-i386-unknown-mingw32.tar\\ghc-6.4.2\\bin\\ghci.exe";
    return new GHCi(Command);
}
