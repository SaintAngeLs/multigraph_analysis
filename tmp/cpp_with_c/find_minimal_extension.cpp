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

struct ElementaryCyclesSearch {
    int cycle_count = 0;
    std::vector<std::vector<int>> adjMatrix;
    std::vector<int> graphNodes;
    std::vector<bool> blocked;
    std::vector<std::vector<int>> B;
    std::list<int> stack;

    int max_cycle_size = 0;
    int max_cycle_width = 0;

    explicit ElementaryCyclesSearch(const std::vector<std::vector<int>>& matrix) :
        graphNodes(matrix.size()) {
       
        adjMatrix = matrix;
        for (int i = 0; i < matrix.size(); ++i) {
            graphNodes[i] = i;
        }
    }

    void getElementaryCycles() {
        //std::cerr << "E";
        blocked.clear();
        blocked.resize(adjMatrix.size());
        B.clear();
        B.resize(adjMatrix.size());
        int s = 0;

        findCycles(s, s, adjMatrix);
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

                // indent
                if (stack.size() > max_cycle_size) {
                    cycle_count = 0;
                    max_cycle_size = stack.size();
                    max_cycle_width = 0;
                }

                bool found_cycle = true;
                int prev = stack.back();
                int curr_min_width = INT_MAX;
                for (int now : stack) {
                    curr_min_width = std::min(curr_min_width, adjMatrix[prev][now]);
                    if (curr_min_width < max_cycle_width) {
                        found_cycle = false;
                        break;
                    }
                    prev = now;
                }

                if (curr_min_width > max_cycle_width) {
                    cycle_count = 0;
                    max_cycle_width = curr_min_width;
                }

                if (found_cycle)
                    ++cycle_count;
                // !indent

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

struct MinExtensionSearch {
    std::list<std::vector<int>> cycles;
    std::size_t cycle_count = 0;

    std::vector<bool> blocked;
    std::vector<std::vector<int>> B;
    std::list<int> stack;

    int nr_lacking = 0;
    int min_nr_lacking;

    int max_nr_ham_cycles = 0;

    explicit MinExtensionSearch(int n) :
        blocked(n), B(n), min_nr_lacking(n*n) {}

    bool findCycles(int v, int s, int n, const std::vector<std::vector<int>>& adjMatrix, bool second_stage) {
        bool f = false;


        int prev_stack_back = stack.empty() ? 0 : stack.back();
        if (!stack.empty() && !adjMatrix[prev_stack_back][v]) {
            ++nr_lacking;
        }

        if (nr_lacking > min_nr_lacking) {
            --nr_lacking;
            return true;
        }

        stack.push_back(v);
        blocked[v] = true;

        for (int w = 0; w < n; ++w) {
            // force the algorithm to process full cycles
            if (w == s && stack.size() == n) {

                if (!adjMatrix[stack.back()][w]) {
                    ++nr_lacking;
                }

                if (nr_lacking < min_nr_lacking) {
                    cycle_count = 0;
                    min_nr_lacking = nr_lacking;
                }
                if (nr_lacking <= min_nr_lacking && second_stage) {
                    std::vector<std::vector<int>> adjMatrixExtended{ adjMatrix };
                    int prev = stack.back();
                    for (int now : stack) {
                        if (!adjMatrixExtended[prev][now]) {
                            adjMatrixExtended[prev][now] = 1;
                        }
                        prev = now;
                    }

                    ElementaryCyclesSearch hamSearch{ adjMatrixExtended };
                    hamSearch.getElementaryCycles();
                    if (hamSearch.cycle_count >= max_nr_ham_cycles) {
                        if (hamSearch.cycle_count > max_nr_ham_cycles) {
                            cycle_count = 0;
                            max_nr_ham_cycles = hamSearch.cycle_count;
                        }
                        ++cycle_count;
                    }
                }

                if (!adjMatrix[stack.back()][w]) {
                    --nr_lacking;
                }

                f = true;
            } else if (!blocked[w]) {
                if (findCycles(w, s, n, adjMatrix, second_stage)) {
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

        if (stack.size() > 1 && !adjMatrix[prev_stack_back][stack.back()]) {
            --nr_lacking;
        }

        stack.pop_back();
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

    std::vector<int> nodes(N);
    for (int i = 0; i < N; ++i) {
        nodes[i] = i+1000;
    }

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
    adjMatrix[6][5] = 1;

    MinExtensionSearch ecs(N);
    ecs.findCycles(0, 0, N, adjMatrix, false);
    ecs.cycle_count = 0;
    ecs.B.clear(); ecs.B.resize(N); ecs.blocked.clear(); ecs.blocked.resize(N);
    std::cerr << "Second stage: min nr lacking = " << ecs.min_nr_lacking << std::endl;
    ecs.findCycles(0, 0, N, adjMatrix, true);
    std::cout << "cycle count: " << ecs.cycle_count << std::endl;
    std::cout << "max ham cycles: " << ecs.max_nr_ham_cycles << std::endl;

    return 0;
}
