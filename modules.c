/*
 * DMO Language Module System Implementation
 * Handles dynamic loading of modules (stdlib, dmo_graphs, request)
 */

#define _POSIX_C_SOURCE 200809L
#include "modules.h"
#include "stdlib_funcs.h"
#include "dmo_graphs.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

void init_modules(InterpreterContext* ctx) {
    // Modules are loaded on demand
    printf("Module system initialized\n");
}

void init_module_system() {
    // Initialize the module system for the main program
    printf("Module system initialized\n");
}

void cleanup_module_system() {
    // Clean up the module system
    printf("Module system cleaned up\n");
}

bool load_module(const char* module_name, InterpreterContext* ctx) {
    if (strcmp(module_name, "stdlib") == 0) {
        load_stdlib_module(ctx);
        return true;
    } else if (strcmp(module_name, "dmo_graphs") == 0) {
        load_dmo_graphs_module(ctx);
        return true;
    } else if (strcmp(module_name, "request") == 0) {
        load_request_module(ctx);
        return true;
    } else if (strcmp(module_name, "math") == 0) {
        load_math_module(ctx);
        return true;
    }
    
    fprintf(stderr, "Error: Unknown module '%s'\n", module_name);
    return false;
}

void load_stdlib_module(InterpreterContext* ctx) {
    init_stdlib_functions(ctx);
}

void load_dmo_graphs_module(InterpreterContext* ctx) {
    init_dmo_graphics();
    printf("Loading module: dmo_graphs\n");
}

void load_request_module(InterpreterContext* ctx) {
    printf("Loading module: request\n");
    // Request module functions will be available
    // request.get(url), request.post(url, data), etc.
}

void load_math_module(InterpreterContext* ctx) {
    printf("Loading module: math\n");
    // Math module functions will be available
    // sin(), cos(), tan(), sigmoid(), sqrt(), pow(), etc.
}

// Request module functions
Value request_get(ASTNode** args, int arg_count, InterpreterContext* ctx) {
    if (arg_count == 0) {
        fprintf(stderr, "Error: request.get requires a URL\n");
        return create_string_value("");
    }
    
    Value url_val = execute_node(args[0], ctx);
    if (url_val.type != VALUE_STRING) {
        fprintf(stderr, "Error: request.get URL must be a string\n");
        free_value(url_val);
        return create_string_value("");
    }
    
    // Simple HTTP GET using curl command
    char command[1024];
    snprintf(command, sizeof(command), "curl -s '%s'", url_val.string);
    
    FILE* pipe = popen(command, "r");
    if (!pipe) {
        fprintf(stderr, "Error: Failed to execute HTTP request\n");
        free_value(url_val);
        return create_string_value("");
    }
    
    // Read response
    char* response = malloc(4096);
    size_t bytes_read = fread(response, 1, 4095, pipe);
    response[bytes_read] = '\0';
    
    pclose(pipe);
    free_value(url_val);
    
    Value result = create_string_value(response);
    free(response);
    
    return result;
}

Value request_post(ASTNode** args, int arg_count, InterpreterContext* ctx) {
    if (arg_count < 2) {
        fprintf(stderr, "Error: request.post requires URL and data\n");
        return create_string_value("");
    }
    
    Value url_val = execute_node(args[0], ctx);
    Value data_val = execute_node(args[1], ctx);
    
    if (url_val.type != VALUE_STRING || data_val.type != VALUE_STRING) {
        fprintf(stderr, "Error: request.post URL and data must be strings\n");
        free_value(url_val);
        free_value(data_val);
        return create_string_value("");
    }
    
    // Simple HTTP POST using curl command
    char command[2048];
    snprintf(command, sizeof(command), "curl -s -X POST -d '%s' '%s'", 
             data_val.string, url_val.string);
    
    FILE* pipe = popen(command, "r");
    if (!pipe) {
        fprintf(stderr, "Error: Failed to execute HTTP request\n");
        free_value(url_val);
        free_value(data_val);
        return create_string_value("");
    }
    
    // Read response
    char* response = malloc(4096);
    size_t bytes_read = fread(response, 1, 4095, pipe);
    response[bytes_read] = '\0';
    
    pclose(pipe);
    free_value(url_val);
    free_value(data_val);
    
    Value result = create_string_value(response);
    free(response);
    
    return result;
}

bool is_request_function(const char* name) {
    return strstr(name, "request.") != NULL;
}

Value call_request_function(const char* name, ASTNode** args, int arg_count, InterpreterContext* ctx) {
    if (strcmp(name, "request.get") == 0) {
        return request_get(args, arg_count, ctx);
    } else if (strcmp(name, "request.post") == 0) {
        return request_post(args, arg_count, ctx);
    }
    
    fprintf(stderr, "Error: Unknown request function '%s'\n", name);
    return create_string_value("");
}

// Math module functions
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

Value math_sigmoid(ASTNode** args, int arg_count, InterpreterContext* ctx) {
    if (arg_count != 1) {
        fprintf(stderr, "Error: sigmoid() requires exactly one argument\n");
        return create_number_value(0);
    }
    
    Value val = execute_node(args[0], ctx);
    if (val.type != VALUE_NUMBER) {
        fprintf(stderr, "Error: sigmoid() argument must be a number\n");
        free_value(val);
        return create_number_value(0);
    }
    
    double result = 1.0 / (1.0 + exp(-val.number));
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
        fprintf(stderr, "Error: sqrt() of negative number\n");
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
    Value exp_val = execute_node(args[1], ctx);
    
    if (base.type != VALUE_NUMBER || exp_val.type != VALUE_NUMBER) {
        fprintf(stderr, "Error: pow() arguments must be numbers\n");
        free_value(base);
        free_value(exp_val);
        return create_number_value(0);
    }
    
    double result = pow(base.number, exp_val.number);
    free_value(base);
    free_value(exp_val);
    return create_number_value(result);
}

bool is_math_function(const char* name) {
    return strcmp(name, "sin") == 0 || strcmp(name, "cos") == 0 || 
           strcmp(name, "tan") == 0 || strcmp(name, "sigmoid") == 0 ||
           strcmp(name, "sqrt") == 0 || strcmp(name, "pow") == 0;
}

Value call_math_function(const char* name, ASTNode** args, int arg_count, InterpreterContext* ctx) {
    if (strcmp(name, "sin") == 0) {
        return math_sin(args, arg_count, ctx);
    } else if (strcmp(name, "cos") == 0) {
        return math_cos(args, arg_count, ctx);
    } else if (strcmp(name, "tan") == 0) {
        return math_tan(args, arg_count, ctx);
    } else if (strcmp(name, "sigmoid") == 0) {
        return math_sigmoid(args, arg_count, ctx);
    } else if (strcmp(name, "sqrt") == 0) {
        return math_sqrt(args, arg_count, ctx);
    } else if (strcmp(name, "pow") == 0) {
        return math_pow(args, arg_count, ctx);
    }
    
    fprintf(stderr, "Error: Unknown math function '%s'\n", name);
    return create_number_value(0);
}