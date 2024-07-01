#include "lexer.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "error.h"
#include "hash.h"
#include "token.h"

#ifdef _WIN32
#include "winfuncs.h"
#endif  // _WIN32

// Create a new lexer
Lexer *lNew(char *input) {
    Lexer *l        = malloc(sizeof(Lexer));
    l->input        = input;
    l->position     = 0;
    l->readPosition = 0;
    l->varCounter   = 0;
    l->litCounter   = 0;
    l->line         = 1;
    l->errors       = eNew();

    l->keywords = hmNew(hStrHash, hStrCmp, 64);
    tInitKeywords(l->keywords);

    lReadChar(l);

    return l;
}

// Free the lexer
void lFree(Lexer *l) {
    free(l->input);
    hmFree(l->keywords);
    eFree(l->errors);
    free(l);
}

// Get the next token
Token *lNextToken(Lexer *l) {
    Token *tok;

    lSkipWhitespace(l);

    char literal[2] = {l->ch, '\0'};

    switch (l->ch) {
        case '+':
            tok = tNewToken(PLUS, literal);
            break;
        case '-':
            tok = tNewToken(MINUS, literal);
            break;
        case '*':
            tok = tNewToken(ASTERISK, literal);
            break;
        case '/':
            tok = tNewToken(SLASH, literal);
            break;
        case '=':
            tok = tNewToken(EQ, literal);
            break;
        case '<':
            tok = lCompoundableToken(l, ">=", LT, (TokenType[]){NOT_EQ, LTE});
            break;
        case '>':
            tok = lCompoundableToken(l, "=", GT, (TokenType[]){GTE});
            break;
        case ',':
            tok = tNewToken(COMMA, literal);
            break;
        case '.':
            tok = tNewToken(DOT, literal);
            break;
        case ';':
            tok = tNewToken(SEMICOLON, literal);
            break;
        case ':':
            tok = lCompoundableToken(l, "=", COLON, (TokenType[]){ASSIGN});
            break;
        case '(':
            tok = tNewToken(LPAREN, literal);
            break;
        case ')':
            tok = tNewToken(RPAREN, literal);
            break;
        case '{':
            tok = tNewToken(LBRACE, literal);
            break;
        case '}':
            tok = tNewToken(RBRACE, literal);
            break;
        case '[':
            tok = tNewToken(LBRACKET, literal);
            break;
        case ']':
            tok = tNewToken(RBRACKET, literal);
            break;
        case '"':
            tok       = malloc(sizeof(Token));
            tok->type = lReadString(l, &tok->literal);
            break;
        case '\'':
            tok       = malloc(sizeof(Token));
            tok->type = lReadCharLiteral(l, &tok->literal);
            break;
        case 0:
            tok = tNewToken(_EOF, "");
            break;
        default:
            if (isalpha(l->ch)) {
                tok          = malloc(sizeof(Token));
                tok->literal = lReadIdentifier(l);
                tok->type    = tLookupIdent(l->keywords, tok->literal);
                if (tok->type == IDENT) {
                    l->varCounter++;
                }
                return tok;
            } else if (isdigit(l->ch) || (l->ch == '.' && isdigit(lPeekChar(l)))) {
                tok       = malloc(sizeof(Token));
                tok->type = lReadNumber(l, &tok->literal);
                return tok;
            } else {
                tok = tNewToken(ILLEGAL, literal);
                char error[64];
                sprintf(error, "Caractere inválido: '%c', linha: %d", l->ch, l->line);
                eAdd(l->errors, error);
            }
            break;
    }

    lReadChar(l);
    return tok;
}

// Read the next character and update both positions
void lReadChar(Lexer *l) {
    if (l->readPosition >= strlen(l->input)) {
        l->ch = 0;
    } else {
        l->ch = l->input[l->readPosition];
    }

    l->position = l->readPosition;
    l->readPosition++;
}

// Skip whitespaces and count lines
void lSkipWhitespace(Lexer *l) {
    while (l->ch == ' ' || l->ch == '\t' || l->ch == '\n' || l->ch == '\r') {
        if (l->ch == '\n') {
            l->line++;
        }
        lReadChar(l);
    }
}

