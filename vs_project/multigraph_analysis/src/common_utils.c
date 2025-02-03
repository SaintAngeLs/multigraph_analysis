#include "common_utils.h"
#include <math.h>

int common_min(int a, int b) {
    return (a < b) ? a : b;
}

int common_max(int a, int b) {
    return (a > b) ? a : b;
}

void common_swap(int *a, int *b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}

double common_exp(double value_argument) {
    return exp(value_argument);
}
