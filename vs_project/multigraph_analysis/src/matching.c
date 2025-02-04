#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <limits.h>
#include <string.h>
#include "matching.h"

#define INF 1000000

int find_maximum_weight_matching(int** adj_matrix, int n, int* match) {
    bool* used = (bool*)calloc(n, sizeof(bool));

    for (int i = 0; i < n; i++) match[i] = -1;

    for (int i = 0; i < n; i++) {
        if (!used[i]) {
            int best_j = -1, max_weight = -1;
            for (int j = 0; j < n; j++) {
                if (!used[j] && adj_matrix[i][j] > max_weight) {
                    max_weight = adj_matrix[i][j];
                    best_j = j;
                }
            }
            if (best_j != -1) {
                match[i] = best_j;
                match[best_j] = i;
                used[i] = used[best_j] = true;
            }
        }
    }
    free(used);
    return 1;
}
