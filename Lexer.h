
enum Lexeme {
    LexKeyword,
    LexOperator,
    LexOther,
    LexString,
};

Lexeme GetLexeme(LPCTSTR Buffer, int* Pos);
void MismatchedBrackets(LPTSTR Buffer);
bool ParseCommand(LPCTSTR Command, LPTSTR Verb, LPTSTR Argument);
