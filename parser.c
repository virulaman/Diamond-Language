/*
 * DMO Language Parser Implementation
 * Converts tokens into Abstract Syntax Tree
 */

#define _POSIX_C_SOURCE 200809L
#include "parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Parser* create_parser(TokenList* tokens) {
    Parser* parser = malloc(sizeof(Parser));
    parser->tokens = tokens;
    parser->current = 0;
    parser->has_error = false;
    parser->error_message = NULL;
    return parser;
}

void free_parser(Parser* parser) {
    if (parser->error_message) {
        free(parser->error_message);
    }
    free(parser);
}

Token* current_token(Parser* parser) {
    if (parser->current >= parser->tokens->count) {
        return &parser->tokens->tokens[parser->tokens->count - 1]; // Return EOF token
    }
    return &parser->tokens->tokens[parser->current];
}

Token* peek_token(Parser* parser, int offset) {
    int index = parser->current + offset;
    if (index >= parser->tokens->count) {
        return &parser->tokens->tokens[parser->tokens->count - 1]; // Return EOF token
    }
    return &parser->tokens->tokens[index];
}

bool match_token(Parser* parser, TokenType type) {
    return current_token(parser)->type == type;
}

bool consume_token(Parser* parser, TokenType type, const char* error_msg) {
    if (match_token(parser, type)) {
        advance_token(parser);
        return true;
    }
    parser_error(parser, error_msg);
    return false;
}

void parser_error(Parser* parser, const char* message) {
    parser->has_error = true;
    Token* token = current_token(parser);
    
    char* full_message = malloc(256);
    snprintf(full_message, 256, "Parse error at line %d, column %d: %s", 
             token->line, token->column, message);
    
    if (parser->error_message) {
        free(parser->error_message);
    }
    parser->error_message = full_message;
    
    fprintf(stderr, "%s\n", full_message);
}

bool is_at_end(Parser* parser) {
    return current_token(parser)->type == TOKEN_EOF;
}

void advance_token(Parser* parser) {
    if (!is_at_end(parser)) {
        parser->current++;
    }
}

// Skip newline tokens
void skip_newlines(Parser* parser) {
    while (match_token(parser, TOKEN_NEWLINE)) {
        advance_token(parser);
    }
}

ASTNode* parse(TokenList* tokens) {
    Parser* parser = create_parser(tokens);
    ASTNode* ast = parse_program(parser);
    
    if (parser->has_error) {
        if (ast) {
            free_ast(ast);
        }
        ast = NULL;
    }
    
    free_parser(parser);
    return ast;
}

ASTNode* parse_program(Parser* parser) {
    ASTNode** statements = malloc(sizeof(ASTNode*) * 100);
    int count = 0;
    int capacity = 100;
    
    skip_newlines(parser);
    
    while (!is_at_end(parser) && !parser->has_error) {
        ASTNode* stmt = parse_statement(parser);
        if (stmt) {
            if (count >= capacity) {
                capacity *= 2;
                statements = realloc(statements, sizeof(ASTNode*) * capacity);
            }
            statements[count++] = stmt;
        }
        skip_newlines(parser);
    }
    
    if (parser->has_error) {
        // Cleanup on error
        for (int i = 0; i < count; i++) {
            free_ast(statements[i]);
        }
        free(statements);
        return NULL;
    }
    
    return create_program_node(statements, count);
}

ASTNode* parse_statement(Parser* parser) {
    skip_newlines(parser);
    
    if (match_token(parser, TOKEN_USE)) {
        return parse_use_statement(parser);
    }
    
    // Check for type keywords to identify function definitions or variable declarations
    Token* current = current_token(parser);
    Token* next = peek_token(parser, 1);
    Token* next_next = peek_token(parser, 2);
    
    if ((current->type == TOKEN_INT || current->type == TOKEN_STRING_TYPE || 
         current->type == TOKEN_CHAR || current->type == TOKEN_VOID) &&
        next->type == TOKEN_IDENTIFIER) {
        
        if (next_next->type == TOKEN_LPAREN) {
            return parse_function_definition(parser);
        } else {
            return parse_variable_declaration(parser);
        }
    }
    
    if (match_token(parser, TOKEN_IF)) {
        return parse_if_statement(parser);
    }
    
    if (match_token(parser, TOKEN_WHILE)) {
        return parse_while_statement(parser);
    }
    
    if (match_token(parser, TOKEN_FOR)) {
        return parse_for_statement(parser);
    }
    
    if (match_token(parser, TOKEN_RETURN)) {
        return parse_return_statement(parser);
    }
    
    if (match_token(parser, TOKEN_LBRACE)) {
        return parse_block(parser);
    }
    
    // Check for assignment
    if (current->type == TOKEN_IDENTIFIER && next->type == TOKEN_ASSIGN) {
        return parse_assignment(parser);
    }
    
    // Expression statement
    return parse_expression_statement(parser);
}

