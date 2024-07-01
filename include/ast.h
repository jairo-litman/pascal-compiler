#ifndef AST_H
#define AST_H

#include <stdint.h>

#include "token.h"

void astAppendToString(char** buffer, char* str);

//
// Generic AST nodes (pseudo OOP interfaces/abstract)
//

// Base AST node
typedef struct astNode {
    Token* token;
    void (*free)(struct astNode*);
    char* (*toString)(struct astNode*);
} astNode;

// Statements
typedef struct astStatement {
    Token* token;
    void (*free)(struct astStatement*);
    char* (*toString)(struct astStatement*);
} astStatement;

// Expressions
typedef struct astExpression {
    Token* token;
    void (*free)(struct astExpression*);
    char* (*toString)(struct astExpression*);
} astExpression;

//
// Forward declarations
//

typedef struct astProgram astProgram;

typedef struct astBlockStmt       astBlockStmt;
typedef struct astVarStmt         astVarStmt;
typedef struct astDeclarationStmt astDeclarationStmt;
typedef struct astFunctionStmt    astFunctionStmt;
typedef struct astParameterStmt   astParameterStmt;
typedef struct astBeginEndStmt    astBeginEndStmt;
typedef struct astConditionalStmt astConditionalStmt;
typedef struct astWhileStmt       astWhileStmt;
typedef struct astExpressionStmt  astExpressionStmt;

typedef struct astPrefixExpr     astPrefixExpr;
typedef struct astInfixExpr      astInfixExpr;
typedef struct astAssignmentExpr astAssignmentExpr;
typedef struct astIdentifierExpr astIdentifierExpr;
typedef struct astIntegerExpr    astIntegerExpr;
typedef struct astFloatExpr      astFloatExpr;
typedef struct astBooleanExpr    astBooleanExpr;
typedef struct astStringExpr     astStringExpr;
typedef struct astCharExpr       astCharExpr;
typedef struct astTypeExpr       astTypeExpr;
typedef struct astCallExpr       astCallExpr;

//
// Statements
//

// Program is the root node of the AST, `program <identifier>; <block>.`
struct astProgram {
    Token* token;                    // token::PROGRAM
    void (*free)(astProgram*);       // Destructor
    char* (*toString)(astProgram*);  // String representation (for debugging)

    astIdentifierExpr* identifier;   // Program name
    astBlockStmt*      block;        // Block statement
};

astProgram* astProgramNew(Token* token);
void        astProgramFree(astProgram* p);
char*       astProgramToString(astProgram* p);

// Block statement, e.g. `begin <statements> end`
struct astBlockStmt {
    Token* token;                      // Not used
    void (*free)(astBlockStmt*);       // Destructor
    char* (*toString)(astBlockStmt*);  // String representation (for debugging)

    astStatement** statements;         // Series of statements
    uint32_t       size;               // Number of statements
};

astBlockStmt* astBlockStmtNew(Token* token);
void          astBlockStmtFree(astBlockStmt* b);
char*         astBlockStmtToString(astBlockStmt* b);

// Variable declaration block, e.g. `var x: integer; y: real;`
struct astVarStmt {
    Token* token;                       // token::VAR
    void (*free)(astVarStmt*);          // Destructor
    char* (*toString)(astVarStmt*);     // String representation (for debugging)

    astDeclarationStmt** declarations;  // Series of declaration statements
    uint16_t             size;          // Number of declaration statements
};

astVarStmt* astVarStmtNew(Token* token);
void        astVarStmtFree(astVarStmt* v);
char*       astVarStmtToString(astVarStmt* v);

// Variable declaration statement, e.g. `x, y: integer`
struct astDeclarationStmt {
    Token* token;                            // token::IDENT
    void (*free)(astDeclarationStmt*);       // Destructor
    char* (*toString)(astDeclarationStmt*);  // String representation (for debugging)

    astIdentifierExpr** identifier;          // Series of identifiers
    uint16_t            size;                // Number of identifiers
    astTypeExpr*        type;                // Variable(s) type
};

astDeclarationStmt* astDeclarationStmtNew(Token* token);
void                astDeclarationStmtFree(astDeclarationStmt* d);
char*               astDeclarationStmtToString(astDeclarationStmt* d);

/*
Function statement, e.g. `function myFunction(x: integer): real; begin end`
Also a procedure statement when `returnType` is `NULL`
*/
struct astFunctionStmt {
    Token* token;                         // token::FUNCTION
    void (*free)(astFunctionStmt*);       // Destructor
    char* (*toString)(astFunctionStmt*);  // String representation (for debugging)

