#ifndef LEXER_H
#define LEXER_H

#include <stdlib.h>
#include <stdbool.h>

#define LEXER_MAXINPUT 1000 // not absolute
#define LEXER_MAX_IDENTIFIER_LENGTH 60 // absolute

typedef enum Optr {
    OPTR_QUOTATION = 0,
    OPTR_SLASH,
    OPTR_REDIR_SINGLE,
    OPTR_REDIR_DOUBLE,
    OPTR_DOT,
    OPTR_DASH_SINGLE
} Optr;

static char const *const OPTR_STRS[] = {"\"", "/", ">", ">>", ".", "-"};

typedef enum {
    TOKEN_LATIN,  
    TOKEN_OPERATOR, // for example, |, >, or >>
    TOKEN_ASCII,
    TOKEN_EOF
} TokenType;

typedef struct IndexedData {
    size_t from;
    size_t to;
} IndexedData;

typedef union TokenData {
    IndexedData indexed_data;
    Optr optr;
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
bool lexer_match_word(const char *input, int *idx, IndexedData *output);
void lexer_match_ascii(const char *input, int *idx, IndexedData *output);
bool lexer_match_optr(const char *input, int *idx, Optr *output);


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
