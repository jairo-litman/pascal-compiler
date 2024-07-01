#include "parser.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "ast.h"
#include "error.h"
#include "hash.h"
#include "hashmap.h"
#include "token.h"

//
// General and setup functions
//

// Create a new parser
Parser *pNew(Lexer *l) {
    Parser *p = (Parser *)malloc(sizeof(Parser));
    if (!p) {
        return NULL;
    }

    p->l = l;

    p->precedences = hmNew(hI32Hash, hI32Cmp, 32);
    pInitPrecedences(p);

    p->prefixParseFns = hmNew(hI32Hash, hI32Cmp, 32);
    pInitPrefixParseFns(p);

    p->infixParseFns = hmNew(hI32Hash, hI32Cmp, 32);
    pInitInfixParseFns(p);

    p->errors = eNew();

    // Read two tokens, so curToken and peekToken are both set
    pNextToken(p);
    pNextToken(p);

    p->assignCounter = 0;

    return p;
}

// Free the parser
void pFree(Parser *p) {
    hmFree(p->precedences);
    hmFree(p->prefixParseFns);
    hmFree(p->infixParseFns);
    eFree(p->errors);
    free(p);
}

// Register the operator precedences
void pInitPrecedences(Parser *p) {
    pRegisterPrecedence(p, ASSIGN, ASSIGNMENT);
    pRegisterPrecedence(p, OR, LOGICAL_OR);
    pRegisterPrecedence(p, AND, LOGICAL_AND);
    pRegisterPrecedence(p, EQ, EQUALITY);
    pRegisterPrecedence(p, NOT_EQ, EQUALITY);
    pRegisterPrecedence(p, LT, LESSGREATER);
    pRegisterPrecedence(p, GT, LESSGREATER);
    pRegisterPrecedence(p, LTE, LESSGREATER);
    pRegisterPrecedence(p, GTE, LESSGREATER);
    pRegisterPrecedence(p, PLUS, SUM);
    pRegisterPrecedence(p, MINUS, SUM);
    pRegisterPrecedence(p, SLASH, PRODUCT);
    pRegisterPrecedence(p, ASTERISK, PRODUCT);
    pRegisterPrecedence(p, MOD, PRODUCT);
    pRegisterPrecedence(p, DIV, PRODUCT);
    pRegisterPrecedence(p, LPAREN, CALL);
    pRegisterPrecedence(p, LBRACKET, INDEX);
}

// Auxiliary function to register a precedence
void pRegisterPrecedence(Parser *p, TokenType t, Precedence pr) {
    TokenType *tok = (TokenType *)malloc(sizeof(TokenType));
    *tok           = t;

    Precedence *prec = (Precedence *)malloc(sizeof(Precedence));
    *prec            = pr;

    hmInsert(p->precedences, tok, prec, free, free);
}

// Initialize the prefix parsing functions
void pInitPrefixParseFns(Parser *p) {
    pRegisterPrefix(p, IDENT, (pPrefixParseFn)pParseIdentifierExpr);  // identifier
    pRegisterPrefix(p, INT, (pPrefixParseFn)pParseIntegerExpr);       // integer literal
    pRegisterPrefix(p, FLOAT, (pPrefixParseFn)pParseFloatExpr);       // float literal
    pRegisterPrefix(p, TRUE, (pPrefixParseFn)pParseBooleanExpr);      // boolean true literal
    pRegisterPrefix(p, FALSE, (pPrefixParseFn)pParseBooleanExpr);     // boolean false literal
    pRegisterPrefix(p, STR, (pPrefixParseFn)pParseStringExpr);        // string literal
    pRegisterPrefix(p, CHAR, (pPrefixParseFn)pParseCharExpr);         // character literal
    pRegisterPrefix(p, MINUS, (pPrefixParseFn)pParsePrefixExpr);      // -
    pRegisterPrefix(p, NOT, (pPrefixParseFn)pParsePrefixExpr);        // not
    pRegisterPrefix(p, LPAREN, (pPrefixParseFn)pParseGroupedExpr);    // (grouped expression)
}

