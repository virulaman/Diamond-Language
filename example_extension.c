/*
 * Example Diamond Extension
 * Custom string manipulation functions
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../interpreter.h"

Value string_reverse(ASTNode** args, int arg_count, InterpreterContext* ctx) {
    if (arg_count != 1) {
        fprintf(stderr, "Error: reverse requires string\n");
        return create_string_value("");
    }
    
    Value input = execute_node(args[0], ctx);
    int len = strlen(input.string);
    char* reversed = malloc(len + 1);
    
    for (int i = 0; i < len; i++) {
        reversed[i] = input.string[len - 1 - i];
    }
    reversed[len] = '\0';
    
    printf("🔄 Reversed string: %s -> %s\n", input.string, reversed);
    
    Value result = create_string_value(reversed);
    free(reversed);
    free_value(input);
    
    return result;
}

Value string_uppercase(ASTNode** args, int arg_count, InterpreterContext* ctx) {
    if (arg_count != 1) {
        fprintf(stderr, "Error: uppercase requires string\n");
        return create_string_value("");
    }
    
    Value input = execute_node(args[0], ctx);
    int len = strlen(input.string);
    char* upper = malloc(len + 1);
    
    for (int i = 0; i < len; i++) {
        upper[i] = toupper(input.string[i]);
    }
    upper[len] = '\0';
    
    printf("📤 Uppercase: %s -> %s\n", input.string, upper);
    
    Value result = create_string_value(upper);
    free(upper);
    free_value(input);
    
    return result;
}

// Extension initialization
void init_stringutils_extension(InterpreterContext* ctx) {
    printf("String Utils Extension loaded\n");
    
    register_function("reverse", string_reverse, ctx);
    register_function("uppercase", string_uppercase, ctx);
}