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

// Built-in function names
static const char* builtin_functions[] = {
    "show.txt",
    "scanf",
    "fget",
    "main",
    NULL
};

void init_stdlib_functions(InterpreterContext* ctx) {
    // Standard library functions are called directly
    // No need to register them in the context
    printf("Standard library functions initialized\n");
}

bool is_builtin_function(const char* name) {
    for (int i = 0; builtin_functions[i]; i++) {
        if (strcmp(name, builtin_functions[i]) == 0) {
            return true;
        }
    }

    // Check for dmo graphics functions
    return is_dmo_graphics_function(name);
}

bool is_dmo_graphics_function(const char* name) {
    return strstr(name, "dmo.gr.") != NULL;
}

Value call_builtin_function(const char* name, ASTNode** args, int arg_count, InterpreterContext* ctx) {
    // Handle show.txt function
    if (strcmp(name, "show.txt") == 0) {
        return builtin_show_txt(args, arg_count, ctx);
    }

    // Handle scanf function
    if (strcmp(name, "scanf") == 0) {
        return builtin_scanf(args, arg_count, ctx);
    }

    // Handle fget function
    if (strcmp(name, "fget") == 0) {
        return builtin_fget(args, arg_count, ctx);
    }

    // Handle dmo graphics functions
    if (is_dmo_graphics_function(name)) {
        return call_dmo_graphics_function(name, args, arg_count, ctx);
    }

    // Function not found
    return create_void_value();
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
