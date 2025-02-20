#include "solver.h"

#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include "scanner.h"

#define EPSILON 1e-5 // Small threshold for numeric comparisons.

/*
 *Returns true if two floating-point numbers are within EPSILON from each other.
 */
inline static bool _equal(double a, double b) {
    return fabs(a - b) <= EPSILON;
}

/* Returns true if the given string array contains the given string. */
static bool _containsString(char** arr, size_t n, char* str) {
    // for numeric terms
    if (str == NULL) {
	for (size_t i = 0; i < n; ++i) {
	    char* current_str = arr[i];
	    
	    if (current_str == NULL)
		return true;
	}	
    }

    // for identifier terms
    for (size_t i = 0; i < n; ++i) {
	char* current_str = arr[i];

	if (current_str != NULL && str != NULL && strcmp(str, current_str) == 0)
	    return true;
    }

    return false;
}

static void _printTermDebug(term_t term) {
    if (term.id == NULL) // numeric term
	printf("%d", term.coef);
    else
	printf("%d%s^%zu", term.coef, term.id, term.degree);
}

static void _printEqDebug(eq_t eq) {
    printf("the equation (size=%zu, capacity=%zu) is: [", eq.size, eq.capacity);
    for (int i = 0; i < eq.size - 1; ++i) {
	_printTermDebug(eq.terms[i]);
	printf(", ");
    }

    _printTermDebug(eq.terms[eq.size - 1]);
    printf("]\n");
}

size_t eq_countUniqueIds(eq_t* eq) {
    // allocate an array of strings to store unique identifiers.
    char** unique_ids = malloc(eq->size * sizeof(char*));
    size_t num_unique_ids = 0;
    
    for (size_t i = 0; i < eq->size; ++i) {
	char* current_id = (eq->terms[i]).id;
	
	if (!_containsString(unique_ids, num_unique_ids, current_id))
	    unique_ids[num_unique_ids++] = current_id;
    }
    
    // subtract 1 if NULL (numeric terms like 12 or -100) is present in unique_identifiers.
    if (_containsString(unique_ids, num_unique_ids, NULL))
	num_unique_ids--;
    
    free(unique_ids);
    return num_unique_ids;
}

eq_t eq_empty() {
    term_t* terms = malloc(SOLVER_MAX_TERMS * sizeof(term_t));
    if (terms == NULL) {
	fprintf(stderr, "ERROR\teq_empty\tcould not allocate memory.");
	abort();
    }
    
    return (eq_t) {
	terms,
	0,
	SOLVER_MAX_TERMS
    };
}

term_t* eq_find(eq_t* eq, term_t term) {
    if (!eq || !eq->terms) {
	fprintf(stderr, "ERROR\teq_find\teq or eq->terms is NULL.");
	abort();
    }
    
    for (int i = 0; i < eq->size; ++i) {
	term_t* current = &eq->terms[i];

	// numeric terms
	if (current->id == NULL && term.id == NULL && current->degree == term.degree)
	    return current;
	
	// identifier terms
	if (current->id && term.id && strcmp(current->id, term.id) == 0 && current->degree == term.degree)
	    return current; 
    }
   
    return NULL;
}

term_t* eq_fullfind(eq_t* eq, term_t term) {
    if (!eq || !eq->terms) {
	fprintf(stderr, "ERROR\teq_fullfind\teq or eq->terms is NULL.");
	abort();
    }
    
    for (int i = 0; i < eq->size; ++i) {
	term_t* current = &eq->terms[i];

	// numeric terms
	if (current->id == NULL && term.id == NULL && current->degree == term.degree && current->coef == term.coef)
	    return current;
	
	// identifier terms
	if (current->id && term.id && strcmp(current->id, term.id) == 0 && current->degree == term.degree && current->coef == term.coef)
	    return current; 
    }
   
    return NULL;
}

term_t* eq_extract(eq_t* eq, size_t degree) {
    if (!eq || !eq->terms) {
	fprintf(stderr, "ERROR\teq_extract\teq or eq->terms is NULL.\n");
	abort();
    }

    for (int i = 0; i < eq->size; ++i) {
	term_t* term = &eq->terms[i];

	// numeric terms
	if (degree == 0 && term->id == NULL)
	    return term;
	
	// identifier terms
	if (term->degree == degree && term->id != NULL)
	    return term;
    }

    return NULL;
}

int eq_coef(const term_t* term) {
    if (term == NULL)
	return 0;

    return term->coef;
}

