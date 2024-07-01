#include "ast.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "token.h"

uint32_t indentLevel = 0;

// Function for appending a string to another string, allocating memory as needed
void astAppendToString(char** buffer, char* str) {
    if (*buffer == NULL) {
        *buffer = (char*)malloc(strlen(str) + 1);
        strcpy(*buffer, str);
    } else {
        *buffer = (char*)realloc(*buffer, strlen(*buffer) + strlen(str) + 1);
        strcat(*buffer, str);
    }
}

// Function for creating an indentation string
char* indentString() {
    char* indent = (char*)malloc(indentLevel + 1);
    for (uint32_t i = 0; i < indentLevel; i++) {
        indent[i] = '\t';
    }
    indent[indentLevel] = '\0';
    return indent;
}

//
// Program
//

// Create a new program node
astProgram* astProgramNew(Token* token) {
    astProgram* p = (astProgram*)malloc(sizeof(astProgram));
    if (p == NULL) {
        return NULL;
    }

    p->token      = token;
    p->identifier = NULL;
    p->block      = NULL;

    p->free     = astProgramFree;
    p->toString = astProgramToString;

    return p;
}

// Free the program node
void astProgramFree(astProgram* p) {
    if (p) {
        tFreeToken(p->token);

        if (p->identifier) {
            p->identifier->free(p->identifier);
        }

        if (p->block) {
            p->block->free(p->block);
        }

        free(p);
    }
}

// Convert the program node to a string
char* astProgramToString(astProgram* p) {
    char* buffer = NULL;

    astAppendToString(&buffer, "Program: {\n\tIdentifier: ");
    astAppendToString(&buffer, p->identifier->toString(p->identifier));
    astAppendToString(&buffer, "\n");

    indentLevel++;

    char* indent = indentString();

    char* block = p->block->toString(p->block);

    astAppendToString(&buffer, indent);
    astAppendToString(&buffer, block);

    free(block);
    free(indent);

    astAppendToString(&buffer, "}\n");

    indentLevel--;

    return buffer;
}

//
// Block statement
//

// Create a new block statement node
astBlockStmt* astBlockStmtNew(Token* token) {
    astBlockStmt* b = (astBlockStmt*)malloc(sizeof(astBlockStmt));
    if (b == NULL) {
        return NULL;
    }

    b->token      = token;
    b->statements = NULL;
    b->size       = 0;

    b->free     = astBlockStmtFree;
    b->toString = astBlockStmtToString;

    return b;
}

// Free the block statement node
void astBlockStmtFree(astBlockStmt* b) {
    if (b) {
        // Doesn't free the token because it's a reference to the token in the AST

        if (b->statements) {
            for (uint32_t i = 0; i < b->size; i++) {
                if (b->statements[i]) {
                    b->statements[i]->free(b->statements[i]);
                }
            }

            free(b->statements);
        }

        free(b);
    }
}

// Convert the block statement node to a string
char* astBlockStmtToString(astBlockStmt* b) {
    char* buffer = NULL;

    astAppendToString(&buffer, "Block: {\n");
    indentLevel++;
    char* indent = indentString();

    for (uint32_t i = 0; i < b->size; i++) {
        char* stmt = b->statements[i]->toString(b->statements[i]);
        astAppendToString(&buffer, indent);
        astAppendToString(&buffer, stmt);
        astAppendToString(&buffer, "\n");
        free(stmt);
    }

    indentLevel--;
    indent[indentLevel] = '\0';

    astAppendToString(&buffer, indent);

    astAppendToString(&buffer, "}\n");

    free(indent);
    return buffer;
}

//
// Var Statement
//

// Create a new var statement node
astVarStmt* astVarStmtNew(Token* token) {
    astVarStmt* v = (astVarStmt*)malloc(sizeof(astVarStmt));
    if (v == NULL) {
        return NULL;
    }

    v->token        = token;
    v->declarations = NULL;
    v->size         = 0;

    v->free     = astVarStmtFree;
    v->toString = astVarStmtToString;

    return v;
}

