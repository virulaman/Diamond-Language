/*
 * DMO Language Lexer Header
 * Defines tokens and lexical analysis functions
 */

#ifndef LEXER_H
#define LEXER_H

#include <stdbool.h>

// Token types for DMO language
typedef enum {
    // Literals
    TOKEN_NUMBER,
    TOKEN_STRING,
    TOKEN_IDENTIFIER,

    // Keywords
    TOKEN_USE,          // use keyword for imports
    TOKEN_INT,          // int type
    TOKEN_STRING_TYPE,  // string type
    TOKEN_CHAR,         // char type
    TOKEN_IF,           // if statement
    TOKEN_ELSE,         // else statement
    TOKEN_WHILE,        // while loop
    TOKEN_FOR,          // for loop
    TOKEN_RETURN,       // return statement
    TOKEN_VOID,         // void type

    // Operators
    TOKEN_ASSIGN,       // =
    TOKEN_PLUS,         // +
    TOKEN_MINUS,        // -
    TOKEN_MULTIPLY,     // *
    TOKEN_DIVIDE,       // /
    TOKEN_EQUAL,        // ==
    TOKEN_NOT_EQUAL,    // !=
    TOKEN_LESS,         // <
    TOKEN_GREATER,      // >
    TOKEN_LESS_EQUAL,   // <=
    TOKEN_GREATER_EQUAL,// >=
    TOKEN_AND,          // &&
    TOKEN_OR,           // ||
    TOKEN_NOT,          // !
    TOKEN_INCREMENT,    // ++
    TOKEN_DECREMENT,    // --
    TOKEN_MODULO,       // %

    // Punctuation
    TOKEN_SEMICOLON,    // ;
    TOKEN_COMMA,        // ,
    TOKEN_DOT,          // .
    TOKEN_LPAREN,       // (
    TOKEN_RPAREN,       // )
    TOKEN_LBRACE,       // {
    TOKEN_RBRACE,       // }
    TOKEN_LBRACKET,     // [
    TOKEN_RBRACKET,     // ]

    // Special
    TOKEN_EOF,          // End of file
    TOKEN_NEWLINE,      // Newline
    TOKEN_UNKNOWN       // Unknown token
} TokenType;

// Token structure
typedef struct {
    TokenType type;
    char* value;
    int line;
    int column;
} Token;

// Token list structure
typedef struct {
    Token* tokens;
    int count;
    int capacity;
} TokenList;

// Function prototypes
TokenList* tokenize(const char* source);
void free_token_list(TokenList* list);
const char* token_type_to_string(TokenType type);
void print_tokens(TokenList* tokens);
bool is_keyword(const char* word);
TokenType get_keyword_type(const char* word);

#endif // LEXER_H