// Initialize the infix parsing functions
void pInitInfixParseFns(Parser *p) {
    pRegisterInfix(p, ASSIGN, (pInfixParseFn)pParseAssignmentExpr);  // := // assignment
    pRegisterInfix(p, PLUS, (pInfixParseFn)pParseInfixExpr);         // +
    pRegisterInfix(p, MINUS, (pInfixParseFn)pParseInfixExpr);        // -
    pRegisterInfix(p, ASTERISK, (pInfixParseFn)pParseInfixExpr);     // *
    pRegisterInfix(p, SLASH, (pInfixParseFn)pParseInfixExpr);        // /
    pRegisterInfix(p, MOD, (pInfixParseFn)pParseInfixExpr);          // mod
    pRegisterInfix(p, DIV, (pInfixParseFn)pParseInfixExpr);          // div
    pRegisterInfix(p, EQ, (pInfixParseFn)pParseInfixExpr);           // =
    pRegisterInfix(p, NOT_EQ, (pInfixParseFn)pParseInfixExpr);       // <>
    pRegisterInfix(p, LT, (pInfixParseFn)pParseInfixExpr);           // <
    pRegisterInfix(p, GT, (pInfixParseFn)pParseInfixExpr);           // >
    pRegisterInfix(p, LTE, (pInfixParseFn)pParseInfixExpr);          // <=
    pRegisterInfix(p, GTE, (pInfixParseFn)pParseInfixExpr);          // >=
    pRegisterInfix(p, LPAREN, (pInfixParseFn)pParseCallExpr);        // ( // call function/procedure
}

// Auxiliary function to register a prefix parsing function
void pRegisterPrefix(Parser *p, TokenType t, pPrefixParseFn fn) {
    TokenType *tok = (TokenType *)malloc(sizeof(TokenType));
    *tok           = t;

    hmInsert(p->prefixParseFns, tok, fn, free, NULL);
}

// Auxiliary function to register an infix parsing function
void pRegisterInfix(Parser *p, TokenType t, pInfixParseFn fn) {
    TokenType *tok = (TokenType *)malloc(sizeof(TokenType));
    *tok           = t;

    hmInsert(p->infixParseFns, tok, fn, free, NULL);
}

// Get the next token, setting the current and peek tokens
void pNextToken(Parser *p) {
    p->curToken  = p->peekToken;
    p->peekToken = lNextToken(p->l);
}

// Check if the current token is of a given type
bool pCurTokenIs(Parser *p, TokenType t) { return p->curToken->type == t; }

// Check if the peek token is of a given type
bool pPeekTokenIs(Parser *p, TokenType t) { return p->peekToken->type == t; }

// Check if the peek token is of a given type, and advance the token if it is
bool pExpectPeek(Parser *p, TokenType t, char *msg) {
    if (pPeekTokenIs(p, t)) {
        pNextToken(p);
        return true;
    } else {
        pPeekError(p, msg);
        return false;
    }
}

// Get the precedence of the current token
Precedence pCurPrecedence(Parser *p) {
    HashMapResult res = hmGet(p->precedences, &p->curToken->type);
    if (res.ok) {
        return *(Precedence *)res.data;
    }
    return LOWEST;
}

// Get the precedence of the peek token
Precedence pPeekPrecedence(Parser *p) {
    HashMapResult res = hmGet(p->precedences, &p->peekToken->type);
    if (res.ok) {
        return *(Precedence *)res.data;
    }
    return LOWEST;
}

//
// Parsing functions
//

