
enum Lexeme {
    LexKeyword,
    LexOperator,
    LexWhitespace,
    LexOther,
    LexString,
    LexCommand
};

struct LexToken
{
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

void MismatchedBrackets(LPTSTR Buffer);
