/*
 * DMO Language Interpreter Implementation
 * Executes the Abstract Syntax Tree
 */

#define _POSIX_C_SOURCE 200809L
#include "interpreter.h"
#include "stdlib_funcs.h"
#include "dmo_graphs.h"
#include "modules.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

InterpreterContext* create_interpreter_context() {
    InterpreterContext* ctx = malloc(sizeof(InterpreterContext));
    ctx->variables = NULL;
    ctx->functions = NULL;
    ctx->global_vars = NULL;
    ctx->global_funcs = NULL;
    ctx->has_return = false;
    ctx->return_value = create_void_value();
    return ctx;
}

void free_interpreter_context(InterpreterContext* ctx) {
    // Free variables
    Variable* var = ctx->variables;
    while (var) {
        Variable* next = var->next;
        free(var->name);
        free(var->type);
        free_value(var->value);
        free(var);
        var = next;
    }
    
    // Free functions
    Function* func = ctx->functions;
    while (func) {
        Function* next = func->next;
        free(func->name);
        free(func->return_type);
        free(func);
        func = next;
    }
    
    free_value(ctx->return_value);
    free(ctx);
}

Value create_number_value(double num) {
    Value value;
    value.type = VALUE_NUMBER;
    value.number = num;
    return value;
}

Value create_string_value(const char* str) {
    Value value;
    value.type = VALUE_STRING;
    value.string = strdup(str);
    return value;
}

Value create_void_value() {
    Value value;
    value.type = VALUE_VOID;
    return value;
}

void free_value(Value value) {
    if (value.type == VALUE_STRING && value.string) {
        free(value.string);
    }
}

Value copy_value(Value value) {
    if (value.type == VALUE_STRING) {
        return create_string_value(value.string);
    }
    return value;
}

void print_value(Value value) {
    switch (value.type) {
        case VALUE_NUMBER:
            printf("%.6g", value.number);
            break;
        case VALUE_STRING:
            printf("%s", value.string);
            break;
        case VALUE_VOID:
            printf("void");
            break;
    }
}

void set_variable(InterpreterContext* ctx, const char* name, const char* type, Value value) {
    Variable* var = ctx->variables;
    
    // Check if variable already exists
    while (var) {
        if (strcmp(var->name, name) == 0) {
            free_value(var->value);
            var->value = copy_value(value);
            return;
        }
        var = var->next;
    }
    
    // Create new variable
    var = malloc(sizeof(Variable));
    var->name = strdup(name);
    var->type = strdup(type);
    var->value = copy_value(value);
    var->next = ctx->variables;
    ctx->variables = var;
}

Variable* get_variable(InterpreterContext* ctx, const char* name) {
    Variable* var = ctx->variables;
    while (var) {
        if (strcmp(var->name, name) == 0) {
            return var;
        }
        var = var->next;
    }
    
    // Check global variables
    var = ctx->global_vars;
    while (var) {
        if (strcmp(var->name, name) == 0) {
            return var;
        }
        var = var->next;
    }
    
    return NULL;
}

void set_function(InterpreterContext* ctx, Function* func) {
    func->next = ctx->functions;
    ctx->functions = func;
}

Function* get_function(InterpreterContext* ctx, const char* name) {
    Function* func = ctx->functions;
    while (func) {
        if (strcmp(func->name, name) == 0) {
            return func;
        }
        func = func->next;
    }
    
    // Check global functions
    func = ctx->global_funcs;
    while (func) {
        if (strcmp(func->name, name) == 0) {
            return func;
        }
        func = func->next;
    }
    
    return NULL;
}

int interpret(ASTNode* ast, const char* source_file) {
    (void)source_file; // Suppress unused parameter warning
    
    if (!ast) {
        fprintf(stderr, "Error: No AST to interpret\n");
        return 1;
    }
    
    InterpreterContext* ctx = create_interpreter_context();
    
    // Initialize built-in functions and graphics system
    init_stdlib_functions(ctx);
    init_dmo_graphics();
    
    printf("Executing DMO program...\n");
    
    // First, execute the AST to define all functions and variables
    Value result = execute_node(ast, ctx);
    
    // Now automatically call the main function if it exists
    Function* main_func = get_function(ctx, "main");
    if (main_func) {
        // Create a function call node for main
        ASTNode* main_call = malloc(sizeof(ASTNode));
        main_call->type = AST_FUNCTION_CALL;
        main_call->func_call.name = strdup("main");
        main_call->func_call.arguments = NULL;
        main_call->func_call.arg_count = 0;
        
        // Call main function
        Value main_result = execute_function_call(main_call, ctx);
        
        if (main_result.type == VALUE_NUMBER) {
            printf("Program returned: %.6g\n", main_result.number);
        }
        
        // Clean up
        free(main_call->func_call.name);
        free(main_call);
        free_value(main_result);
    }
    
    if (result.type == VALUE_NUMBER) {
        printf("Program setup returned: %.6g\n", result.number);
    }
    
    free_value(result);
    cleanup_dmo_graphics();
    free_interpreter_context(ctx);
    
    return 0;
}

