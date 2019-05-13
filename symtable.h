#ifndef SYMTABLE_H
#define SYMTABLE_H

#include <llvm-c-7/llvm-c/Core.h>

typedef struct memory_cell {
    char *env, *var_name;
    LLVMValueRef val;
    struct memory_cell *next_env, *next_cell;
} memory_cell;

memory_cell* create_cell(char*, char*, LLVMValueRef);
memory_cell* add_cell(memory_cell*, char*, char*, LLVMValueRef);
memory_cell* clear_env(memory_cell*, char*);
LLVMValueRef search_val(memory_cell*, char*, char*);
void print_table(memory_cell*);

#endif