ASTNode* parse_use_statement(Parser* parser) {
    if (!consume_token(parser, TOKEN_USE, "Expected 'use' keyword")) {
        return NULL;
    }
    
    if (!match_token(parser, TOKEN_IDENTIFIER)) {
        parser_error(parser, "Expected module name after 'use'");
        return NULL;
    }
    
    char* module_name = strdup(current_token(parser)->value);
    advance_token(parser);
    
    if (!consume_token(parser, TOKEN_SEMICOLON, "Expected ';' after use statement")) {
        free(module_name);
        return NULL;
    }
    
    ASTNode* node = create_ast_node(AST_USE_STATEMENT, 0, 0);
    node->use_stmt.module_name = module_name;
    return node;
}

ASTNode* parse_function_definition(Parser* parser) {
    // Parse return type
    if (!match_token(parser, TOKEN_INT) && !match_token(parser, TOKEN_STRING_TYPE) && 
        !match_token(parser, TOKEN_CHAR) && !match_token(parser, TOKEN_VOID)) {
        parser_error(parser, "Expected return type");
        return NULL;
    }
    
    char* return_type = strdup(current_token(parser)->value);
    advance_token(parser);
    
    // Parse function name
    if (!match_token(parser, TOKEN_IDENTIFIER)) {
        parser_error(parser, "Expected function name");
        free(return_type);
        return NULL;
    }
    
    char* func_name = strdup(current_token(parser)->value);
    advance_token(parser);
    
    if (!consume_token(parser, TOKEN_LPAREN, "Expected '(' after function name")) {
        free(return_type);
        free(func_name);
        return NULL;
    }
    
    // Parse parameters
    ASTNode** parameters = NULL;
    int param_count = 0;
    
    if (!match_token(parser, TOKEN_RPAREN)) {
        parameters = malloc(sizeof(ASTNode*) * 10);
        int capacity = 10;
        
        do {
            if (param_count >= capacity) {
                capacity *= 2;
                parameters = realloc(parameters, sizeof(ASTNode*) * capacity);
            }
            
            // Parse parameter: type identifier (without semicolon)
            if (!match_token(parser, TOKEN_INT) && !match_token(parser, TOKEN_STRING_TYPE) && 
                !match_token(parser, TOKEN_CHAR)) {
                parser_error(parser, "Expected parameter type");
                for (int i = 0; i < param_count; i++) {
                    free_ast(parameters[i]);
                }
                free(parameters);
                free(return_type);
                free(func_name);
                return NULL;
            }
            
            char* param_type = strdup(current_token(parser)->value);
            advance_token(parser);
            
            if (!match_token(parser, TOKEN_IDENTIFIER)) {
                parser_error(parser, "Expected parameter name");
                free(param_type);
                for (int i = 0; i < param_count; i++) {
                    free_ast(parameters[i]);
                }
                free(parameters);
                free(return_type);
                free(func_name);
                return NULL;
            }
            
            char* param_name = strdup(current_token(parser)->value);
            advance_token(parser);
            
            ASTNode* param = create_variable_decl_node(param_type, param_name, NULL);
            if (!param) {
                // Cleanup on error
                for (int i = 0; i < param_count; i++) {
                    free_ast(parameters[i]);
                }
                free(parameters);
                free(return_type);
                free(func_name);
                return NULL;
            }
            
            parameters[param_count++] = param;
            
            if (match_token(parser, TOKEN_COMMA)) {
                advance_token(parser);
            } else {
                break;
            }
        } while (!match_token(parser, TOKEN_RPAREN));
    }
    
    if (!consume_token(parser, TOKEN_RPAREN, "Expected ')' after parameters")) {
        if (parameters) {
            for (int i = 0; i < param_count; i++) {
                free_ast(parameters[i]);
            }
            free(parameters);
        }
        free(return_type);
        free(func_name);
        return NULL;
    }
    
    // Parse function body
    ASTNode* body = parse_block(parser);
    if (!body) {
        if (parameters) {
            for (int i = 0; i < param_count; i++) {
                free_ast(parameters[i]);
            }
            free(parameters);
        }
        free(return_type);
        free(func_name);
        return NULL;
    }
    
    return create_function_def_node(return_type, func_name, parameters, param_count, body);
}