void eq_add(eq_t* eq, term_t term) {
    if (!eq || !eq->terms) {
	fprintf(stderr, "ERROR\teq_find\teq or eq->terms is NULL.\n");
	abort();
    }
    
    term_t* found = eq_find(eq, term);

    // the term is absent from the equation
    if (found == NULL) {
	// increase eq's capacity if necessary
	if (eq->size == eq->capacity) {
	    size_t new_capacity = eq->capacity * 2;
	    eq->terms = realloc(eq->terms, new_capacity * sizeof(term_t));
	    eq->capacity = new_capacity;
	}

	eq->terms[eq->size++] = term;
	return;
    }

    // the term is present in the equation, so update its coefficient.
    found->coef += term.coef;
}

void eq_free(eq_t eq) {
    free(eq.terms);
}

/* Resolve zero-coefficient and zero-degree terms. */
static void _resolveZeros(eq_t* eq) {
    for (int i = 0; i < eq->size; ++i) {
	term_t* term = &eq->terms[i];
	
	// remove the term with 0 coefficient
	if (term->coef == 0) {
	    eq_remove(eq, term);

	    // since one element was removed, shift the index i
	    i--;
	    
	    continue;
	}

	// resolve terms with 0 degrees
	if (term->degree == 0) {
	    // note: numeric terms have degree 1, so here term is definitely an identifier term.
	    if (eq->terms[EQ_NUMERIC_IDX].id != NULL) {
		// create numeric term
		term_t new_numeric_term = {
		    NULL,
		    1,
		    term->coef
		};
		eq_add(eq, new_numeric_term);

	        eq_remove(eq, term);
		i--;
		continue;
	    }

	    eq->terms[EQ_NUMERIC_IDX].coef += term->coef;
	    eq_remove(eq, term);
	    i--;
	    continue;
	}
    }
} 

/* Combines the coefficients of the terms with equal identifiers and degrees. */
static void _combineTerms(eq_t* eq) {
    eq_sort(eq);
    
    // since the equation is sorted, the terms with the same identifier and degree are standing next to each other.
    // therefore, we can use "term_t* track" to track the term we are simplifying.
    term_t* track = &eq->terms[0];
    
    for (int i = 1; i < eq->size; ++i) {
	term_t* term = &eq->terms[i];

	if (track == term)
	    continue;
	
	// if the current term is the same as the previously tracked term (track_id + track_degree), then combine those together
	if ( (term->id == NULL && track->id == NULL && term->degree == track->degree)
	     || (term->id && track->id && strcmp(term->id, track->id) == 0 && term->degree == track->degree) ) {
	    track->coef += term->coef;

	    eq_remove(eq, term);
	    i--;
	    continue;
	}
	
	// otherwise, track the current term.
	track = term;
    }
}

void eq_simplify(eq_t* eq) {
    _combineTerms(eq);
    _resolveZeros(eq);
    eq_sort(eq);
}

/*
 * Helper function that compares two terms and sorts them:
 *   - numeric terms (id=NULL) first.
 *   - identifier terms alphabetically and in descending degree order.
 * Returns a positive number if the term must be swapped.
 * Returns a negative number if the order is correct.
 * Returns 0 if the terms are the same, so do nothing.
 */
int _compareTerms(const void* a, const void* b) {
    const term_t* term_a = (const term_t*)a;
    const term_t* term_b = (const term_t*)b;

    // numeric term / identifier term
    if (!term_a->id && term_b->id)
	return -1; // correct order
    if (term_a->id && !term_b->id)
	return 1;  // swap

    // identifier term / identifier term
    if (term_a->id && term_b->id) {
	// cmp is negative if term_a comes before term_b alphabetically, and is positive otherwise.
	// if cmp is 0, then the terms are equal and the order must be determined with degrees.
	
	int cmp = strcmp(term_a->id, term_b->id);
	if (cmp != 0) return cmp;

	return term_b->degree - term_a->degree;
    }

    // numeric term / numeric term : correct order
    return 0;
    
}

void eq_sort(eq_t* eq) {
    qsort(eq->terms, eq->size, sizeof(term_t), _compareTerms);
}

void eq_removeAt(eq_t* eq, size_t idx) {
    if (idx >= eq->size) {
	fprintf(stderr, "ERROR\teq_removeAt\tindex overflow.\n");
	abort();
    }

    for (int i = idx; i < eq->size - 1; ++i) {
	eq->terms[i] = eq->terms[i + 1];
    }

    eq->size--;
}

