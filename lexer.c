/*
 * DMO Language Lexer Implementation
 * Converts source code into tokens
 */

#define _POSIX_C_SOURCE 200809L
#include "lexer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// Keywords mapping
typedef struct {
    const char* word;
    TokenType type;
} Keyword;

static const Keyword keywords[] = {
    {"use", TOKEN_USE},
    {"int", TOKEN_INT},
    {"string", TOKEN_STRING_TYPE},
    {"char", TOKEN_CHAR},
    {"if", TOKEN_IF},
    {"else", TOKEN_ELSE},
    {"while", TOKEN_WHILE},
    {"for", TOKEN_FOR},
    {"return", TOKEN_RETURN},
    {"void", TOKEN_VOID},
    {NULL, TOKEN_UNKNOWN}
};

bool is_keyword(const char* word) {
    for (int i = 0; keywords[i].word; i++) {
        if (strcmp(word, keywords[i].word) == 0) {
            return true;
        }
    }
    return false;
}

TokenType get_keyword_type(const char* word) {
    for (int i = 0; keywords[i].word; i++) {
        if (strcmp(word, keywords[i].word) == 0) {
            return keywords[i].type;
        }
    }
    return TOKEN_IDENTIFIER;
}

TokenList* create_token_list() {
    TokenList* list = malloc(sizeof(TokenList));
    list->tokens = malloc(sizeof(Token) * 100);
    list->count = 0;
    list->capacity = 100;
    return list;
}

void add_token(TokenList* list, TokenType type, const char* value, int line, int column) {
    if (list->count >= list->capacity) {
        list->capacity *= 2;
        list->tokens = realloc(list->tokens, sizeof(Token) * list->capacity);
    }
    
    Token* token = &list->tokens[list->count++];
    token->type = type;
    token->value = value ? strdup(value) : NULL;
    token->line = line;
    token->column = column;
}

char* read_string(const char* source, int* pos) {
    char quote = source[*pos];
    (*pos)++; // Skip opening quote
    
    int start = *pos;
    while (source[*pos] && source[*pos] != quote) {
        if (source[*pos] == '\\') {
            (*pos)++; // Skip escape character
        }
        (*pos)++;
    }
    
    if (!source[*pos]) {
        fprintf(stderr, "Error: Unterminated string literal\n");
        return NULL;
    }
    
    int length = *pos - start;
    char* str = malloc(length + 1);
    strncpy(str, &source[start], length);
    str[length] = '\0';
    
    (*pos)++; // Skip closing quote
    return str;
}

char* read_number(const char* source, int* pos) {
    int start = *pos;
    
    while (isdigit(source[*pos])) {
        (*pos)++;
    }
    
    // Handle decimal point
    if (source[*pos] == '.') {
        (*pos)++;
        while (isdigit(source[*pos])) {
            (*pos)++;
        }
    }
    
    int length = *pos - start;
    char* num = malloc(length + 1);
    strncpy(num, &source[start], length);
    num[length] = '\0';
    
    return num;
}

char* read_identifier(const char* source, int* pos) {
    int start = *pos;
    
    while (isalnum(source[*pos]) || source[*pos] == '_') {
        (*pos)++;
    }
    
    int length = *pos - start;
    char* id = malloc(length + 1);
    strncpy(id, &source[start], length);
    id[length] = '\0';
    
    return id;
}

