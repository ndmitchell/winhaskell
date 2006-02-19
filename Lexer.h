
enum Lexeme {
    LexKeyword,
    LexOperator,
    LexOther,
    LexString,
    LexCommand
};

struct LexItem
{
    int Start;
    int End;
    Lexeme Lex;
};

Lexeme GetLexeme(LPCTSTR Buffer, int* Pos);
int GetLexemes(LPCTSTR Buffer, LexItem* Items, int ItemsSize);
void MismatchedBrackets(LPTSTR Buffer);
bool ParseCommand(LPCTSTR Command, LPTSTR Verb, LPTSTR Argument);

