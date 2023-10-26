#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <ctype.h>

char look;

/* Protótipos das funções */
void init();
void nextChar();
void error(char *fmt, ...);
void fatal(char *fmt, ...);
void expected(char *fmt, ...);
void match(char c);
char getName();
int getNum();
void emit(char *fmt, ...);
void expression();
void term();
void add();
void subtract();
void factor();
void multiply();
void divide();
int isAddOp(char c);

/* Função principal */
int main()
{
    init();

    // Chame a função expression para analisar e traduzir uma expressão
    expression();

    return 0;
}

void init()
{
    nextChar();
}

void nextChar()
{
    look = getchar();
}

void error(char *fmt, ...)
{
    va_list args;
    fputs("Error: ", stderr);
    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);
    fputc('\n', stderr);
}

void fatal(char *fmt, ...)
{
    va_list args;
    fputs("Error: ", stderr);
    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);
    fputc('\n', stderr);
    exit(1);
}

void expected(char *fmt, ...)
{
    va_list args;
    fputs("Error: ", stderr);
    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);
    fputs(" expected!\n", stderr);
    exit(1);
}

void match(char c)
{
    if (look != c)
        expected("'%c'", c);
    nextChar();
}

char getName()
{
    char name;
    expected("Name");
    name = toupper(look);
    nextChar();
    return name;
}

int getNum()
{
    int num = 0;
    while (isdigit(look))
    {
        num = num * 10 + (look - '0');
        nextChar();
    }
    return num;
}

void emit(char *fmt, ...)
{
    va_list args;
    putchar('\t');
    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);
    putchar('\n');
}

/* Função para analisar e traduzir uma expressão */
void expression()
{
    if (isAddOp(look))
        emit("XOR AX, AX");  // Adiciona suporte a operadores unários
    else
        term();
    while (isAddOp(look))
    {
        emit("PUSH AX");
        switch (look)
        {
            case '+':
                add();
                break;
            case '-':
                subtract();
                break;
            default:
                expected("AddOp");
                break;
        }
    }
}

/* Função para analisar e traduzir um termo */
void term()
{
    factor();
    while (look == '*' || look == '/')
    {
        emit("PUSH AX");
        switch (look)
        {
            case '*':
                multiply();
                break;
            case '/':
                divide();
                break;
            default:
                expected("MulOp");
                break;
        }
    }
}

/* Função para analisar e traduzir uma adição */
void add()
{
    match('+');
    term();
    emit("POP BX");
    emit("ADD AX, BX");
}

/* Função para analisar e traduzir uma subtração */
void subtract()
{
    match('-');
    term();
    emit("POP BX");
    emit("SUB AX, BX");
    emit("NEG AX");
}

/* Função para analisar e traduzir um fator */
void factor()
{
    if (look == '(')
    {
        match('(');
        expression();
        match(')');
    }
    else
    {
        emit("MOV AX, %d", getNum());
    }
}

/* Função para analisar e traduzir uma multiplicação */
void multiply()
{
    match('*');
    factor();
    emit("POP BX");
    emit("IMUL BX");
}

/* Função para analisar e traduzir uma divisão */
void divide()
{
    match('/');
    factor();
    emit("POP BX");
    emit("XCHG AX, BX");
    emit("CWD");
    emit("IDIV BX");
}

/* Função para reconhecer operador aditivo */
int isAddOp(char c)
{
    return (c == '+' || c == '-');
}