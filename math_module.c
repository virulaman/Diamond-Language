#include "math_module.h"
#include "interpreter.h"
#include <math.h>
#include <stdio.h>

// --- Math Function Implementations ---

Value math_sin(ASTNode** args, int arg_count, InterpreterContext* ctx) {
    if (arg_count != 1) {
        fprintf(stderr, "Error: sin() requires exactly one argument\n");
        return create_number_value(0);
    }
    Value val = execute_node(args[0], ctx);
    if (val.type != VALUE_NUMBER) {
        fprintf(stderr, "Error: sin() argument must be a number\n");
        free_value(val);
        return create_number_value(0);
    }
    double result = sin(val.number);
    free_value(val);
    return create_number_value(result);
}

Value math_cos(ASTNode** args, int arg_count, InterpreterContext* ctx) {
    if (arg_count != 1) {
        fprintf(stderr, "Error: cos() requires exactly one argument\n");
        return create_number_value(0);
    }
    Value val = execute_node(args[0], ctx);
    if (val.type != VALUE_NUMBER) {
        fprintf(stderr, "Error: cos() argument must be a number\n");
        free_value(val);
        return create_number_value(0);
    }
    double result = cos(val.number);
    free_value(val);
    return create_number_value(result);
}

Value math_tan(ASTNode** args, int arg_count, InterpreterContext* ctx) {
    if (arg_count != 1) {
        fprintf(stderr, "Error: tan() requires exactly one argument\n");
        return create_number_value(0);
    }
    Value val = execute_node(args[0], ctx);
    if (val.type != VALUE_NUMBER) {
        fprintf(stderr, "Error: tan() argument must be a number\n");
        free_value(val);
        return create_number_value(0);
    }
    double result = tan(val.number);
    free_value(val);
    return create_number_value(result);
}

Value math_sqrt(ASTNode** args, int arg_count, InterpreterContext* ctx) {
    if (arg_count != 1) {
        fprintf(stderr, "Error: sqrt() requires exactly one argument\n");
        return create_number_value(0);
    }
    Value val = execute_node(args[0], ctx);
    if (val.type != VALUE_NUMBER) {
        fprintf(stderr, "Error: sqrt() argument must be a number\n");
        free_value(val);
        return create_number_value(0);
    }
    if (val.number < 0) {
        fprintf(stderr, "Error: sqrt() of a negative number is not allowed\n");
        free_value(val);
        return create_number_value(0);
    }
    double result = sqrt(val.number);
    free_value(val);
    return create_number_value(result);
}

Value math_pow(ASTNode** args, int arg_count, InterpreterContext* ctx) {
    if (arg_count != 2) {
        fprintf(stderr, "Error: pow() requires exactly two arguments\n");
        return create_number_value(0);
    }
    Value base = execute_node(args[0], ctx);
    Value exp = execute_node(args[1], ctx);
    if (base.type != VALUE_NUMBER || exp.type != VALUE_NUMBER) {
        fprintf(stderr, "Error: pow() arguments must be numbers\n");
        free_value(base);
        free_value(exp);
        return create_number_value(0);
    }
    double result = pow(base.number, exp.number);
    free_value(base);
    free_value(exp);
    return create_number_value(result);
}


// --- Module Initialization ---

void init_math_module(InterpreterContext* ctx) {
    register_native_function(ctx, "sin", math_sin);
    register_native_function(ctx, "cos", math_cos);
    register_native_function(ctx, "tan", math_tan);
    register_native_function(ctx, "sqrt", math_sqrt);
    register_native_function(ctx, "pow", math_pow);
}