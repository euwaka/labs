/* scanner.h, Gerard Renardel, 29 January 2014 */

#ifndef SCANNER_H
#define SCANNER_H

#define MAXINPUT 100  /* maximal length of the input */
#define MAXIDENT 10   /* maximal length of an identifier */

/* Some type definitions:
 * tokenType with three values, to indicate the three types of tokens, viz.
 * number, identifier and symbol;
 * the union type token in which these types are unified;
 * the type list for lists with nodes that contain tokens.
 */

typedef enum TokenType {
    Number,
    Identifier,
    Symbol
} TokenType;

typedef union Token {
    int number;
    char *identifier;
    char symbol;
} Token;

typedef struct ListNode *List;

typedef struct ListNode {
    TokenType tt;
    Token t;
    List next;
} ListNode;

/* Now the declaration of the functions that are defined in scanner.c
 * and are to be used outside it, e.g. in recognizeExp.c en in evalExp.c
 */

char *readInput();
List tokenList(char *array);
int valueNumber(List *lp, double *wp);
void printList(List l);
void freeTokenList(List l);
void scanExpressions();

#endif