TokenList* tokenize(const char* source) {
    TokenList* tokens = create_token_list();
    int pos = 0;
    int line = 1;
    int column = 1;
    
    while (source[pos]) {
        char current = source[pos];
        
        // Skip whitespace
        if (isspace(current)) {
            if (current == '\n') {
                add_token(tokens, TOKEN_NEWLINE, NULL, line, column);
                line++;
                column = 1;
            } else {
                column++;
            }
            pos++;
            continue;
        }
        
        // Skip comments
        if (current == '/' && source[pos + 1] == '/') {
            while (source[pos] && source[pos] != '\n') {
                pos++;
            }
            continue;
        }
        
        // String literals
        if (current == '"' || current == '\'') {
            char* str = read_string(source, &pos);
            if (str) {
                add_token(tokens, TOKEN_STRING, str, line, column);
                free(str);
            }
            column = pos;
            continue;
        }
        
        // Numbers
        if (isdigit(current)) {
            char* num = read_number(source, &pos);
            add_token(tokens, TOKEN_NUMBER, num, line, column);
            free(num);
            column = pos;
            continue;
        }
        
        // Identifiers and keywords
        if (isalpha(current) || current == '_') {
            char* id = read_identifier(source, &pos);
            TokenType type = is_keyword(id) ? get_keyword_type(id) : TOKEN_IDENTIFIER;
            add_token(tokens, type, id, line, column);
            free(id);
            column = pos;
            continue;
        }
        
        // Two-character operators
        if (current == '=' && source[pos + 1] == '=') {
            add_token(tokens, TOKEN_EQUAL, "==", line, column);
            pos += 2;
            column += 2;
            continue;
        }
        if (current == '!' && source[pos + 1] == '=') {
            add_token(tokens, TOKEN_NOT_EQUAL, "!=", line, column);
            pos += 2;
            column += 2;
            continue;
        }
        if (current == '<' && source[pos + 1] == '=') {
            add_token(tokens, TOKEN_LESS_EQUAL, "<=", line, column);
            pos += 2;
            column += 2;
            continue;
        }
        if (current == '>' && source[pos + 1] == '=') {
            add_token(tokens, TOKEN_GREATER_EQUAL, ">=", line, column);
            pos += 2;
            column += 2;
            continue;
        }
        if (current == '&' && source[pos + 1] == '&') {
            add_token(tokens, TOKEN_AND, "&&", line, column);
            pos += 2;
            column += 2;
            continue;
        }
        if (current == '|' && source[pos + 1] == '|') {
            add_token(tokens, TOKEN_OR, "||", line, column);
            pos += 2;
            column += 2;
            continue;
        }
        
        // Single-character tokens
        TokenType single_char_type = TOKEN_UNKNOWN;
        switch (current) {
            case '=': single_char_type = TOKEN_ASSIGN; break;
            case '+': single_char_type = TOKEN_PLUS; break;
            case '-': single_char_type = TOKEN_MINUS; break;
            case '*': single_char_type = TOKEN_MULTIPLY; break;
            case '/': single_char_type = TOKEN_DIVIDE; break;
            case '%': single_char_type = TOKEN_MODULO; break;
            case '<': single_char_type = TOKEN_LESS; break;
            case '>': single_char_type = TOKEN_GREATER; break;
            case '!': single_char_type = TOKEN_NOT; break;
            case ';': single_char_type = TOKEN_SEMICOLON; break;
            case ',': single_char_type = TOKEN_COMMA; break;
            case '.': single_char_type = TOKEN_DOT; break;
            case '(': single_char_type = TOKEN_LPAREN; break;
            case ')': single_char_type = TOKEN_RPAREN; break;
            case '{': single_char_type = TOKEN_LBRACE; break;
            case '}': single_char_type = TOKEN_RBRACE; break;
            case '[': single_char_type = TOKEN_LBRACKET; break;
            case ']': single_char_type = TOKEN_RBRACKET; break;
        }
        
        if (single_char_type != TOKEN_UNKNOWN) {
            char char_str[2] = {current, '\0'};
            add_token(tokens, single_char_type, char_str, line, column);
        } else {
            char char_str[2] = {current, '\0'};
            add_token(tokens, TOKEN_UNKNOWN, char_str, line, column);
            fprintf(stderr, "Warning: Unknown character '%c' at line %d, column %d\n", current, line, column);
        }
        
        pos++;
        column++;
    }
    
    add_token(tokens, TOKEN_EOF, NULL, line, column);
    return tokens;
}

void free_token_list(TokenList* list) {
    if (!list) return;
    
    for (int i = 0; i < list->count; i++) {
        if (list->tokens[i].value) {
            free(list->tokens[i].value);
        }
    }
    
    free(list->tokens);
    free(list);
}

const char* token_type_to_string(TokenType type) {
    switch (type) {
        case TOKEN_NUMBER: return "NUMBER";
        case TOKEN_STRING: return "STRING";
        case TOKEN_IDENTIFIER: return "IDENTIFIER";
        case TOKEN_USE: return "USE";
        case TOKEN_INT: return "INT";
        case TOKEN_STRING_TYPE: return "STRING_TYPE";
        case TOKEN_CHAR: return "CHAR";
        case TOKEN_IF: return "IF";
        case TOKEN_ELSE: return "ELSE";
        case TOKEN_WHILE: return "WHILE";
        case TOKEN_FOR: return "FOR";
        case TOKEN_RETURN: return "RETURN";
        case TOKEN_VOID: return "VOID";
        case TOKEN_ASSIGN: return "ASSIGN";
        case TOKEN_PLUS: return "PLUS";
        case TOKEN_MINUS: return "MINUS";
        case TOKEN_MULTIPLY: return "MULTIPLY";
        case TOKEN_DIVIDE: return "DIVIDE";
        case TOKEN_EQUAL: return "EQUAL";
        case TOKEN_NOT_EQUAL: return "NOT_EQUAL";
        case TOKEN_LESS: return "LESS";
        case TOKEN_GREATER: return "GREATER";
        case TOKEN_LESS_EQUAL: return "LESS_EQUAL";
        case TOKEN_GREATER_EQUAL: return "GREATER_EQUAL";
        case TOKEN_AND: return "AND";
        case TOKEN_OR: return "OR";
        case TOKEN_NOT: return "NOT";
        case TOKEN_SEMICOLON: return "SEMICOLON";
        case TOKEN_COMMA: return "COMMA";
        case TOKEN_DOT: return "DOT";
        case TOKEN_LPAREN: return "LPAREN";
        case TOKEN_RPAREN: return "RPAREN";
        case TOKEN_LBRACE: return "LBRACE";
        case TOKEN_RBRACE: return "RBRACE";
        case TOKEN_LBRACKET: return "LBRACKET";
        case TOKEN_RBRACKET: return "RBRACKET";
        case TOKEN_EOF: return "EOF";
        case TOKEN_NEWLINE: return "NEWLINE";
        case TOKEN_UNKNOWN: return "UNKNOWN";
        default: return "INVALID";
    }
}

void print_tokens(TokenList* tokens) {
    printf("=== TOKENS ===\n");
    for (int i = 0; i < tokens->count; i++) {
        Token* token = &tokens->tokens[i];
        printf("Line %d, Col %d: %s", token->line, token->column, token_type_to_string(token->type));
        if (token->value) {
            printf(" (%s)", token->value);
        }
        printf("\n");
    }
    printf("=== END TOKENS ===\n");
}
