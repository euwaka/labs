#ifndef LEXER_H
#define LEXER_H

#include <stdlib.h>
#include <stdbool.h>

#define LEXER_MAXINPUT 1000 // not absolute
#define LEXER_MAX_IDENTIFIER_LENGTH 60 // absolute

typedef enum Symbol {
    SYMBOL_QUOTATION = 0,
    SYMBOL_SLASH,
    SYMBOL_REDIR_SINGLE,
    SYMBOL_REDIR_DOUBLE,
    SYMBOL_DOT,
    SYMBOL_DASH_SINGLE
} Symbol;

static char const *const SYMBOL_STRS[] = {"\"", "/", ">", ">>", ".", "-"};

typedef enum {
    TOKEN_WORD,  
    TOKEN_SYMBOL, // for example, |, >, or >>
    TOKEN_ASCII,
    TOKEN_EOF
} TokenType;

typedef struct Slice {
    size_t size;
    const char *data;
} Slice;

typedef union TokenData {
    Slice slice;
    Symbol symbol;
} TokenData;
    
typedef struct TokenNode *Token;
typedef struct TokenNode {
    TokenData data;
    TokenType type;
    Token next;
} TokenNode;

/*
 * Functions for matching specific components from the string input, and propagating the pointer of the current
 * character in the string input.
 */
bool lexer_match_word(const char *input, int *idx, Slice *output);
void lexer_match_ascii(const char *input, int *idx, Slice *output);
bool lexer_match_SYMBOL(const char *input, int *idx, Symbol *output);


/*
 * Creates a new token node from the current position in the string input, and propagates the index of the string
 * input.
 */
Token lexer_create_token(const char *input, int *idx);

/* Read off a line from stdin and return it. */
char *lexer_read_input();

/* Tokenizes the input into a token linked list. */
Token lexer_tokenize_input(const char *input);

/* Frees any dynamically allocated memory used by the Token Linked List. */
void lexer_free_tokens(Token token);

/* Returns the string representation of the token linked list. */
void lexer_repr_tokens(Token token, const char *input);

#endif // LEXER_H
