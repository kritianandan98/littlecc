#include "lexer.h"

int line = 1, col = 1;
const char* tokentype[] = {"IDENTIFIER",      "KEYWORD",       "SPECIAL SYMBOL",
                           "INTEGER LITERAL", "CHARACTER",     "STRING",
                           "ARITHMETIC OP",   "RELATIONAL OP", "LOGICAL OP",
                           "BOOLEAN OP",      "ASSIGNMENT OP", "DATATYPE"};

TOKEN* new_token() {
    TOKEN* token = (TOKEN*)malloc(sizeof(TOKEN));
    token->line = 1;
    token->col = 1;
    return token;
}

char fpeek(FILE* fr) {
    char c = getc(fr);
    ungetc(c, fr);
    return c;
}

int check_keyword(char* word) {
    if ((strcmp(word, "if") == 0) || (strcmp(word, "while") == 0) ||
        (strcmp(word, "void") == 0) || (strcmp(word, "switch") == 0) ||
        (strcmp(word, "case") == 0) || (strcmp(word, "return") == 0) ||
        (strcmp(word, "for") == 0) || (strcmp(word, "break") == 0))
        return 1;
    else
        return 0;
}

int check_datatype(char* word) {
    if ((strcmp(word, "int") == 0)||
        (strcmp(word, "void") == 0))
        return 1;
    else
        return 0;
}

void print_token(TOKEN* t) {
    if (t->lexeme != NULL) {
        printf("%-30s %-25s %-12d %-12d\n", t->lexeme, tokentype[t->type],
               t->line, t->col);
    }
}

TOKEN* id_or_key(TOKEN* t, FILE* fr, char c) {
    int index = 0, len = 1;
    char* word = (char*)malloc(sizeof(char) * len);
    word[index++] = c;
    while (1) {
        char c = getc(fr);
        len++;
        word = realloc(word, sizeof(char) * len);
        if ((c == '_') | (c >= 'a' && c <= 'z') | (c >= 'A' && c <= 'Z') |
            (c >= '0' && c <= '9')) {
            word[index++] = c;
        } else {
            word[index++] = '\0';
            t->lexeme = word;
            t->line = line;
            t->col = col;
            ungetc(c, fr);
            break;
        }
    }
    if (check_keyword(word))
        t->type = KEYWORD;
    else if (check_datatype(word))
        t->type = DATATYPE;
    else
        t->type = ID;
    col += strlen(word);
    return t;
}

TOKEN* integer_literal(TOKEN* t, FILE* fr, char c) {
    char* num = (char*)malloc(sizeof(char) * 2);
    num[0] = c;
    int i = 1;
    while (fpeek(fr) >= '0' && fpeek(fr) <= '9') {
        c = getc(fr);
        num = realloc(num, sizeof(char) * (i + 2));
        num[i++] = c;
    }
    num[i] = '\0';
    t->lexeme = num;
    t->type = INT_LITERAL;
    t->line = line;
    t->col = col;
    col += strlen(num);
    return t;
}

