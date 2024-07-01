# Analisador sintático de Pascal reduzido

## Descrição

Este analisador sintático foi desenvolvido para a disciplina de Compiladores I, ministrada pela professora Juliana da Costa Feitosa.

O analisador sintático foi desenvolvido em C e identifica os tokens presentes no código fonte em Pascal e constrói uma arvore sintática abstrata. O analisador léxico foi desenvolvido para uma gramática reduzida de Pascal e não contempla todas as regras da linguagem.

## Funcionamento

O analisador léxico lê um arquivo de entrada, que contém o código fonte em Pascal, e identifica os tokens presentes no código. Logo, os tokens são analisados pelo analisador sintático, que constrói uma árvore sintática abstrata. A árvore sintática abstrata é escrita em um arquivo de saída, ou no terminal, dependendo do modo de execução do programa.

Também é possível utilizar um REPL para digitar o código fonte diretamente no terminal.

A arvore sintática abstrata é escrita no arquivo de saída no seguinte formato:

```
Program: {
    Identifier: <identificador>
    Block: {
        <Var>: {...}
        <Procedure>: {...}
        <Function>: {...}
        Begin: {
            <Statement>: {...}
            <Statement>: {...}
            ...
        }
    }
}
```

## Execução

Para executar o analisador sintático, basta compilar o código fonte e executar o programa passando o arquivo de entrada como argumento. O arquivo de entrada deve conter o código fonte em Pascal.

Para compilar o código fonte, recomenda-se utilizar cmake com o seguinte comando:

```
cmake -B build -S .
cmake --build build --config Release --target all
```

O executável será gerado na pasta `bin` e pode ser executado da seguinte forma:

```
./PascalSyntaxAnalyzer <arquivo de entrada> <arquivo de saída>
```

Onde `<arquivo de entrada>` é o arquivo contendo o código fonte em Pascal, `<arquivo de saída>` é o arquivo onde a árvore sintática abstrata será escrita.

Alternativamente, pode-se iniciar o REPL passando o argumento `repl`:

```
./PascalSyntaxAnalyzer repl
```

O REPL permite que o usuário digite o código fonte diretamente no terminal e exibe a árvore sintática abstrata no terminal.

## Exemplo

Para exemplificar o funcionamento do analisador sintático, considere o seguinte código fonte em Pascal:

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

O analisador sintático criará a seguinte árvore sintática abstrata:

```
Program: {
	Identifier: sumtwonumbers
	Block: {
		Var: {
			Declaration: {
				Identifiers: {a, b}
				Type: integer
			}
			Declaration: {
				Identifiers: {sum}
				Type: integer
			}
		}
		Begin: {
			Expression: {
				Assignment: {
					Identifier: a
					Value: 10
				}
			}
			Expression: {
				Assignment: {
					Identifier: b
					Value: 20
				}
			}
			Expression: {
				Assignment: {
					Identifier: sum
					Value: (a + b)
				}
			}
		}
	}
}
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

## Gramática

Ném todos os tokens são reconhecidos pela gramática. A gramática é a seguinte:

```
<program> ::= program <identifier>; <block>.
<block> ::= [<variable declarations>] [<subroutines>] <composite command>
<variable declarations> ::= var <declaration> {;<declaration>};
<declaration> ::= <identifiers> : <type>
<identifiers> ::= <identifier> {, <identifier>}
<subroutines> ::= {<function declaration> | <procedure declaration>}
<function declaration> ::= function <identifier> [<formal parameters>] : <type> ; <block>
<procedure declaration> ::= procedure <identifier> [<formal parameters>] ; <block>
<formal parameters> ::= (<parameter> {; <parameter>})
<parameter> ::= [var] <identifiers> : <type>
<composite command> ::= begin <command> {; <command>}; end
<command> ::= <assignment> | <call> | <composite command> | <conditional> | <while>
<assignment> ::= <variable> := <expression>
<call> ::= <identifier>(<expressions>)
<conditional> ::= if <expression> then <command> [else <command>]
<while> ::= while <expression> do <command>
<expressions> ::= <expression> {, <expression>}
<expression> ::= <simple expression> [ <relational operator> <simple expression>]
<simple expression> ::= [+|-] <term> {(+|-|or) <term>}
<term> ::= <factor> {(*|/|div|mod|and) <factor>}
<factor> ::= <variable> | <number> | <string> | <char> | <boolean> | (<expression>) | not <factor>
<variable> ::= <identifier>
<number> ::= <digit> {<digit>}
<string> ::= "<character> {<character>}"
<char> ::= '<character>'
<boolean> ::= true | false
<relational operator> ::= = | <> | < | > | <= | >=
<type> ::= integer | real | boolean | char | string
<digit> ::= 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9
<character> ::= a | b | c | ... | z | A | B | C | ... | Z
<identifier> ::= <character> {<character> | <digit>}
```

## Considerações

O analisador sintático foi desenvolvido para uma gramática reduzida de Pascal e não contempla todas as regras da linguagem. O analisador sintático foi desenvolvido com o intuito de criar uma árvore sintática abstrata simples e não realiza a analise semântica do código fonte.

O código foi adaptado do analisador sintático que desenvolvi para minha propia linguagem de programação, escrita em Go, disponível em [Github](github.com/jairo-litman/cidoka-lang).

## Licença

Este projeto é licenciado pela licença GNU AFFERO GENERAL PUBLIC LICENSE Version 3. Para mais informações, consulte o arquivo LICENSE.