ASTNode* parse_variable_declaration(Parser* parser) {
    // Parse type
    if (!match_token(parser, TOKEN_INT) && !match_token(parser, TOKEN_STRING_TYPE) && 
        !match_token(parser, TOKEN_CHAR)) {
        parser_error(parser, "Expected variable type");
        return NULL;
    }
    
    char* var_type = strdup(current_token(parser)->value);
    advance_token(parser);
    
    // Parse variable name
    if (!match_token(parser, TOKEN_IDENTIFIER)) {
        parser_error(parser, "Expected variable name");
        free(var_type);
        return NULL;
    }
    
    char* var_name = strdup(current_token(parser)->value);
    advance_token(parser);
    
    // Parse optional initializer
    ASTNode* initializer = NULL;
    if (match_token(parser, TOKEN_ASSIGN)) {
        advance_token(parser);
        initializer = parse_expression(parser);
        if (!initializer) {
            free(var_type);
            free(var_name);
            return NULL;
        }
    }
    
    if (!consume_token(parser, TOKEN_SEMICOLON, "Expected ';' after variable declaration")) {
        if (initializer) {
            free_ast(initializer);
        }
        free(var_type);
        free(var_name);
        return NULL;
    }
    
    return create_variable_decl_node(var_type, var_name, initializer);
}

ASTNode* parse_assignment(Parser* parser) {
    ASTNode* target = parse_primary(parser);
    if (!target) {
        return NULL;
    }
    
    if (!consume_token(parser, TOKEN_ASSIGN, "Expected '=' in assignment")) {
        free_ast(target);
        return NULL;
    }
    
    ASTNode* value = parse_expression(parser);
    if (!value) {
        free_ast(target);
        return NULL;
    }
    
    if (!consume_token(parser, TOKEN_SEMICOLON, "Expected ';' after assignment")) {
        free_ast(target);
        free_ast(value);
        return NULL;
    }
    
    return create_assignment_node(target, value);
}

ASTNode* parse_expression_statement(Parser* parser) {
    ASTNode* expr = parse_expression(parser);
    if (!expr) {
        return NULL;
    }
    
    if (!consume_token(parser, TOKEN_SEMICOLON, "Expected ';' after expression")) {
        free_ast(expr);
        return NULL;
    }
    
    return expr;
}

ASTNode* parse_block(Parser* parser) {
    if (!consume_token(parser, TOKEN_LBRACE, "Expected '{'")) {
        return NULL;
    }
    
    ASTNode** statements = malloc(sizeof(ASTNode*) * 50);
    int count = 0;
    int capacity = 50;
    
    skip_newlines(parser);
    
    while (!match_token(parser, TOKEN_RBRACE) && !is_at_end(parser) && !parser->has_error) {
        ASTNode* stmt = parse_statement(parser);
        if (stmt) {
            if (count >= capacity) {
                capacity *= 2;
                statements = realloc(statements, sizeof(ASTNode*) * capacity);
            }
            statements[count++] = stmt;
        }
        skip_newlines(parser);
    }
    
    if (!consume_token(parser, TOKEN_RBRACE, "Expected '}'")) {
        for (int i = 0; i < count; i++) {
            free_ast(statements[i]);
        }
        free(statements);
        return NULL;
    }
    
    ASTNode* block = create_ast_node(AST_BLOCK, 0, 0);
    block->block.statements = statements;
    block->block.statement_count = count;
    return block;
}

