/* scanner.c, Gerard Renardel, 29 January 2014
 *
 * In this file a scanner is defined that can read expressions consisting of
 * numbers, identifiers and symbols (ie. characters that are not a character or a digit).
 * Scanning is: transforming an input string into a token list.
 * A token is: a number, an identifier or a symbol.
 */

#include <stdio.h>  /* getchar, printf */
#include <stdlib.h> /* NULL, malloc, free */
#include <string.h> /* strcpy */
#include <ctype.h>  /* isspace, isdigit, isalpha, isalnum */
#include <assert.h> /* assert */
#include "scanner.h"

/* The function readInput reads the input and yields a string containing this input.
 * Initially, the length of s is MAXINPUT: it is doubled when necessary.
 */

char *readInput() {
    int strLen = MAXINPUT;
    int c = getchar();
    int i = 0;
    char *s = malloc((strLen + 1) * sizeof(char));
    assert(s != NULL);
    while (c != '\n') {
	s[i] = c;
	i++;
	if (i >= strLen) { /* s is not large enough, its length is doubled */
	    strLen = 2 * strLen;
	    s = realloc(s, (strLen + 1) * sizeof(char));
	    assert(s != NULL);
	}
	c = getchar();
    }
    s[i] = '\0';
    return s;
}

/* The functions matchNumber, matchCharacter and matchIdentifier do what their name indicates
 * and yield what has been read. Their parameters are the array from which to read and a pointer
 * to an index in the array. The value of the index is adapted during reading.
 */

int matchNumber(char *ar, int *ip) {
    int n = 0;
    while (isdigit(ar[*ip])) {
	n = 10 * n + (ar[*ip] - '0');
	(*ip)++;
    }
    return n;
}

char matchCharacter(char *ar, int *ip) {
    char s = ar[*ip];
    (*ip)++;
    return s;
}

/* In matchIdentifier the length of the string is adapted whenver necessary, as in readInput.
 */

char *matchIdentifier(char *ar, int *ip) {
    int j = 0;
    int strLen = MAXIDENT;
    char *s = malloc((strLen + 1) * sizeof(char));
    assert(s != NULL);
    while (isalnum(ar[*ip + j])) {
	s[j] = ar[*ip + j];
	j++;
	if (j >= strLen) { /* s is not large enough, its length is doubled */
	    strLen = 2 * strLen;
	    s = realloc(s, (strLen + 1) * sizeof(char));
	    assert(s != NULL);
	}
    }
    s[j] = '\0';
    *ip = *ip + j;
    return s;
}

/* The function newNode makes a new node for the token list and fills it with the token that
 * has been read.
 */

List newNode(char *ar, int *ip) { /* precondition: !isspace(a[*ip]) */
    List node = malloc(sizeof(struct ListNode));
    assert(node != NULL);
    node->next = NULL;
    if (isdigit(ar[*ip])) { /* we see a digit, so a number starts here */
	node->tt = Number;
	(node->t).number = matchNumber(ar, ip);
	return node;
    }
    if (isalpha(ar[*ip])) { /* we see a letter, so an identifier starts here */
	node->tt = Identifier;
	(node->t).identifier = matchIdentifier(ar, ip);
	return node;
    } /* no space, no number, no identifier: we call it a symbol */
    node->tt = Symbol;
    (node->t).symbol = matchCharacter(ar, ip);
    return node;
}

/* The function tokenList reads an array and puts the tokens that are read in a list.
 * The result is a pointer to the beginning of the list.
 */

List tokenList(char *ar) {
    List lastNode = NULL;
    List node = NULL;
    List tl = NULL;
    int i = 0;
    int length = strlen(ar);
    while (i < length) {
	if (isspace(ar[i])) { /* spaces are skipped */
	    i++;
	} else {
	    node = newNode(ar, &i);
	    if (lastNode == NULL) { /* there is no list yet */
		tl = node;
	    } else { /* there is already a list; add node at the end */
		(lastNode)->next = node;
	    }
	    lastNode = node;
	}
    }
    return tl;
}

/* The function printList prints the tokens in a token list, separated by spaces.
 */

void printList(List li) {
    while (li != NULL) {
	switch (li->tt) {
	case Number:
	    printf("%d ", (li->t).number);
	    break;
	case Identifier:
	    printf("%s ", (li->t).identifier);
	    break;
	case Symbol:
	    printf("%c ", (li->t).symbol);
	    break;
	}
	li = li->next;
    }
    printf("\n");
}

/* The function freeTokenList frees the memory of the nodes of the list, and of the strings
 * in the nodes.
 */

void freeTokenList(List li) {
    if (li == NULL) {
	return;
    }
    if (li->tt == Identifier) {
	free((li->t).identifier);
    }
    freeTokenList(li->next);
    free(li);
}

/* The function scanExpressions is not in the lecture notes.
 * It demonstrates the scanner.
 */

void scanExpressions() {
    char *ar;
    List li;
    printf("give an expression: ");
    ar = readInput();
    while (ar[0] != '!') {
	li = tokenList(ar);
	printList(li);
	free(ar);
	freeTokenList(li);
	printf("\ngive an expression: ");
	ar = readInput();
    }
    free(ar);
    printf("good bye\n");
}
