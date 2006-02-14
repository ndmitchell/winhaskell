#include "Header.h"
#include "Lexer.h"

LPCTSTR SingleChars = ",()[]{}`";

LPCTSTR OpChars = "!#$%&*+./<=>?@\\^|-~:";

LPCTSTR Operators[] = {
    "=", "@", "<-", "->", "::", "..", "|", "!", "~", "\\", NULL
};

LPCTSTR Keywords[] = {
    "as", "case", "of", "class", "data", "default", "deriving",
    "do", "forall", "hiding", "if", "then", "else", "import",
    "infix", "infixl", "infixr", "instance", "let", "in", "module",
    "newtype", "qualified", "type", "where", NULL
};

bool IsSpace(TCHAR c)
{
    return (isspace(c) ? true : false);
}

bool IsSingleOp(TCHAR c)
{
    return (strchr(SingleChars, c) != NULL);
}

Lexeme GetType(TCHAR c)
{
    if (strchr(OpChars, c) != NULL)
        return LexOperator;
    else if (IsSpace(c) || IsSingleOp(c))
        return LexOther;
    else
        return LexKeyword;
}

int CountString(LPCTSTR Buffer)
{
    TCHAR c = Buffer[0];
    if (c != '\"' && c != '\'')
        return 0;

    int Len;
    for (Len = 1; ; Len++)
    {
        TCHAR d = Buffer[Len];
        if (d == '\\' && Buffer[Len+1] != 0)
            Len++;
        else if (d == 0)
            return Len;
        else if (d == c)
            return Len+1;
    }
}

Lexeme GetLexeme(LPCTSTR Buffer, int* Pos)
{
    LPCTSTR c = &Buffer[*Pos];
    LPCTSTR Orig = c;
    *Pos = *Pos + 1;

    if (IsSpace(*c))
        return LexSpace;
    if (IsSingleOp(*c))
        return LexOperator;

    int Len = CountString(c);
    if (Len != 0)
    {
        *Pos = *Pos + Len - 1;
        return LexString;
    }

    Lexeme Res = GetType(*c);
    while (true)
    {
        c++;
        if (*c == 0 || GetType(*c) != Res)
            break;
        *Pos = *Pos + 1;
    }

    TCHAR b = *c;
    *((LPTSTR) c) = 0;

    LPCTSTR* Items;
    if (Res == LexOperator)
        Items = Operators;
    else
        Items = Keywords;

    bool Valid = false;
    for (int i = 0; Items[i] != NULL; i++)
    {
        if (strcmp(Items[i], Orig) == 0)
            Valid = true;
    }
    if (!Valid)
        Res = LexOther;

    *((LPTSTR) c) = b;
    return Res;
}

LPCTSTR OpenBracket = "([{";
LPCTSTR ShutBracket = ")]}";

int FindPos(LPCTSTR s, TCHAR c)
{
    for (int i = 0; s[i] != 0; i++)
    {
        if (s[i] == c)
            return i;
    }
    return -1;
}

LPTSTR ScanBrackets(LPTSTR Buffer, bool Expecting)
{
    for (; *Buffer != 0; Buffer++)
    {
        int OpenPos = FindPos(OpenBracket, *Buffer);
        int ShutPos = FindPos(ShutBracket, *Buffer);

        if (ShutPos != -1)
        {
            if (Expecting)
                return Buffer;
            else
                *Buffer = 0;
        }
        else if (OpenPos != -1)
        {
            LPTSTR Res = ScanBrackets(Buffer+1, true);
            if (Res == NULL)
            {
                *Buffer = 0;
                return NULL;
            }
            else if (*Res != ShutBracket[OpenPos])
            {
                *Buffer = 0;
                *Res = 0;
            }
            Buffer = Res;
        }
    }
    return NULL;
}

// set all characters which are bad to 1, all others to 0
void MismatchedBrackets(LPTSTR Buffer)
{
    ScanBrackets(Buffer, false);
}