TOKEN* next_token(FILE* fr) {
    TOKEN* t = new_token();
    char c = getc(fr);
    char cnext;
    while (c != EOF) {
        switch (c) {
            case '_':
            case 'a' ... 'z':
            case 'A' ... 'Z':
                return id_or_key(t, fr, c);
                break;
            case '0' ... '9':
                return integer_literal(t, fr, c);
                break;
            case '\n':
                line++;
                col = 1;
                break;
            case '#':
                while (c != '\n' && c != EOF) c = getc(fr);
                col = 1;
                line++;  // ignore preprocessor directives
                break;
            case '/':
                c = getc(fr);
                if (c == '/') {  // single line comment
                    while (c != '\n' && c != EOF) {
                        c = getc(fr);
                        printf("%c", c);
                    }
                    col = 1;
                    line++;
                } else if (c == '*') {  // multi-line comment
                    while (c != EOF) {
                        c = getc(fr);
                        col++;
                        if (c == '\n') {
                            line++;
                            col = 1;
                        } else if (c == '*') {  // check if multi-line comment
                                                // has ended
                            while (c == '*') {
                                c = getc(fr);
                                col++;
                            }
                            if (c == '/') {  // multi-line comment has ended
                                while (c == '/') {
                                    c = getc(fr);
                                    col++;
                                }
                                ungetc(c, fr);
                                break;
                            } else
                                ungetc(c, fr);
                        }
                    }
                } else {
                    ungetc(c, fr);
                    t->lexeme = (char*)malloc(sizeof(char) * 2);
                    t->lexeme[0] = '/';
                    t->lexeme[1] = '\0';
                    t->type = ARITH_OP;
                    t->line = line;
                    t->col = col;
                    col++;
                    return t;
                    break;
                }
                break;
            case '\'':
                c = getc(fr);
                col++;
                t->lexeme = (char*)malloc(sizeof(char) * 3);
                t->lexeme[0] = c;
                t->type = CHAR_LITERAL;
                t->line = line;
                if (c == '\\') {  // '\t' or '\n'
                    c = getc(fr);
                    t->lexeme[1] = c;
                    col++;
                }
                t->col = col;
                t->lexeme[2] = '\0';
                col += 2;
                c = getc(fr);
                return t;
                break;
            case '"':  // string
                c = getc(fr);
                col++;
                int index = 0;
                t->lexeme = (char*)malloc(sizeof(char) * 2);
                t->type = STRING;
                t->col = col;
                t->line = line;
                while (c != '"') {
                    t->lexeme[index++] = c;
                    if (c == '\\') {
                        c = getc(fr);
                        t->lexeme[index++] = c;
                    }  // if '"' present in string ignore
                    c = getc(fr);
                    t->lexeme = realloc(t->lexeme, sizeof(char) * index + 1);
                }
                t->lexeme[index] = '\0';
                col += strlen(t->lexeme) + 1;
                return t;
                break;
            case '&':
            case '|':
                t->lexeme = (char*)malloc(sizeof(char) * 3);
                t->lexeme[0] = c;
                t->lexeme[1] = '\0';
                t->line = line;
                t->col = col;
                cnext = getc(fr);
                if (c == cnext) {
                    t->lexeme[1] = cnext;
                    t->lexeme[2] = '\0';
                    t->type = LOGICAL_OP;
                    col += 2;
                } else {
                    ungetc(cnext, fr);
                    t->type = BOOL_OP;
                    col++;
                }
                return t;
                break;
            case '=':
                t->lexeme = (char*)malloc(sizeof(char) * 2);
                t->lexeme[0] = c;
                t->lexeme[1] = '\0';
                t->line = line;
                t->col = col;
                c = getc(fr);
                if (c == '=') {
                    t->lexeme = (char*)malloc(sizeof(char) * 3);
                    t->lexeme[0] = '=';
                    t->lexeme[1] = '=';
                    t->lexeme[2] = '\0';
                    t->type = RELOP;
                    col += 2;
                } else {
                    ungetc(c, fr);
                    t->type = ASSIGN_OP;
                    col++;
                }
                return t;
            case '!':
            case '<':
            case '>':
                t->lexeme = (char*)malloc(sizeof(char) * 3);
                t->lexeme[0] = c;
                t->lexeme[1] = '\0';
                t->line = line;
                t->col = col;
                cnext = getc(fr);
                if (cnext == '=') {
                    t->lexeme[1] = cnext;
                    t->lexeme[2] = '\0';
                    t->type = RELOP;
                    col += 2;
                } else {
                    ungetc(cnext, fr);
                    if (c == '!')
                        t->type = BOOL_OP;
                    else
                        t->type = RELOP;
                    col++;
                }
                return t;
                break;
            case '+':
            case '-':
            case '*':
                t->lexeme = (char*)malloc(sizeof(char) * 2);
                t->lexeme[0] = c;
                t->lexeme[1] = '\0';
                t->type = ARITH_OP;
                t->line = line;
                t->col = col;
                col++;
                return t;
                break;
            case ',':
            case '(':
            case ')':
            case '}':
            case '{':
            case ';':
            case ':':
                t->lexeme = (char*)malloc(sizeof(char) * 2);
                t->lexeme[0] = c;
                t->type = SYMBOL;
                t->lexeme[1] = '\0';
                t->col = col;
                t->line = line;
                col++;
                return t;
            case '\t':
                col += 4;
                break;
            default:
                col += 1;
                break;
        }
        c = getc(fr);
    }
    if (t->lexeme == NULL)
        return NULL;
    else
        return t;
}
