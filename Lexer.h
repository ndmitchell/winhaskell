
enum Lexeme {
    LexKeyword,
    LexOperator,
    LexOther,
    LexString,
    LexCommand
};

Lexeme GetLexeme(LPCTSTR Buffer, int* Pos);
void MismatchedBrackets(LPTSTR Buffer);
bool ParseCommand(LPCTSTR Command, LPTSTR Verb, LPTSTR Argument);
