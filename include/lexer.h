#ifndef LEXER_H
#define LEXER_H

#include <stdbool.h>
#include <stdint.h>

#include "error.h"
#include "token.h"

typedef struct {
    char       *input;         // input to be tokenized
    uint32_t    position;      // current position in input (points to current char)
    uint32_t    readPosition;  // current reading position in input (after current char)
    char        ch;            // current char under examination
    HashMap    *keywords;      // keywords hashmap
    uint16_t    varCounter;    // variable counter
    uint16_t    litCounter;    // literal counter
    uint16_t    line;          // current line
    eErrorList *errors;        // list of errors
} Lexer;

Lexer *lNew(char *input);
void   lFree(Lexer *l);

void lReadChar(Lexer *l);
void lCountLines(Lexer *l);
void lSkipWhitespace(Lexer *l);
char lPeekChar(Lexer *l);

Token *lNextToken(Lexer *l);
Token *lCompoundableToken(Lexer *l, char *nextCh, TokenType singleToken, TokenType *compoundToken);

char     *lReadIdentifier(Lexer *l);
TokenType lReadNumber(Lexer *l, char **literal);
TokenType lReadString(Lexer *l, char **literal);
TokenType lReadCharLiteral(Lexer *l, char **literal);

bool lIsPossibleTerminator(char ch);

#endif  // LEXER_H