#include "c-lib.h"

#include <llvm-c-7/llvm-c/Analysis.h>
#include "codegen.h"
#include "symtable.h"

memory_cell* root = NULL;

LLVMValueRef codegen_variable(ast_node* node, char* env) {
    printf("Generating codegen module for variable: %s\n", node->var.name);
    LLVMValueRef val = search_val(root, node->var.name, env);
    if (val == NULL) {
        fprintf(stderr, "Variable not declared\n");
    }
    return val;
}

LLVMValueRef codegen_number(ast_node* node) {
    printf("Generating codegen module for number: %d\n", node->num.value);
    return LLVMConstInt(LLVMInt32Type(), node->num.value, 1);
}

LLVMValueRef codegen_expr(ast_node* node, LLVMModuleRef module,
                          LLVMBuilderRef builder, char* env) {
    LLVMValueRef l_oper = codegen_node(node->expr.l_oper, module, builder, env);
    LLVMValueRef r_oper = codegen_node(node->expr.r_oper, module, builder, env);

    if ((l_oper == NULL) || (r_oper == NULL)) {
        printf("Left operad or right operand is NULL\n");
        return NULL;
    }

    switch (node->expr.op) {
        case PLUS: {
            return LLVMBuildAdd(builder, l_oper, r_oper, "add");
        }
        case MINUS: {
            return LLVMBuildSub(builder, l_oper, r_oper, "sub");
        }
        case DIV: {
            return LLVMBuildUDiv(builder, l_oper, r_oper, "div");
        }
        case MUL: {
            return LLVMBuildMul(builder, l_oper, r_oper, "mul");
        }
    }

    return NULL;
}

LLVMValueRef codegen_prototype(ast_node* node, LLVMModuleRef module,
                               LLVMBuilderRef builder, char* env) {
    printf("Generating codegen value for prototype %s\n", node->proto.name);
    unsigned int args_count = node->proto.args_count;
    printf("Argument count: %d\n", args_count);

    LLVMValueRef func = LLVMGetNamedFunction(module, node->proto.name);
    if (func != NULL) {
        if (LLVMCountParams(func) != args_count) {
            printf("Function defined with different number of parameters\n");
            return NULL;
        }

        if (LLVMCountBasicBlocks(func) != 0) {
            printf("Function already defined\n");
            return NULL;
        }
    } else {
        printf("Function %s not defined, defining now\n", node->proto.name);
        LLVMTypeRef* params = malloc(sizeof(LLVMTypeRef) * args_count);
        for (unsigned int i = 0; i < args_count; i++) {
            params[i] = LLVMInt32Type();
        }

        printf("Parameters defined\n");

        LLVMTypeRef funcType =
            LLVMFunctionType(LLVMInt32Type(), params, args_count, 0);

        printf("Func type defined\n");

        // Create function.
        func = LLVMAddFunction(module, node->proto.name, funcType);
        printf("Function added\n");
        LLVMSetLinkage(func, LLVMExternalLinkage);
    }

    for (unsigned int i = 0; i < args_count; i++) {
        LLVMValueRef param = LLVMGetParam(func, i);
        LLVMSetValueName(param, node->proto.args[i]);
        printf("Arg: %s Env:%s\n", node->proto.args[i], env);
        root = add_cell(root, env, node->proto.args[i], param);
    }
    return func;
}

LLVMValueRef codegen_func(ast_node* node, LLVMModuleRef module,
                          LLVMBuilderRef builder, char* env) {
    printf("Generating codegen module for function %s\n",
           node->func.prototype->proto.name);
    LLVMValueRef func = codegen_prototype(node->func.prototype, module, builder,
                                          node->func.prototype->proto.name);
    if (func == NULL) {
        return NULL;
    }

    // Create basic block.
    LLVMBasicBlockRef block = LLVMAppendBasicBlock(func, "entry");
    LLVMPositionBuilderAtEnd(builder, block);

    // Generate body.
    printf("Generating code for %d statements\n", node->func.stmt_count);
    for (unsigned int i = 0; i < node->func.stmt_count; i++) {
        LLVMValueRef val = codegen_node(node->func.stmts[i], module, builder,
                                        node->func.prototype->proto.name);
        printf("Adding %s\n", node->func.results[i]);
        root = add_cell(root, node->func.prototype->proto.name,
                        node->func.results[i], val);
    }
    printf("Generating code for return statement\n");
    LLVMValueRef ret = codegen_node(node->func.ret, module, builder,
                                    node->func.prototype->proto.name);
    if (ret == NULL) {
        printf("Return is NULL\n");
    } else {
        LLVMBuildRet(builder, ret);
    }

    if (LLVMVerifyFunction(func, LLVMPrintMessageAction) == 1) {
        fprintf(stderr, "Verification of function failed\n");
        LLVMDeleteFunction(func);
        return NULL;
    }

    return func;
}

LLVMValueRef codegen_call(ast_node* node, LLVMModuleRef module,
                          LLVMBuilderRef builder, char* env) {
    printf("Generating codegen module for function call %s\n", node->call.name);
    LLVMValueRef func = LLVMGetNamedFunction(module, node->call.name);
    if (func == NULL) {
        printf("Function not defined\n");
        return NULL;
    }

    if (LLVMCountParams(func) != node->call.args_count) {
        printf("Invalid number of arguments\n");
        return NULL;
    }

    LLVMValueRef* args = malloc(sizeof(LLVMValueRef) * node->call.args_count);
    unsigned int args_count = node->call.args_count;
    for (unsigned int i = 0; i < args_count; i++) {
        args[i] = codegen_node(node->call.args[i], module, builder, env);
        if (args[i] == NULL) {
            free(args);
            printf("Invalid input\n");
            return NULL;
        }
    }
    return LLVMBuildCall(builder, func, args, args_count, "call_ins");
}

LLVMValueRef codegen_return(ast_node* node, LLVMModuleRef module,
                            LLVMBuilderRef builder, char* env) {
    if (node == NULL) {
        return NULL;
    }
    return codegen_node(node, module, builder, env);
}

LLVMValueRef codegen_program(ast_node* node, LLVMModuleRef module,
                             LLVMBuilderRef builder, char* env) {
    int total_function_count = node->prog.func_count;
    LLVMValueRef val = NULL;
    printf("Generating codegen modules for program\n");
    printf("Function count: %d\n", total_function_count);
    for (int i = 0; i < total_function_count; i++) {
        val = codegen_node(node->prog.func_list[i], module, builder, env);
    }
    print_table(root);
    return val;
}

LLVMValueRef codegen_node(ast_node* node, LLVMModuleRef module,
                          LLVMBuilderRef builder, char* env) {
    switch (node->type) {
        case AST_NODE_PROGRAM: {
            return codegen_program(node, module, builder, env);
        }
        case AST_NODE_PROTOTYPE: {
            return codegen_prototype(node, module, builder, env);
        }
        case AST_NODE_FUNCION: {
            return codegen_func(node, module, builder, env);
        }
        case AST_NODE_CALL: {
            return codegen_call(node, module, builder, env);
        }
        case AST_NODE_RETURN_STMT: {
            return codegen_return(node, module, builder, env);
        }
        case AST_NODE_EXPR: {
            return codegen_expr(node, module, builder, env);
        }
        case AST_NODE_VARIABLE: {
            return codegen_variable(node, env);
        }
        case AST_NODE_NUMBER: {
            return codegen_number(node);
        }
    }
    return NULL;
}
