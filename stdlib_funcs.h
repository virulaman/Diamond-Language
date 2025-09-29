/*
 * DMO Language Standard Library Functions Header
 * Built-in functions like show.txt, scanf, fget
 */

#ifndef STDLIB_FUNCS_H
#define STDLIB_FUNCS_H

#include "interpreter.h"

// Forward declarations
Value execute_node(ASTNode* node, InterpreterContext* ctx);

// Function prototypes
void init_stdlib_functions(InterpreterContext* ctx);
Value call_builtin_function(const char* name, ASTNode** args, int arg_count, InterpreterContext* ctx);

// Built-in function implementations
Value builtin_show_txt(ASTNode** args, int arg_count, InterpreterContext* ctx);
Value builtin_scanf(ASTNode** args, int arg_count, InterpreterContext* ctx);
Value builtin_fget(ASTNode** args, int arg_count, InterpreterContext* ctx);
Value builtin_main(ASTNode** args, int arg_count, InterpreterContext* ctx);

// Utility functions
bool is_builtin_function(const char* name);
bool is_dmo_graphics_function(const char* name);

#endif // STDLIB_FUNCS_H
