#ifndef AST_H
#define AST_H

typedef enum ast_node_e {
    AST_NODE_NUMBER,
    AST_NODE_VARIABLE,
    AST_NODE_RETURN_STMT,
    AST_NODE_CALL,
    AST_NODE_EXPR,
    AST_NODE_FUNCION,
    AST_NODE_PROTOTYPE,
    AST_NODE_PROGRAM
} ast_node_e;

typedef enum ast_node_op_e { PLUS, MINUS, MUL, DIV } ast_node_op_e;

struct ast_node;

typedef struct ast_node_number {
    int value;
} ast_node_number;

typedef struct ast_node_variable {
    char *name, *env;
} ast_node_variable;

typedef struct ast_node_expr {
    char* env;
    ast_node_op_e op;
    struct ast_node* l_oper;
    struct ast_node* r_oper;
} ast_node_expr;

typedef struct ast_node_prototype {
    char *name, *return_type;
    unsigned int args_count;
    char** args;
} ast_node_prototype;

typedef struct ast_node_func {
    struct ast_node* prototype;
    struct ast_node** stmts;
    char** results;
    unsigned int stmt_count;
    struct ast_node* ret;
} ast_node_func;

typedef struct ast_node_call {
    char* name;
    unsigned int args_count;
    struct ast_node** args;
} ast_node_call;

typedef struct ast_node_return {
    struct ast_node* stmt;
} ast_node_return;

typedef struct ast_node_program {
    struct ast_node** func_list;
    int func_count;
} ast_node_program;

typedef struct ast_node {
    ast_node_e type;
    union {
        ast_node_number num;
        ast_node_variable var;
        ast_node_expr expr;
        ast_node_prototype proto;
        ast_node_func func;
        ast_node_call call;
        ast_node_return ret;
        ast_node_program prog;
    };
} ast_node;

ast_node* ast_node_create_number(int);
ast_node* ast_node_create_variable(char*, char*);
ast_node* ast_node_create_expr(ast_node*, ast_node*, ast_node_op_e);
ast_node* ast_node_create_prototype(char*, char*, int, char**);
ast_node* ast_node_create_func(ast_node*, ast_node**, char**, int, ast_node*);
ast_node* ast_node_create_call(char*, int, ast_node**);
ast_node* ast_node_create_return(ast_node*);
ast_node* ast_node_create_program(ast_node**, int);

#endif
