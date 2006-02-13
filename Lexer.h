
enum Lexeme {
    LexKeyword,
    LexOperator,
    LexOther,
    LexString,
    LexSpace
};

Lexeme GetLexeme(LPCTSTR Buffer, int* Pos);