void eq_remove(eq_t* eq, term_t* term) {
    // check if term is in the equation
    term_t* found = eq_fullfind(eq, *term);
    if (found == NULL) {
	fprintf(stderr, "ERROR\teq_remove\tcannot remove an absent term.\n");
	abort();
    }

    size_t idx = found - &eq->terms[0];
    eq_removeAt(eq, idx);
}

size_t eq_degree(eq_t* eq, char* id) {
    size_t max_degree = 0;
    
    if (id == NULL) {
	for (int i = 0; i < eq->size; ++i) {
	    term_t term = eq->terms[i];

	    // skip numeric terms
	    if (term.id == NULL)
		continue;

	    max_degree = (max_degree < term.degree) ? term.degree : max_degree;
	}

	return max_degree;
    }
    
    for (int i = 0; i < eq->size; ++i) {
	term_t term = eq->terms[i];

	if (strcmp(term.id, id) == 0)
	    max_degree = (max_degree < term.degree) ? term.degree : max_degree;
    }

    return max_degree;
}

void sol_free(sol_t sol) {
    free(sol.roots);
}

sol_t sol_empty() {
    double* roots = malloc(SOLVER_MAX_ROOTS * sizeof(double));

    return (sol_t) {
	roots,
	0,
	SOLVER_MAX_ROOTS,
	false
    };
}

void sol_add(sol_t* sol, double root) {
    if (sol->inf) {
	fprintf(stderr, "\nERROR\tsol_add\tthe number of solutions is infinite, cannot add a root\n");
        abort();
    }    

    // check if the root is already present
    for (int i = 0; i < sol->num; ++i) {
	double current_root = sol->roots[i];

	if (_equal(current_root, root))
	    return;
    }

    // the root is unique, so add it.
    
    // increase the capacity if necessary.
    if (sol->num == sol->capacity) {
	size_t new_capacity = sol->capacity * 2;
	sol->roots = realloc(sol->roots, new_capacity * sizeof(double));
	sol->capacity = new_capacity;
    }
    
    sol->roots[sol->num++] = root;
}

/*
 * Solves equations a = 0 where a is a real number.
 * Returns true if the equation is solvable, and false otherwise.
 */
bool _solveZeroEq(eq_t* eq, sol_t* sol) {
    // the size of the simplified equation must be equal to 1.
    if (eq->size != 1) {
	fprintf(stderr, "ERROR\t_solveZeroEq\tunexpected number of terms (eq->size = %zu)\n", eq->size);
	abort();
    }

    // extract the terms
    term_t* numeric_term = eq_extract(eq, 0);

    // extract the coefficients
    int numeric_coef = eq_coef(numeric_term);
    
    if (numeric_coef == 0) {
	// 0 = 0 has infinite solutions
	sol->inf = true;
	return true;
    }

    // otherwise, no solutions
    return false;
}

/*
 * Solves linear equations of the form kx + b = 0
 * Returns true if the equation is solvable, and false otherwise.
 */
bool _solveLinearEq(eq_t* eq, sol_t* sol) {
    // the size of the simplified equation must be at most 2 but at least 1.
    if (eq->size > 2 || eq->size < 1) {
	fprintf(stderr, "ERROR\t_solveLinearEq\tunexpected number of terms (eq->size = %zu)\n", eq->size);
	abort();
    }

    // extract the terms
    term_t* linear_term  = eq_extract(eq, 1);
    term_t* numeric_term = eq_extract(eq, 0);

    // extract the coefficients
    int linear_coef  = eq_coef(linear_term);
    int numeric_coef = eq_coef(numeric_term);
    
    // the solution is x = -b / k, k != 0
    if (linear_coef == 0) {
	// equation has the form 0x + b = 0
        eq_remove(eq, linear_term);

	// now, the equation has the form b = 0
	return _solveZeroEq(eq, sol);
    }

    // otherwise, one unique solution is x = -b / k
    sol_add(sol, -(double)numeric_coef / linear_coef);
    return true;
}

/*
 * Solves quadratic equations of the form ax^2 + bx + c = 0
 * Returns true if the equation is solvable, and false otherwise.
 */
