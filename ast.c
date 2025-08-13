/*
 * DMO Language AST Implementation
 * Creates and manages Abstract Syntax Tree nodes
 */

#define _POSIX_C_SOURCE 200809L
#include "ast.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

ASTNode* create_ast_node(ASTNodeType type, int line, int column) {
    ASTNode* node = malloc(sizeof(ASTNode));
    memset(node, 0, sizeof(ASTNode));
    node->type = type;
    node->line = line;
    node->column = column;
    return node;
}

void free_ast(ASTNode* node) {
    if (!node) return;
    
    switch (node->type) {
        case AST_PROGRAM:
            for (int i = 0; i < node->program.statement_count; i++) {
                free_ast(node->program.statements[i]);
            }
            free(node->program.statements);
            break;
            
        case AST_USE_STATEMENT:
            free(node->use_stmt.module_name);
            break;
            
        case AST_FUNCTION_DEF:
            free(node->func_def.return_type);
            free(node->func_def.name);
            for (int i = 0; i < node->func_def.param_count; i++) {
                free_ast(node->func_def.parameters[i]);
            }
            free(node->func_def.parameters);
            free_ast(node->func_def.body);
            break;
            
        case AST_VARIABLE_DECL:
            free(node->var_decl.type);
            free(node->var_decl.name);
            free_ast(node->var_decl.initializer);
            break;
            
        case AST_ASSIGNMENT:
            free_ast(node->assignment.target);
            free_ast(node->assignment.value);
            break;
            
        case AST_FUNCTION_CALL:
            free(node->func_call.name);
            for (int i = 0; i < node->func_call.arg_count; i++) {
                free_ast(node->func_call.arguments[i]);
            }
            free(node->func_call.arguments);
            break;
            
        case AST_IF_STATEMENT:
            free_ast(node->if_stmt.condition);
            free_ast(node->if_stmt.then_stmt);
            free_ast(node->if_stmt.else_stmt);
            break;
            
        case AST_WHILE_LOOP:
            free_ast(node->while_loop.condition);
            free_ast(node->while_loop.body);
            break;
            
        case AST_FOR_LOOP:
            free_ast(node->for_loop.init);
            free_ast(node->for_loop.condition);
            free_ast(node->for_loop.increment);
            free_ast(node->for_loop.body);
            break;
            
        case AST_RETURN_STATEMENT:
            free_ast(node->return_stmt.value);
            break;
            
        case AST_BLOCK:
            for (int i = 0; i < node->block.statement_count; i++) {
                free_ast(node->block.statements[i]);
            }
            free(node->block.statements);
            break;
            
        case AST_BINARY_OP:
            free_ast(node->binary_op.left);
            free_ast(node->binary_op.right);
            break;
            
        case AST_UNARY_OP:
            free_ast(node->unary_op.operand);
            break;
            
        case AST_IDENTIFIER:
            free(node->identifier.value);
            break;
            
        case AST_STRING:
            free(node->string.value);
            break;
            
        case AST_ARRAY_ACCESS:
            free_ast(node->array_access.array);
            free_ast(node->array_access.index);
            break;
            
        case AST_MEMBER_ACCESS:
            free_ast(node->member_access.object);
            free(node->member_access.member);
            break;
            
        default:
            // Handle other node types
            break;
    }
    
    free(node);
}

ASTNode* create_program_node(ASTNode** statements, int count) {
    ASTNode* node = create_ast_node(AST_PROGRAM, 0, 0);
    node->program.statements = statements;
    node->program.statement_count = count;
    return node;
}

ASTNode* create_function_def_node(char* return_type, char* name, ASTNode** params, int param_count, ASTNode* body) {
    ASTNode* node = create_ast_node(AST_FUNCTION_DEF, 0, 0);
    node->func_def.return_type = return_type;
    node->func_def.name = name;
    node->func_def.parameters = params;
    node->func_def.param_count = param_count;
    node->func_def.body = body;
    return node;
}