ASTNode* parse_expression(Parser* parser) {
    return parse_logical_or(parser);
}

ASTNode* parse_logical_or(Parser* parser) {
    ASTNode* expr = parse_logical_and(parser);
    
    while (match_token(parser, TOKEN_OR)) {
        TokenType operator = current_token(parser)->type;
        advance_token(parser);
        ASTNode* right = parse_logical_and(parser);
        expr = create_binary_op_node(operator, expr, right);
    }
    
    return expr;
}

ASTNode* parse_logical_and(Parser* parser) {
    ASTNode* expr = parse_equality(parser);
    
    while (match_token(parser, TOKEN_AND)) {
        TokenType operator = current_token(parser)->type;
        advance_token(parser);
        ASTNode* right = parse_equality(parser);
        expr = create_binary_op_node(operator, expr, right);
    }
    
    return expr;
}

ASTNode* parse_equality(Parser* parser) {
    ASTNode* expr = parse_comparison(parser);
    
    while (match_token(parser, TOKEN_EQUAL) || match_token(parser, TOKEN_NOT_EQUAL)) {
        TokenType operator = current_token(parser)->type;
        advance_token(parser);
        ASTNode* right = parse_comparison(parser);
        expr = create_binary_op_node(operator, expr, right);
    }
    
    return expr;
}

ASTNode* parse_comparison(Parser* parser) {
    ASTNode* expr = parse_addition(parser);
    
    while (match_token(parser, TOKEN_GREATER) || match_token(parser, TOKEN_GREATER_EQUAL) ||
           match_token(parser, TOKEN_LESS) || match_token(parser, TOKEN_LESS_EQUAL)) {
        TokenType operator = current_token(parser)->type;
        advance_token(parser);
        ASTNode* right = parse_addition(parser);
        expr = create_binary_op_node(operator, expr, right);
    }
    
    return expr;
}

ASTNode* parse_addition(Parser* parser) {
    ASTNode* expr = parse_multiplication(parser);
    
    while (match_token(parser, TOKEN_PLUS) || match_token(parser, TOKEN_MINUS)) {
        TokenType operator = current_token(parser)->type;
        advance_token(parser);
        ASTNode* right = parse_multiplication(parser);
        expr = create_binary_op_node(operator, expr, right);
    }
    
    return expr;
}

ASTNode* parse_multiplication(Parser* parser) {
    ASTNode* expr = parse_unary(parser);
    
    while (match_token(parser, TOKEN_MULTIPLY) || match_token(parser, TOKEN_DIVIDE) || match_token(parser, TOKEN_MODULO)) {
        TokenType operator = current_token(parser)->type;
        advance_token(parser);
        ASTNode* right = parse_unary(parser);
        expr = create_binary_op_node(operator, expr, right);
    }
    
    return expr;
}

ASTNode* parse_unary(Parser* parser) {
    if (match_token(parser, TOKEN_NOT) || match_token(parser, TOKEN_MINUS) ||
        match_token(parser, TOKEN_INCREMENT) || match_token(parser, TOKEN_DECREMENT)) {
        TokenType operator = current_token(parser)->type;
        advance_token(parser);
        ASTNode* operand = parse_unary(parser);
        
        ASTNode* unary = create_ast_node(AST_UNARY_OP, 0, 0);
        unary->unary_op.operator = operator;
        unary->unary_op.operand = operand;
        unary->unary_op.is_postfix = false;
        return unary;
    }
    
    return parse_primary(parser);
}

