#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stddef.h>
#include <math.h>

void init_sum(float* arr, int n) {
    for (int i = 1; i < n; ++i) {
       arr[i] += arr[i-1];
    }
}

int bin_search(float* arr, int n, float v) {
    if (arr[n-1] == 0) {
        assert(0);
        return n;
    }
    if (v <= arr[0]) return 0;
    int l = n;
    int f = 0; 
    while (1) {
        int m = f + (l - f - 1)/2;
        if (v > arr[m] && v <= arr[m+1]) {
            return m + 1;
        }
        if (v <= arr[m]) {
            l = m + 1;
        } else {
            f = m;
        }
    }
}

int arr_find(int* arr, int n, int v) {
    for (int i = 0; i < n; ++i) {
        if (v == arr[i]) {
            return i; 
        }
    }
    return n;
}

float ant_algo(int* graph, int n, int N1, float p_nondistinct, float ph_att, float Q, float a, float b, float c, float d, float e, int is_min_ext) {
    float* curr_prob = malloc(sizeof(float)*n);
    if (!curr_prob) return 0;
    
    int* sequence = malloc(sizeof(float)*n*(n+1));
    if (!sequence) return 0;
    
    int* seq_size = malloc(sizeof(int)*n);
    if (!seq_size) return 0;

    float* seq_value = malloc(sizeof(int)*n);
    if (!seq_value) return 0;

    float* pheromone = malloc(sizeof(float)*n*n);
    if (!pheromone) return 0;
    
    for (int i = 0; i < n*n; ++i) {
        pheromone[i] = 1;
    }

    for (int k1 = 0; k1 < N1; ++k1) {
        for (int k = 0; k < n; ++k) {
            int curr = k;
            sequence[(n+1)*k+0] = curr;
            for (int l = 1; l <= n; ++l) {
                memcpy(curr_prob, pheromone + n*curr, sizeof(float)*n);

                for (int i = 0; i < n; ++i) {
                    curr_prob[i] = powf(curr_prob[i], (is_min_ext ? 1 : 1));
                }

                if (is_min_ext) {
                    for (int i = 0; i < n; ++i) {
                        curr_prob[i] *= (a + b * graph[n*curr + i]) / (d + c * graph[n*curr + i]);
                    }
                    for (int i = 0; i < l; ++i) {
                        if (!(l == n && i == 0))
                            curr_prob[sequence[k*(n+1)+i]] = 0;
                    } 
                } else {
                    for (int i = 0; i < n; ++i) {
                        curr_prob[i] *= e * graph[n*curr + i];
                    }
                    for (int i = 0; i < l; ++i) {
                        curr_prob[sequence[k*(n+1)+i]] *= p_nondistinct;
                    }
                }

                init_sum(curr_prob, n);

                float sum = curr_prob[n-1];
                float r = (float)rand() / RAND_MAX;
                r *= sum;

                int next = bin_search(curr_prob, n, r);
                
                if (is_min_ext) {
                    seq_value[k] += !!graph[curr*n + next];
                }

                curr = next;
                sequence[(n+1)*k+l] = next;

                if (arr_find(&sequence[(n+1)*k+0], l, curr) != l) {
                    seq_size[k] = l;
                    break;
                }

                if (l == n) assert(0);
            }
        }
        
        for (int i = 0; i < n*n; ++i) {
            pheromone[i] *= ph_att;
        }
        if (is_min_ext) {     
            for (int k = 0; k < n; ++k) {
                for (int i = 0; i < seq_size[k]; ++i) {
                    int out = sequence[k*(n+1)+i];
                    int in  = sequence[k*(n+1)+i+1];
                    pheromone[out*n + in] += Q * seq_value[k] / n;
                }
            }
        } else {
           for (int k = 0; k < n; ++k) {
                for (int i = 0; i < seq_size[k]; ++i) {
                    int out = sequence[k*(n+1)+i];
                    int in  = sequence[k*(n+1)+i+1];
                    pheromone[out*n + in] += Q * seq_size[k] / n;
                }
            } 
        }
    }

    float result = 1;
    for (int i = 0; i < n; ++i) {
        float t = 0;
        float t2 = 0;
        int deg = 0;
        for (int j = 0; j < n; ++j) {
            t += pheromone[n*i+j];
            t2 += pheromone[n*i+j] * pheromone[n*i+j];
            deg += !!graph[n*i+j];
        }
        if (deg && t > 0 && t2 > 0)
            result *= t2 / t / t / deg;
    }

    return result;
}

int main() {
    const int N = 6;
    int graph[36] = {
        0,2,0,0,0,0,
        0,0,2,0,0,0,
        2,0,0,0,0,0,
        0,0,0,0,0,2,
        0,0,0,2,0,0,
        0,0,0,0,2,0,
    };

    printf("Nr of longest cycles: %f\n", ant_algo(graph, N, 20, 0.1f, 0.5f, 1, 0.2f, 1, 1, 0.5f, 1, 0));
    printf("Nr of Hamiltonian cycles in min ext: %f\n", ant_algo(graph, N, 20, 0.1f, 0.5f, 1, 0.2f, 1, 1, 0.5f, 1, 1));

    return 0;
}
