#ifndef REPL_H
#define REPL_H

#include "lexer.h"

void startRepl();

void lexerNewInput(Lexer *l, char *input);
void freeLexerNoInput(Lexer *l);

#endif  // REPL_H