// Program parsing function, entry point of the parser
astProgram *pParseProgram(Parser *p) {
    astProgram *program = astProgramNew(p->curToken);
    if (!program) {
        return NULL;
    }

    if (!pExpectPeek(p, IDENT, "IDENT")) {
        return NULL;
    }

    program->identifier = pParseIdentifierExpr(p);

    if (!pExpectPeek(p, SEMICOLON, ";")) {
        return NULL;
    }

    tFreeToken(p->curToken);  // Free the unused `;` token
    // pNextToken(p);

    program->block = pParseBlockStmt(p);

    if (!pExpectPeek(p, DOT, ".")) {
        return NULL;
    }

    tFreeToken(p->curToken);  // Free the unused `.` token

    if (!pExpectPeek(p, _EOF, "EOF")) {
        return NULL;
    }

    tFreeToken(p->curToken);  // Free the unused `EOF` token

    return program;
}

//
// Statement parsing functions
//

// Block statement parsing function
astBlockStmt *pParseBlockStmt(Parser *p) {
    astBlockStmt *stmt = astBlockStmtNew(p->curToken);
    if (!stmt) {
        return NULL;
    }

    if (pPeekTokenIs(p, VAR)) {
        pNextToken(p);
        astStatement *s = (astStatement *)pParseVarStmt(p, true);

        if (s) {
            if (stmt->size == 0 && !stmt->statements) {
                stmt->statements = (astStatement **)malloc(sizeof(astStatement *));
                stmt->size       = 1;
            } else {
                stmt->size++;
                stmt->statements = (astStatement **)realloc(stmt->statements, sizeof(astStatement *) * stmt->size);
            }
            stmt->statements[stmt->size - 1] = s;
        }
    }

    while (pPeekTokenIs(p, PROCEDURE) || pPeekTokenIs(p, FUNCTION)) {
        pNextToken(p);
        astStatement *s = (astStatement *)pParseFunctionStmt(p);

        if (s) {
            if (stmt->size == 0 && !stmt->statements) {
                stmt->statements = (astStatement **)malloc(sizeof(astStatement *));
                stmt->size       = 1;
            } else {
                stmt->size++;
                stmt->statements = (astStatement **)realloc(stmt->statements, sizeof(astStatement *) * stmt->size);
            }
            stmt->statements[stmt->size - 1] = s;
        }
    }

    if (pPeekTokenIs(p, BEGIN)) {
        pNextToken(p);
        astStatement *s = (astStatement *)pParseBeginEndStmt(p);

        if (s) {
            if (stmt->size == 0 && !stmt->statements) {
                stmt->statements = (astStatement **)malloc(sizeof(astStatement *));
                stmt->size       = 1;
            } else {
                stmt->size++;
                stmt->statements = (astStatement **)realloc(stmt->statements, sizeof(astStatement *) * stmt->size);
            }
            stmt->statements[stmt->size - 1] = s;
        }
    } else {
        pCustomError(p, "Bloco inválido, esperava-se `BEGIN`");
        return NULL;
    }

    if (!pExpectPeek(p, END, "END")) {
        return NULL;
    }

    tFreeToken(p->curToken);  // Free the unused `END` token

    return stmt;
}

// Var statement parsing function
astVarStmt *pParseVarStmt(Parser *p, bool isGlobal) {
    astVarStmt *stmt = astVarStmtNew(p->curToken);
    if (!stmt) {
        return NULL;
    }

    while (pPeekTokenIs(p, IDENT)) {
        pNextToken(p);
        astDeclarationStmt *decl = pParseDeclarationStmt(p);
        if (decl) {
            if (stmt->size == 0 && !stmt->declarations) {
                stmt->declarations = (astDeclarationStmt **)malloc(sizeof(astDeclarationStmt *));
                stmt->size         = 1;
            } else {
                stmt->size++;
                stmt->declarations =
                    (astDeclarationStmt **)realloc(stmt->declarations, sizeof(astDeclarationStmt *) * stmt->size);
            }
            stmt->declarations[stmt->size - 1] = decl;
        }

        if (isGlobal) {
            if (!pExpectPeek(p, SEMICOLON, ";")) {
                return NULL;
            }

            tFreeToken(p->curToken);  // Free the unused `;` token
        }
    }

    return stmt;
}

