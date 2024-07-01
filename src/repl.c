#include "repl.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "lexer.h"
#include "token.h"

#define PROMPT ">> "

// Start the REPL
void startRepl() {
    char   line[1024];
    Lexer *l = newLexer("");

    while (true) {
        printf(PROMPT);

        if (!fgets(line, 1024, stdin)) {
            printf("\n");
            break;
        }

        lexerNewInput(l, line);
        uint8_t counter = 0;

        while (true) {
            Token *t = nextToken(l);

            if (t->type == _EOF) {
                break;
            }

            if (t->type == ILLEGAL) {
                printf("INVÁLIDO [%s]\n", t->literal);
                freeToken(t);
                continue;
            }

            printf("%#04x", t->type);

            if (t->type == IDENT) {
                printf("%#04x ", l->varCounter);
                counter = l->varCounter;
            } else if (t->type == INT || t->type == REAL || t->type == CHAR || t->type == STRING) {
                printf("%#04x ", l->litCounter);
                counter = l->litCounter;
            } else {
                printf("0x00 ");
                counter = 0;
            }

            printf(" (%d %d)[%s]\n", t->type, counter, t->literal);

            freeToken(t);
        }

        if (l->errors->size > 0) {
            for (uint32_t i = 0; i < l->errors->size; i++) {
                printf("%s\n", l->errors->data[i]);
            }
        }
    }

    freeLexerNoInput(l);
}

// Create a new with a given input, no memory allocation
void lexerNewInput(Lexer *l, char *input) {
    l->input        = input;
    l->position     = 0;
    l->readPosition = 0;
    l->line         = 1;
    freeErrorList(l->errors);
    l->errors = newErrorList();

    readChar(l);
}

// Free the lexer without freeing the input
void freeLexerNoInput(Lexer *l) {
    freeHashMap(l->keywords);
    freeErrorList(l->errors);
    free(l);
}