ASTNode* create_variable_decl_node(char* type, char* name, ASTNode* initializer) {
    ASTNode* node = create_ast_node(AST_VARIABLE_DECL, 0, 0);
    node->var_decl.type = type;
    node->var_decl.name = name;
    node->var_decl.initializer = initializer;
    return node;
}

ASTNode* create_assignment_node(ASTNode* target, ASTNode* value) {
    ASTNode* node = create_ast_node(AST_ASSIGNMENT, 0, 0);
    node->assignment.target = target;
    node->assignment.value = value;
    return node;
}

ASTNode* create_function_call_node(char* name, ASTNode** args, int arg_count) {
    ASTNode* node = create_ast_node(AST_FUNCTION_CALL, 0, 0);
    node->func_call.name = name;
    node->func_call.arguments = args;
    node->func_call.arg_count = arg_count;
    return node;
}

ASTNode* create_binary_op_node(TokenType operator, ASTNode* left, ASTNode* right) {
    ASTNode* node = create_ast_node(AST_BINARY_OP, 0, 0);
    node->binary_op.operator = operator;
    node->binary_op.left = left;
    node->binary_op.right = right;
    return node;
}

ASTNode* create_identifier_node(char* name) {
    ASTNode* node = create_ast_node(AST_IDENTIFIER, 0, 0);
    node->identifier.value = name;
    return node;
}

ASTNode* create_number_node(double value) {
    ASTNode* node = create_ast_node(AST_NUMBER, 0, 0);
    node->number.value = value;
    return node;
}

ASTNode* create_string_node(char* value) {
    ASTNode* node = create_ast_node(AST_STRING, 0, 0);
    node->string.value = value;
    return node;
}

ASTNode* create_member_access_node(ASTNode* object, char* member) {
    ASTNode* node = create_ast_node(AST_MEMBER_ACCESS, 0, 0);
    node->member_access.object = object;
    node->member_access.member = member;
    return node;
}

void print_ast(ASTNode* node, int depth) {
    if (!node) return;
    
    // Print indentation
    for (int i = 0; i < depth; i++) {
        printf("  ");
    }
    
    switch (node->type) {
        case AST_PROGRAM:
            printf("PROGRAM\n");
            for (int i = 0; i < node->program.statement_count; i++) {
                print_ast(node->program.statements[i], depth + 1);
            }
            break;
            
        case AST_FUNCTION_DEF:
            printf("FUNCTION_DEF: %s %s\n", node->func_def.return_type, node->func_def.name);
            for (int i = 0; i < node->func_def.param_count; i++) {
                print_ast(node->func_def.parameters[i], depth + 1);
            }
            print_ast(node->func_def.body, depth + 1);
            break;
            
        case AST_VARIABLE_DECL:
            printf("VAR_DECL: %s %s\n", node->var_decl.type, node->var_decl.name);
            if (node->var_decl.initializer) {
                print_ast(node->var_decl.initializer, depth + 1);
            }
            break;
            
        case AST_FUNCTION_CALL:
            printf("FUNC_CALL: %s\n", node->func_call.name);
            for (int i = 0; i < node->func_call.arg_count; i++) {
                print_ast(node->func_call.arguments[i], depth + 1);
            }
            break;
            
        case AST_IDENTIFIER:
            printf("IDENTIFIER: %s\n", node->identifier.value);
            break;
            
        case AST_NUMBER:
            printf("NUMBER: %.6g\n", node->number.value);
            break;
            
        case AST_STRING:
            printf("STRING: \"%s\"\n", node->string.value);
            break;
            
        case AST_BINARY_OP:
            printf("BINARY_OP: %s\n", token_type_to_string(node->binary_op.operator));
            print_ast(node->binary_op.left, depth + 1);
            print_ast(node->binary_op.right, depth + 1);
            break;
            
        case AST_BLOCK:
            printf("BLOCK\n");
            for (int i = 0; i < node->block.statement_count; i++) {
                print_ast(node->block.statements[i], depth + 1);
            }
            break;
            
        default:
            printf("UNKNOWN_NODE: %d\n", node->type);
            break;
    }
}