// Free the var statement node
void astVarStmtFree(astVarStmt* v) {
    if (v) {
        tFreeToken(v->token);

        if (v->declarations) {
            for (uint16_t i = 0; i < v->size; i++) {
                if (v->declarations[i]) {
                    v->declarations[i]->free(v->declarations[i]);
                }
            }

            free(v->declarations);
        }

        free(v);
    }
}

// Convert the var statement node to a string
char* astVarStmtToString(astVarStmt* v) {
    char* buffer = NULL;

    astAppendToString(&buffer, "Var: {\n");
    indentLevel++;
    char* indent = indentString();

    for (uint16_t i = 0; i < v->size; i++) {
        char* decl = v->declarations[i]->toString(v->declarations[i]);
        astAppendToString(&buffer, indent);
        astAppendToString(&buffer, decl);
        astAppendToString(&buffer, "\n");
        free(decl);
    }

    indentLevel--;
    indent[indentLevel] = '\0';

    astAppendToString(&buffer, indent);
    astAppendToString(&buffer, "}");

    free(indent);
    return buffer;
}

//
// Variable declaration statement
//

// Create a new declaration node
astDeclarationStmt* astDeclarationStmtNew(Token* token) {
    astDeclarationStmt* d = (astDeclarationStmt*)malloc(sizeof(astDeclarationStmt));
    if (d == NULL) {
        return NULL;
    }

    d->token      = token;
    d->identifier = NULL;
    d->size       = 0;
    d->type       = NULL;

    d->free     = astDeclarationStmtFree;
    d->toString = astDeclarationStmtToString;

    return d;
}

// Free the declaration node
void astDeclarationStmtFree(astDeclarationStmt* d) {
    if (d) {
        tFreeToken(d->token);

        if (d->identifier) {
            for (uint16_t i = 0; i < d->size; i++) {
                if (d->identifier[i]) {
                    d->identifier[i]->free(d->identifier[i]);
                }
            }

            free(d->identifier);
        }

        if (d->type) {
            d->type->free(d->type);
        }

        free(d);
    }
}

// Convert the declaration node to a string
char* astDeclarationStmtToString(astDeclarationStmt* d) {
    char* buffer = NULL;

    astAppendToString(&buffer, "Declaration: {\n");
    indentLevel++;
    char* indent = indentString();

    astAppendToString(&buffer, indent);
    astAppendToString(&buffer, "Identifiers: {");

    for (uint16_t i = 0; i < d->size; i++) {
        char* id = d->identifier[i]->toString(d->identifier[i]);
        astAppendToString(&buffer, id);
        free(id);

        if (i < d->size - 1) {
            astAppendToString(&buffer, ", ");
        }
    }

    astAppendToString(&buffer, "}\n");
    astAppendToString(&buffer, indent);
    astAppendToString(&buffer, "Type: ");

    if (d->type) {
        char* type = d->type->toString(d->type);
        astAppendToString(&buffer, type);
        free(type);
    }

    astAppendToString(&buffer, "\n");

    indentLevel--;
    indent[indentLevel] = '\0';

    astAppendToString(&buffer, indent);

    astAppendToString(&buffer, "}");

    free(indent);
    return buffer;
}

//
// Function statement
//

// Create a new function statement node
astFunctionStmt* astFunctionStmtNew(Token* token) {
    astFunctionStmt* f = (astFunctionStmt*)malloc(sizeof(astFunctionStmt));
    if (f == NULL) {
        return NULL;
    }

    f->token      = token;
    f->identifier = NULL;
    f->parameters = NULL;
    f->size       = 0;
    f->returnType = NULL;
    f->block      = NULL;

    f->free     = astFunctionStmtFree;
    f->toString = astFunctionStmtToString;

    return f;
}