ASTNode* parse_primary(Parser* parser) {
    ASTNode* expr = NULL;

    if (match_token(parser, TOKEN_NUMBER)) {
        double value = atof(current_token(parser)->value);
        advance_token(parser);
        expr = create_number_node(value);
    } else if (match_token(parser, TOKEN_STRING)) {
        char* value = strdup(current_token(parser)->value);
        advance_token(parser);
        expr = create_string_node(value);
    } else if (match_token(parser, TOKEN_IDENTIFIER)) {
        char* name = strdup(current_token(parser)->value);
        advance_token(parser);
        
        if (match_token(parser, TOKEN_LPAREN)) {
            expr = parse_function_call(parser, name);
        } else {
            ASTNode* node = create_identifier_node(name);
            while (match_token(parser, TOKEN_DOT)) {
                advance_token(parser); // consume '.'
                if (!match_token(parser, TOKEN_IDENTIFIER)) {
                    parser_error(parser, "Expected identifier after '.'");
                    free_ast(node);
                    return NULL;
                }
                char* member = strdup(current_token(parser)->value);
                advance_token(parser);
                node = create_member_access_node(node, member);
            }
            
            if (match_token(parser, TOKEN_LPAREN)) {
                char* func_name = malloc(256);
                func_name[0] = '\0';
                
                if (node->type == AST_MEMBER_ACCESS) {
                    ASTNode* current = node;
                    char* parts[10];
                    int part_count = 0;

                    while (current->type == AST_MEMBER_ACCESS && part_count < 9) {
                        parts[part_count++] = current->member_access.member;
                        current = current->member_access.object;
                    }

                    if (current->type == AST_IDENTIFIER && part_count < 9) {
                        parts[part_count++] = current->identifier.value;
                    }

                    for (int i = part_count - 1; i >= 0; i--) {
                        strcat(func_name, parts[i]);
                        if (i > 0) {
                            strcat(func_name, ".");
                        }
                    }
                } else if (node->type == AST_IDENTIFIER) {
                    strcpy(func_name, node->identifier.value);
                }

                free_ast(node);
                expr = parse_function_call(parser, func_name);
            } else {
                expr = node;
            }
        }
    } else if (match_token(parser, TOKEN_LPAREN)) {
        advance_token(parser);
        expr = parse_expression(parser);
        if (!consume_token(parser, TOKEN_RPAREN, "Expected ')' after expression")) {
            free_ast(expr);
            return NULL;
        }
    } else {
        parser_error(parser, "Expected expression");
        return NULL;
    }

    if (parser->has_error) {
        if (expr) free_ast(expr);
        return NULL;
    }

    // Now handle postfix increment/decrement
    while (match_token(parser, TOKEN_INCREMENT) || match_token(parser, TOKEN_DECREMENT)) {
        TokenType op_type = current_token(parser)->type;
        advance_token(parser);

        ASTNode* unary_node = create_ast_node(AST_UNARY_OP, 0, 0);
        unary_node->unary_op.operand = expr;
        unary_node->unary_op.operator = op_type;
        unary_node->unary_op.is_postfix = true;
        expr = unary_node;
    }

    return expr;
}

ASTNode* parse_function_call(Parser* parser, char* name) {
    if (!consume_token(parser, TOKEN_LPAREN, "Expected '(' in function call")) {
        free(name);
        return NULL;
    }
    
    ASTNode** arguments = NULL;
    int arg_count = 0;
    
    if (!match_token(parser, TOKEN_RPAREN)) {
        arguments = malloc(sizeof(ASTNode*) * 10);
        int capacity = 10;
        
        do {
            if (arg_count >= capacity) {
                capacity *= 2;
                arguments = realloc(arguments, sizeof(ASTNode*) * capacity);
            }
            
            ASTNode* arg = parse_expression(parser);
            if (!arg) {
                for (int i = 0; i < arg_count; i++) {
                    free_ast(arguments[i]);
                }
                free(arguments);
                free(name);
                return NULL;
            }
            
            arguments[arg_count++] = arg;
            
            if (match_token(parser, TOKEN_COMMA)) {
                advance_token(parser);
            } else {
                break;
            }
        } while (!match_token(parser, TOKEN_RPAREN));
    }
    
    if (!consume_token(parser, TOKEN_RPAREN, "Expected ')' after arguments")) {
        if (arguments) {
            for (int i = 0; i < arg_count; i++) {
                free_ast(arguments[i]);
            }
            free(arguments);
        }
        free(name);
        return NULL;
    }
    
    return create_function_call_node(name, arguments, arg_count);
}

// Additional parsing functions for if, while, for, return statements would go here
// For brevity, I'll implement simplified versions

