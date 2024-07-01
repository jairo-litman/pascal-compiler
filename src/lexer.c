#include "lexer.h"

#include <ctype.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32  // Windows
// https://github.com/msys2/MINGW-packages/issues/4999#issuecomment-1530791650
char *strndup(const char *src, size_t size) {
    size_t len = strnlen(src, size);
    len        = len < size ? len : size;
    char *dst  = malloc(len + 1);
    if (!dst) return NULL;
    memcpy(dst, src, len);
    dst[len] = '\0';
    return dst;
}
#endif

#include "token.h"

// Create a new lexer
Lexer *newLexer(char *input) {
    Lexer *l        = malloc(sizeof(Lexer));
    l->input        = input;
    l->position     = 0;
    l->readPosition = 0;
    l->varCounter   = 0;
    l->litCounter   = 0;
    l->line         = 1;
    l->errors       = newErrorList();

    l->keywords = newHashMap(strHash, strcmp);
    initKeywords(l->keywords);

    readChar(l);

    return l;
}

// Free the lexer
void freeLexer(Lexer *l) {
    free(l->input);
    freeHashMap(l->keywords);
    freeErrorList(l->errors);
    free(l);
}

// Get the next token
Token *nextToken(Lexer *l) {
    Token *tok;

    skipWhitespace(l);

    char literal[2] = {l->ch, '\0'};

    switch (l->ch) {
        case '+':
            tok = newToken(PLUS, literal);
            break;
        case '-':
            tok = newToken(MINUS, literal);
            break;
        case '*':
            tok = newToken(ASTERISK, literal);
            break;
        case '/':
            tok = newToken(SLASH, literal);
            break;
        case '=':
            tok = newToken(EQ, literal);
            break;
        case '<':
            tok = compoundableToken(l, ">=", LT, (TokenType[]){NOT_EQ, LTE});
            break;
        case '>':
            tok = compoundableToken(l, "=", GT, (TokenType[]){GTE});
            break;
        case ',':
            tok = newToken(COMMA, literal);
            break;
        case '.':
            tok = newToken(DOT, literal);
            break;
        case ';':
            tok = newToken(SEMICOLON, literal);
            break;
        case ':':
            tok = compoundableToken(l, "=", COLON, (TokenType[]){ASSIGN});
            break;
        case '(':
            tok = newToken(LPAREN, literal);
            break;
        case ')':
            tok = newToken(RPAREN, literal);
            break;
        case '{':
            tok = newToken(LBRACE, literal);
            break;
        case '}':
            tok = newToken(RBRACE, literal);
            break;
        case '[':
            tok = newToken(LBRACKET, literal);
            break;
        case ']':
            tok = newToken(RBRACKET, literal);
            break;
        case '"':
            tok       = malloc(sizeof(Token));
            tok->type = readString(l, &tok->literal);
            break;
        case '\'':
            tok       = malloc(sizeof(Token));
            tok->type = readCharLiteral(l, &tok->literal);
            break;
        case 0:
            tok = newToken(_EOF, "");
            break;
        default:
            if (isalpha(l->ch)) {
                tok          = malloc(sizeof(Token));
                tok->literal = readIdentifier(l);
                tok->type    = lookupIdent(l->keywords, tok->literal);
                if (tok->type == IDENT) {
                    l->varCounter++;
                }
                return tok;
            } else if (isdigit(l->ch) || (l->ch == '.' && isdigit(peekChar(l)))) {
                tok       = malloc(sizeof(Token));
                tok->type = readNumber(l, &tok->literal);
                return tok;
            } else {
                tok = newToken(ILLEGAL, literal);
                char error[64];
                sprintf(error, "Caractere inválido: '%c', linha: %d", l->ch, l->line);
                addError(l, error);
            }
            break;
    }

    readChar(l);
    return tok;
}

// Read the next character and update both positions
void readChar(Lexer *l) {
    if (l->readPosition >= strlen(l->input)) {
        l->ch = 0;
    } else {
        l->ch = l->input[l->readPosition];
    }

    l->position = l->readPosition;
    l->readPosition++;
}

// Skip whitespaces and count lines
void skipWhitespace(Lexer *l) {
    while (l->ch == ' ' || l->ch == '\t' || l->ch == '\n' || l->ch == '\r') {
        if (l->ch == '\n') {
            l->line++;
        }
        readChar(l);
    }
}

// Peek the next character
char peekChar(Lexer *l) {
    if (l->readPosition >= strlen(l->input)) {
        return 0;
    } else {
        return l->input[l->readPosition];
    }
}

