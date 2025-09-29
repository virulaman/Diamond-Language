/*
 * DMO Language Standard Library Functions Implementation
 * Built-in functions like show.txt, scanf, fget
 */

#define _POSIX_C_SOURCE 200809L
#include "stdlib_funcs.h"
#include "dmo_graphs.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

// Forward declarations for built-in functions
Value builtin_show_txt(ASTNode** args, int arg_count, InterpreterContext* ctx);
Value builtin_scanf(ASTNode** args, int arg_count, InterpreterContext* ctx);
Value builtin_fget(ASTNode** args, int arg_count, InterpreterContext* ctx);

void init_stdlib_functions(InterpreterContext* ctx) {
    register_native_function(ctx, "show.txt", builtin_show_txt);
    register_native_function(ctx, "scanf", builtin_scanf);
    register_native_function(ctx, "fget", builtin_fget);
}

Value builtin_show_txt(ASTNode** args, int arg_count, InterpreterContext* ctx) {
    if (arg_count == 0) {
        printf("\n");
        return create_void_value();
    }

    for (int i = 0; i < arg_count; i++) {
        Value arg = execute_node(args[i], ctx);

        switch (arg.type) {
            case VALUE_NUMBER:
                printf("%.6g", arg.number);
                break;
            case VALUE_STRING:
                printf("%s", arg.string);
                break;
            case VALUE_VOID:
                printf("void");
                break;
        }

        if (i < arg_count - 1) {
            printf(" ");
        }

        free_value(arg);
    }

    printf("\n");
    return create_void_value();
}

Value builtin_scanf(ASTNode** args, int arg_count, InterpreterContext* ctx) {
    if (arg_count == 0) {
        fprintf(stderr, "Error: scanf requires at least one argument\n");
        return create_string_value("");
    }

    // Get format string
    Value format = execute_node(args[0], ctx);
    if (format.type != VALUE_STRING) {
        fprintf(stderr, "Error: scanf format must be a string\n");
        free_value(format);
        return create_string_value("");
    }

    char buffer[1024];

    // Simple implementation - just read a line
    if (strcmp(format.string, "%s") == 0 || strcmp(format.string, "%d") == 0) {
        printf("Enter input: ");
        fflush(stdout);

        if (fgets(buffer, sizeof(buffer), stdin)) {
            // Remove newline
            char* newline = strchr(buffer, '\n');
            if (newline) {
                *newline = '\0';
            }

            // If format is %d, try to convert to number
            if (strcmp(format.string, "%d") == 0) {
                double num = atof(buffer);
                free_value(format);
                return create_number_value(num);
            } else {
                free_value(format);
                return create_string_value(buffer);
            }
        }
    }

    free_value(format);
    return create_string_value("");
}

Value builtin_fget(ASTNode** args, int arg_count, InterpreterContext* ctx) {
    if (arg_count == 0) {
        fprintf(stderr, "Error: fget requires a filename argument\n");
        return create_string_value("");
    }

    Value filename_val = execute_node(args[0], ctx);
    if (filename_val.type != VALUE_STRING) {
        fprintf(stderr, "Error: fget filename must be a string\n");
        free_value(filename_val);
        return create_string_value("");
    }

    FILE* file = fopen(filename_val.string, "r");
    if (!file) {
        fprintf(stderr, "Error: Cannot open file '%s'\n", filename_val.string);
        free_value(filename_val);
        return create_string_value("");
    }

    // Read entire file
    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    fseek(file, 0, SEEK_SET);

    char* content = malloc(size + 1);
    fread(content, 1, size, file);
    content[size] = '\0';

    fclose(file);
    free_value(filename_val);

    Value result = create_string_value(content);
    free(content);

    return result;
}

Value builtin_main(ASTNode** args, int arg_count, InterpreterContext* ctx) {
    // Main function - just return 0 for success
    return create_number_value(0);
}
