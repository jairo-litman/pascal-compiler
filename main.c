// Jairo Litman - BSI 2024 - UNESP Bauru - Compiladores I
// Desenvolvido com Linux Fedora 39 - Kernel 6.8.10-200.x86_64
// Compilador Clang 17.0.6 x86_64

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ast.h"
#include "lexer.h"
#include "parser.h"
#include "repl.h"

char *stringFromFile(char *filename);

int main(int argc, char *argv[]) {
    if (argc == 1 || (argc != 3 && argc != 2)) {
        printf(
            "Ajuda\n"
            "Uso arquivo: %s <entrada> <saida> <configuração>\n"
            "entrada: arquivo de entrada\n"
            "saida: arquivo de saida\n"
            "\n\nUso REPL: %s repl\n",
            argv[0], argv[0]);
        return 1;
    }

    if (strcmp(argv[1], "repl") == 0) {
        rStartRepl();
        return 0;
    }

    // Would probably have been better to just read the file directly in the lexer
    char       *input   = stringFromFile(argv[1]);
    Lexer      *l       = lNew(input);
    Parser     *p       = pNew(l);
    astProgram *program = pParseProgram(p);

    if (p->errors->size > 0) {
        for (uint32_t i = 0; i < p->errors->size; i++) {
            printf("Erro %04d: %s\n", i + 1, p->errors->data[i]);
        }

        astProgramFree(program);
        lFree(l);
        pFree(p);

        return 1;
    }

    FILE *file = fopen(argv[2], "w");

    fprintf(file, "%s\n", astProgramToString(program));

    fclose(file);

    printf("Programa analisado com sucesso!\n");

    astProgramFree(program);
    lFree(l);
    pFree(p);

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