// Free the function statement node
void astFunctionStmtFree(astFunctionStmt* f) {
    if (f) {
        tFreeToken(f->token);

        if (f->identifier) {
            f->identifier->free(f->identifier);
        }

        if (f->parameters) {
            for (uint16_t i = 0; i < f->size; i++) {
                if (f->parameters[i]) {
                    f->parameters[i]->free(f->parameters[i]);
                }
            }

            free(f->parameters);
        }

        if (f->returnType) {
            f->returnType->free(f->returnType);
        }

        if (f->block) {
            f->block->free(f->block);
        }

        free(f);
    }
}

// Convert the function statement node to a string
char* astFunctionStmtToString(astFunctionStmt* f) {
    char* buffer = NULL;

    if (f->returnType) {
        astAppendToString(&buffer, "Function: {\n");
    } else {
        astAppendToString(&buffer, "Procedure: {\n");
    }
    indentLevel++;
    char* indent = indentString();

    astAppendToString(&buffer, indent);
    astAppendToString(&buffer, "Identifier: ");
    char* id = f->identifier->toString(f->identifier);
    astAppendToString(&buffer, id);
    free(id);
    astAppendToString(&buffer, "\n");

    astAppendToString(&buffer, indent);
    astAppendToString(&buffer, "Parameters: {\n");

    free(indent);
    indentLevel++;
    indent = indentString();

    for (uint16_t i = 0; i < f->size; i++) {
        char* param = f->parameters[i]->toString(f->parameters[i]);
        astAppendToString(&buffer, indent);
        astAppendToString(&buffer, param);
        astAppendToString(&buffer, "\n");
        free(param);
    }

    indentLevel--;
    indent[indentLevel] = '\0';

    astAppendToString(&buffer, indent);
    astAppendToString(&buffer, "}\n");

    if (f->returnType) {
        astAppendToString(&buffer, indent);
        astAppendToString(&buffer, "Return type: ");
        char* type = f->returnType->toString(f->returnType);
        astAppendToString(&buffer, type);
        free(type);
        astAppendToString(&buffer, "\n");
    }

    char* block = f->block->toString(f->block);
    astAppendToString(&buffer, indent);
    astAppendToString(&buffer, block);
    free(block);

    indentLevel--;
    indent[indentLevel] = '\0';

    astAppendToString(&buffer, indent);

    astAppendToString(&buffer, "}");

    free(indent);
    return buffer;
}

//
// Parameter statement
//

// Create a new parameter node
astParameterStmt* astParameterStmtNew(Token* token) {
    astParameterStmt* p = (astParameterStmt*)malloc(sizeof(astParameterStmt));
    if (p == NULL) {
        return NULL;
    }

    p->token        = token;
    p->declarations = NULL;
    p->size         = 0;
    p->isVar        = false;

    p->free     = astParameterStmtFree;
    p->toString = astParameterStmtToString;

    return p;
}

// Free the parameter node
void astParameterStmtFree(astParameterStmt* p) {
    if (p) {
        tFreeToken(p->token);

        if (p->declarations) {
            for (uint16_t i = 0; i < p->size; i++) {
                if (p->declarations[i]) {
                    p->declarations[i]->free(p->declarations[i]);
                }
            }

            free(p->declarations);
        }

        free(p);
    }
}

// Convert the parameter node to a string
char* astParameterStmtToString(astParameterStmt* p) {
    char* buffer = NULL;

    astAppendToString(&buffer, "Parameter block: {\n");
    indentLevel++;
    char* indent = indentString();

    if (p->isVar) {
        astAppendToString(&buffer, indent);
        astAppendToString(&buffer, "Var: true\n");
    } else {
        astAppendToString(&buffer, indent);
        astAppendToString(&buffer, "Var: false\n");
    }

    astAppendToString(&buffer, indent);
    astAppendToString(&buffer, "Declarations: {\n");
    indentLevel++;
    free(indent);
    indent = indentString();

    for (uint16_t i = 0; i < p->size; i++) {
        char* decl = p->declarations[i]->toString(p->declarations[i]);
        astAppendToString(&buffer, indent);
        astAppendToString(&buffer, decl);
        astAppendToString(&buffer, "\n");
        free(decl);
    }

    indentLevel--;
    indent[indentLevel] = '\0';

    astAppendToString(&buffer, indent);
    astAppendToString(&buffer, "}\n");

    indentLevel--;
    indent[indentLevel] = '\0';

    astAppendToString(&buffer, indent);

    astAppendToString(&buffer, "}");

    free(indent);
    return buffer;
}

