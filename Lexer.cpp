#include "Header.h"
#include "Lexer.h"

LPCTSTR SingleChars = ",()[]{}`";

LPCTSTR OpChars = "!#$%&*+./<=>?@\\^|-~:";

LPCTSTR Operators[] = {
    "=", "@", "<-", "=>", "->", "::", "..", "|", "!", "~", "\\", NULL
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

bool InList(LPCTSTR str, TCHAR c)
{
    return (c != 0 && strchr(str, c) != NULL);
}

bool InitKeyword(TCHAR c)
{
    return (c == '_' || isalpha(c));
}

bool ContKeyword(TCHAR c)
{
    return (InitKeyword(c) || isdigit(c) || c == '\'');
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

Lexer::Lexer(LPCTSTR String)
{
    Copy = strdup(String);
    Pos = 0;
}

Lexer::~Lexer()
{
    free(Copy);
}

void Lexer::Reset()
{
    if (Pos != 0)
        Copy[Pos] = c;
    Pos = 0;
}

void Lexer::Skip()
{
    if (IsSpace(Copy[Pos]))
    {
        item.Lex = LexWhitespace;
        while (IsSpace(Copy[Pos]))
            Pos++;
    }
    else if (Copy[Pos] == '\'' || Copy[Pos] == '\"')
    {
        item.Lex = LexString;
        TCHAR b = Copy[Pos];
        bool LastEscape = false;
        for (Pos++; Copy[Pos] != 0; Pos++)
        {
            if (!LastEscape && Copy[Pos] == b)
            {
                Pos++;
                break;
            }
            LastEscape = (Copy[Pos] == '\\');
        }
    }
    else if (Pos == 0 && Copy[Pos] == ':')
    {
        item.Lex = LexCommand;
        for (Pos++; Copy[Pos] != 0 && !IsSpace(Copy[Pos]); Pos++)
            ; //nothing to do
    }
    else if (InList(SingleChars, Copy[Pos]))
    {
        item.Lex = LexOperator;
        Pos++;
    }
    else if (InList(OpChars, Copy[Pos]))
    {
        item.Lex = LexOperator;
        Depend = Operators;
        for (Pos++; InList(OpChars, Copy[Pos]); Pos++)
            ; //nothing to do
    }
    else if (InitKeyword(Copy[Pos]))
    {
        item.Lex = LexKeyword;
        Depend = Keywords;
        for (Pos++; ContKeyword(Copy[Pos]); Pos++)
            ; //nothing to do
    }
    else
    {
        Pos++;
        item.Lex = LexOther;
    }
}

LexToken* Lexer::Next()
{
    if (Pos != 0)
        Copy[Pos] = c;

    if (Copy[Pos] == 0)
        return NULL;

    item.Start = Pos;
    item.Str = &Copy[Pos];
    Depend = NULL;
    Skip();

    c = Copy[Pos];
    Copy[Pos] = 0;
    item.End = Pos;
    item.Length = Pos - item.Start;

    if (Depend != NULL)
    {
        bool Found = false;
        for (int i = 0; Depend[i] != NULL; i++)
        {
            if (strcmp(Depend[i], item.Str) == 0)
            {
                Found = true;
                break;
            }
        }
        if (!Found)
            item.Lex = LexOther;
    }
    return &item;
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
		int Str = CountString(Buffer);
		if (Str != 0)
		{
			Buffer += Str-1;
			continue;
		}

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

// LEX LIST STUFF
LexList::LexList()
{
}

LexList::LexList(Lexer* lex)
{
    LexToken* lt = lex->Next();
    if (lt == NULL)
    {
        Str = strdup("");
        Length = 0;
        Lex = LexWhitespace;
        Next = NULL;
    }
    else
    {
        Str = strdup(lt->Str);
        Length = lt->Length;
        Lex = lt->Lex;
        Next = new LexList(lex);
    }
}

LexList::LexList(LexToken* lt)
{
    Str = strdup(lt->Str);
    Length = lt->Length;
    Lex = lt->Lex;
}

LexList::~LexList()
{
    free(Str);
    if (Next != NULL) delete Next;
}
