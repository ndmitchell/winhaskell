
enum Lexeme {
    LexKeyword,
    LexOperator,
    LexWhitespace,
    LexOther,
    LexString,
    LexCommand,
    LexError,
};

class LexToken
{
public:
    int Start;
    int End;
    int Length;
    Lexeme Lex;
    LPCTSTR Str;
};

class Lexer
{
public:
    Lexer(LPCTSTR String);
    ~Lexer();

    void Reset();
    LexToken* Next();

private:
    void Skip();

    LPTSTR Copy;
    int Pos;
    char c;

    LPCTSTR* Depend;
    LexToken item;
};

class LexList: LexToken
{
public:
    int Length;
    Lexeme Lex;
    LPTSTR Str;

    LexList(Lexer* lex);
    LexList(LexToken* lt);
    LexList();

    ~LexList();
    LexList* Next;
};

void MismatchedBrackets(LPTSTR Buffer);
