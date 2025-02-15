/* file: recognizer.h
 * author: Artur Topal (s5942128)
 * date: 14/02/2025
 *
 * This header declares functions for recognizing and parsing equations that follow the grammar:
 *
 * <letter>     ::= 'a' | 'b' | ... | 'z' | 'A' | 'B' | ... | 'Z'
 * <digit>      ::= '0' | '1' | ... | '8' | '9'
 * <identifier> ::= <letter> { <digit> | <letter> }
 * <nat>        ::= <digit> { <digit> }
 * <term>       ::= <nat> | [ <nat> ] <identifier> [ '^' <nat> ]
 * <expression> ::= ['-'] <term> { '+' <term> | '-' <term> }
 * <equation>   ::= <expression> '=' <expression>
 *
 * The functions in this module validate token lists (produced by the scanner.h) against the grammar above. They also extract variables and calculate the degree of the equation.
 */

#ifndef RECOGNIZER_H
#define RECOGNIZER_H

#include <stdbool.h>
#include <stdlib.h>
#include "scanner.h"


// Maximum number of identifiers/variables expected in an equation (not absolute).
#define MAXIMUM_NUMBER_VARIABLES 10

// Recognizer symbol definitions used in the grammar.
#define RECOGNIZER_SYMBOL_STOP  '!'
#define RECOGNIZER_SYMBOL_PLUS  '+'
#define RECOGNIZER_SYMBOL_MINUS '-'
#define RECOGNIZER_SYMBOL_EXP   '^'
#define RECOGNIZER_SYMBOL_EQ    '='

// Structure to store a variable with its coefficient (e.g., 3x or 6apples)
typedef struct Variable {
    char* id;
    int coef;
} Variable;

// Structure for storing identified variables and cumulative coefficients of each variable.
typedef struct Equation {
    Variable* data; 
    size_t size;
    size_t capacity;
} Equation;

// TODO
// 3x + 4 + 12 - 7x= 0
// x: 3 - 7
// '1': 4 + 12
//
//

/* Initializes and returns an empty Variables structure with an initial capacity defined by MAXIMUM_NUMBER_VARIABLES. */
Variables initVariables();

/* Returns true if the given variable is already present in the Variables structure, and false otherwise. */
bool containsVariable(Variables* vars, char* var);

/* Inserts the variable into the Variables structure if it is not already present. */
void insertVariable(Variables* vars, char* var);

/* Frees any dynamically allocated memory used by the Variables structure. */
void freeVariables(Variables* vars);

/*
 * The following functions parse components of the equation grammar. Each function works on a pointer to a token list, attempting to match the expected grammatical component.
 *
 * General behavior:
 *   1. Accepts a pointer to the token list.
 *   2. Checks if the current token(s) match the expected component.
 *   3. If matched, advances the token list pointer and returns true.
 *   4. If not, leaves the pointer unchanged and returns false.
 *
 * Specifics:
 *   - acceptNatural: Recognizes a natural number and updates *val with its numerical value if not NULL.
 *   - acceptIdentifier: Recognizes an identifier and updates the Variables structure if this identifier is not present.
 *   - acceptSymbol: Checks if the current token equals the given symbol.
 *   - acceptTerm, acceptExpression, acceptEquation: Combine acceptNatural, acceptIdentifier, and acceptTerm to validate more complex grammar components while tracking variables (via the Variables structure) and the equations's degree.
 */

bool acceptNatural(List* tokenList, size_t* val);
bool acceptIdentifier(List* tokenList, Variables* vars);
bool acceptSymbol(List* tokenList, char symbol);
bool acceptTerm(List* tokenList, Variables* vars, size_t* degree);
bool acceptExpression(List* tokenList, Variables* vars, size_t* degree);
bool acceptEquation(List* tokenList, Variables* vars, size_t* degree);


/*
 * Prompts the user for an equation, tokenizes the input, and checks whether the equation conforms to the grammar in the beginning of this header. It reports if the equation is grammatically correct, the number of variables, and its degree.
 */
void recognizeEquations();

#endif // RECOGNIZER_H
