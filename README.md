# Analisador léxico de Pascal reduzido

## Descrição

Este analisador léxico foi desenvolvido para a disciplina de Compiladores I, ministrada pela professora Juliana da Costa Feitosa.

O analisador léxico foi desenvolvido em C e tem como objetivo analisar um código fonte escrito em Pascal, com uma gramática reduzida, e identificar os tokens presentes no código.

## Funcionamento

O analisador léxico lê um arquivo de entrada, que contém o código fonte em Pascal, e identifica os tokens presentes no código. Também é possível utilizar um REPL para digitar o código fonte diretamente no terminal.

Os tokens são representados da seguinte forma:

```
0xnn0xmm
```

Onde `nn` é o número identificador do tipo do token e `mm` é o número identificador para identificadores, ou variáveis, e literais. O valor de `mm` é um contador que é incrementado a cada novo identificador ou literal encontrado, identificadores e literais possuem contadores separados.

Os tokens são escritos em um arquivo de saída, ou no terminal, dependendo do modo de execução do programa. O arquivo, dependendo do argumento passado na execução do programa, terá uma das seguintes estruturas:

```
MODO DEBUG

Linha 0001: 0x010x00 (1 0)[literal1] 0x020x00 (2 0)[literal2]
Linha 0002: 0x030x00 (3 0)[literal3] 0x040x00 (4 0)[literal4]
```

```
MODO NORMAL

Linha 0001: 0x010x00 0x020x00
Linha 0002: 0x030x00 0x040x00
```

```
MODO SIMPLES

0x010x000x020x000x030x000x040x00
```

## Execução

Para executar o analisador léxico, basta compilar o código fonte e executar o programa passando o arquivo de entrada como argumento. O arquivo de entrada deve conter o código fonte em Pascal.

Para compilar o código fonte, recomenda-se utilizar cmake com o seguinte comando:

```
cmake -B build -S .
cmake --build build --config Release --target all
```

O executável será gerado na pasta `bin` e pode ser executado da seguinte forma:

```
./PascalLexicalAnalyzer <arquivo de entrada> <arquivo de saída> <modo>
```

Onde `<arquivo de entrada>` é o arquivo contendo o código fonte em Pascal, `<arquivo de saída>` é o arquivo onde os tokens serão escritos e `<modo>` é o modo de escrita dos tokens, podendo ser `debug`, `normal` ou `simple`.

Alternativamente, pode-se iniciar o REPL passando o argumento `repl`:

```
./PascalLexicalAnalyzer repl
```

O REPL permite que o usuário digite o código fonte diretamente no terminal e exibe os tokens identificados.

## Exemplo

Para exemplificar o funcionamento do analisador léxico, considere o seguinte código fonte em Pascal:

```
program SumTwoNumbers;
var
   a, b : integer;
   sum : integer;
begin
    a := 10;
    b := 20;
    sum := a + b;
end.
```

O analisador léxico identificará os seguintes tokens:

```
MODO DEBUG

Linha 0001: 0x280x00 (40 0)[program] 0x020x01 (2 1)[sumtwonumbers] 0x200x00 (32 0)[;] 
Linha 0002: 0x3a0x00 (58 0)[var] 
Linha 0003: 0x020x02 (2 2)[a] 0x1e0x00 (30 0)[,] 0x020x03 (2 3)[b] 0x210x00 (33 0)[:] 0x0b0x00 (11 0)[integer] 0x200x00 (32 0)[;] 
Linha 0004: 0x020x04 (2 4)[sum] 0x210x00 (33 0)[:] 0x0b0x00 (11 0)[integer] 0x200x00 (32 0)[;] 
Linha 0005: 0x290x00 (41 0)[begin] 
Linha 0006: 0x020x05 (2 5)[a] 0x0e0x00 (14 0)[:=] 0x060x01 (6 1)[10] 0x200x00 (32 0)[;] 
Linha 0007: 0x020x06 (2 6)[b] 0x0e0x00 (14 0)[:=] 0x060x02 (6 2)[20] 0x200x00 (32 0)[;] 
Linha 0008: 0x020x07 (2 7)[sum] 0x0e0x00 (14 0)[:=] 0x020x08 (2 8)[a] 0x0f0x00 (15 0)[+] 0x020x09 (2 9)[b] 0x200x00 (32 0)[;] 
Linha 0009: 0x2a0x00 (42 0)[end] 0x1f0x00 (31 0)[.] 
```

## Tokens

Os tokens identificados pelo analisador léxico são:

```
EOF       = 0x00    Final do arquivo
ILLEGAL   = 0x01    Token inválido
IDENT     = 0x02    add, read, x, y, ...
CHAR      = 0x03    'a'
TRUE      = 0x04    true
FALSE     = 0x05    false
INT       = 0x06    123456
FLOAT     = 0x07    123.456
STR       = 0x08    "foobar"
CHARACTER = 0x09    Tipo de variável character
BOOLEAN   = 0x0a    Tipo de variável boolean
INTEGER   = 0x0b    Tipo de variável integer
REAL      = 0x0c    Tipo de variável real
STRING    = 0x0d    Tipo de variável string
ASSIGN    = 0x0e    :=
PLUS      = 0x0f    +
MINUS     = 0x10    -
ASTERISK  = 0x11    *
SLASH     = 0x12    /
MOD       = 0x13    mod
DIV       = 0x14    div
EQ        = 0x15    =
NOT_EQ    = 0x16    <>
LT        = 0x17    <
GT        = 0x18    >
LTE       = 0x19    <=
GTE       = 0x1a    >=
AND       = 0x1b    and
OR        = 0x1c    or
NOT       = 0x1d    not
COMMA     = 0x1e    ,
DOT       = 0x1f    .
SEMICOLON = 0x20    ;
COLON     = 0x21    :
LPAREN    = 0x22    (
RPAREN    = 0x23    )
LBRACE    = 0x24    {
RBRACE    = 0x25    }
LBRACKET  = 0x26    [
RBRACKET  = 0x27    ]
PROGRAM   = 0x28    program
BEGIN     = 0x29    begin
END       = 0x2a    end
IF        = 0x2b    if
THEN      = 0x2c    then
ELSE      = 0x2d    else
CONTINUE  = 0x2e    continue
BREAK     = 0x2f    break
RETURN    = 0x30    return
WHILE     = 0x31    while
DO        = 0x32    do
REPEAT    = 0x33    repeat
UNTIL     = 0x34    until
FOR       = 0x35    for
TO        = 0x36    to
DOWNTO    = 0x37    downto
CASE      = 0x38    case
OF        = 0x39    of
VAR       = 0x3a    var
CONST     = 0x3b    const
TYPE      = 0x3c    type
FUNCTION  = 0x3d    function
PROCEDURE = 0x3e    procedure
GOTO      = 0x3f    goto
LABEL     = 0x40    label
NIL       = 0x41    nil
```

## Considerações

O analisador léxico foi desenvolvido para uma gramática reduzida de Pascal e não contempla todas as regras da linguagem. O analisador léxico foi desenvolvido com o intuito de identificar os tokens presentes no código fonte e não realiza a análise sintática ou semântica do código.

O código foi adaptado do analisador léxico que desenvolvi para minha propia linguagem de programação, escrita em Go, disponível em [Github](github.com/jairo-litman/cidoka-lang).

## Licença

Este projeto é licenciado pela licença GNU AFFERO GENERAL PUBLIC LICENSE Version 3. Para mais informações, consulte o arquivo LICENSE.