Value execute_node(ASTNode* node, InterpreterContext* ctx) {
    if (!node) {
        return create_void_value();
    }
    
    switch (node->type) {
        case AST_PROGRAM:
            return execute_program(node, ctx);
        case AST_USE_STATEMENT:
            // Handle module imports
            load_module(node->use_stmt.module_name, ctx);
            return create_void_value();
        case AST_FUNCTION_DEF:
            return execute_function_def(node, ctx);
        case AST_VARIABLE_DECL:
            return execute_variable_decl(node, ctx);
        case AST_ASSIGNMENT:
            return execute_assignment(node, ctx);
        case AST_FUNCTION_CALL:
            return execute_function_call(node, ctx);
        case AST_IF_STATEMENT:
            return execute_if_statement(node, ctx);
        case AST_WHILE_LOOP:
            return execute_while_loop(node, ctx);
        case AST_FOR_LOOP:
            return execute_for_loop(node, ctx);
        case AST_RETURN_STATEMENT:
            return execute_return_statement(node, ctx);
        case AST_BLOCK:
            return execute_block(node, ctx);
        case AST_BINARY_OP:
            return execute_binary_op(node, ctx);
        case AST_UNARY_OP:
            return execute_unary_op(node, ctx);
        case AST_IDENTIFIER:
            return execute_identifier(node, ctx);
        case AST_NUMBER:
            return create_number_value(node->number.value);
        case AST_STRING:
            return create_string_value(node->string.value);
        case AST_MEMBER_ACCESS:
            return execute_member_access(node, ctx);
        default:
            fprintf(stderr, "Error: Unknown AST node type: %d\n", node->type);
            return create_void_value();
    }
}

Value execute_program(ASTNode* node, InterpreterContext* ctx) {
    Value result = create_void_value();
    
    for (int i = 0; i < node->program.statement_count; i++) {
        free_value(result);
        result = execute_node(node->program.statements[i], ctx);
        
        if (ctx->has_return) {
            break;
        }
    }
    
    return result;
}

Value execute_function_def(ASTNode* node, InterpreterContext* ctx) {
    Function* func = malloc(sizeof(Function));
    func->name = strdup(node->func_def.name);
    func->return_type = strdup(node->func_def.return_type);
    func->parameters = node->func_def.parameters;
    func->param_count = node->func_def.param_count;
    func->body = node->func_def.body;
    
    set_function(ctx, func);
    
    return create_void_value();
}

Value execute_variable_decl(ASTNode* node, InterpreterContext* ctx) {
    Value value = create_void_value();
    
    if (node->var_decl.initializer) {
        value = execute_node(node->var_decl.initializer, ctx);
    } else {
        // Default initialization based on type
        if (strcmp(node->var_decl.type, "int") == 0) {
            value = create_number_value(0);
        } else if (strcmp(node->var_decl.type, "string") == 0) {
            value = create_string_value("");
        } else if (strcmp(node->var_decl.type, "char") == 0) {
            value = create_string_value("");
        }
    }
    
    set_variable(ctx, node->var_decl.name, node->var_decl.type, value);
    
    return value;
}

Value execute_assignment(ASTNode* node, InterpreterContext* ctx) {
    Value value = execute_node(node->assignment.value, ctx);
    
    if (node->assignment.target->type == AST_IDENTIFIER) {
        Variable* var = get_variable(ctx, node->assignment.target->identifier.value);
        if (var) {
            free_value(var->value);
            var->value = copy_value(value);
        } else {
            fprintf(stderr, "Error: Undefined variable '%s'\n", node->assignment.target->identifier.value);
        }
    }
    
    return value;
}