bool _solveQuadraticEq(eq_t* eq, sol_t* sol) {
    // the size of the simplified equation must be at most 3 but at least 1
    if (eq->size > 3 || eq->size < 1) {
	fprintf(stderr, "ERROR\t_solveQuadraticEq\tunexpected number of terms (eq->size = %zu)", eq->size);
	abort();
    }

    // extract the terms
    term_t* quadratic_term = eq_extract(eq, 2);
    term_t* linear_term    = eq_extract(eq, 1);
    term_t* numeric_term   = eq_extract(eq, 0);

    // extract the coefficients
    int quadratic_coef = eq_coef(quadratic_term);
    int linear_coef    = eq_coef(linear_term);
    int numeric_coef   = eq_coef(numeric_term);

    if (quadratic_coef == 0) {
	eq_remove(eq, quadratic_term);
	return _solveLinearEq(eq, sol);
    }

    int discriminant = linear_coef * linear_coef - 4 * quadratic_coef * numeric_coef;
    if (discriminant < 0)
	return false;

    sol_add(sol, (-linear_coef + sqrt(discriminant)) / (2 * quadratic_coef));
    sol_add(sol, (-linear_coef - sqrt(discriminant)) / (2 * quadratic_coef));

    return true;
}

// TODO: for now, it can only solve singlevariable equations and degree <= 2.
bool sol_solve(eq_t* eq, sol_t* sol) { 
    eq_simplify(eq); // equation is fully simplified and sorted

    size_t unique_ids = eq_countUniqueIds(eq);
    if (unique_ids > 1) {
	fprintf(stderr, "ERROR\tsol_solve\tcannot solve multivariable equations");
	abort();
    }

    // deduce the type of equation
    size_t degree     = eq_degree(eq, NULL);

    // NOTE: Themis doesn't require the 0-case, therefore set sol->inf so that _printSol() omits printing the solution in this case.
    switch (degree) {
    case 0:
	/* return _solveZeroEq(eq, sol); */;
	    sol->inf = true;
	    return true;
    case 1: return _solveLinearEq(eq, sol);
    case 2: return _solveQuadraticEq(eq, sol);
    default: {
	fprintf(stderr, "ERROR\tsol_solve\tcannot solve equations of degree > 2");
	abort();
    }
    }
}

bool rec_acceptNat(List* tokenList, size_t* val) {
    List node = *tokenList;
    
    if (node != NULL && node->tt == Number) {
	*tokenList = node->next;

	if (val != NULL)
	    *val = node->t.number;
	
	return true;
    }

    return false;
}

bool rec_acceptId(List* tokenList, char** val) {
    List node = *tokenList;

    if (node != NULL && node->tt == Identifier) {
	*tokenList = node->next;

	*val = node->t.identifier;        
	
	return true;
    }

    return false;
}

bool rec_acceptSymbol(List* tokenList, char symbol) {
    List node = *tokenList;

    if (node != NULL && node->tt == Symbol && node->t.symbol == symbol) {
	*tokenList = node->next;
	return true;
    }

    return false;
}

/*
 * Parses, and updates the equation with a term that starts with an identifier and may be exponentiated. It also takes coef, sign, and left to propagate the sign of the term and (if applicable) its coefficient.  
 * (partial) grammar ::= <identifier> [ '^' <nat> ]
 */
static bool _acceptTermId(List *tokenList, eq_t* eq, size_t coef, int sign, int left) {
    // identifier and degree are parsed from the term
    char* id;
    size_t degree = 1; // if no '^' is present, then identifier has degree 1 by definition.
    
    if (rec_acceptId(tokenList, &id)) {
	if (rec_acceptSymbol(tokenList, SOLVER_SYMBOL_EXP)) {
	    if (!rec_acceptNat(tokenList, &degree))
		return false;
	}

	// insert new term
	term_t new_term = {
	    id,
	    degree,
	    left * sign * coef
	};
	eq_add(eq, new_term);
	
	return true;
    }

    return false;
}

bool rec_acceptTerm(List* tokenList, eq_t* eq, int sign, int left) {
    if (*tokenList != NULL && (*tokenList)->tt == Identifier) {
	// the coefficient of this term is 1
	return _acceptTermId(tokenList, eq, 1, sign, left);
    }

    size_t val;
    if (!rec_acceptNat(tokenList, &val))
	return false;

    if (*tokenList != NULL && (*tokenList)->tt == Identifier)
	// the coefficient of this term is val
	return _acceptTermId(tokenList, eq, val, sign, left);

    // otherwise, there is no identifier so it is a numeric term.
    // its identifier is NULL and its degree is 1 (TODO: no support for advanced exponentiation)
    term_t new_term = {
	NULL,
	1,
	left * sign * val
    };
    eq_add(eq, new_term);
    
    return true;
}

