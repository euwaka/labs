#include "lexer.h"

#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <ctype.h>

char *lexer_read_input() {
    unsigned max_length = LEXER_MAXINPUT;
    unsigned current_length = 0;

    char *input = malloc(max_length * sizeof(char));
    if (input == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        abort();
    }

    char current_char;
    while ((current_char = getchar()) != '\n' && current_char != EOF) {
        input[current_length++] = current_char;

        if (current_length >= max_length) {
            max_length *= 2;
            input = realloc(input, max_length * sizeof(char));
            assert(input != NULL);
        }
    }

    input[current_length] = '\0';
    return input;
}

static Token _create_empty_token() {
    Token token = malloc(sizeof(TokenNode));
    if (token == NULL) {
        fprintf(stderr, "Could not allocated memory for Token.");
        abort();
    }
    
    token->next = NULL;
    return token;
}

bool lexer_match_word(const char *input, int *idx, IndexedData *output) {
    int _idx = *idx;
    while (isalpha(input[_idx++]));

    if (isspace(input[_idx])) {
        // a word
        output->from = *idx;
        output->to   = _idx;
        *idx = _idx;
        return true;
    }

    // otherwise, not a word
    return false;
}

void lexer_match_ascii(const char *input, int *idx, IndexedData *output) {
    int _idx = *idx;

    // TODO: "data hahaha\" " case is not correct
    while(input[_idx] != '"' && input[_idx] != '\0')
        _idx++;

    output->from = *idx;
    output->to   = _idx;
    *idx = _idx;
}

bool lexer_match_optr(const char *input, int *idx, Optr *output) {
    switch(input[*idx]) {
    case '"':
        *idx += 1;
        *output = OPTR_QUOTATION;
        return true;
    case '/':
        *idx += 1;
        *output = OPTR_SLASH;
        return true;
    case '>':
        *idx += 1;

        if (input[*idx] == '>') {
            *output = OPTR_REDIR_DOUBLE;
            *idx += 1;
        }
        else
            *output = OPTR_REDIR_SINGLE;
        return true;
    case '.':
        *idx += 1;
        *output = OPTR_DOT;
        return true;
    case '-':
        *idx += 1;
        *output = OPTR_DASH_SINGLE;
        return true;
    default:
        return false;
    }
}

Token lexer_create_token(const char *input, int *idx) {
    Token token = _create_empty_token();

    // NOTE: The order is important here.
    
    if (input[*idx] == '\0') {
        *idx += 1;
        token->type = TOKEN_EOF;
        return token;
    }

    Optr optr;
    if (lexer_match_optr(input, idx, &optr)) {
        token->type = TOKEN_OPERATOR;
        token->data.optr = optr;
        return token;
    }

    IndexedData data;
    if (lexer_match_word(input, idx, &data)) {
        token->type = TOKEN_LATIN;
        token->data.indexed_data = data; 
        return token;
    }

    // otherwise, the token is some ASCII text
    lexer_match_ascii(input, idx, &data);
    token->type = TOKEN_ASCII;
    token->data.indexed_data = data;
    return token;
}

Token lexer_tokenize_input(const char *input) {
    Token token_head = NULL;
    Token token_tail = NULL;
    
    // traverse the input, including '\0'
    for (int idx = 0; idx <= strlen(input);) {
        if (isspace(input[idx])) {
            idx++;
            continue;
        }

        Token new_node = lexer_create_token(input, &idx);
        if (!token_head) {
            token_head = new_node;
            token_tail = new_node;
        } else {
            token_tail->next = new_node;
            token_tail = new_node;
        }
    }

    return token_head;
}


void lexer_free_tokens(Token token) {
    if (token == NULL)
        return;
    lexer_free_tokens(token->next);
    free(token);
}

static void _repr_token_string(IndexedData data, const char *input) {
    printf("%.*s", (int)(data.to - data.from), input + data.from);
}

static void _repr_token_optr(Optr optr) {
    printf("%s", OPTR_STRS[optr]);
}

void lexer_repr_tokens(Token token, const char *input) {
    while (token != NULL) {
        switch (token->type) {
        case TOKEN_LATIN:
        case TOKEN_ASCII:
            _repr_token_string(token->data.indexed_data, input);
            printf(" ");
            break;
        case TOKEN_OPERATOR:
            _repr_token_optr(token->data.optr);
            printf(" ");
            break;
        case TOKEN_EOF:
            printf("EOF\n");
            break;
        }

        token = token->next;
    }
}