Value execute_function_call(ASTNode* node, InterpreterContext* ctx) {
    // Check for built-in functions first
    Value result = call_builtin_function(node->func_call.name, node->func_call.arguments, 
                                       node->func_call.arg_count, ctx);
    
    // Check if builtin function was actually called (not just returned void by default)
    // For successful builtin calls that return void, we still want to return that result
    if (result.type != VALUE_VOID || 
        strcmp(node->func_call.name, "show.txt") == 0 ||
        strcmp(node->func_call.name, "scanf") == 0 ||
        strcmp(node->func_call.name, "fget") == 0 ||
        is_dmo_graphics_function(node->func_call.name)) {
        return result;
    }
    
    // Look for user-defined functions
    Function* func = get_function(ctx, node->func_call.name);
    if (!func) {
        fprintf(stderr, "Error: Undefined function '%s'\n", node->func_call.name);
        return create_void_value();
    }
    
    // Create new context for function execution
    InterpreterContext* func_ctx = create_interpreter_context();
    func_ctx->global_vars = ctx->variables;
    func_ctx->global_funcs = ctx->functions;
    
    // Bind parameters
    for (int i = 0; i < func->param_count && i < node->func_call.arg_count; i++) {
        Value arg_value = execute_node(node->func_call.arguments[i], ctx);
        ASTNode* param = func->parameters[i];
        set_variable(func_ctx, param->var_decl.name, param->var_decl.type, arg_value);
        free_value(arg_value);
    }
    
    // Execute function body
    Value return_val = execute_node(func->body, func_ctx);
    
    if (func_ctx->has_return) {
        free_value(return_val);
        return_val = copy_value(func_ctx->return_value);
    }
    
    free_interpreter_context(func_ctx);
    return return_val;
}

Value execute_binary_op(ASTNode* node, InterpreterContext* ctx) {
    Value left = execute_node(node->binary_op.left, ctx);
    Value right = execute_node(node->binary_op.right, ctx);
    
    Value result = create_void_value();
    
    if (left.type == VALUE_NUMBER && right.type == VALUE_NUMBER) {
        switch (node->binary_op.operator) {
            case TOKEN_PLUS:
                result = create_number_value(left.number + right.number);
                break;
            case TOKEN_MINUS:
                result = create_number_value(left.number - right.number);
                break;
            case TOKEN_MULTIPLY:
                result = create_number_value(left.number * right.number);
                break;
            case TOKEN_DIVIDE:
                if (right.number != 0) {
                    result = create_number_value(left.number / right.number);
                } else {
                    fprintf(stderr, "Error: Division by zero\n");
                    result = create_number_value(0);
                }
                break;
            case TOKEN_MODULO:
                if (right.number != 0) {
                    result = create_number_value(fmod(left.number, right.number));
                } else {
                    fprintf(stderr, "Error: Modulo by zero\n");
                    result = create_number_value(0);
                }
                break;
            case TOKEN_EQUAL:
                result = create_number_value(left.number == right.number ? 1 : 0);
                break;
            case TOKEN_NOT_EQUAL:
                result = create_number_value(left.number != right.number ? 1 : 0);
                break;
            case TOKEN_LESS:
                result = create_number_value(left.number < right.number ? 1 : 0);
                break;
            case TOKEN_GREATER:
                result = create_number_value(left.number > right.number ? 1 : 0);
                break;
            case TOKEN_LESS_EQUAL:
                result = create_number_value(left.number <= right.number ? 1 : 0);
                break;
            case TOKEN_GREATER_EQUAL:
                result = create_number_value(left.number >= right.number ? 1 : 0);
                break;
            default:
                fprintf(stderr, "Error: Unknown binary operator\n");
                result = create_number_value(0);
        }
    } else if (left.type == VALUE_STRING && right.type == VALUE_STRING) {
        switch (node->binary_op.operator) {
            case TOKEN_PLUS: {
                // String concatenation
                int len1 = strlen(left.string);
                int len2 = strlen(right.string);
                char* concat = malloc(len1 + len2 + 1);
                strcpy(concat, left.string);
                strcat(concat, right.string);
                result = create_string_value(concat);
                free(concat);
                break;
            }
            case TOKEN_EQUAL:
                result = create_number_value(strcmp(left.string, right.string) == 0 ? 1 : 0);
                break;
            case TOKEN_NOT_EQUAL:
                result = create_number_value(strcmp(left.string, right.string) != 0 ? 1 : 0);
                break;
            default:
                fprintf(stderr, "Error: Invalid operator for strings\n");
                result = create_string_value("");
        }
    }
    
    free_value(left);
    free_value(right);
    return result;
}

Value execute_identifier(ASTNode* node, InterpreterContext* ctx) {
    Variable* var = get_variable(ctx, node->identifier.value);
    if (var) {
        return copy_value(var->value);
    }
    
    fprintf(stderr, "Error: Undefined variable '%s'\n", node->identifier.value);
    return create_void_value();
}

