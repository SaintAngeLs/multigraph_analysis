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

struct ElementaryCyclesSearch {
    std::list<std::vector<int>> cycles;
    std::vector<bool> blocked;
    std::vector<std::vector<int>> B;
    std::list<int> stack;

    int nr_lacking = 0;
    int min_nr_lacking;

    explicit ElementaryCyclesSearch(int n) :
        blocked(n), B(n), min_nr_lacking(n*n) {}

    bool findCycles(int v, int s, int n, const std::vector<std::vector<int>>& adjMatrix) {
        bool f = false;

        int prev_stack_back = stack.empty() ? 0 : stack.back();
        if (!stack.empty() && !adjMatrix[prev_stack_back][v]) {
            ++nr_lacking;
        }

        if (nr_lacking <= min_nr_lacking) {
            stack.push_back(v);
            blocked[v] = true;

            for (int w = 0; w < n; ++w) {
                // force the algorithm to process full cycles
                if (w == s && stack.size() == n) {
                    if (nr_lacking < min_nr_lacking) {
                        cycles.clear();
                        min_nr_lacking = nr_lacking;
                    }

                    if (nr_lacking <= min_nr_lacking) {
                        std::vector<int> cycle;
                        for (int now : stack) {
                            cycle.push_back(now);
                        }
                        cycles.push_back(cycle);
                    }
                    f = true;
                } else if (!blocked[w]) {
                    if (findCycles(w, s, n, adjMatrix)) {
                        f = true;
                    }
                }
            }

            if (f) {
                unblock(v);
            } else {
                for (int w = 0; w < n; ++w) {
                    if (std::find(B[w].begin(), B[w].end(), v) == B[w].end()) {
                        B[w].push_back(v);
                    }
                }
            }
        }

        if (nr_lacking > min_nr_lacking) {
            --nr_lacking;
            return f;
        }

        if (stack.size() > 1 && !adjMatrix[prev_stack_back][stack.back()]) {
            --nr_lacking;
        }

        stack.pop_back();
        return f;
    }

    void unblock(int node) {
        blocked[node] = false;
        auto Bnode = B[node];
        while (Bnode.size() > 0) {
            int w = Bnode[0];
            Bnode.erase(std::find(Bnode.begin(), Bnode.end(), w));
            if (blocked[w]) {
                unblock(w);
            }
        }
    }
};

int main() {

    // more than 11 takes too much RAM
    const int N = 6;

    std::vector<std::vector<int>> adjMatrix(N, std::vector<int>(N, 0));
   
    adjMatrix[4][3] = 1;
    adjMatrix[2][3] = 1;

    //std::fill(adjMatrix[0].begin(), adjMatrix[0].end(), 0);
    //std::fill(adjMatrix[1].begin(), adjMatrix[1].end(), 0);
    //std::fill(adjMatrix[2].begin(), adjMatrix[2].end(), 0);
    //std::fill(adjMatrix[3].begin(), adjMatrix[3].end(), 0);
    //std::fill(adjMatrix[4].begin(), adjMatrix[4].end(), 0);

    ElementaryCyclesSearch ecs(N);
    ecs.findCycles(0, 0, N, adjMatrix); 
    auto& cycles = ecs.cycles;
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
