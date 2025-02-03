#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include "stack.h"

void initStack(Stack* stack, int capacity) {
    stack->data = (int*)malloc(capacity * sizeof(int));
    stack->top = -1;
    stack->capacity = capacity;
}

bool isStackEmpty(Stack* stack) {
    return stack->top == -1;
}

bool isStackFull(Stack* stack) {
    return stack->top == stack->capacity - 1;
}

void push(Stack* stack, int value) {
    if (isStackFull(stack)) return;
    stack->data[++stack->top] = value;
}

int pop(Stack* stack) {
    if (isStackEmpty(stack)) return -1;
    return stack->data[stack->top--];
}

int peek(Stack* stack) {
    if (isStackEmpty(stack)) return -1;
    return stack->data[stack->top];
}

void freeStack(Stack* stack) {
    free(stack->data);
}