Value execute_member_access(ASTNode* node, InterpreterContext* ctx) {
    (void)ctx; // Suppress unused parameter warning
    
    // Handle dmo.gr.* calls
    if (node->member_access.object->type == AST_IDENTIFIER &&
        strcmp(node->member_access.object->identifier.value, "dmo") == 0) {
        
        // This is a dmo graphics call, return a special marker
        // The actual call will be handled in function call execution
        return create_string_value("dmo_graphics_call");
    }
    
    return create_void_value();
}

// Simplified implementations for control structures
Value execute_if_statement(ASTNode* node, InterpreterContext* ctx) {
    Value condition = execute_node(node->if_stmt.condition, ctx);
    
    bool is_true = false;
    if (condition.type == VALUE_NUMBER) {
        is_true = condition.number != 0;
    } else if (condition.type == VALUE_STRING) {
        is_true = strlen(condition.string) > 0;
    }
    
    Value result = create_void_value();
    
    if (is_true) {
        result = execute_node(node->if_stmt.then_stmt, ctx);
    } else if (node->if_stmt.else_stmt) {
        result = execute_node(node->if_stmt.else_stmt, ctx);
    }
    
    free_value(condition);
    return result;
}

Value execute_while_loop(ASTNode* node, InterpreterContext* ctx) {
    Value result = create_void_value();
    
    while (true) {
        Value condition = execute_node(node->while_loop.condition, ctx);
        
        bool is_true = false;
        if (condition.type == VALUE_NUMBER) {
            is_true = condition.number != 0;
        }
        
        free_value(condition);
        
        if (!is_true) {
            break;
        }
        
        free_value(result);
        result = execute_node(node->while_loop.body, ctx);
        
        if (ctx->has_return) {
            break;
        }
    }
    
    return result;
}

Value execute_block(ASTNode* node, InterpreterContext* ctx) {
    Value result = create_void_value();
    
    for (int i = 0; i < node->block.statement_count; i++) {
        free_value(result);
        result = execute_node(node->block.statements[i], ctx);
        
        if (ctx->has_return) {
            break;
        }
    }
    
    return result;
}

Value execute_return_statement(ASTNode* node, InterpreterContext* ctx) {
    Value value = create_void_value();
    
    if (node->return_stmt.value) {
        value = execute_node(node->return_stmt.value, ctx);
    }
    
    ctx->has_return = true;
    free_value(ctx->return_value);
    ctx->return_value = copy_value(value);
    
    return value;
}

// Additional execute functions for other node types would be implemented here
// For brevity, I've included the main ones needed for basic functionality

Value execute_for_loop(ASTNode* node, InterpreterContext* ctx) {
    // Execute initialization
    if (node->for_loop.init) {
        Value init_result = execute_node(node->for_loop.init, ctx);
        free_value(init_result);
    }
    
    Value result = create_void_value();
    
    while (true) {
        // Check condition
        if (node->for_loop.condition) {
            Value condition = execute_node(node->for_loop.condition, ctx);
            
            bool is_true = false;
            if (condition.type == VALUE_NUMBER) {
                is_true = condition.number != 0;
            }
            
            free_value(condition);
            
            if (!is_true) {
                break;
            }
        }
        
        // Execute body
        free_value(result);
        result = execute_node(node->for_loop.body, ctx);
        
        if (ctx->has_return) {
            break;
        }
        
        // Execute increment
        if (node->for_loop.increment) {
            Value inc_result = execute_node(node->for_loop.increment, ctx);
            free_value(inc_result);
        }
    }
    
    return result;
}

Value execute_unary_op(ASTNode* node, InterpreterContext* ctx) {
    Value operand = execute_node(node->unary_op.operand, ctx);
    Value result = create_void_value();
    
    switch (node->unary_op.operator) {
        case TOKEN_MINUS:
            if (operand.type == VALUE_NUMBER) {
                result = create_number_value(-operand.number);
            }
            break;
        case TOKEN_NOT:
            if (operand.type == VALUE_NUMBER) {
                result = create_number_value(operand.number == 0 ? 1 : 0);
            } else if (operand.type == VALUE_STRING) {
                result = create_number_value(strlen(operand.string) == 0 ? 1 : 0);
            }
            break;
        default:
            fprintf(stderr, "Error: Unknown unary operator\n");
            result = create_number_value(0);
    }
    
    free_value(operand);
    return result;
}
