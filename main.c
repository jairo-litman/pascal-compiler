// Jairo Litman - BSI 2024 - UNESP Bauru - Compiladores I
// Desenvolvido com Linux Fedora 39 - Kernel 6.7.10 x86_64
// Compilador Clang 17.0.6 x86_64

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lexer.h"
#include "repl.h"
#include "token.h"

typedef enum {
    DEBUG,
    NORMAL,
    SIMPLE,
} Mode;

char *stringFromFile(char *filename);
void  tokensToFile(Lexer *l, Mode mode, char *filename);

int main(int argc, char *argv[]) {
    if (argc == 1 || (argc != 4 && argc != 2)) {
        printf(
            "Ajuda\n"
            "Uso arquivo: %s <entrada> <saida> <configuração>\n"
            "entrada: arquivo de entrada\n"
            "saida: arquivo de saida\n"
            "configuração: debug | normal | simple\n"
            "\n\nUso REPL: %s repl\n",
            argv[0], argv[0]);
        return 1;
    }

    if (strcmp(argv[1], "repl") == 0) {
        startRepl();
        return 0;
    }

    Mode mode;

    if (strcmp(argv[3], "debug") == 0) {
        mode = DEBUG;
    } else if (strcmp(argv[3], "normal") == 0) {
        mode = NORMAL;
    } else if (strcmp(argv[3], "simple") == 0) {
        mode = SIMPLE;
    } else {
        printf("Modo desconhecido\n");
        return 1;
    }

    // Would probably have been better to just read the file directly in the lexer
    char  *input = stringFromFile(argv[1]);
    Lexer *l     = newLexer(input);

    tokensToFile(l, mode, argv[2]);

    freeLexer(l);

    return 0;
}

// Read a file and return its content as a string
char *stringFromFile(char *filename) {
    FILE *file = fopen(filename, "r");

    if (!file) {
        printf("Nao foi possivel abrir o arquivo %s\n", filename);
        exit(1);
    }

    fseek(file, 0, SEEK_END);
    int64_t length = ftell(file);
    fseek(file, 0, SEEK_SET);

    char *buffer = malloc(length + 1);
    fread(buffer, 1, length, file);
    buffer[length] = '\0';

    fclose(file);

    return buffer;
}

// Write tokens to a file
void tokensToFile(Lexer *l, Mode mode, char *filename) {
    FILE *file = fopen(filename, "w");

    if (!file) {
        printf("Nao foi possivel abrir o arquivo %s\n", filename);
        exit(1);
    }

    uint32_t line = -1;
    switch (mode) {
        case DEBUG:
            fprintf(file, "MODO DEBUG\n");
            break;
        case NORMAL:
            fprintf(file, "MODO NORMAL\n");
            break;
        case SIMPLE:
            fprintf(file, "MODO SIMPLES\n");
            break;
    }

    Token  *tok;
    uint8_t counter;

    while (tok = nextToken(l), tok->type != _EOF) {
        if (mode == DEBUG || mode == NORMAL) {
            if (l->line != line) {
                fprintf(file, "\n");
                line = l->line;
                fprintf(file, "Linha %04d: ", l->line);
            }

            if (tok->type == ILLEGAL) {
                fprintf(file, "INVÁLIDO [%s] ", tok->literal);
                freeToken(tok);
                continue;
            }
        }

        fprintf(file, "%#04x", tok->type);

        if (tok->type == IDENT) {
            fprintf(file, "%#04x ", l->varCounter);
            counter = l->varCounter;
        } else if (tok->type == INT || tok->type == REAL || tok->type == CHAR || tok->type == STRING) {
            fprintf(file, "%#04x ", l->litCounter);
            counter = l->litCounter;
        } else {
            fprintf(file, "0x00 ");
            counter = 0;
        }

        if (mode == DEBUG) {
            fprintf(file, "(%d %d)[%s] ", tok->type, counter, tok->literal);
        }

        freeToken(tok);
    }

    if (mode == DEBUG || mode == NORMAL) {
        if (l->errors->size > 0) {
            fprintf(file, "\n\nErros:\n");

            for (uint32_t i = 0; i < l->errors->size; i++) {
                fprintf(file, "Erro %04d: %s\n", i + 1, l->errors->data[i]);
            }

            fprintf(file, "\n");
        }
    }

    fclose(file);
}