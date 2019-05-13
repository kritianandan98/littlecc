#include <stdbool.h>

#include <llvm-c-7/llvm-c/Analysis.h>
#include <llvm-c-7/llvm-c/BitWriter.h>
#include <llvm-c-7/llvm-c/ExecutionEngine.h>
#include <llvm-c-7/llvm-c/Target.h>
#include <llvm-c-7/llvm-c/Transforms/Scalar.h>
#include <llvm-c-7/llvm-c/Transforms/Utils.h>

#include "codegen.h"
#include "parser.h"

int main() {
    LLVMModuleRef module = LLVMModuleCreateWithName("littlecc");
    LLVMBuilderRef builder = LLVMCreateBuilder();
    LLVMExecutionEngineRef engine;

    char* error = NULL;
    LLVMVerifyModule(module, LLVMAbortProcessAction, &error);
    LLVMDisposeMessage(error);

    LLVMInitializeNativeTarget();
    LLVMLinkInMCJIT();
    LLVMInitializeNativeAsmPrinter();

    char* msg;
    if (LLVMCreateExecutionEngineForModule(&engine, module, &msg) == 1) {
        fprintf(stderr, "%s\n", msg);
        LLVMDisposeMessage(msg);
        exit(1);
    }

    LLVMPassManagerRef pass_manager =
        LLVMCreateFunctionPassManagerForModule(module);
    LLVMAddPromoteMemoryToRegisterPass(pass_manager);
    LLVMAddInstructionCombiningPass(pass_manager);
    LLVMAddReassociatePass(pass_manager);
    LLVMAddGVNPass(pass_manager);
    LLVMAddCFGSimplificationPass(pass_manager);
    LLVMInitializeFunctionPassManager(pass_manager);

    FILE* fr = fopen("testfile.txt", "r");
    if (!fr) {
        printf("File not opened ERROR\n");
        exit(1);
    }
    printf("Test file opened\n");

    char* env = malloc(sizeof(char)*7);
    strcpy(env, "global");
    ast_node* prog = parse_program(fr, env);
    printf("AST DONE\n");

    strcpy(env, "global");
    LLVMValueRef prog_ = codegen_node(prog, module, builder, env);
    printf("Codegen modules DONE\n");

    if (prog_ == NULL) {
        fprintf(stderr, "Unable to generate code for module / empty\n");
        exit(0);
    }
    LLVMRunFunctionPassManager(pass_manager, prog_);
    printf("LLVMFunctionPassManager DONE\n");
    void* fp = LLVMGetPointerToGlobal(engine, prog_);
    if(fp == NULL){
        printf("Please define main function\n");
    }
    else{
        int (*FP)() = (int (*)())(intptr_t)fp;
        fprintf(stderr, "\n\n\nFINAL ANSWER %d\n\n\n\n", FP());
    }

    if (LLVMWriteBitcodeToFile(module, "littlecc.bc") != 0) {
        fprintf(stderr, "error writing bitcode to file, skipping\n");
    }

    LLVMDumpModule(module);

    LLVMDisposePassManager(pass_manager);
    LLVMDisposeBuilder(builder);
    LLVMDisposeModule(module);
}
