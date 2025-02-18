/* file: solver.h
 * author: Artur Topal (s5942128)
 * date: 14/02/2025 (TODO)
 *
 * This header declares functions for recognizing, parsing, and (sometimes) solving equations that follow
 * the grammar:
 *
 * <letter>     ::= 'a' | 'b' | ... | 'z' | 'A' | 'B' | ... | 'Z'
 * <digit>      ::= '0' | '1' | ... | '8' | '9'
 * <identifier> ::= <letter> { <digit> | <letter> }
 * <nat>        ::= <digit> { <digit> }
 * <term>       ::= <nat> | [ <nat> ] <identifier> [ '^' <nat> ]
 * <expression> ::= ['-'] <term> { '+' <term> | '-' <term> }
 * <equation>   ::= <expression> '=' <expression>
 *
 * The functions in this module validate token lists (produced by the scanner.h) against the grammar above.
 * They also extract identifiers with their respective coefficients and degrees into an Equation structure.
 * Also, this module's functions can solve some Equations.
 */

#ifndef SOLVER_H
#define SOLVER_H

#include <stdbool.h>
#include <stdlib.h>
#include "scanner.h"

// Maximum number of identifiers expected in an equation (not absolute).
#define SOLVER_MAX_TERMS 10

// Maximum number of solutions in an equation (not absolute).
#define SOLVER_MAX_ROOTS   10

// Symbol definitions used in the grammar.
#define SOLVER_SYMBOL_STOP  '!'
#define SOLVER_SYMBOL_PLUS  '+'
#define SOLVER_SYMBOL_MINUS '-'
#define SOLVER_SYMBOL_EXP   '^'
#define SOLVER_SYMBOL_EQ    '='

/* =========================================================================== */
/* Equations: eq_t                                                             */
/* =========================================================================== */

/*
 * Structure for storing a grammatically-correct term including the sign: an identifier with its
 * coefficient and degree. (e.g., 2x^2 or -10apples^0)
 */
typedef struct term_t {
    char*   id;
    size_t  degree;
    int     coef;
} term_t;

/*
 * Structure representing an equation as a collection of term.
 * When constructing an equation, terms are automatically simplified:
 * - If a term with the same identifier and degree exists, its coefficient is updated.
 * - Terms with a zero coefficient or degree are resolved in-plance (e.g., 0x^1 is discarded and 12x^0
 *   gets converted to 12)
 *
 * Terms in an 'eq_t' equation follow the following order at any moment of time:
 * 1. The constant term (if present) appears first.
 * 2. Identifier terms follow. Terms with the identical dentifiers are sorted by descending degree. And individual identifiers are sorted alphabetically.
 *
 * Example:
 * Given the equation: x^2 - 3x^2 + x^1 - y^12 + z^30 - 30 = 100 + 10x^1
 * It is stored in 'eq_t' as: terms = [ -130, -2x^2, -9x^1, -y^12, z^30 ]
 */
typedef struct eq_t {
    term_t* terms; 
    size_t  size;
    size_t  capacity;
} eq_t;


// Index where the numeric term is stored in an equation
#define EQ_NUMERIC_IDX 0

/*
 * Returns the number of unique identifiers in the equation.
 *
 * Example:
 * An equation with {30, x^3, x^1, y^100} terms has two unique identifiers: x and y.
 */
size_t eq_countUniqueIds(eq_t* eq);

/* Initializes and returns an empty equation with an initial capacity defined by SOLVER_MAX_TERMS. */
eq_t eq_empty();

/* Returns the pointer to the term if the given term (thus, its identifier and degree) is present in the equation, and NULL otherwise. */
term_t* eq_find(eq_t* eq, term_t term);

/*
 * Inserts the term if absent from the equation. Otherwise, it updates the coefficient of the equation's term (effectively simplifying
 * the equation without calling extra eq_simlpify function).
 */
void eq_add(eq_t* eq, term_t term);

/* Frees any dynamically allocated memory in 'eq_t'. */
void eq_free(eq_t eq);

/*
 * Simplifies an equation by adding terms with the same identifiers and degrees, and
 * by resolving zero-coefficient and zero-degree terms.
 */
void eq_simplify(eq_t* eq);

/* Sorts equation like described in eq_t. */
void eq_sort(eq_t* eq);

/* Removes the term at index. */
void eq_removeAt(eq_t* eq, size_t idx);

/*
 * Returns the maximum degree of the identifier in the equation.
 * If the identifier is absent from the equation, generates an error.
 * If the identifier is NULL, the function returns maximum degree among all identifier terms.
 */
size_t eq_degree(eq_t* eq, char* id);

/* =========================================================================== */
/* Solutions to equations: sol_                                                */
/* =========================================================================== */

/*
 * Structure for storing the solution of an equation, including
 * 
 * roots:    all the roots, dynamically stored on the heap;
 * num:      the number of solutions;
 * capacity: the maximum number of solutions that can be stored in roots (is adjusted automatically);
 * inf:      true if there are infinite solutions;
 */
typedef struct sol_t {
    double* roots;
    size_t  num;
    size_t  capacity;
    bool    inf;
} sol_t;

/* Frees any dynamically allocated objects by sol. */
void sol_free(sol_t sol);

/* Creates an empty sol_t structure with an inital capacity of SOLVER_MAX_ROOTS. */
sol_t sol_empty();

/* Adds a root (if absent) and updates the number of solutions. If the root is present, it does nothing. */
void sol_add(sol_t* sol, double root);

/* If the equation is solvable, it solves the equation, stores the result in sol, and returns true. Otherwise, it returns false.  */
bool sol_solve(eq_t* eq, sol_t* sol);

/* =========================================================================== */
/* Recognition: rec_                                                           */
/* =========================================================================== */

/*
 * The following functions parse components of the equation grammar. Each function works on a pointer to a token list, attempting to
 * match the expected grammatical component and updates 'eq_t' equation..
 *
 * General behavior:
 *   1. Accepts a pointer to the token list with additional pointers that will store the value of each component (such as size_t* val or eq_t* eq).
 *   2. Checks if the current token(s) match the expected component.
 *   3. If matched, advances the token list pointer, stores the component's value using the provided pointer, and returns true.
 *   4. If not, leaves the token list pointer unchanged and returns false.
 *   5. Also, the sign between terms and the sign caused by the equality get propagated.
 *
 * Specifics:
 *   - rec_acceptNat: Recognizes a natural number and updates *val with its numerical value if val is not NULL.
 *   - rec_acceptId: Recognizes an identifier and updates *val with its value if val is not NULL;
 *   - rec_acceptSymbol: Checks if the current token equals the given symbol.
 *   - rec_acceptTerm, rec_acceptExpr, rec_acceptEq: Combine rec_acceptNat, rec_acceptId, rec_acceptSymbol, and rec_acceptTerm to validate more complex
 *     grammar components while tracking the terms (via the term_t structure) and propagating the sign used by the terms.
 */

bool rec_acceptNat(List* tokenList, size_t* val);
bool rec_acceptId(List* tokenList, char** val);

bool rec_acceptSymbol(List* tokenList, char symbol);
bool rec_acceptTerm(List* tokenList, eq_t* eq, int sign, int left);
bool rec_acceptExpr(List* tokenList, eq_t* eq, int left);
bool rec_acceptEq(List* tokenList, eq_t* eq);

/*
 * Prompts the user for an equation, tokenizes the input, checks whether the equation conforms to the grammar in the beginning of this header, and constructs
 * an equation. If solve is true, it also prints the solution to each equation.
 */
void rec_prompt(bool solve);

#endif // SOLVER_H
