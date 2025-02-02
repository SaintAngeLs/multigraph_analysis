#ifndef STACK_H
#define STACK_H

#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>

typedef struct {
    int* data;
    int top;
    int capacity;
} Stack;

void initStack(Stack* stack, int capacity);
bool isStackEmpty(Stack* stack);
bool isStackFull(Stack* stack);
void push(Stack* stack, int value);
int pop(Stack* stack);
int peek(Stack* stack);
void freeStack(Stack* stack);

#endif // STACK_H
