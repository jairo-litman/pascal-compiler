#ifndef REPL_H
#define REPL_H

#include "lexer.h"

void rStartRepl();

void rLexerNewInput(Lexer *l, char *input);
void rFreeLexerNoInput(Lexer *l);

#endif  // REPL_H