#ifndef CODEGEN_H
#define CODEGEN_H

#include <llvm-c-7/llvm-c/Core.h>
#include "ast.h"

LLVMValueRef codegen_number(ast_node*);
LLVMValueRef codegen_variable(ast_node*, char*);
LLVMValueRef codegen_expr(ast_node*, LLVMModuleRef, LLVMBuilderRef, char*);
LLVMValueRef codegen_prototype(ast_node*, LLVMModuleRef, LLVMBuilderRef, char*);
LLVMValueRef codegen_func(ast_node*, LLVMModuleRef, LLVMBuilderRef, char*);
LLVMValueRef codegen_call(ast_node*, LLVMModuleRef, LLVMBuilderRef, char*);
LLVMValueRef codegen_return(ast_node*, LLVMModuleRef, LLVMBuilderRef, char*);
LLVMValueRef codegen_program(ast_node*, LLVMModuleRef, LLVMBuilderRef, char*);
LLVMValueRef codegen_node(ast_node*, LLVMModuleRef, LLVMBuilderRef, char*);

#endif
