#ifndef TOKEN_H
#define TOKEN_H

#include "hashmap.h"

#define HM_CAPACITY 1024

typedef enum {
    // Special tokens
    _EOF = 0,  // End of file
    ILLEGAL,   // Unknown token

    // Identifiers + literals
    IDENT,  // add, foobar, x, y, ...
    CHAR,   // 'a'
    TRUE,   // true
    FALSE,  // false
    INT,    // 1343456
    FLOAT,  // 3.14
    STR,    // "foobar"

    // Types
    CHARACTER,  // character type
    BOOLEAN,    // boolean type
    INTEGER,    // integer type
    REAL,       // real type
    STRING,     // string type

    // Operators
    ASSIGN,  // :=

    // Arithmetic operators
    PLUS,      // +
    MINUS,     // -
    ASTERISK,  // *
    SLASH,     // /
    MOD,       // mod
    DIV,       // div

    // Comparison operators
    EQ,      // =
    NOT_EQ,  // <>
    LT,      // <
    GT,      // >
    LTE,     // <=
    GTE,     // >=

    // Logical operators
    AND,  // and
    OR,   // or
    NOT,  // not

    // Delimiters
    COMMA,      // ,
    DOT,        // .
    SEMICOLON,  // ;
    COLON,      // :

    // Brackets
    LPAREN,    // (
    RPAREN,    // )
    LBRACE,    // {
    RBRACE,    // }
    LBRACKET,  // [
    RBRACKET,  // ]

    // Keywords
    PROGRAM,
    BEGIN,
    END,
    IF,
    THEN,
    ELSE,
    CONTINUE,
    BREAK,
    RETURN,
    WHILE,
    DO,
    REPEAT,
    UNTIL,
    FOR,
    TO,
    DOWNTO,
    CASE,
    OF,
    VAR,
    CONST,
    TYPE,
    FUNCTION,
    PROCEDURE,
    GOTO,
    LABEL,
    NIL,
} TokenType;

typedef struct {
    TokenType type;
    char     *literal;
} Token;

Token *tNewToken(TokenType type, char *literal);
void   tFreeToken(Token *t);

void      tInitKeywords(HashMap *keywords);
void      tRegisterKeyword(HashMap *keywords, char *key, TokenType value);
TokenType tLookupIdent(HashMap *keywords, char *ident);

#endif  // TOKEN_H