#include "Header.h"
#include "Console.h"
#include "Interpreter.h"
#include "Hugs.h"
#include "Output.h"
#include "Lexer.h"
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

bool IsPrefix(LPCTSTR Prefix, LPCTSTR Str)
{
    return (memcmp(Prefix, Str, strlen(Prefix)) == 0);
}

bool Hugs::ShowError(LPCTSTR Input, LPCTSTR Result)
{
    LPCTSTR UndefVar = "ERROR - Undefined variable \"";
    int iUndefVar = (int) strlen(UndefVar);

    LPCTSTR UntermStr = "ERROR - Improperly terminated string";
    LPCTSTR UntermChr = "ERROR - Improperly terminated character constant";

    if (strncmp(Result, UndefVar, iUndefVar) == 0)
    {
        LPTSTR Name = (LPTSTR) &Result[iUndefVar];
        LPTSTR NameEnd = strchr(Name, '\"');
        NameEnd[0] = 0;

        Lexer lex(Input);
        LexList ll(&lex);

        for (LexList* i = &ll; i != NULL; i = i->Next)
        {
            if (strcmp(i->Str, Name) == 0)
                i->Lex = LexError;
        }
        NameEnd[0] = '\"';

        output->AppendLex(&ll);
        return true;
    }
    else if (IsPrefix(UntermStr, Result) || IsPrefix(UntermChr, Result))
    {
        Lexer lex(Input);
        LexList ll(&lex);

        for (LexList* i = &ll; i != NULL; i = i->Next)
        {
            if (i->Lex == LexString &&
                (i->Length <= 1 || i->Str[0] != i->Str[i->Length-1]))
                i->Lex = LexError;
        }

        output->AppendLex(&ll);
        return true;
    }
    else
        return false;
}