    astIdentifierExpr* identifier;        // Function name
    astParameterStmt** parameters;        // Function parameters
    uint16_t           size;              // Number of parameters
    astTypeExpr*       returnType;        // Function return type
    astBlockStmt*      block;             // Block statement
};

astFunctionStmt* astFunctionStmtNew(Token* token);
void             astFunctionStmtFree(astFunctionStmt* f);
char*            astFunctionStmtToString(astFunctionStmt* f);

// Function parameter statement, e.g. `x, y: integer` or `var x, y: integer`
struct astParameterStmt {
    Token* token;                          // token::IDENT or token::VAR
    void (*free)(astParameterStmt*);       // Destructor
    char* (*toString)(astParameterStmt*);  // String representation (for debugging)

    astDeclarationStmt** declarations;     // Series of declaration statements
    uint16_t             size;             // Number of declaration statements
    bool                 isVar;            // Is a reference parameter
};

astParameterStmt* astParameterStmtNew(Token* token);
void              astParameterStmtFree(astParameterStmt* p);
char*             astParameterStmtToString(astParameterStmt* p);

// Begin-end statement, e.g. `begin <statements> end`
struct astBeginEndStmt {
    Token* token;                         // token::BEGIN
    void (*free)(astBeginEndStmt*);       // Destructor
    char* (*toString)(astBeginEndStmt*);  // String representation (for debugging)

    astExpressionStmt** statements;       // Series of expressions
    uint32_t            size;             // Number of expressions
};

astBeginEndStmt* astBeginEndStmtNew(Token* token);
void             astBeginEndStmtFree(astBeginEndStmt* b);
char*            astBeginEndStmtToString(astBeginEndStmt* b);

// Conditional statement, e.g. `if <condition> then <consequence> else <alternative>`
struct astConditionalStmt {
    Token* token;                            // token::IF
    void (*free)(astConditionalStmt*);       // Destructor
    char* (*toString)(astConditionalStmt*);  // String representation (for debugging)

    astExpression* condition;                // Condition
    astStatement*  consequence;              // Consequence
    astStatement*  alternative;              // Alternative
};

astConditionalStmt* astConditionalStmtNew(Token* token);
void                astConditionalStmtFree(astConditionalStmt* c);
char*               astConditionalStmtToString(astConditionalStmt* c);

// While statement, e.g. `while <condition> do <body>`
struct astWhileStmt {
    Token* token;                      // token::WHILE
    void (*free)(astWhileStmt*);       // Destructor
    char* (*toString)(astWhileStmt*);  // String representation (for debugging)

    astExpression* condition;          // Condition
    astStatement*  body;               // Body
};

astWhileStmt* astWhileStmtNew(Token* token);
void          astWhileStmtFree(astWhileStmt* w);
char*         astWhileStmtToString(astWhileStmt* w);

// Expression statement, e.g. `5 + 5`
struct astExpressionStmt {
    Token* token;                           // First token of the expression
    void (*free)(astExpressionStmt*);       // Destructor
    char* (*toString)(astExpressionStmt*);  // String representation (for debugging)

    astExpression* expr;                    // Expression
};

astExpressionStmt* astExpressionStmtNew(Token* token);
void               astExpressionStmtFree(astExpressionStmt* e);
char*              astExpressionStmtToString(astExpressionStmt* e);

//
// Expressions
//

// Prefix expression, e.g. `-5`
struct astPrefixExpr {
    Token* token;                       // Operator token, e.g. token::MINUS
    void (*free)(astPrefixExpr*);       // Destructor
    char* (*toString)(astPrefixExpr*);  // String representation (for debugging)

    char*          op;                  // Operator
    astExpression* right;               // Right-hand side expression
};

astPrefixExpr* astPrefixExprNew(Token* token);
void           astPrefixExprFree(astPrefixExpr* p);
char*          astPrefixExprToString(astPrefixExpr* p);

// Infix expression, e.g. `5 + 5`
struct astInfixExpr {
    Token* token;                      // Operator token, e.g. token::PLUS
    void (*free)(astInfixExpr*);       // Destructor
    char* (*toString)(astInfixExpr*);  // String representation (for debugging)

    char*          op;                 // Operator
    astExpression* left;               // Left-hand side expression
    astExpression* right;              // Right-hand side expression
};

astInfixExpr* astInfixExprNew(Token* token);
void          astInfixExprFree(astInfixExpr* i);
char*         astInfixExprToString(astInfixExpr* i);