// Declaration statement parsing function
astDeclarationStmt *pParseDeclarationStmt(Parser *p) {
    astDeclarationStmt *stmt = astDeclarationStmtNew(p->curToken);
    if (!stmt) {
        return NULL;
    }

    stmt->identifier    = (astIdentifierExpr **)malloc(sizeof(astIdentifierExpr *));
    stmt->identifier[0] = pParseIdentifierExpr(p);
    stmt->size          = 1;

    while (pPeekTokenIs(p, COMMA)) {
        pNextToken(p);
        tFreeToken(p->curToken);  // Free the unused `,` token
        pNextToken(p);

        stmt->size++;
        stmt->identifier = (astIdentifierExpr **)realloc(stmt->identifier, sizeof(astIdentifierExpr *) * stmt->size);
        stmt->identifier[stmt->size - 1] = pParseIdentifierExpr(p);
    }

    if (!pExpectPeek(p, COLON, ":")) {
        return NULL;
    }

    tFreeToken(p->curToken);  // Free the unused `:` token
    pNextToken(p);

    stmt->type = pParseTypeExpr(p);

    return stmt;
}

// Function/Procedure statement parsing function
astFunctionStmt *pParseFunctionStmt(Parser *p) {
    astFunctionStmt *stmt = astFunctionStmtNew(p->curToken);
    if (!stmt) {
        return NULL;
    }

    if (!pExpectPeek(p, IDENT, "IDENT")) {
        return NULL;
    }

    stmt->identifier = pParseIdentifierExpr(p);

    if (pPeekTokenIs(p, LPAREN)) {
        pNextToken(p);
        tFreeToken(p->curToken);  // Free the unused `(` token

        while (!pPeekTokenIs(p, RPAREN)) {
            astParameterStmt *param = pParseParameterStmt(p);
            if (!param) {
                pCustomError(p, "Parâmetro inválido");
                return NULL;
            }

            if (stmt->size == 0 && !stmt->parameters) {
                stmt->parameters = (astParameterStmt **)malloc(sizeof(astParameterStmt *));
                stmt->size       = 1;
            } else {
                stmt->size++;
                stmt->parameters =
                    (astParameterStmt **)realloc(stmt->parameters, sizeof(astParameterStmt *) * stmt->size);
            }
            stmt->parameters[stmt->size - 1] = param;

            if (!pPeekTokenIs(p, RPAREN)) {
                if (!pExpectPeek(p, SEMICOLON, ";")) {
                    return NULL;
                }

                tFreeToken(p->curToken);  // Free the unused `;` token
            }
        }

        if (!pExpectPeek(p, RPAREN, ")")) {
            return NULL;
        }

        tFreeToken(p->curToken);  // Free the unused `)` token
    }

    if (stmt->token->type == FUNCTION) {
        if (!pExpectPeek(p, COLON, ":")) {
            return NULL;
        }

        tFreeToken(p->curToken);  // Free the unused `:` token
        pNextToken(p);

        stmt->returnType = pParseTypeExpr(p);
    }

    if (!pExpectPeek(p, SEMICOLON, ";")) {
        return NULL;
    }

    tFreeToken(p->curToken);  // Free the unused `;` token

    stmt->block = pParseBlockStmt(p);

    return stmt;
}

