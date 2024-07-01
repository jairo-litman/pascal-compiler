#include "token.h"

#include <stdlib.h>
#include <string.h>

#include "hashmap.h"

#ifdef _WIN32
#include "winfuncs.h"
#endif  // _WIN32

// Create a new token
Token *tNewToken(TokenType type, char *literal) {
    Token *tok   = malloc(sizeof(Token));
    tok->type    = type;
    tok->literal = malloc(strlen(literal) + 1);
    strcpy(tok->literal, literal);

    return tok;
}

// Free the token
void tFreeToken(Token *t) {
    if (t) {
        if (t->literal) {
            free(t->literal);
        }
        free(t);
    }
}

// Create a new keyword hashmap
void tInitKeywords(HashMap *keywords) {
    tRegisterKeyword(keywords, "true", TRUE);
    tRegisterKeyword(keywords, "false", FALSE);
    tRegisterKeyword(keywords, "char", CHARACTER);
    tRegisterKeyword(keywords, "boolean", BOOLEAN);
    tRegisterKeyword(keywords, "integer", INTEGER);
    tRegisterKeyword(keywords, "real", REAL);
    tRegisterKeyword(keywords, "string", STRING);
    tRegisterKeyword(keywords, "mod", MOD);
    tRegisterKeyword(keywords, "div", DIV);
    tRegisterKeyword(keywords, "and", AND);
    tRegisterKeyword(keywords, "or", OR);
    tRegisterKeyword(keywords, "not", NOT);
    tRegisterKeyword(keywords, "program", PROGRAM);
    tRegisterKeyword(keywords, "begin", BEGIN);
    tRegisterKeyword(keywords, "end", END);
    tRegisterKeyword(keywords, "if", IF);
    tRegisterKeyword(keywords, "then", THEN);
    tRegisterKeyword(keywords, "else", ELSE);
    tRegisterKeyword(keywords, "continue", CONTINUE);
    tRegisterKeyword(keywords, "break", BREAK);
    tRegisterKeyword(keywords, "return", RETURN);
    tRegisterKeyword(keywords, "while", WHILE);
    tRegisterKeyword(keywords, "do", DO);
    tRegisterKeyword(keywords, "repeat", REPEAT);
    tRegisterKeyword(keywords, "until", UNTIL);
    tRegisterKeyword(keywords, "for", FOR);
    tRegisterKeyword(keywords, "to", TO);
    tRegisterKeyword(keywords, "downto", DOWNTO);
    tRegisterKeyword(keywords, "case", CASE);
    tRegisterKeyword(keywords, "of", OF);
    tRegisterKeyword(keywords, "var", VAR);
    tRegisterKeyword(keywords, "const", CONST);
    tRegisterKeyword(keywords, "type", TYPE);
    tRegisterKeyword(keywords, "function", FUNCTION);
    tRegisterKeyword(keywords, "procedure", PROCEDURE);
    tRegisterKeyword(keywords, "goto", GOTO);
    tRegisterKeyword(keywords, "label", LABEL);
    tRegisterKeyword(keywords, "nil", NIL);
}

// Insert a keyword into the keyword hashmap
void tRegisterKeyword(HashMap *keywords, char *key, TokenType value) {
    char      *keyCopy = strndup(key, strlen(key));
    TokenType *tok     = (TokenType *)malloc(sizeof(TokenType));
    *tok               = value;

    hmInsert(keywords, keyCopy, tok, free, free);
}

// Lookup an identifier in the keyword hashmap, if it's not a keyword it's an identifier
TokenType tLookupIdent(HashMap *keywords, char *ident) {
    HashMapResult res = hmGet(keywords, ident);
    if (res.ok) {
        return *(TokenType *)res.data;
    }
    return IDENT;
}