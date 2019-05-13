#include "symtable.h"
#include "c-lib.h"

memory_cell* create_cell(char* env, char* var_name, LLVMValueRef val) {
    memory_cell* mem_cell = (memory_cell*)malloc(sizeof(memory_cell));
    mem_cell->env = strdup(env);
    mem_cell->var_name = strdup(var_name);
    mem_cell->val = val;
    mem_cell->next_env = mem_cell->next_cell = NULL;
    return mem_cell;
}

memory_cell* add_cell(memory_cell* root, char* env, char* var_name,
                      LLVMValueRef val) {
    memory_cell* temp = create_cell(env, var_name, val);
    if (root == NULL) {
        return temp;
    }
    memory_cell* iter = root;
    while (iter->next_env != NULL) {
        if (strcmp(iter->env, env) == 0) {
            break;
        }
        iter = iter->next_env;
    }
    if (strcmp(iter->env, env) != 0) {
        iter->next_env = temp;
        return root;
    }
    while (iter->next_cell != NULL) {
        if (!strcmp(iter->var_name, var_name)) {
            iter->val = val;
            return root;
        }
        iter = iter->next_cell;
    }
    if (!strcmp(iter->var_name, var_name)) {
        iter->val = val;
        return root;
    }
    iter->next_cell = temp;
    return root;
}

memory_cell* clear_env(memory_cell* root, char* env) {
    if (root == NULL) {
        return root;
    }
    memory_cell* iter = root;
    while (strcmp(iter->env, env) != 0) {
        iter = iter->next_env;
    }
    while (iter != NULL) {
        memory_cell* temp = iter;
        iter = iter->next_cell;
        free(temp);
    }
    return root;
}

LLVMValueRef search_val(memory_cell* root, char* name, char* env) {
    if (root == NULL) {
        return NULL;
    }
    memory_cell* iter = root;
    while (strcmp(iter->env, env) != 0) {
        iter = iter->next_env;
    }
    while (iter != NULL) {
        if (strcmp(iter->var_name, name) == 0) {
            return iter->val;
        }
        iter = iter->next_cell;
    }
    return NULL;
}

void print_table(memory_cell* root) {
    memory_cell *iter = root, *iter_x = root;
    printf("PRINTING SYMBOL TABLE\n");
    while (iter != NULL) {
        while (iter_x != NULL) {
            printf("Variable: %s Environment: %s\n", iter_x->var_name,
                   iter_x->env);
            iter_x = iter_x->next_cell;
        }
        iter = iter->next_env;
        iter_x = iter;
    }
}