// Parameter statement parsing function
astParameterStmt *pParseParameterStmt(Parser *p) {
    astParameterStmt *stmt = astParameterStmtNew(p->curToken);
    if (!stmt) {
        return NULL;
    }

    if (pPeekTokenIs(p, VAR)) {
        pNextToken(p);
        tFreeToken(p->curToken);  // Free the unused `VAR` token

        stmt->isVar = true;
    }

    if (!pExpectPeek(p, IDENT, "IDENT")) {
        return NULL;
    }

    astDeclarationStmt *decl = pParseDeclarationStmt(p);
    if (!decl) {
        pCustomError(p, "Declaração de parâmetro inválida");
        return NULL;
    }
    stmt->declarations                 = (astDeclarationStmt **)malloc(sizeof(astDeclarationStmt *));
    stmt->size                         = 1;
    stmt->declarations[stmt->size - 1] = decl;

    while (pPeekTokenIs(p, COMMA)) {
        pNextToken(p);
        tFreeToken(p->curToken);  // Free the unused `,` token
        pNextToken(p);

        decl = NULL;
        decl = pParseDeclarationStmt(p);
        if (!decl) {
            pCustomError(p, "Declaração de parâmetro inválida");
            return NULL;
        }

        stmt->size++;
        stmt->declarations =
            (astDeclarationStmt **)realloc(stmt->declarations, sizeof(astDeclarationStmt *) * stmt->size);
        stmt->declarations[stmt->size - 1] = decl;
    }

    return stmt;
}

// Begin/End statement parsing function
astBeginEndStmt *pParseBeginEndStmt(Parser *p) {
    astBeginEndStmt *stmt = astBeginEndStmtNew(p->curToken);
    if (!stmt) {
        return NULL;
    }

    while (!pPeekTokenIs(p, END)) {
        pNextToken(p);
        astExpressionStmt *expr = pParseExpressionStmt(p);
        if (expr) {
            if (stmt->size == 0 && !stmt->statements) {
                stmt->statements = (astExpressionStmt **)malloc(sizeof(astExpressionStmt *));
                stmt->size       = 1;
            } else {
                stmt->size++;
                stmt->statements =
                    (astExpressionStmt **)realloc(stmt->statements, sizeof(astExpressionStmt *) * stmt->size);
            }
            stmt->statements[stmt->size - 1] = expr;
        }
    }

    return stmt;
}

// Conditional statement parsing function
astConditionalStmt *pParseConditionalStmt(Parser *p) {
    astConditionalStmt *stmt = astConditionalStmtNew(p->curToken);
    if (!stmt) {
        return NULL;
    }

    pNextToken(p);
    stmt->condition = pParseExpression(p, LOWEST);

    if (!pExpectPeek(p, THEN, "THEN")) {
        return NULL;
    }

    tFreeToken(p->curToken);  // Free the unused `THEN` token
    pNextToken(p);

    if (pCurTokenIs(p, BEGIN)) {
        stmt->consequence = (astStatement *)pParseBeginEndStmt(p);

        if (!pExpectPeek(p, END, "END")) {
            return NULL;
        }

        tFreeToken(p->curToken);  // Free the unused `END` token
    } else {
        stmt->consequence = (astStatement *)pParseExpressionStmt(p);
    }

    if (pPeekTokenIs(p, ELSE)) {
        pNextToken(p);
        tFreeToken(p->curToken);  // Free the unused `ELSE` token
        pNextToken(p);

        if (pCurTokenIs(p, BEGIN)) {
            stmt->alternative = (astStatement *)pParseBeginEndStmt(p);

            if (!pExpectPeek(p, END, "END")) {
                return NULL;
            }

            tFreeToken(p->curToken);  // Free the unused `END` token
        } else {
            stmt->alternative = (astStatement *)pParseExpressionStmt(p);
        }
    }

    return stmt;
}

// While statement parsing function
astWhileStmt *pParseWhileStmt(Parser *p) {
    astWhileStmt *stmt = astWhileStmtNew(p->curToken);
    if (!stmt) {
        return NULL;
    }

    pNextToken(p);
    stmt->condition = pParseExpression(p, LOWEST);

    if (!pExpectPeek(p, DO, "DO")) {
        return NULL;
    }

    tFreeToken(p->curToken);  // Free the unused `DO` token
    pNextToken(p);

    if (pCurTokenIs(p, BEGIN)) {
        stmt->body = (astStatement *)pParseBeginEndStmt(p);

        if (!pExpectPeek(p, END, "END")) {
            return NULL;
        }

        tFreeToken(p->curToken);  // Free the unused `END` token
    } else {
        stmt->body = (astStatement *)pParseExpressionStmt(p);
    }

    return stmt;
}

