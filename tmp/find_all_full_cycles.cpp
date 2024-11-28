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

    explicit ElementaryCyclesSearch(int n) :
        blocked(n), B(n) {}

    bool findCycles(int v, int s, int n) {
        bool f = false;
        stack.push_back(v);
        blocked[v] = true;

        for (int w = 0; w < n; ++w) {
            // force the algorithm to process full cycles
            if (w == s && stack.size() == n) { 
                std::vector<int> cycle;
                for (int now : stack) {
                    cycle.push_back(now);
                }
                cycles.push_back(cycle);
                f = true;
            } else if (!blocked[w]) {
                if (findCycles(w, s, n)) {
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

        stack.erase(std::find(stack.begin(), stack.end(), v));
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
    int N;
    std::cerr << "Enter N: ";
    std::cin >> N;

    ElementaryCyclesSearch ecs(N);
    ecs.findCycles(0, 0, N); 
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
