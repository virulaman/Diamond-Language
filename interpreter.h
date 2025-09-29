/*
 * DMO Language Interpreter Header
 * Executes the Abstract Syntax Tree
 */

#ifndef INTERPRETER_H
#define INTERPRETER_H

#include "ast.h"

// Value types for runtime
typedef enum {
    VALUE_NUMBER,
    VALUE_STRING,
    VALUE_VOID
} ValueType;

// Runtime value
typedef struct {
    ValueType type;
    union {
        double number;
        char* string;
    };
} Value;

// Variable structure
typedef struct Variable {
    char* name;
    char* type;
    Value value;
    struct Variable* next;
} Variable;

// Function structure
typedef struct Function {
    char* name;
    char* return_type;
    ASTNode** parameters;
    int param_count;
    ASTNode* body;
    struct Function* next;
} Function;

// Interpreter context
typedef struct {
    Variable* variables;
    Function* functions;
    Variable* global_vars;
    Function* global_funcs;
    bool has_return;
    Value return_value;
} InterpreterContext;

// Function prototypes
int interpret(ASTNode* ast, const char* source_file);
InterpreterContext* create_interpreter_context();
void free_interpreter_context(InterpreterContext* ctx);

// Execution functions
Value execute_node(ASTNode* node, InterpreterContext* ctx);
Value execute_program(ASTNode* node, InterpreterContext* ctx);
Value execute_function_def(ASTNode* node, InterpreterContext* ctx);
Value execute_variable_decl(ASTNode* node, InterpreterContext* ctx);
Value execute_assignment(ASTNode* node, InterpreterContext* ctx);
Value execute_function_call(ASTNode* node, InterpreterContext* ctx);
Value execute_if_statement(ASTNode* node, InterpreterContext* ctx);
Value execute_while_loop(ASTNode* node, InterpreterContext* ctx);
Value execute_for_loop(ASTNode* node, InterpreterContext* ctx);
Value execute_return_statement(ASTNode* node, InterpreterContext* ctx);
Value execute_block(ASTNode* node, InterpreterContext* ctx);
Value execute_binary_op(ASTNode* node, InterpreterContext* ctx);
Value execute_unary_op(ASTNode* node, InterpreterContext* ctx);
Value execute_identifier(ASTNode* node, InterpreterContext* ctx);
Value execute_member_access(ASTNode* node, InterpreterContext* ctx);

// Variable management
void set_variable(InterpreterContext* ctx, const char* name, const char* type, Value value);
Variable* get_variable(InterpreterContext* ctx, const char* name);
void set_function(InterpreterContext* ctx, Function* func);
Function* get_function(InterpreterContext* ctx, const char* name);

// Value utilities
Value create_number_value(double num);
Value create_string_value(const char* str);
Value create_void_value();
void free_value(Value value);
Value copy_value(Value value);
void print_value(Value value);

// Built-in function execution
Value call_builtin_function(const char* name, ASTNode** args, int arg_count, InterpreterContext* ctx);

#endif // INTERPRETER_H
