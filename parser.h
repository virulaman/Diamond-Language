/*
 * DMO Language Parser Header
 * Converts tokens into Abstract Syntax Tree
 */

#ifndef PARSER_H
#define PARSER_H

#include "lexer.h"
#include "ast.h"

// Parser state
typedef struct {
    TokenList* tokens;
    int current;
    bool has_error;
    char* error_message;
} Parser;

// Function prototypes
ASTNode* parse(TokenList* tokens);
Parser* create_parser(TokenList* tokens);
void free_parser(Parser* parser);

// Parsing functions
ASTNode* parse_program(Parser* parser);
ASTNode* parse_statement(Parser* parser);
ASTNode* parse_use_statement(Parser* parser);
ASTNode* parse_function_definition(Parser* parser);
ASTNode* parse_variable_declaration(Parser* parser);
ASTNode* parse_assignment(Parser* parser);
ASTNode* parse_expression_statement(Parser* parser);
ASTNode* parse_if_statement(Parser* parser);
ASTNode* parse_while_statement(Parser* parser);
ASTNode* parse_for_statement(Parser* parser);
ASTNode* parse_return_statement(Parser* parser);
ASTNode* parse_block(Parser* parser);

// Expression parsing
ASTNode* parse_expression(Parser* parser);
ASTNode* parse_logical_or(Parser* parser);
ASTNode* parse_logical_and(Parser* parser);
ASTNode* parse_equality(Parser* parser);
ASTNode* parse_comparison(Parser* parser);
ASTNode* parse_addition(Parser* parser);
ASTNode* parse_multiplication(Parser* parser);
ASTNode* parse_unary(Parser* parser);
ASTNode* parse_primary(Parser* parser);
ASTNode* parse_function_call(Parser* parser, char* name);
ASTNode* parse_member_access(Parser* parser, ASTNode* object);

// Utility functions
Token* current_token(Parser* parser);
Token* peek_token(Parser* parser, int offset);
bool match_token(Parser* parser, TokenType type);
bool consume_token(Parser* parser, TokenType type, const char* error_msg);
void parser_error(Parser* parser, const char* message);
bool is_at_end(Parser* parser);
void advance_token(Parser* parser);

#endif // PARSER_H
