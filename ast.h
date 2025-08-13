/*
 * DMO Language Abstract Syntax Tree Definitions
 * Defines AST node types and structures
 */

#ifndef AST_H
#define AST_H

#include "lexer.h"

// AST Node types
typedef enum {
    AST_PROGRAM,
    AST_USE_STATEMENT,
    AST_FUNCTION_DEF,
    AST_VARIABLE_DECL,
    AST_ASSIGNMENT,
    AST_FUNCTION_CALL,
    AST_IF_STATEMENT,
    AST_WHILE_LOOP,
    AST_FOR_LOOP,
    AST_RETURN_STATEMENT,
    AST_BLOCK,
    AST_EXPRESSION,
    AST_BINARY_OP,
    AST_UNARY_OP,
    AST_IDENTIFIER,
    AST_NUMBER,
    AST_STRING,
    AST_ARRAY_ACCESS,
    AST_MEMBER_ACCESS
} ASTNodeType;

// Forward declaration
typedef struct ASTNode ASTNode;

// AST Node structure
struct ASTNode {
    ASTNodeType type;
    int line;
    int column;
    
    union {
        // Program node
        struct {
            ASTNode** statements;
            int statement_count;
        } program;
        
        // Use statement
        struct {
            char* module_name;
        } use_stmt;
        
        // Function definition
        struct {
            char* return_type;
            char* name;
            ASTNode** parameters;
            int param_count;
            ASTNode* body;
        } func_def;
        
        // Variable declaration
        struct {
            char* type;
            char* name;
            ASTNode* initializer;
        } var_decl;
        
        // Assignment
        struct {
            ASTNode* target;
            ASTNode* value;
        } assignment;
        
        // Function call
        struct {
            char* name;
            ASTNode** arguments;
            int arg_count;
        } func_call;
        
        // If statement
        struct {
            ASTNode* condition;
            ASTNode* then_stmt;
            ASTNode* else_stmt;
        } if_stmt;
        
        // While loop
        struct {
            ASTNode* condition;
            ASTNode* body;
        } while_loop;
        
        // For loop
        struct {
            ASTNode* init;
            ASTNode* condition;
            ASTNode* increment;
            ASTNode* body;
        } for_loop;
        
        // Return statement
        struct {
            ASTNode* value;
        } return_stmt;
        
        // Block statement
        struct {
            ASTNode** statements;
            int statement_count;
        } block;
        
        // Binary operation
        struct {
            TokenType operator;
            ASTNode* left;
            ASTNode* right;
        } binary_op;
        
        // Unary operation
        struct {
            TokenType operator;
            ASTNode* operand;
        } unary_op;
        
        // Literals
        struct {
            char* value;
        } identifier;
        
        struct {
            double value;
        } number;
        
        struct {
            char* value;
        } string;
        
        // Array access
        struct {
            ASTNode* array;
            ASTNode* index;
        } array_access;
        
        // Member access (for dmo.gr.* calls)
        struct {
            ASTNode* object;
            char* member;
        } member_access;
    };
};

// Function prototypes
ASTNode* create_ast_node(ASTNodeType type, int line, int column);
void free_ast(ASTNode* node);
void print_ast(ASTNode* node, int depth);
ASTNode* create_program_node(ASTNode** statements, int count);
ASTNode* create_function_def_node(char* return_type, char* name, ASTNode** params, int param_count, ASTNode* body);
ASTNode* create_variable_decl_node(char* type, char* name, ASTNode* initializer);
ASTNode* create_assignment_node(ASTNode* target, ASTNode* value);
ASTNode* create_function_call_node(char* name, ASTNode** args, int arg_count);
ASTNode* create_binary_op_node(TokenType operator, ASTNode* left, ASTNode* right);
ASTNode* create_identifier_node(char* name);
ASTNode* create_number_node(double value);
ASTNode* create_string_node(char* value);
ASTNode* create_member_access_node(ASTNode* object, char* member);

#endif // AST_H