ASTNode* parse_if_statement(Parser* parser) {
    advance_token(parser); // consume 'if'
    
    if (!consume_token(parser, TOKEN_LPAREN, "Expected '(' after 'if'")) {
        return NULL;
    }
    
    ASTNode* condition = parse_expression(parser);
    if (!condition) {
        return NULL;
    }
    
    if (!consume_token(parser, TOKEN_RPAREN, "Expected ')' after if condition")) {
        free_ast(condition);
        return NULL;
    }
    
    ASTNode* then_stmt = parse_statement(parser);
    if (!then_stmt) {
        free_ast(condition);
        return NULL;
    }
    
    ASTNode* else_stmt = NULL;
    if (match_token(parser, TOKEN_ELSE)) {
        advance_token(parser); // Consume 'else'
        if (match_token(parser, TOKEN_IF)) {
            // This is an 'else if' construct.
            else_stmt = parse_if_statement(parser);
        } else {
            // This is a regular 'else' block.
            else_stmt = parse_statement(parser);
        }

        if (!else_stmt) {
            free_ast(condition);
            free_ast(then_stmt);
            return NULL;
        }
    }
    
    ASTNode* if_node = create_ast_node(AST_IF_STATEMENT, 0, 0);
    if_node->if_stmt.condition = condition;
    if_node->if_stmt.then_stmt = then_stmt;
    if_node->if_stmt.else_stmt = else_stmt;
    return if_node;
}

ASTNode* parse_while_statement(Parser* parser) {
    advance_token(parser); // consume 'while'
    
    if (!consume_token(parser, TOKEN_LPAREN, "Expected '(' after 'while'")) {
        return NULL;
    }
    
    ASTNode* condition = parse_expression(parser);
    if (!condition) {
        return NULL;
    }
    
    if (!consume_token(parser, TOKEN_RPAREN, "Expected ')' after while condition")) {
        free_ast(condition);
        return NULL;
    }
    
    ASTNode* body = parse_statement(parser);
    if (!body) {
        free_ast(condition);
        return NULL;
    }
    
    ASTNode* while_node = create_ast_node(AST_WHILE_LOOP, 0, 0);
    while_node->while_loop.condition = condition;
    while_node->while_loop.body = body;
    return while_node;
}

ASTNode* parse_for_statement(Parser* parser) {
    // Simplified for loop parsing
    advance_token(parser); // consume 'for'
    
    if (!consume_token(parser, TOKEN_LPAREN, "Expected '(' after 'for'")) {
        return NULL;
    }
    
    // Parse initialization
    ASTNode* init = parse_statement(parser);
    
    // Parse condition
    ASTNode* condition = parse_expression(parser);
    if (!consume_token(parser, TOKEN_SEMICOLON, "Expected ';' after for condition")) {
        free_ast(init);
        free_ast(condition);
        return NULL;
    }
    
    // Parse increment
    ASTNode* increment = parse_expression(parser);
    
    if (!consume_token(parser, TOKEN_RPAREN, "Expected ')' after for increment")) {
        free_ast(init);
        free_ast(condition);
        free_ast(increment);
        return NULL;
    }
    
    ASTNode* body = parse_statement(parser);
    if (!body) {
        free_ast(init);
        free_ast(condition);
        free_ast(increment);
        return NULL;
    }
    
    ASTNode* for_node = create_ast_node(AST_FOR_LOOP, 0, 0);
    for_node->for_loop.init = init;
    for_node->for_loop.condition = condition;
    for_node->for_loop.increment = increment;
    for_node->for_loop.body = body;
    return for_node;
}

ASTNode* parse_return_statement(Parser* parser) {
    advance_token(parser); // consume 'return'
    
    ASTNode* value = NULL;
    if (!match_token(parser, TOKEN_SEMICOLON)) {
        value = parse_expression(parser);
        if (!value) {
            return NULL;
        }
    }
    
    if (!consume_token(parser, TOKEN_SEMICOLON, "Expected ';' after return statement")) {
        if (value) {
            free_ast(value);
        }
        return NULL;
    }
    
    ASTNode* return_node = create_ast_node(AST_RETURN_STATEMENT, 0, 0);
    return_node->return_stmt.value = value;
    return return_node;
}
