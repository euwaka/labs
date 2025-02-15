/* based on stackTypeAndFunctions.c by Gerard Renardel, 31 January 2018 */

#ifndef LIBSTACK_H
#define LIBSTACK_H

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

typedef struct Stack {
  int *array;
  int top;
  int size;
} Stack;

Stack newStack(int s);

void doubleStackSize(Stack *stp);
int isEmptyStack(Stack st);
void stackEmptyError();
void push(int value, Stack *stp);
int pop(Stack *stp);
void freeStack(Stack st);

#endif
