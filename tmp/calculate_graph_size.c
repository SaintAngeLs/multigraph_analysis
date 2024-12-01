#include <math.h>

float graph_size(int* graph, int n) {
    int result = n*n;
    for (int i = 0; i < n; ++i) {
        int deg = 0;
        for (int j = 0; j < n; ++j) {
            deg += graph[i*n+j];
        }
        result += deg*deg;
    }
    return sqrtf(result);
}