//
// Begin End statement
//

// Create a new begin-end statement node
astBeginEndStmt* astBeginEndStmtNew(Token* token) {
    astBeginEndStmt* b = (astBeginEndStmt*)malloc(sizeof(astBeginEndStmt));
    if (b == NULL) {
        return NULL;
    }

    b->token      = token;
    b->statements = NULL;
    b->size       = 0;

    b->free     = astBeginEndStmtFree;
    b->toString = astBeginEndStmtToString;

    return b;
}

// Free the begin-end statement node
void astBeginEndStmtFree(astBeginEndStmt* b) {
    if (b) {
        tFreeToken(b->token);

        if (b->statements) {
            for (uint32_t i = 0; i < b->size; i++) {
                if (b->statements[i]) {
                    b->statements[i]->free(b->statements[i]);
                }
            }

            free(b->statements);
        }

        free(b);
    }
}

// Convert the begin-end statement node to a string
char* astBeginEndStmtToString(astBeginEndStmt* b) {
    char* buffer = NULL;

    astAppendToString(&buffer, "Begin: {\n");
    indentLevel++;
    char* indent = indentString();

    for (uint32_t i = 0; i < b->size; i++) {
        char* stmt = b->statements[i]->toString(b->statements[i]);
        astAppendToString(&buffer, indent);
        astAppendToString(&buffer, stmt);
        astAppendToString(&buffer, "\n");
        free(stmt);
    }

    indentLevel--;
    indent[indentLevel] = '\0';

    astAppendToString(&buffer, indent);

    astAppendToString(&buffer, "}");

    return buffer;
}

//
// Conditional statement
//

// Create a new conditional node
astConditionalStmt* astConditionalStmtNew(Token* token) {
    astConditionalStmt* c = (astConditionalStmt*)malloc(sizeof(astConditionalStmt));
    if (c == NULL) {
        return NULL;
    }

    c->token       = token;
    c->condition   = NULL;
    c->consequence = NULL;
    c->alternative = NULL;

    c->free     = astConditionalStmtFree;
    c->toString = astConditionalStmtToString;

    return c;
}

// Free the conditional node
void astConditionalStmtFree(astConditionalStmt* c) {
    if (c) {
        tFreeToken(c->token);

        if (c->condition) {
            c->condition->free(c->condition);
        }

        if (c->consequence) {
            c->consequence->free(c->consequence);
        }

        if (c->alternative) {
            c->alternative->free(c->alternative);
        }

        free(c);
    }
}

// Convert the conditional node to a string
char* astConditionalStmtToString(astConditionalStmt* c) {
    char* buffer = NULL;

    astAppendToString(&buffer, "Conditional: {\n");
    indentLevel++;
    char* indent = indentString();

    astAppendToString(&buffer, indent);
    astAppendToString(&buffer, "Condition: ");
    char* condition = c->condition->toString(c->condition);
    astAppendToString(&buffer, condition);
    free(condition);
    astAppendToString(&buffer, "\n");

    astAppendToString(&buffer, indent);
    astAppendToString(&buffer, "Consequence: {\n");

    free(indent);
    indentLevel++;
    indent = indentString();

    char* cons = c->consequence->toString(c->consequence);
    astAppendToString(&buffer, indent);
    astAppendToString(&buffer, cons);
    astAppendToString(&buffer, "\n");
    free(cons);

    indentLevel--;
    indent[indentLevel] = '\0';

    astAppendToString(&buffer, indent);
    astAppendToString(&buffer, "}\n");

    if (c->alternative) {
        astAppendToString(&buffer, indent);
        astAppendToString(&buffer, "Alternative: {\n");

        free(indent);
        indentLevel++;
        indent = indentString();

        char* alt = c->alternative->toString(c->alternative);
        astAppendToString(&buffer, indent);
        astAppendToString(&buffer, alt);
        astAppendToString(&buffer, "\n");
        free(alt);

        indentLevel--;
        indent[indentLevel] = '\0';

        astAppendToString(&buffer, indent);
        astAppendToString(&buffer, "}\n");
    }

    indentLevel--;
    indent[indentLevel] = '\0';

    astAppendToString(&buffer, indent);

    astAppendToString(&buffer, "}");

    free(indent);
    return buffer;
}