// Assignment expression, e.g. `x := 5`
struct astAssignmentExpr {
    Token* token;                           // token::ASSIGN
    void (*free)(astAssignmentExpr*);       // Destructor
    char* (*toString)(astAssignmentExpr*);  // String representation (for debugging)

    astIdentifierExpr* identifier;          // Identifier
    astExpression*     value;               // Value
};

astAssignmentExpr* astAssignmentExprNew(Token* token);
void               astAssignmentExprFree(astAssignmentExpr* a);
char*              astAssignmentExprToString(astAssignmentExpr* a);

// Identifier expression, e.g. `foo`
struct astIdentifierExpr {
    Token* token;                           // token::IDENT
    void (*free)(astIdentifierExpr*);       // Destructor
    char* (*toString)(astIdentifierExpr*);  // String representation (for debugging)

    char* value;                            // Identifier name
};

astIdentifierExpr* astIdentifierExprNew(Token* token);
void               astIdentifierExprFree(astIdentifierExpr* i);
char*              astIdentifierExprToString(astIdentifierExpr* i);

// Integer literal expression, e.g. `5`
struct astIntegerExpr {
    Token* token;                        // token::INT
    void (*free)(astIntegerExpr*);       // Destructor
    char* (*toString)(astIntegerExpr*);  // String representation (for debugging)

    int64_t value;                       // Integer value
};

astIntegerExpr* astIntegerExprNew(Token* token);
void            astIntegerExprFree(astIntegerExpr* i);
char*           astIntegerExprToString(astIntegerExpr* i);

// Float literal expression, e.g. `5.0`
struct astFloatExpr {
    Token* token;                      // token::FLOAT
    void (*free)(astFloatExpr*);       // Destructor
    char* (*toString)(astFloatExpr*);  // String representation (for debugging)

    double value;                      // Float value
};

astFloatExpr* astFloatExprNew(Token* token);
void          astFloatExprFree(astFloatExpr* f);
char*         astFloatExprToString(astFloatExpr* f);

// Boolean literal expression, e.g. `true` or `false`
struct astBooleanExpr {
    Token* token;                        // token::TRUE or token::FALSE
    void (*free)(astBooleanExpr*);       // Destructor
    char* (*toString)(astBooleanExpr*);  // String representation (for debugging)

    bool value;                          // Boolean value
};

astBooleanExpr* astBooleanExprNew(Token* token);
void            astBooleanExprFree(astBooleanExpr* b);
char*           astBooleanExprToString(astBooleanExpr* b);

// String literal expression, e.g. `"hello"`
struct astStringExpr {
    Token* token;                       // token::STRING
    void (*free)(astStringExpr*);       // Destructor
    char* (*toString)(astStringExpr*);  // String representation (for debugging)

    char* value;                        // String value
};

astStringExpr* astStringExprNew(Token* token);
void           astStringExprFree(astStringExpr* s);
char*          astStringExprToString(astStringExpr* s);

// Character literal expression, e.g. `'a'`
struct astCharExpr {
    Token* token;                     // token::CHAR
    void (*free)(astCharExpr*);       // Destructor
    char* (*toString)(astCharExpr*);  // String representation (for debugging)

    char value;                       // Character value
};

astCharExpr* astCharExprNew(Token* token);
void         astCharExprFree(astCharExpr* c);
char*        astCharExprToString(astCharExpr* c);

// Type expression, e.g. `integer`
struct astTypeExpr {
    Token* token;                     // token::INTEGER, token::REAL, token::BOOLEAN, token::CHARACTER, token::STRING
    void (*free)(astTypeExpr*);       // Destructor
    char* (*toString)(astTypeExpr*);  // String representation (for debugging)

    char* value;                      // Type name
};

astTypeExpr* astTypeExprNew(Token* token);
void         astTypeExprFree(astTypeExpr* t);
char*        astTypeExprToString(astTypeExpr* t);

struct astCallExpr {
    Token* token;                     // token::IDENT
    void (*free)(astCallExpr*);       // Destructor
    char* (*toString)(astCallExpr*);  // String representation (for debugging)

    astIdentifierExpr* identifier;    // Function name
    astExpression**    arguments;     // Function arguments
    uint16_t           size;          // Number of arguments
};

astCallExpr* astCallExprNew(Token* token);
void         astCallExprFree(astCallExpr* c);
char*        astCallExprToString(astCallExpr* c);

#endif  // AST_H