// Expression statement parsing function
astExpressionStmt *pParseExpressionStmt(Parser *p) {
    if (pCurTokenIs(p, IF)) {
        return (astExpressionStmt *)pParseConditionalStmt(p);
    } else if (pCurTokenIs(p, WHILE)) {
        return (astExpressionStmt *)pParseWhileStmt(p);
    }

    astExpressionStmt *stmt = astExpressionStmtNew(p->curToken);
    if (!stmt) {
        return NULL;
    }

    p->assignCounter = 0;

    stmt->expr = pParseExpression(p, LOWEST);

    if (!pExpectPeek(p, SEMICOLON, ";")) {
        return NULL;
    }

    tFreeToken(p->curToken);  // Free the unused `;` token

    // if (pPeekTokenIs(p, SEMICOLON)) {
    //     pNextToken(p);
    //     tFreeToken(p->curToken);  // Free the unused `;` token
    // }

    if (p->assignCounter > 1) {
        pCustomError(p, "Multiplos operadores de atribuição em uma única expressão");
        return NULL;
    }

    return stmt;
}

//
// Expression parsing functions
//

// Expression parsing function
astExpression *pParseExpression(Parser *p, Precedence pr) {
    HashMapResult res = hmGet(p->prefixParseFns, &p->curToken->type);
    if (!res.ok) {
        pNoPrefixParseFnError(p, p->curToken);
        return NULL;
    }

    pPrefixParseFn prefix = (pPrefixParseFn)res.data;

    astExpression *left = prefix(p);

    while (!pPeekTokenIs(p, SEMICOLON) && pr < pPeekPrecedence(p)) {
        res = hmGet(p->infixParseFns, &p->peekToken->type);
        if (!res.ok) {
            return left;
        }

        pInfixParseFn infix = (pInfixParseFn)res.data;

        pNextToken(p);

        left = infix(p, left);
    }

    return left;
}

// Prefix expression parsing function
astPrefixExpr *pParsePrefixExpr(Parser *p) {
    astPrefixExpr *expr = astPrefixExprNew(p->curToken);
    if (!expr) {
        return NULL;
    }

    pNextToken(p);
    expr->right = pParseExpression(p, PREFIX);

    return expr;
}

// Infix expression parsing function
astInfixExpr *pParseInfixExpr(Parser *p, astExpression *left) {
    astInfixExpr *expr = astInfixExprNew(p->curToken);
    if (!expr) {
        return NULL;
    }

    expr->left = left;

    Precedence pr = pCurPrecedence(p);
    pNextToken(p);
    expr->right = pParseExpression(p, pr);

    return expr;
}

// Grouped expression parsing function
astExpression *pParseGroupedExpr(Parser *p) {
    pNextToken(p);

    astExpression *expr = pParseExpression(p, LOWEST);

    if (!pExpectPeek(p, RPAREN, ")")) {
        return NULL;
    }

    return expr;
}

// Assignment expression parsing function
astAssignmentExpr *pParseAssignmentExpr(Parser *p, astExpression *left) {
    p->assignCounter++;

    astAssignmentExpr *expr = astAssignmentExprNew(p->curToken);
    if (!expr) {
        return NULL;
    }

    expr->identifier = (astIdentifierExpr *)left;

    pNextToken(p);
    expr->value = pParseExpression(p, ASSIGNMENT);

    return expr;
}

// Identifier expression parsing function
astIdentifierExpr *pParseIdentifierExpr(Parser *p) {
    astIdentifierExpr *ident = astIdentifierExprNew(p->curToken);
    if (!ident) {
        return NULL;
    }

    return ident;
}