//
// While Loop statement
//

// Create a new while loop node
astWhileStmt* astWhileStmtNew(Token* token) {
    astWhileStmt* w = (astWhileStmt*)malloc(sizeof(astWhileStmt));
    if (w == NULL) {
        return NULL;
    }

    w->token     = token;
    w->condition = NULL;
    w->body      = NULL;

    w->free     = astWhileStmtFree;
    w->toString = astWhileStmtToString;

    return w;
}

// Free the while loop node
void astWhileStmtFree(astWhileStmt* w) {
    if (w) {
        tFreeToken(w->token);

        if (w->condition) {
            w->condition->free(w->condition);
        }

        if (w->body) {
            w->body->free(w->body);
        }

        free(w);
    }
}

// Convert the while loop node to a string
char* astWhileStmtToString(astWhileStmt* w) {
    char* buffer = NULL;

    astAppendToString(&buffer, "While: {\n");
    indentLevel++;
    char* indent = indentString();

    astAppendToString(&buffer, indent);
    astAppendToString(&buffer, "Condition: ");
    char* condition = w->condition->toString(w->condition);
    astAppendToString(&buffer, condition);
    free(condition);
    astAppendToString(&buffer, "\n");

    astAppendToString(&buffer, indent);
    astAppendToString(&buffer, "Body: {\n");

    free(indent);
    indentLevel++;
    indent = indentString();

    char* body = w->body->toString(w->body);
    astAppendToString(&buffer, indent);
    astAppendToString(&buffer, body);
    astAppendToString(&buffer, "\n");
    free(body);

    indentLevel--;
    indent[indentLevel] = '\0';

    astAppendToString(&buffer, indent);
    astAppendToString(&buffer, "}\n");

    indentLevel--;
    indent[indentLevel] = '\0';

    astAppendToString(&buffer, indent);

    astAppendToString(&buffer, "}");

    free(indent);
    return buffer;
}

//
// Expression statement
//

// Create a new expression statement node
astExpressionStmt* astExpressionStmtNew(Token* token) {
    astExpressionStmt* e = (astExpressionStmt*)malloc(sizeof(astExpressionStmt));
    if (e == NULL) {
        return NULL;
    }

    e->token = token;
    e->expr  = NULL;

    e->free     = astExpressionStmtFree;
    e->toString = astExpressionStmtToString;

    return e;
}

// Free the expression statement node
void astExpressionStmtFree(astExpressionStmt* e) {
    if (e) {
        tFreeToken(e->token);

        if (e->expr) {
            e->expr->free(e->expr);
        }

        free(e);
    }
}

// Convert the expression statement node to a string
char* astExpressionStmtToString(astExpressionStmt* e) {
    char* buffer = NULL;

    astAppendToString(&buffer, "Expression: {\n");

    indentLevel++;
    char* indent = indentString();
    astAppendToString(&buffer, indent);

    char* expr = e->expr->toString(e->expr);
    astAppendToString(&buffer, expr);
    free(expr);

    astAppendToString(&buffer, "\n");

    indentLevel--;
    indent[indentLevel] = '\0';

    astAppendToString(&buffer, indent);

    astAppendToString(&buffer, "}");

    free(indent);

    return buffer;
}

//
// Prefix expression
//

