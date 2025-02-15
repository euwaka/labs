#include <stdio.h>
#include <string.h>
#include "recognizer.h"
#include "scanner.h"

Variables initVariables() {
    char** data = malloc(MAXIMUM_NUMBER_VARIABLES * sizeof(char*));
    
    return (Variables) {
	data,
	0,
	MAXIMUM_NUMBER_VARIABLES
    };
}

bool containsVariable(Variables* vars, char* var) {
    for (size_t i = 0; i < vars->size; ++i) {
	char* current = vars->data[i];

	// strcmp returns 0 if the strings are equal
	if (strcmp(current, var) == 0)
	    return true;
    }

    return false;
}

void insertVariable(Variables* vars, char* var) {
    // Only insert if the variable is not already stored.
    if (containsVariable(vars, var))
	return;

    // Check and expand capacity if necessary.
    if (vars->size == vars->capacity) {
	size_t new_capacity = vars->capacity * 2;
	vars->data = realloc(vars->data, new_capacity * sizeof(char*));
	vars->capacity = new_capacity;
    }

    // Insert the new variable and increment the count.
    vars->data[vars->size++] = var;
}

void freeVariables(Variables* vars) {
    // the actual identifier string are assumed to be managed elsewhere.
    free(vars->data);
}

bool acceptNatural(List* tokenList, size_t* val) {
    List node = *tokenList;
    
    if (node != NULL && node->tt == Number) {
	*tokenList = node->next;

	if (val != NULL)
	    *val = node->t.number;
	
	return true;
    }

    return false;
}

bool acceptIdentifier(List* tokenList, Variables* vars) {
    List node = *tokenList;

    if (node != NULL && node->tt == Identifier) {
	*tokenList = node->next;

	char* current_variable = node->t.identifier;
	insertVariable(vars, current_variable);
	
	return true;
    }

    return false;
}

bool acceptSymbol(List* tokenList, char symbol) {
    List node = *tokenList;

    if (node != NULL && node->tt == Symbol && node->t.symbol == symbol) {
	*tokenList = node->next;
	return true;
    }

    return false;
}

/*
 * Parses a term that starts with an identifier and may be follow by an exponentation operator with a natural number.
 * Partial <term> grammar ::= <identifier> [ '^' <nat> ]
 * If an exponent on the identifier is present, updates the degree value.
 * Returns true if the term is accepted; otherwise, returns false. 
 */
static bool acceptTermIdentifier(List *tokenList, Variables* vars, size_t* degree) {
    if (acceptIdentifier(tokenList, vars)) {
	if (acceptSymbol(tokenList, RECOGNIZER_SYMBOL_EXP)) {
	    size_t new_degree;

	    if (!acceptNatural(tokenList, &new_degree))
		return false;
	    
	    *degree = (*degree > new_degree) ? *degree : new_degree;
	    return true;
	}

	if (*degree == 0)
	    *degree = 1;
	
	return true;
    }

    return false;
}

bool acceptTerm(List* tokenList, Variables* vars, size_t* degree) {
    if (*tokenList != NULL && (*tokenList)->tt == Identifier) {
	return acceptTermIdentifier(tokenList, vars, degree);
    }

    if (!acceptNatural(tokenList, NULL))
	return false;

    if (*tokenList != NULL && (*tokenList)->tt == Identifier)
	return acceptTermIdentifier(tokenList, vars, degree);

    return true;
}

/*
 * Parses an expression without handling a leading negation.
 * Partial <expression> grammar ::= <term> { '+' <term> | '-' <term>}
 * Returns true if the expression is parsed correctly; otherwise, false.
 */
static bool acceptExpressionNoNegation(List* tokenList, Variables* vars, size_t* degree) {
    if (!acceptTerm(tokenList, vars, degree))
	return false;

    while (acceptSymbol(tokenList, RECOGNIZER_SYMBOL_PLUS) || acceptSymbol(tokenList, RECOGNIZER_SYMBOL_MINUS)) {
	if (!acceptTerm(tokenList, vars, degree))
	    return false;
    }

    return true;
}

bool acceptExpression(List* tokenList, Variables* vars, size_t* degree) {
    if (acceptSymbol(tokenList, RECOGNIZER_SYMBOL_MINUS))
	return acceptExpressionNoNegation(tokenList, vars, degree);

    return acceptExpressionNoNegation(tokenList, vars, degree);
}

bool acceptEquation(List* tokenList, Variables* vars, size_t* degree) {
    return acceptExpression(tokenList, vars, degree)
	&& acceptSymbol(tokenList, RECOGNIZER_SYMBOL_EQ)
	&& acceptExpression(tokenList, vars, degree);
}

void recognizeEquations() {
    char* equation_str;

    printf("give an equation: ");
    equation_str = readInput();
    
    while (equation_str[0] != RECOGNIZER_SYMBOL_STOP) {
	// generate token list from the equation string
	List token_list = tokenList(equation_str);
	List token_list_copy = token_list;

	printList(token_list);
	
	// try recognizing the equation
	Variables vars = initVariables();
	size_t degree = 0;
	
	if (acceptEquation(&token_list_copy, &vars, &degree) && token_list_copy == NULL) {
	    if (vars.size != 1)
		printf("this is an equation, but not in 1 variable\n");
	    else
		printf("this is an equation in %zu variable of degree %zu\n", vars.size, degree);
	} else {
	    printf("this is not an equation\n");
	}

	freeTokenList(token_list);
	freeVariables(&vars);
	free(equation_str);
	
	printf("\ngive an equation: ");
	equation_str = readInput();
    }

    printf("good bye");
    free(equation_str);
}
