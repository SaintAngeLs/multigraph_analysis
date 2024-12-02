// Converted from Java to C++

// http://normalisiert.de/code/java/elementaryCycles.zip
// http://www.cs.tufts.edu/comp/150GA/homeworks/hw1/Johnson%2075.PDF
// 27 Nov 2024

#include <iostream>
#include <set>
#include <map>
#include <unordered_set>
#include <unordered_map>
#include <array>
#include <list>
#include <vector>
#include <forward_list>
#include <cstdlib>
#include <cstdint>
#include <cstddef>
#include <utility>
#include <algorithm>
#include <cassert>
#include <climits>

#include "../scc_tarjan.h"

struct ElementaryCyclesSearch {
    std::list<std::vector<int>> cycles;
    std::vector<std::vector<int>> adjMatrix;
    std::vector<int> graphNodes;
    std::vector<bool> blocked;
    std::vector<std::vector<int>> B;
    std::list<int> stack;

    int max_cycle_size = 0;
    int max_cycle_width = 0;

    ElementaryCyclesSearch(const std::vector<std::vector<int>>& matrix) {
        adjMatrix = matrix;
    }

    std::list<std::vector<int>> getElementaryCycles() {
        blocked.clear();
        blocked.resize(adjMatrix.size());
        B.clear();
        B.resize(adjMatrix.size());
        
        auto& matrix = adjMatrix;

        std::vector<int> unfolded(matrix.size()*matrix.size());
        for (int i = 0; i < matrix.size(); ++i) {
            for (int j = 0; j < matrix.size(); ++j) {
                unfolded[i*matrix.size()+j] = matrix[i][j];
            }
        } 

        std::vector<int> disc(100);
        std::vector<int> low(100);
        std::vector<int> stackMember(100);
        std::vector<int> stack__(100);
        int top = -1;
        int time = 0;
        
        int label = 0;
        std::vector<int> results(100);
        SCC(unfolded.data(), matrix.size(), disc.data(), low.data(), stackMember.data(), stack__.data(), &top, &time, results.data(), &label);

        std::vector<std::vector<int>> scc(matrix.size(), std::vector<int>(matrix.size(), 0));
        for (int ss = 0; ss < matrix.size(); ++ss) {
            bool exists = false;
            int s;
            scc = std::vector<std::vector<int>>(matrix.size(), std::vector<int>(matrix.size(), 0));
            for (int k = 0; k < matrix.size(); ++k) {
                if (results[k] == ss) {
                    if (!exists) {
                        exists = true;
                        s = k;
                    }
                    for (int l = 0; l < matrix.size(); ++l) {
                        if (results[l] == ss) {
                            scc[k][l] = matrix[k][l];
                        }
                    }
                    B[k].clear();
                    blocked[k] = false;
                }
            }
            if (exists) {
                findCycles(s, s, scc);
            }
        }

        return cycles;
    }

    bool findCycles(int v, int s, const std::vector<std::vector<int>>& adjMatrix) {
        bool f = false;
        stack.push_back(v);
        blocked[v] = true;

        for (int w = 0; w < adjMatrix.size(); ++w) {
            int ww = adjMatrix[v][w];
            if (!ww) continue;

            // force the algorithm not to count smaller cycles
            if (w == s) {
                if (stack.size() >= max_cycle_size) {
                    if (stack.size() > max_cycle_size) {
                        cycles.clear();
                        max_cycle_size = stack.size();
                        max_cycle_width = 0;
                    }

                    std::vector<int> cycle;
                    int prev = stack.back();
                    int curr_min_width = INT_MAX;
                    for (int now : stack) {
                        curr_min_width = std::min(curr_min_width, adjMatrix[prev][now]);
                        if (curr_min_width < max_cycle_width) {
                            cycle.clear();
                            break;
                        }
                        cycle.push_back(now);
                        prev = now;
                    }

                    if (curr_min_width > max_cycle_width) {
                        cycles.clear();
                        max_cycle_width = curr_min_width;
                    }

                    if (!cycle.empty())
                        cycles.push_back(cycle);
                }

                f = true;
            } else if (!blocked[w]) {
                if (findCycles(w, s, adjMatrix)) {
                    f = true;
                }
            }
        }

        if (f) {
            unblock(v);
        } else {
            for (int w = 0; w < adjMatrix.size(); ++w) {
                int ww = adjMatrix[v][w];
                if (!ww) continue;

                if (std::find(B[w].begin(), B[w].end(), v) == B[w].end()) {
                    B[w].push_back(v);
                }
            }
        }

        stack.erase(std::find(stack.begin(), stack.end(), v));
        return f;
    }

    void unblock(int node) {
        blocked[node] = false;
        auto& Bnode = B[node];
        while (Bnode.size() > 0) {
            int w = Bnode[0];
            Bnode.erase(Bnode.begin());
            if (blocked[w]) {
                unblock(w);
            }
        }
    }
};

int main() {

    // more than 11 takes too much RAM
    int N = 8;
    std::vector<std::vector<int>> adjMatrix(N, std::vector<int>(N, 0));
    
    adjMatrix[0][1] = 1;
    adjMatrix[1][0] = 1;
    adjMatrix[0][2] = 1;
    adjMatrix[2][0] = 1;
    adjMatrix[1][3] = 1;
    adjMatrix[3][1] = 1;
    adjMatrix[2][3] = 1;
    adjMatrix[3][2] = 1;
    adjMatrix[0][3] = 1;
    adjMatrix[2][1] = 1;
    adjMatrix[4][5] = 1;
    adjMatrix[5][4] = 1;
    adjMatrix[4][6] = 1;
    adjMatrix[6][4] = 1;
    adjMatrix[6][7] = 1;
    adjMatrix[7][6] = 1;
    adjMatrix[5][7] = 1;
    adjMatrix[7][5] = 1;
    adjMatrix[5][6] = 1;
    adjMatrix[7][4] = 1;

    adjMatrix[3][5] = 1;
    adjMatrix[4][2] = 1;

    ElementaryCyclesSearch ecs(adjMatrix);
    std::list<std::vector<int>> cycles = ecs.getElementaryCycles();
    int i;
    std::list<std::vector<int>>::iterator it;
    std::cout << cycles.size() << std::endl;
    for (i = 0, it = cycles.begin(); i < cycles.size(); ++i, ++it) {
        std::vector<int> cycle = *it;
        for (int j = 0; j < cycle.size(); ++j) {
            int node = cycle[j];
            std::cout << node << " -> ";
        }
        std::cout << std::endl;
    }

    return 0;
}