// Create a new prefix expression node
astPrefixExpr* astPrefixExprNew(Token* token) {
    astPrefixExpr* p = (astPrefixExpr*)malloc(sizeof(astPrefixExpr));
    if (p == NULL) {
        return NULL;
    }

    p->token = token;
    p->op    = token->literal;
    p->right = NULL;

    p->free     = astPrefixExprFree;
    p->toString = astPrefixExprToString;

    return p;
}

// Free the prefix expression node
void astPrefixExprFree(astPrefixExpr* p) {
    if (p) {
        tFreeToken(p->token);  // frees 'token' and 'op'

        if (p->right) {
            p->right->free(p->right);
        }

        free(p);
    }
}

// Convert the prefix expression node to a string
char* astPrefixExprToString(astPrefixExpr* p) {
    char* buffer = NULL;

    astAppendToString(&buffer, "(");
    astAppendToString(&buffer, p->token->literal);

    if (p->right) {
        char* right = p->right->toString(p->right);
        astAppendToString(&buffer, right);
        free(right);
    }

    astAppendToString(&buffer, ")");

    return buffer;
}

//
// Infix expression
//

// Create a new infix expression node
astInfixExpr* astInfixExprNew(Token* token) {
    astInfixExpr* i = (astInfixExpr*)malloc(sizeof(astInfixExpr));
    if (i == NULL) {
        return NULL;
    }

    i->token = token;
    i->op    = token->literal;
    i->left  = NULL;
    i->right = NULL;

    i->free     = astInfixExprFree;
    i->toString = astInfixExprToString;

    return i;
}

// Free the infix expression node
void astInfixExprFree(astInfixExpr* i) {
    if (i) {
        tFreeToken(i->token);  // frees 'token' and 'op'

        if (i->left) {
            i->left->free(i->left);
        }

        if (i->right) {
            i->right->free(i->right);
        }

        free(i);
    }
}

// Convert the infix expression node to a string
char* astInfixExprToString(astInfixExpr* i) {
    char* buffer = NULL;

    astAppendToString(&buffer, "(");

    if (i->left) {
        char* left = i->left->toString(i->left);
        astAppendToString(&buffer, left);
        free(left);
    }

    if (i->op) {
        astAppendToString(&buffer, " ");
        astAppendToString(&buffer, i->op);
        astAppendToString(&buffer, " ");
    }

    if (i->right) {
        char* right = i->right->toString(i->right);
        astAppendToString(&buffer, right);
        free(right);
    }

    astAppendToString(&buffer, ")");

    return buffer;
}

//
// Assignment expression
//

// Create a new assignment node
astAssignmentExpr* astAssignmentExprNew(Token* token) {
    astAssignmentExpr* a = (astAssignmentExpr*)malloc(sizeof(astAssignmentExpr));
    if (a == NULL) {
        return NULL;
    }

    a->token      = token;
    a->identifier = NULL;
    a->value      = NULL;

    a->free     = astAssignmentExprFree;
    a->toString = astAssignmentExprToString;

    return a;
}

// Free the assignment node
void astAssignmentExprFree(astAssignmentExpr* a) {
    if (a) {
        tFreeToken(a->token);

        if (a->identifier) {
            a->identifier->free(a->identifier);
        }

        if (a->value) {
            a->value->free(a->value);
        }

        free(a);
    }
}

// Convert the assignment node to a string
char* astAssignmentExprToString(astAssignmentExpr* a) {
    char* buffer = NULL;

    astAppendToString(&buffer, "Assignment: {\n");
    indentLevel++;
    char* indent = indentString();

    astAppendToString(&buffer, indent);
    astAppendToString(&buffer, "Identifier: ");
    char* id = a->identifier->toString(a->identifier);
    astAppendToString(&buffer, id);
    free(id);
    astAppendToString(&buffer, "\n");

    astAppendToString(&buffer, indent);
    astAppendToString(&buffer, "Value: ");
    char* value = a->value->toString(a->value);
    astAppendToString(&buffer, value);
    free(value);
    astAppendToString(&buffer, "\n");

    indentLevel--;
    indent[indentLevel] = '\0';

    astAppendToString(&buffer, indent);

    astAppendToString(&buffer, "}");

    free(indent);
    return buffer;
}

