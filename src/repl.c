#include "repl.h"

#include <stdio.h>
#include <stdlib.h>

#include "ast.h"
#include "error.h"
#include "lexer.h"
#include "parser.h"

#define PROMPT ">> "

// Start the REPL
void rStartRepl() {
    char    line[1024];
    Lexer  *l = lNew("");
    Parser *p;

    while (true) {
        printf(PROMPT);

        if (!fgets(line, 1024, stdin)) {
            printf("\n");
            break;
        }

        rLexerNewInput(l, line);
        p               = pNew(l);
        astProgram *prg = pParseProgram(p);

        if (p->errors->size != 0) {
            for (uint16_t i = 0; i < p->errors->size; i++) {
                printf("\t%s\n", p->errors->data[i]);
            }

            astProgramFree(prg);
            pFree(p);
            continue;
        }

        printf("%s\n", astProgramToString(prg));

        astProgramFree(prg);
        pFree(p);
    }

    rFreeLexerNoInput(l);
}

// Create a new with a given input, no memory allocation
void rLexerNewInput(Lexer *l, char *input) {
    l->input        = input;
    l->position     = 0;
    l->readPosition = 0;
    l->line         = 1;
    eFree(l->errors);
    l->errors = eNew();

    lReadChar(l);
}

// Free the lexer without freeing the input
void rFreeLexerNoInput(Lexer *l) {
    hmFree(l->keywords);
    eFree(l->errors);
    free(l);
}