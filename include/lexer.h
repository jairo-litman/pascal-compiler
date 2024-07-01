#ifndef LEXER_H
#define LEXER_H

#include <stdbool.h>
#include <stdint.h>

#include "token.h"

typedef struct {
    char   **data;
    uint32_t size;
    uint32_t capacity;
} ErrorList;

typedef struct {
    char      *input;         // input to be tokenized
    uint32_t   position;      // current position in input (points to current char)
    uint32_t   readPosition;  // current reading position in input (after current char)
    char       ch;            // current char under examination
    HashMap   *keywords;      // keywords hashmap
    uint16_t   varCounter;    // variable counter
    uint16_t   litCounter;    // literal counter
    uint16_t   line;          // current line
    ErrorList *errors;        // list of errors
} Lexer;

Lexer *newLexer(char *input);
void   freeLexer(Lexer *l);

void readChar(Lexer *l);
void countLines(Lexer *l);
void skipWhitespace(Lexer *l);
char peekChar(Lexer *l);

Token *nextToken(Lexer *l);
Token *newToken(TokenType type, char *literal);
Token *compoundableToken(Lexer *l, char *nextCh, TokenType singleToken, TokenType *compoundToken);
void   freeToken(Token *t);

char     *readIdentifier(Lexer *l);
TokenType readNumber(Lexer *l, char **literal);
TokenType readString(Lexer *l, char **literal);
TokenType readCharLiteral(Lexer *l, char **literal);

bool isPossibleTerminator(char ch);

ErrorList *newErrorList();
void       freeErrorList(ErrorList *e);
void       addError(Lexer *l, char *error);

#endif  // LEXER_H