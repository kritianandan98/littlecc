#ifndef LEXER_H
#define LEXER_H

#include "c-lib.h"

typedef enum {
    ID,
    KEYWORD,
    SYMBOL,
    INT_LITERAL,
    CHAR_LITERAL,
    STRING,
    ARITH_OP,
    RELOP,
    LOGICAL_OP,
    BOOL_OP,
    ASSIGN_OP,
    DATATYPE
} Tokentype;

typedef struct {
    char* lexeme;
    Tokentype type;
    int line, col;
} TOKEN;

extern int line, col;

char fpeek(FILE*);
int check_keyword(char*);
void print_token(TOKEN*);
TOKEN* id_or_key(TOKEN*, FILE*, char);
TOKEN* integer_literal(TOKEN*, FILE*, char);
TOKEN* next_token(FILE*);

#endif
