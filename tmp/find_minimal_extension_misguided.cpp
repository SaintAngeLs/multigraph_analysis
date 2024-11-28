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
    std::size_t cycle_count = 0;

    std::vector<bool> blocked;
    std::vector<std::vector<int>> B;
    std::list<int> stack;

    int nr_lacking = 0;
    int min_nr_lacking;

    int max_width = 1;
    int max_cycle_width = 1;
    std::vector<int> nr_lacking_per_width;

    explicit ElementaryCyclesSearch(int n) :
        blocked(n), B(n), min_nr_lacking(n*n) {}

    bool findCycles(int v, int s, int n, const std::vector<std::vector<int>>& adjMatrix, bool reject_narrower) {
        if (reject_narrower) {
            nr_lacking_per_width.resize(max_width + 1, 0);
        }

        bool f = false;

        int prev_stack_back = stack.empty() ? 0 : stack.back(); 

        if (!reject_narrower) {

        // indent
        
        if (!stack.empty()) { 
            max_width = std::max(max_width, adjMatrix[prev_stack_back][v]);
        }
        
        if (!stack.empty() && !adjMatrix[prev_stack_back][v]) {
            ++nr_lacking;
        }
       
        if (nr_lacking > min_nr_lacking) {
            --nr_lacking;
            return true;
        }
        // !indent

        }
        else {
            if (!stack.empty() && adjMatrix[prev_stack_back][v] < max_cycle_width) {
                nr_lacking_per_width[max_cycle_width] += (max_cycle_width - adjMatrix[prev_stack_back][v]);
            }

            if (nr_lacking_per_width[max_cycle_width] > min_nr_lacking) {
                nr_lacking_per_width[max_cycle_width] -= (max_cycle_width - adjMatrix[prev_stack_back][v]);
                return true;
            }

            for (int wh = std::max(adjMatrix[prev_stack_back][v] + 1, max_cycle_width + 1); wh <= max_width; ++wh) {
                nr_lacking_per_width[wh] += (wh - adjMatrix[prev_stack_back][v]);
            }
        }

        stack.push_back(v);
        blocked[v] = true;

        int old_max_cycle_width = max_cycle_width;

        for (int w = 0; w < n; ++w) {
            // force the algorithm to process full cycles
            if (w == s && stack.size() == n) {
                
                if (reject_narrower) {
                    if (adjMatrix[stack.back()][w] < max_cycle_width) {
                        nr_lacking_per_width[max_cycle_width] += (max_cycle_width - adjMatrix[stack.back()][w]);
                    }

                    for (int wh = std::max(adjMatrix[stack.back()][w] + 1, max_cycle_width + 1); wh <= max_width; ++wh) {
                        nr_lacking_per_width[wh] += (wh - adjMatrix[stack.back()][w]);
                    }

                    int new_max_cycle_width = max_cycle_width;

                    for (int wh = max_width; wh > max_cycle_width; --wh) {
                        if (nr_lacking_per_width[wh] <= min_nr_lacking) {
                            cycle_count = 0;
                            new_max_cycle_width = wh;
                            break;
                        }
                    }

                    if (nr_lacking_per_width[max_cycle_width] <= min_nr_lacking) {
                        ++cycle_count;
                    }

                    if (adjMatrix[stack.back()][w] < max_cycle_width) {
                        nr_lacking_per_width[max_cycle_width] -= (max_cycle_width - adjMatrix[stack.back()][w]);
                    }

                    for (int wh = std::max(adjMatrix[stack.back()][w] + 1, max_cycle_width + 1); wh <= max_width; ++wh) {
                        nr_lacking_per_width[wh] -= (wh - adjMatrix[stack.back()][w]);
                    }

                    for (int wh = new_max_cycle_width; wh <= max_width; ++wh) {
                        nr_lacking_per_width[wh] += (new_max_cycle_width - max_cycle_width);
                    }

                    max_cycle_width = new_max_cycle_width;

                } else {

                // indent
                max_width = std::max(max_width, adjMatrix[stack.back()][w]);

                if (!adjMatrix[stack.back()][w]) {
                    ++nr_lacking;
                }

                if (nr_lacking < min_nr_lacking) {
                    cycle_count = 0;
                    min_nr_lacking = nr_lacking;
                }

                if (nr_lacking <= min_nr_lacking) {
                    ++cycle_count;
                }

                if (!adjMatrix[stack.back()][w]) {
                    --nr_lacking;
                }
                // !indent

                }

                f = true;
            } else if (!blocked[w]) {
                if (findCycles(w, s, n, adjMatrix, reject_narrower)) {
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

        if (reject_narrower) {
            for (int wh = std::max(max_cycle_width, adjMatrix[prev_stack_back][stack.back()] + 1); wh <= max_width; ++wh) {
                nr_lacking_per_width[wh] -= (wh - adjMatrix[prev_stack_back][stack.back()]);
            }
        } else {
            if (stack.size() > 1 && !adjMatrix[prev_stack_back][stack.back()]) {
                --nr_lacking;
            }
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

    // more than 11 takes too much time
    const int N = 3;

    std::vector<std::vector<int>> adjMatrix(N, std::vector<int>(N, 0));
   
    adjMatrix[0][1] = 0;
    adjMatrix[1][2] = 5;
    adjMatrix[2][0] = 0;
    adjMatrix[1][0] = 0;
    adjMatrix[2][1] = 0;
    adjMatrix[0][2] = 0;
    

    //std::fill(adjMatrix[0].begin(), adjMatrix[0].end(), 0);
    //std::fill(adjMatrix[1].begin(), adjMatrix[1].end(), 0);
    //std::fill(adjMatrix[2].begin(), adjMatrix[2].end(), 0);
    //std::fill(adjMatrix[3].begin(), adjMatrix[3].end(), 0);
    //std::fill(adjMatrix[4].begin(), adjMatrix[4].end(), 0);

    ElementaryCyclesSearch ecs(N);
    //for (int i = 0; i < N; ++i)
    ecs.findCycles(0, 0, N, adjMatrix, false); 
    ecs.nr_lacking = 0;
    ecs.stack.clear();
    ecs.cycle_count = 0;
    ecs.blocked.clear();
    ecs.blocked.resize(N);
    ecs.B.clear();
    ecs.B.resize(N);
    ecs.findCycles(0, 0, N, adjMatrix, true);

    //auto& cycles = ecs.cycles;
    int i;
    std::list<std::vector<int>>::iterator it;
    std::cout << "Nr of cycles: " << ecs.cycle_count << std::endl;
    std::cout << "Cycle width: " << ecs.max_cycle_width << std::endl;
    std::cout << "Nr of edges to add: " << ecs.min_nr_lacking << std::endl;
    /*for (i = 0, it = cycles.begin(); i < cycles.size(); ++i, ++it) {
        std::vector<int> cycle = *it;
        for (int j = 0; j < cycle.size(); ++j) {
            int node = cycle[j];
            std::cout << node << " -> ";
        }
        std::cout << std::endl;
    }*/

    return 0;
}