//
// Identifier expression
//

// Create a new identifier node
astIdentifierExpr* astIdentifierExprNew(Token* token) {
    astIdentifierExpr* id = (astIdentifierExpr*)malloc(sizeof(astIdentifierExpr));
    if (id == NULL) {
        return NULL;
    }

    id->token = token;
    id->value = token->literal;

    id->free     = astIdentifierExprFree;
    id->toString = astIdentifierExprToString;

    return id;
}

// Free the identifier node
void astIdentifierExprFree(astIdentifierExpr* id) {
    if (id) {
        tFreeToken(id->token);  // frees 'token' and 'value'
        free(id);
    }
}

// Convert the identifier node to a string
char* astIdentifierExprToString(astIdentifierExpr* id) {
    char* buffer = NULL;
    astAppendToString(&buffer, id->token->literal);
    return buffer;
}

//
// Integer literal expression
//

// Create a new integer node
astIntegerExpr* astIntegerExprNew(Token* token) {
    astIntegerExpr* integer = (astIntegerExpr*)malloc(sizeof(astIntegerExpr));
    if (integer == NULL) {
        return NULL;
    }

    integer->token = token;
    integer->value = 0;

    integer->free     = astIntegerExprFree;
    integer->toString = astIntegerExprToString;

    return integer;
}

// Free the integer node
void astIntegerExprFree(astIntegerExpr* integer) {
    if (integer) {
        tFreeToken(integer->token);
        free(integer);
    }
}

// Convert the integer node to a string
char* astIntegerExprToString(astIntegerExpr* integer) {
    char* buffer = NULL;
    astAppendToString(&buffer, integer->token->literal);
    return buffer;
}

//
// Float literal expression
//

// Create a new float node
astFloatExpr* astFloatExprNew(Token* token) {
    astFloatExpr* f = (astFloatExpr*)malloc(sizeof(astFloatExpr));
    if (f == NULL) {
        return NULL;
    }

    f->token = token;
    f->value = 0.0;

    f->free     = astFloatExprFree;
    f->toString = astFloatExprToString;

    return f;
}

// Free the float node
void astFloatExprFree(astFloatExpr* f) {
    if (f) {
        tFreeToken(f->token);
        free(f);
    }
}

// Convert the float node to a string
char* astFloatExprToString(astFloatExpr* f) {
    char* buffer = NULL;
    astAppendToString(&buffer, f->token->literal);
    return buffer;
}

//
// Boolean literal expression
//

// Create a new boolean node
astBooleanExpr* astBooleanExprNew(Token* token) {
    astBooleanExpr* b = (astBooleanExpr*)malloc(sizeof(astBooleanExpr));
    if (b == NULL) {
        return NULL;
    }

    b->token = token;
    b->value = false;

    b->free     = astBooleanExprFree;
    b->toString = astBooleanExprToString;

    return b;
}

// Free the boolean node
void astBooleanExprFree(astBooleanExpr* b) {
    if (b) {
        tFreeToken(b->token);
        free(b);
    }
}

// Convert the boolean node to a string
char* astBooleanExprToString(astBooleanExpr* b) {
    char* buffer = NULL;
    astAppendToString(&buffer, b->token->literal);
    return buffer;
}

//
// String literal expression
//

// Create a new string node
astStringExpr* astStringExprNew(Token* token) {
    astStringExpr* s = (astStringExpr*)malloc(sizeof(astStringExpr));
    if (s == NULL) {
        return NULL;
    }

    s->token = token;
    s->value = token->literal;

    s->free     = astStringExprFree;
    s->toString = astStringExprToString;

    return s;
}

// Free the string node
void astStringExprFree(astStringExpr* s) {
    if (s) {
        tFreeToken(s->token);  // frees 'token' and 'value'
        free(s);
    }
}

