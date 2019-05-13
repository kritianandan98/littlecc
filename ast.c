#include "ast.h"
#include "c-lib.h"

ast_node* ast_node_create_number(int value) {
    ast_node* node = malloc(sizeof(ast_node));
    node->type = AST_NODE_NUMBER;
    node->num.value = value;
    return node;
}

ast_node* ast_node_create_variable(char* name, char* env){
    ast_node* node = malloc(sizeof(ast_node));
    node->type = AST_NODE_VARIABLE;
    node->var.name = strdup(name);
    node->var.env = strdup(env);
    return node;
}

ast_node* ast_node_create_expr(ast_node* l_oper, ast_node* r_oper,
                                   ast_node_op_e op) {
    ast_node* node = malloc(sizeof(ast_node));
    node->type = AST_NODE_EXPR;
    node->expr.op = op;
    node->expr.l_oper = l_oper;
    node->expr.r_oper = r_oper;
    return node;
}

ast_node* ast_node_create_prototype(char* name, char* return_type, int args_count, char** args){
    ast_node* node = malloc(sizeof(ast_node));
    node->type = AST_NODE_PROTOTYPE;
    node->proto.name = strdup(name);
    node->proto.return_type = strdup(return_type);
    node->proto.args = args;
    node->proto.args_count = args_count;
    return node;
}

ast_node* ast_node_create_func(ast_node* prototype, ast_node** stmts, char** results, int stmt_count, ast_node* ret){
    ast_node* node = malloc(sizeof(ast_node));
    node->type = AST_NODE_FUNCION;
    node->func.prototype = prototype;
    node->func.stmts = stmts;
    node->func.stmt_count = stmt_count;
    node->func.ret = ret;
    node->func.results = results;
    printf("Statement count: %d\n",stmt_count);
    for(int j=0;j<stmt_count;j++){
        if(node->func.stmts[j] == NULL){
            printf("NULL ---\n");
        }
    }
    return node;
}

ast_node* ast_node_create_call(char* name, int args_count, ast_node** args){
    ast_node* node = malloc(sizeof(ast_node));
    node->type = AST_NODE_CALL;
    node->call.name = strdup(name);
    node->call.args_count = args_count;
    node->call.args = args;
    return node;
}

ast_node* ast_node_create_return(ast_node* ret_stmt){
    ast_node* node = malloc(sizeof(ast_node));
    node->type = AST_NODE_RETURN_STMT;
    node->ret.stmt = ret_stmt;
    return node;
}

ast_node* ast_node_create_program(ast_node** func_list, int func_count){
    ast_node* node = malloc(sizeof(ast_node));
    node->type = AST_NODE_PROGRAM;
    node->prog.func_list = func_list;
    node->prog.func_count = func_count;
    return node;
}

