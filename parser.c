#include "parser.h"

char* env = "global";

void error() {
    printf("Parse Error\n");
    exit(1);
}
ast_node* parse_program(FILE* fr, char* env) {
    printf("Parsing Program...\n");
    ast_node* prog = NULL;
    int func_count = 0;
    ast_node** func_list = malloc(sizeof(ast_node*));
    ast_node* func = parse_function(fr, env);
    while (func != NULL) {
        func_count++;
        func_list = realloc(func_list, func_count * sizeof(ast_node*));
        func_list[func_count - 1] = func;
        func = parse_function(fr, env);
    }
    prog = ast_node_create_program(func_list, func_count);
    return prog;
}

ast_node* parse_function(FILE* fr, char* env) {
    printf("Parsing function..\n");
    TOKEN* token = next_token(fr);
    char** results = (char**)malloc(sizeof(char*));
    ast_node *func = NULL, *proto = NULL;
    ast_node** stmts = malloc(sizeof(ast_node*));
    ast_node* stmt = NULL;
    ast_node* return_stmt = NULL;
    int stmt_count = 0;
    char *returntype, *name;
    char** args = malloc(sizeof(char*));
    int args_count = 0;
    if (token == NULL) return NULL;
    if (!strcmp(token->lexeme, "int")) {
        print_token(token);
        returntype = strdup(token->lexeme);
        token = next_token(fr);
    } else
        error();
    if (token != NULL && token->type == ID) {
        print_token(token);
        name = strdup(token->lexeme);
        token = next_token(fr);
    } else
        error();
    if (token != NULL && !strcmp(token->lexeme, "(")) {
        print_token(token);
        token = next_token(fr);
    } else {
        error();
    }
    if (token != NULL && !strcmp(token->lexeme, ")")) {
        print_token(token);
        token = next_token(fr);
    } else {
        while (1) {
            if (token != NULL && !strcmp(token->lexeme, "int")) {
                print_token(token);
                token = next_token(fr);
            } else {
                print_token(token);
                error();
            }
            if (token != NULL && token->type == ID) {
                print_token(token);
                args_count++;
                args = realloc(args, args_count * sizeof(char*));
                args[args_count - 1] = strdup(token->lexeme);
                token = next_token(fr);
            } else {
                error();
            }
            if (token != NULL && !strcmp(token->lexeme, ",")) {
                print_token(token);
                token = next_token(fr);
            } else if (token != NULL && !strcmp(token->lexeme, ")")) {
                print_token(token);
                break;
            } else {
                error();
            }
        }
        if (token != NULL && !strcmp(token->lexeme, ")")) {
            print_token(token);
            token = next_token(fr);
        } else {
            error();
        }
    }
    if (token != NULL && !strcmp(token->lexeme, "{")) {
        print_token(token);
        do {
            token = next_token(fr);
            ast_node* node = NULL;
            if (token != NULL && !strcmp(token->lexeme, "int")) {
                print_token(token);
                token = next_token(fr);
                if (token != NULL && token->type == ID) {
                    print_token(token);
                    results =
                        realloc(results, (stmt_count + 1) * sizeof(char*));
                    results[stmt_count] = strdup(token->lexeme);
                    token = next_token(fr);
                    if (token != NULL && !strcmp(token->lexeme, "=")) {
                        print_token(token);
                        ast_node* stmt;
                        stmt = parse_expr(fr, env);
                        stmt_count++;
                        if (stmt != NULL) {
                            stmts = realloc(
                                stmts, (stmt_count + 1) * sizeof(ast_node*));
                            stmts[stmt_count - 1] = stmt;
                        }
                    }
                } else {
                    error();
                }
            } else if (token != NULL && !strcmp(token->lexeme, "return")) {
                print_token(token);
                return_stmt = parse_expr(fr, env);
                token = next_token(fr);
                break;
            } else {
                print_token(token);
                error();
            }
        } while (1);
    } else {
        error();
    }
    if (token != NULL && !strcmp(token->lexeme, "}")) {
        print_token(token);
        proto = ast_node_create_prototype(name, returntype, args_count, args);
        func = ast_node_create_func(proto, stmts, results, stmt_count,
                                    return_stmt);
    } else {
        print_token(token);
        error();
    }
    return func;
}

ast_node* parse_expr(FILE* fr, char* env) {
    printf("Parsing expr...\n");
    TOKEN* token = next_token(fr);
    ast_node* node = NULL;
    if (token != NULL && (token->type == INT_LITERAL || token->type == ID)) {
        print_token(token);
        if (token->type == INT_LITERAL)
            node = ast_node_create_number(strtod(token->lexeme, NULL));
        else if (token->type == ID) {
            TOKEN* lookahead = next_token(fr);
            if (lookahead != NULL && !strcmp(lookahead->lexeme, "(")) {
                print_token(lookahead);
                char* name = strdup(token->lexeme);
                ast_node** args = malloc(sizeof(char*));
                int args_count = 0;
                token = next_token(fr);
                while (1) {
                    if (token != NULL &&
                        (token->type == ID || token->type == INT_LITERAL)) {
                        print_token(token);
                        args_count++;
                        args = realloc(args, args_count * sizeof(char*));
                        if (token->type == ID) {
                            args[args_count - 1] =
                                ast_node_create_variable(token->lexeme, env);
                        } else {
                            args[args_count - 1] = ast_node_create_number(
                                strtol(token->lexeme, NULL, 10));
                        }
                    } else if (token != NULL && !strcmp(token->lexeme, ")")) {
                        print_token(token);
                        node = ast_node_create_call(name, args_count, args);
                        break;
                    } else {
                        error();
                    }
                    token = next_token(fr);
                    if (token != NULL && !strcmp(token->lexeme, ",")) {
                        print_token(token);
                        token = next_token(fr);
                    } else if (token != NULL && !strcmp(token->lexeme, ")")) {
                        print_token(token);
                        node = ast_node_create_call(name, args_count, args);
                        break;
                    } else {
                        error();
                    }
                }
            } else {
                fseek(fr, -strlen(token->lexeme), SEEK_CUR);
                node = ast_node_create_variable(token->lexeme, env);
            }
        }
        token = next_token(fr);
    } else {
        error();
    }
    if (token != NULL && token->type == ARITH_OP) {
        print_token(token);
        ast_node* l_oper = node;
        ast_node* r_oper = parse_expr(fr, env);
        ast_node_op_e op = -1;
        switch (token->lexeme[0]) {
            case '+':
                op = PLUS;
                break;
            case '-':
                op = MINUS;
                break;
            case '/':
                op = DIV;
                break;
            case '*':
                op = MUL;
                break;
        }
        node = ast_node_create_expr(l_oper, r_oper, op);
        return node;
    } else if (token != NULL && !strcmp(token->lexeme, ";")) {
        print_token(token);
        return node;
    } else {
        error();
    }
    return NULL;
}
/*
int main() {
   FILE* fin = fopen("test.c", "r");
   parse_program(fin, "global");
}*/