// Integer literal expression parsing function
astIntegerExpr *pParseIntegerExpr(Parser *p) {
    astIntegerExpr *integer = astIntegerExprNew(p->curToken);
    if (!integer) {
        return NULL;
    }

    int64_t value  = strtoll(p->curToken->literal, NULL, 10);
    integer->value = value;

    return integer;
}

// Float literal expression parsing function
astFloatExpr *pParseFloatExpr(Parser *p) {
    astFloatExpr *real = astFloatExprNew(p->curToken);
    if (!real) {
        return NULL;
    }

    double value = strtod(p->curToken->literal, NULL);
    real->value  = value;

    return real;
}

// Boolean literal expression parsing function
astBooleanExpr *pParseBooleanExpr(Parser *p) {
    astBooleanExpr *boolean = astBooleanExprNew(p->curToken);
    if (!boolean) {
        return NULL;
    }

    bool value     = pCurTokenIs(p, TRUE);
    boolean->value = value;

    return boolean;
}

// String literal expression parsing function
astStringExpr *pParseStringExpr(Parser *p) {
    astStringExpr *string = astStringExprNew(p->curToken);
    if (!string) {
        return NULL;
    }

    return string;
}

// Character literal expression parsing function
astCharExpr *pParseCharExpr(Parser *p) {
    astCharExpr *character = astCharExprNew(p->curToken);
    if (!character) {
        return NULL;
    }

    return character;
}

// Type expression parsing function
astTypeExpr *pParseTypeExpr(Parser *p) {
    if (!pCurTokenIs(p, INTEGER) && !pCurTokenIs(p, REAL) && !pCurTokenIs(p, BOOLEAN) && !pCurTokenIs(p, CHARACTER) &&
        !pCurTokenIs(p, STRING)) {
        pCustomError(p, "Tipo inválido");
        return NULL;
    }

    astTypeExpr *type = astTypeExprNew(p->curToken);
    if (!type) {
        return NULL;
    }

    return type;
}

// Call expression parsing function
astCallExpr *pParseCallExpr(Parser *p, astExpression *function) {
    astCallExpr *expr = astCallExprNew(p->curToken);
    if (!expr) {
        return NULL;
    }

    expr->identifier = (astIdentifierExpr *)function;

    if (pPeekTokenIs(p, RPAREN)) {
        pNextToken(p);
        tFreeToken(p->curToken);  // Free the unused `)` token
        return expr;
    }

    pNextToken(p);

    expr->arguments    = (astExpression **)malloc(sizeof(astExpression *));
    expr->arguments[0] = pParseExpression(p, LOWEST);
    expr->size         = 1;

    while (pPeekTokenIs(p, COMMA)) {
        pNextToken(p);
        tFreeToken(p->curToken);  // Free the unused `,` token
        pNextToken(p);

        expr->size++;
        expr->arguments = (astExpression **)realloc(expr->arguments, sizeof(astExpression *) * expr->size);
        expr->arguments[expr->size - 1] = pParseExpression(p, LOWEST);
    }

    if (!pExpectPeek(p, RPAREN, ")")) {
        return NULL;
    }

    tFreeToken(p->curToken);  // Free the unused `)` token

    return expr;
}
//
// Error handling
//

// Add a custom error to the parser error list
void pCustomError(Parser *p, char *msg) {
    char error[256];
    sprintf(error, "Linha %d: %s", p->l->line, msg);

    eAdd(p->errors, error);
}

// Add a peek error to the parser error list
void pPeekError(Parser *p, char *str) {
    char error[256];
    sprintf(error, "Linha %d: Esperava-se que o próximo token fosse: `%s`, em vez disso, obteve: `%s`", p->l->line, str,
            p->peekToken->literal);

    eAdd(p->errors, error);
}

// Add a missing prefix parse function error to the parser error list
void pNoPrefixParseFnError(Parser *p, Token *t) {
    char error[256];
    sprintf(error, "Linha %d: Nenhuma função de análise de prefixo encontrada para: `%s`", p->l->line, t->literal);

    eAdd(p->errors, error);
}