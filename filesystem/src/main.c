#include "lexer.h"

int main() {
    const char *input = lexer_read_input();
    Token token = lexer_tokenize_input(input);

    lexer_repr_tokens(token, input);
    
    return EXIT_SUCCESS;
}