// Create a new token
Token *newToken(TokenType type, char *literal) {
    Token *tok   = malloc(sizeof(Token));
    tok->type    = type;
    tok->literal = malloc(strlen(literal) + 1);
    strcpy(tok->literal, literal);

    return tok;
}

// Create a new token for compoundable tokens e.g :=, <=, >=, <>
Token *compoundableToken(Lexer *l, char *nextCh, TokenType singleToken, TokenType *compoundToken) {
    char ch = l->ch;

    for (uint8_t i = 0; i < strlen(nextCh); i++) {
        if (peekChar(l) == nextCh[i]) {
            readChar(l);
            char compoundLiteral[3] = {ch, nextCh[i], '\0'};
            return newToken(compoundToken[i], compoundLiteral);
        }
    }

    char singleLiteral[2] = {ch, '\0'};
    return newToken(singleToken, singleLiteral);
}

// Free the token
void freeToken(Token *t) {
    free(t->literal);
    free(t);
}

// Read an identifier
char *readIdentifier(Lexer *l) {
    uint32_t position = l->position;

    while (isalnum(l->ch)) {
        readChar(l);
    }

    char *ident = strndup(l->input + position, l->position - position);
    for (uint32_t i = 0; i < strlen(ident); i++) {
        ident[i] = tolower(ident[i]);
    }

    return ident;
}

// Check if a character is a possible terminator
bool isPossibleTerminator(char ch) {
    return ch == ' ' || ch == 0 || ch == '\n' || ch == '\r' || ch == '\t' || ch == ';' || ch == ':' || ch == ')' ||
           ch == '}' || ch == ']' || ch == ',';
}

// Read a number literal, integers and floats, checks for malformed numbers
TokenType readNumber(Lexer *l, char **literal) {
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
        readChar(l);
    }

    while (illegal && !isPossibleTerminator(l->ch)) {
        readChar(l);
    }

    *literal = strndup(l->input + position, l->position - position);

    if (illegal) {
        char error[64];
        sprintf(error, "Número inválido: '%s', linha: %d", *literal, l->line);
        addError(l, error);
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
TokenType readString(Lexer *l, char **literal) {
    uint32_t position = l->position + 1;

    while (true) {
        readChar(l);
        if (l->ch == '"' || l->ch == 0) {
            break;
        }
    }

    if (l->ch == 0) {
        char error[64];
        sprintf(error, "Fim de arquivo inesperado, linha: %d", l->line);
        addError(l, error);

        *literal = strndup(l->input + position - 1, l->position - position + 1);

        return ILLEGAL;
    }

    *literal = strndup(l->input + position, l->position - position);
    l->litCounter++;

    return STR;
}

// Read a character literal enclosed by single quotes ' '
TokenType readCharLiteral(Lexer *l, char **literal) {
    uint32_t position = l->position + 1;

    readChar(l);

    if (l->ch == 0) {
        char error[64];
        sprintf(error, "Fim de arquivo inesperado, linha: %d", l->line);
        addError(l, error);

        *literal = strndup(l->input + position - 1, l->position - position + 1);

        return ILLEGAL;
    }

    readChar(l);

    if (l->ch != '\'') {
        char error[64];
        sprintf(error, "Caractere inválido: '%c', linha: %d", l->ch, l->line);
        addError(l, error);

        *literal = strndup(l->input + position - 1, l->position - position + 1);

        return ILLEGAL;
    }

    *literal = strndup(l->input + position, l->position - position);
    l->litCounter++;

    return CHAR;
}

// Create a new error list, initial capacity is 8
ErrorList *newErrorList() {
    ErrorList *e = malloc(sizeof(ErrorList));
    e->data      = malloc(sizeof(char *) * 8);
    e->size      = 0;
    e->capacity  = 8;

    return e;
}

// Free the error list
void freeErrorList(ErrorList *e) {
    for (uint32_t i = 0; i < e->size; i++) {
        free(e->data[i]);
    }
    free(e->data);
    free(e);
}

// Add an error to the error list, expanding the list if necessary
void addError(Lexer *l, char *error) {
    ErrorList *e = l->errors;
    if (e->size >= e->capacity) {
        e->capacity *= 2;
        e->data      = realloc(e->data, sizeof(char *) * e->capacity);
    }

    e->data[e->size] = malloc(strlen(error) + 1);

    strcpy(e->data[e->size], error);
    e->size++;
}