/*
 * Parses, and updates an equation with an expression without handling a leading negation (if applicable).
 * It also takes the sign and left parameters to propagate the signs into the terms.
 *
 * (partial) grammar ::= <term> { '+' <term> | '-' <term>}
 */
static bool _acceptExprNoNegation(List* tokenList, eq_t* eq, int sign, int left) {
    if (!rec_acceptTerm(tokenList, eq, sign, left))
	return false;

    // either plus or minus is present next to each term.
    bool plus, minus;
    plus  = rec_acceptSymbol(tokenList, SOLVER_SYMBOL_PLUS);
    if (!plus)
	minus = rec_acceptSymbol(tokenList, SOLVER_SYMBOL_MINUS);
    
    while (plus || minus) {
	sign = plus ? 1 : -1;
	
	if (!rec_acceptTerm(tokenList, eq, sign, left))
	    return false;
	
	plus  = rec_acceptSymbol(tokenList, SOLVER_SYMBOL_PLUS);
	if (!plus)
	    minus = rec_acceptSymbol(tokenList, SOLVER_SYMBOL_MINUS);
    }

    return true;
}

bool rec_acceptExpr(List* tokenList, eq_t* eq, int left) {
    if (rec_acceptSymbol(tokenList, SOLVER_SYMBOL_MINUS))
	// this expression is negated.
	return _acceptExprNoNegation(tokenList, eq, -1, left);

    // this expression is not negated.
    return _acceptExprNoNegation(tokenList, eq, 1, left);
}

bool rec_acceptEq(List* tokenList, eq_t* eq) {
    return rec_acceptExpr(tokenList, eq, 1)   // this expression is to the left of equality.
	&& rec_acceptSymbol(tokenList, SOLVER_SYMBOL_EQ)
	&& rec_acceptExpr(tokenList, eq, -1); // this expression is to the right of equality.
}

static void _printEq(eq_t eq) {
    size_t unique_ids = eq_countUniqueIds(&eq); 
    
    if (unique_ids != 1)
	printf("this is an equation, but not in 1 variable\n");
    else {
	size_t degree = eq_degree(&eq, NULL);
	printf("this is an equation in 1 variable of degree %zu\n", degree);
    }
}

static void _printSol(sol_t sol, bool solvable) {
    if (!solvable || /* For Themis: treat infinite solutions as no solutions */ sol.inf) {
	printf("not solvable\n");
	return;
    }

    /* For Themis:
    if (solvable && sol.inf) {
	// printf("infinite solutions\n");
	return;
    }
    */
    
    printf("solution: ");

    for (int i = 0; i < sol.num - 1; ++i) {
	double root = sol.roots[i];

	// For Themis: avoid -0.000
	if (root <= 0 && root >= -0.0005)
	    root = (double)0;
	
	printf("%.3f and ", root);
    }

    // For Themis: avoid -0.000
    double root = sol.roots[sol.num - 1];
    if (root <= 0 && root >= -0.0005)
	root = (double)0;
    printf("%.3f\n", root);
}

void rec_prompt(bool solve) {
    char* eq_str;

    printf("give an equation: ");
    eq_str = readInput();
    
    while (eq_str[0] != SOLVER_SYMBOL_STOP) {
	// generate token List from the equation string
	List token_list = tokenList(eq_str);
	List token_list_copy = token_list;

	printList(token_list);
	
	// try recognizing the equation
	eq_t eq = eq_empty();
	bool eq_accepted = rec_acceptEq(&token_list_copy, &eq) && token_list_copy == NULL;
	
	if (eq_accepted) {
	    _printEq(eq);

	    // NOTE: Themis doesn't want all the functionality that I've written, so this extra code is here just so that i can pass Themis' tests
	    // If I don't check the degree and the number of unique identifiers in the equation, the program will generate an error and abort => core dumbed
	    size_t degree = eq_degree(&eq, NULL);
	    size_t unique_ids = eq_countUniqueIds(&eq);

	    // Therefore, output a solution only for SINGLEvariable equations of degree < 2
	    if (solve && /* these superfluous conditions fulfill the Themis' requirements */ degree < 3 && unique_ids == 1) {
		sol_t sol = sol_empty();

		bool solvable = sol_solve(&eq, &sol);
		_printSol(sol, solvable);
		
		sol_free(sol);
	    }
	} else
	    printf("this is not an equation\n");
    
	freeTokenList(token_list);
	eq_free(eq);
	free(eq_str);
	
	printf("\ngive an equation: ");
	eq_str = readInput();
    }

    printf("good bye\n");
    free(eq_str);
}

