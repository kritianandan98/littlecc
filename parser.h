#ifndef PARSER_H
#define PARSER_H

#include "ast.h"
#include "lexer.h"

ast_node* parse_program(FILE*, char*);
ast_node* parse_function(FILE*, char*);
ast_node* parse_block(FILE*, char*);
ast_node* parse_expr(FILE*, char*);

#endif