// Peek the next character
char lPeekChar(Lexer *l) {
    if (l->readPosition >= strlen(l->input)) {
        return 0;
    } else {
        return l->input[l->readPosition];
    }
}

// Create a new token for compoundable tokens e.g :=, <=, >=, <>
Token *lCompoundableToken(Lexer *l, char *nextCh, TokenType singleToken, TokenType *compoundToken) {
    char ch = l->ch;

    for (uint8_t i = 0; i < strlen(nextCh); i++) {
        if (lPeekChar(l) == nextCh[i]) {
            lReadChar(l);
            char compoundLiteral[3] = {ch, nextCh[i], '\0'};
            return tNewToken(compoundToken[i], compoundLiteral);
        }
    }

    char singleLiteral[2] = {ch, '\0'};
    return tNewToken(singleToken, singleLiteral);
}

// Read an identifier
char *lReadIdentifier(Lexer *l) {
    uint32_t position = l->position;

    while (isalnum(l->ch)) {
        lReadChar(l);
    }

    char *ident = strndup(l->input + position, l->position - position);
    for (uint32_t i = 0; i < strlen(ident); i++) {
        ident[i] = tolower(ident[i]);
    }

    return ident;
}

// Check if a character is a possible terminator
bool lIsPossibleTerminator(char ch) {
    return ch == ' ' || ch == 0 || ch == '\n' || ch == '\r' || ch == '\t' || ch == ';' || ch == ':' || ch == ')' ||
           ch == '}' || ch == ']' || ch == ',';
}

// Read a number literal, integers and floats, checks for malformed numbers
TokenType lReadNumber(Lexer *l, char **literal) {
    uint32_t position = l->position;
    bool     isFloat  = false;
    uint8_t  dotCount = 0;
    bool     illegal  = false;

    while (isdigit(l->ch) || l->ch == '.') {
        if (l->ch == '.') {
            dotCount++;
            if (dotCount > 1) {
                illegal = true;
                break;
            }
            isFloat = true;
        }
        lReadChar(l);
    }

    while (illegal && !lIsPossibleTerminator(l->ch)) {
        lReadChar(l);
    }

    *literal = strndup(l->input + position, l->position - position);

    if (illegal) {
        char error[64];
        sprintf(error, "Número inválido: '%s', linha: %d", *literal, l->line);
        eAdd(l->errors, error);
        return ILLEGAL;
    }

    l->litCounter++;

    if (isFloat) {
        return FLOAT;
    } else {
        return INT;
    }
}

// Read a string literal enclosed by double quotes " "
TokenType lReadString(Lexer *l, char **literal) {
    uint32_t position = l->position + 1;

    while (true) {
        lReadChar(l);
        if (l->ch == '"' || l->ch == 0) {
            break;
        }
    }

    if (l->ch == 0) {
        char error[64];
        sprintf(error, "Fim de arquivo inesperado, linha: %d", l->line);
        eAdd(l->errors, error);

        *literal = strndup(l->input + position - 1, l->position - position + 1);

        return ILLEGAL;
    }

    *literal = strndup(l->input + position, l->position - position);
    l->litCounter++;

    return STR;
}

// Read a character literal enclosed by single quotes ' '
TokenType lReadCharLiteral(Lexer *l, char **literal) {
    uint32_t position = l->position + 1;

    lReadChar(l);

    if (l->ch == 0) {
        char error[64];
        sprintf(error, "Fim de arquivo inesperado, linha: %d", l->line);
        eAdd(l->errors, error);

        *literal = strndup(l->input + position - 1, l->position - position + 1);

        return ILLEGAL;
    }

    lReadChar(l);

    if (l->ch != '\'') {
        char error[64];
        sprintf(error, "Caractere inválido: '%c', linha: %d", l->ch, l->line);
        eAdd(l->errors, error);

        *literal = strndup(l->input + position - 1, l->position - position + 1);

        return ILLEGAL;
    }

    *literal = strndup(l->input + position, l->position - position);
    l->litCounter++;

    return CHAR;
}