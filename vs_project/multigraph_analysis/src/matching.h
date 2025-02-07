#ifndef MATCHING_H
#define MATCHING_H

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <limits.h>
#include "stack.h"
#include "cycle_list.h"

#define INF 1000000


int find_maximum_weight_matching(int** adj_matrix, int n, int* match);

int find_maximum_weight_bipartite_matching(int** cost, int n, int* matchR);

#endif /* MATCHING_H */
