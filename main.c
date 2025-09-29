/*
 * Diamond Programming Language Compiler/Interpreter
 * Main entry point for the Diamond language compiler
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lexer.h"
#include "parser.h"
#include "interpreter.h"
#include "modules.h"

void print_usage(const char* program_name) {
    printf("Usage: %s <source_file.dmo>\n", program_name);
    printf("Diamond Programming Language Compiler/Interpreter\n");
    printf("Supports C#-like syntax with built-in graphics library\n");
}

char* read_file(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        fprintf(stderr, "Error: Cannot open file '%s'\n", filename);
        return NULL;
    }
    
    // Get file size
    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    fseek(file, 0, SEEK_SET);
    
    // Allocate buffer and read file
    char* buffer = malloc(size + 1);
    if (!buffer) {
        fprintf(stderr, "Error: Memory allocation failed\n");
        fclose(file);
        return NULL;
    }
    
    fread(buffer, 1, size, file);
    buffer[size] = '\0';
    fclose(file);
    
    return buffer;
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        print_usage(argv[0]);
        return 1;
    }
    
    const char* source_file = argv[1];
    
    // Check file extension
    const char* ext = strrchr(source_file, '.');
    if (!ext || strcmp(ext, ".dmo") != 0) {
        fprintf(stderr, "Error: File must have .dmo extension\n");
        return 1;
    }
    
    // Read source file
    char* source_code = read_file(source_file);
    if (!source_code) {
        return 1;
    }
    
    printf("Diamond Compiler - Compiling '%s'\n", source_file);
    
    // Initialize module system
    init_module_system();
    
    // Lexical analysis
    printf("Phase 1: Lexical Analysis...\n");
    TokenList* tokens = tokenize(source_code);
    if (!tokens) {
        fprintf(stderr, "Lexical analysis failed\n");
        free(source_code);
        return 1;
    }
    
    printf("Tokens generated: %d\n", tokens->count);
    
    // Parsing
    printf("Phase 2: Parsing...\n");
    ASTNode* ast = parse(tokens);
    if (!ast) {
        fprintf(stderr, "Parsing failed\n");
        free_token_list(tokens);
        free(source_code);
        return 1;
    }
    
    printf("AST generated successfully\n");
    
    // Interpretation/Execution
    printf("Phase 3: Execution...\n");
    int result = interpret(ast, source_file);
    
    // Cleanup
    free_ast(ast);
    free_token_list(tokens);
    free(source_code);
    cleanup_module_system();
    
    if (result == 0) {
        printf("Program executed successfully\n");
    } else {
        printf("Program execution failed with code %d\n", result);
    }
    
    return result;
}