// Convert the string node to a string
char* astStringExprToString(astStringExpr* s) {
    char* buffer = NULL;
    astAppendToString(&buffer, "\"");
    astAppendToString(&buffer, s->token->literal);
    astAppendToString(&buffer, "\"");
    return buffer;
}

//
// Character literal expression
//

// Create a new character node
astCharExpr* astCharExprNew(Token* token) {
    astCharExpr* c = (astCharExpr*)malloc(sizeof(astCharExpr));
    if (c == NULL) {
        return NULL;
    }

    c->token = token;
    c->value = token->literal[0];

    c->free     = astCharExprFree;
    c->toString = astCharExprToString;

    return c;
}

// Free the character node
void astCharExprFree(astCharExpr* c) {
    if (c) {
        tFreeToken(c->token);  // frees 'token' and 'value'
        free(c);
    }
}

// Convert the character node to a string
char* astCharExprToString(astCharExpr* c) {
    char* buffer = NULL;
    astAppendToString(&buffer, "'");
    astAppendToString(&buffer, c->token->literal);
    astAppendToString(&buffer, "'");
    return buffer;
}

//
// Type expression
//

// Create a new type node
astTypeExpr* astTypeExprNew(Token* token) {
    astTypeExpr* type = (astTypeExpr*)malloc(sizeof(astTypeExpr));
    if (type == NULL) {
        return NULL;
    }

    type->token = token;
    type->value = token->literal;

    type->free     = astTypeExprFree;
    type->toString = astTypeExprToString;

    return type;
}

// Free the type node
void astTypeExprFree(astTypeExpr* type) {
    if (type) {
        tFreeToken(type->token);  // frees 'token' and 'value'
        free(type);
    }
}

// Convert the type node to a string
char* astTypeExprToString(astTypeExpr* type) {
    char* buffer = NULL;
    astAppendToString(&buffer, type->token->literal);
    return buffer;
}

//
// Function call expression
//

// Create a new function call node
astCallExpr* astCallExprNew(Token* token) {
    astCallExpr* c = (astCallExpr*)malloc(sizeof(astCallExpr));
    if (c == NULL) {
        return NULL;
    }

    c->token      = token;
    c->identifier = NULL;
    c->arguments  = NULL;
    c->size       = 0;

    c->free     = astCallExprFree;
    c->toString = astCallExprToString;

    return c;
}

// Free the function call node
void astCallExprFree(astCallExpr* c) {
    if (c) {
        tFreeToken(c->token);

        if (c->identifier) {
            c->identifier->free(c->identifier);
        }

        if (c->arguments) {
            for (uint16_t i = 0; i < c->size; i++) {
                if (c->arguments[i]) {
                    c->arguments[i]->free(c->arguments[i]);
                }
            }

            free(c->arguments);
        }

        free(c);
    }
}

// Convert the function call node to a string
char* astCallExprToString(astCallExpr* c) {
    char* buffer = NULL;

    astAppendToString(&buffer, "Call: {\n");
    indentLevel++;
    char* indent = indentString();

    astAppendToString(&buffer, indent);
    astAppendToString(&buffer, "Identifier: ");
    char* id = c->identifier->toString(c->identifier);
    astAppendToString(&buffer, id);
    free(id);
    astAppendToString(&buffer, "\n");

    astAppendToString(&buffer, indent);
    astAppendToString(&buffer, "Arguments: {\n");

    free(indent);
    indentLevel++;
    indent = indentString();

    for (uint16_t i = 0; i < c->size; i++) {
        char* arg = c->arguments[i]->toString(c->arguments[i]);
        astAppendToString(&buffer, indent);
        astAppendToString(&buffer, arg);
        astAppendToString(&buffer, "\n");
        free(arg);
    }

    indentLevel--;
    indent[indentLevel] = '\0';

    astAppendToString(&buffer, indent);
    astAppendToString(&buffer, "}\n");

    indentLevel--;
    indent[indentLevel] = '\0';

    astAppendToString(&buffer, indent);

    astAppendToString(&buffer, "}");

    free(indent);
    return buffer;
}