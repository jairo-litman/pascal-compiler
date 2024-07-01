#ifndef PARSER_H
#define PARSER_H

#include "ast.h"
#include "error.h"
#include "hashmap.h"
#include "lexer.h"
#include "token.h"

typedef enum {
    LOWEST = 0,   // Lowest precedence
    ASSIGNMENT,   // :=
    LOGICAL_OR,   // or
    LOGICAL_AND,  // and
    EQUALITY,     // =, <>
    LESSGREATER,  // <, >, <=, >=
    SUM,          // +, -
    PRODUCT,      // *, /, mod, div
    PREFIX,       // -X, not X
    CALL,         // myFunction(X)
    INDEX         // array[index]
} Precedence;

typedef struct {
    Lexer *l;

    Token *curToken;
    Token *peekToken;

    HashMap *precedences;

    HashMap *prefixParseFns;
    HashMap *infixParseFns;

    eErrorList *errors;

    uint16_t assignCounter;
} Parser;

Parser *pNew(Lexer *l);
void    pFree(Parser *p);

void pCustomError(Parser *p, char *msg);
void pPeekError(Parser *p, char *str);
void pNoPrefixParseFnError(Parser *p, Token *t);
void pIntegerParseError(Parser *p, astExpression *e);
void pFloatParseError(Parser *p, astExpression *e);

void pInitPrecedences(Parser *p);
void pRegisterPrecedence(Parser *p, TokenType t, Precedence pr);

typedef astExpression *(*pPrefixParseFn)(Parser *);
typedef astExpression *(*pInfixParseFn)(Parser *, astExpression *);

void pInitPrefixParseFns(Parser *p);
void pInitInfixParseFns(Parser *p);
void pRegisterPrefix(Parser *p, TokenType t, pPrefixParseFn fn);
void pRegisterInfix(Parser *p, TokenType t, pInfixParseFn fn);

void pNextToken(Parser *p);
bool pCurTokenIs(Parser *p, TokenType t);
bool pPeekTokenIs(Parser *p, TokenType t);
bool pExpectPeek(Parser *p, TokenType t, char *msg);

Precedence pCurPrecedence(Parser *p);
Precedence pPeekPrecedence(Parser *p);

astProgram *pParseProgram(Parser *p);

astBlockStmt       *pParseBlockStmt(Parser *p);
astVarStmt         *pParseVarStmt(Parser *p, bool isGlobal);
astDeclarationStmt *pParseDeclarationStmt(Parser *p);
astFunctionStmt    *pParseFunctionStmt(Parser *p);
astParameterStmt   *pParseParameterStmt(Parser *p);
astBeginEndStmt    *pParseBeginEndStmt(Parser *p);
astConditionalStmt *pParseConditionalStmt(Parser *p);
astWhileStmt       *pParseWhileStmt(Parser *p);
astExpressionStmt  *pParseExpressionStmt(Parser *p);

astExpression     *pParseExpression(Parser *p, Precedence pr);
astPrefixExpr     *pParsePrefixExpr(Parser *p);
astInfixExpr      *pParseInfixExpr(Parser *p, astExpression *left);
astExpression     *pParseGroupedExpr(Parser *p);
astAssignmentExpr *pParseAssignmentExpr(Parser *p, astExpression *left);
astIdentifierExpr *pParseIdentifierExpr(Parser *p);
astIntegerExpr    *pParseIntegerExpr(Parser *p);
astFloatExpr      *pParseFloatExpr(Parser *p);
astBooleanExpr    *pParseBooleanExpr(Parser *p);
astStringExpr     *pParseStringExpr(Parser *p);
astCharExpr       *pParseCharExpr(Parser *p);
astTypeExpr       *pParseTypeExpr(Parser *p);
astCallExpr       *pParseCallExpr(Parser *p, astExpression *function);

#endif  // PARSER_H