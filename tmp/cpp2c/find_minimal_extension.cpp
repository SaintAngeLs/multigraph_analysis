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

struct StrongConnectedComponents {
    std::vector<int> adjListOriginal; // size!
    std::vector<int> adjList;  // size!
    std::vector<int> visited;
    std::forward_list<int> stack;
    std::vector<int> lowlink;
    std::vector<int> number;
    int sccCounter;
    std::vector<std::vector<int>> currentSCCs;

    StrongConnectedComponents(const std::vector<int>& adjList) {
        adjListOriginal = adjList;
        sccCounter = 0;
        visited;
        stack;
        lowlink;
        number;
        currentSCCs;
    }

    void getAdjacencyList(int node, unsigned adjListOriginal_size, std::vector<int>& sccres_adjList, int& sccres_lowestNodeId) {
        auto alos = adjListOriginal_size;
        
        visited.clear();
        visited.resize(alos);
        
        lowlink.clear();
        lowlink.resize(alos);
        
        number.clear();
        number.resize(alos);
        

        stack.clear();
        
        currentSCCs.clear();

        makeAdjListSubgraph(node);

        for (int i = node; i < adjListOriginal_size; ++i) {
            if (!visited[i]) {
                getStrongConnectedComponents(i);
                std::vector<int> nodes = getLowestIdComponent();
                if (!nodes.empty() && std::find(nodes.begin(), nodes.end(), node) == nodes.end() &&
                                      std::find(nodes.begin(), nodes.end(), node+1) == nodes.end()) {
                    getAdjacencyList(node + 1, sccres_adjList, sccres_lowestNodeId);
                } else {
                    std::vector<int> adjList1 = getAdjList(nodes);
                    if (!adjList1.empty()) { // exists?
                        for (int j = 0; j < adjListOriginal_size; ++j) {
                            if (adjList1[j].size() > 0) { // exists?
                                sccres_adjList = adjList1;
                                sccres_lowestNodeId = j;
                            }
                        }
                    }
                }
            }
        }
    }

    void makeAdjListSubgraph(int node, unsigned adjListOriginal_size) {
        adjList.clear();
        adjList.resize(adjListOriginal_size);

        for (int i = node; i < adjListOriginal_size; ++i) {
            std::vector<std::pair<int, int>> successors;
            for (auto j : adjListOriginal[i]) {
                if (j.first >= node) {
                    successors.push_back(j);
                }
            }

            if (successors.size() > 0) {
                adjList[i].clear();
                for (int j = 0; j < successors.size(); ++j) {
                    auto succ = successors[j];
                    adjList[i][succ.first] = succ.second;
                }
            }
        }
    }

    std::vector<int> getLowestIdComponent() {
        int min = adjList.size();
        std::vector<int> currScc;

        for (int i = 0; i < currentSCCs.size(); ++i) {
            std::vector<int> scc = currentSCCs[i];
            for (int j = 0; j < scc.size(); ++j) {
                int node = scc[j];
                if (node < min) {
                    currScc = scc;
                    min = node;
                }
            }
        }

        return currScc;
    }

    std::vector<std::map<int, int>> getAdjList(const std::vector<int>& nodes) {
        std::vector<std::map<int, int>> lowestIdAdjacencyList;

        if (!nodes.empty()) {
            lowestIdAdjacencyList.clear();
            lowestIdAdjacencyList.resize(adjList.size());
            for (int i = 0; i < lowestIdAdjacencyList.size(); ++i) {
                lowestIdAdjacencyList[i].clear();
            }
            for (int i = 0; i < nodes.size(); ++i) {
                int node = nodes[i];
                for (auto j : adjList[node]) {
                    if (std::find(nodes.begin(), nodes.end(), j.first) != nodes.end()) {
                        lowestIdAdjacencyList[node].insert(j);
                    }   
                }
            }
        }

        return lowestIdAdjacencyList;
    }

    void getStrongConnectedComponents(int root) {
        ++sccCounter;
        lowlink[root] = sccCounter;
        number[root] = sccCounter;
        visited[root] = true;
        stack.push_front(root);

        for (auto w : adjList[root]) {
            if (!visited[w.first]) {
                getStrongConnectedComponents(w.first);
                lowlink[root] = std::min(lowlink[root], lowlink[w.first]);
            } else if (number[w.first] < number[root]) {
                if (std::find(stack.begin(), stack.end(), w.first) != stack.end())
                    lowlink[root] = std::min(lowlink[root], number[w.first]);
            }
        }
    

        if ((lowlink[root] == number[root]) && !stack.empty()) {
            int next = -1;
            std::vector<int> scc;

            do {
                next = stack.front();
                stack.pop_front();
                scc.push_back(next);
            } while (number[next] > number[root]);

            if (scc.size() > 1) {
                currentSCCs.push_back(scc);
            }
        }
    }

};


struct ElementaryCyclesSearch {
    int cycle_count = 0;
    std::vector<std::map<int, int>> adjList;
    std::vector<int> graphNodes;
    std::vector<bool> blocked;
    std::vector<std::vector<int>> B;
    std::list<int> stack;

    int max_cycle_size = 0;
    int max_cycle_width = 0;

    explicit ElementaryCyclesSearch(const std::vector<std::vector<int>>& matrix) :
        graphNodes(matrix.size()) {
        
        adjList = AdjacencyList::getAdjList(matrix);
        for (int i = 0; i < matrix.size(); ++i) {
            graphNodes[i] = i;
        }
    }

    void getElementaryCycles() {
        //std::cerr << "E";
        blocked.clear();
        blocked.resize(adjList.size());
        B.clear();
        B.resize(adjList.size());
        StrongConnectedComponents sccs(adjList);
        int s = 0;

        while (true) {
            SCCResult sccResult = sccs.getAdjacencyList(s);
            if (!sccResult.adjList.empty()) {
                const auto& scc = sccResult.adjList;
                s = sccResult.lowestNodeId;
                for (int j = 0; j < scc.size(); ++j) {
                    if (!scc[j].empty()) {
                        B[j].clear();
                        blocked[j] = false;
                    }
                }

                findCycles(s, s, scc);
                ++s;
            } else {
                break;
            }
        }
    }

    bool findCycles(int v, int s, const std::vector<std::map<int, int>>& adjList) {
        bool f = false;
        stack.push_back(v);
        blocked[v] = true;

        for (auto w : adjList[v]) {
            // force the algorithm not to count smaller cycles
            if (w.first == s) {
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
                    curr_min_width = std::min(curr_min_width, adjList[prev].find(now)->second);
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
            } else if (!blocked[w.first]) {
                if (findCycles(w.first, s, adjList)) {
                    f = true;
                }
            }
        }

        if (f) {
            unblock(v);
        } else {
            for (auto w : adjList[v]) {
                if (std::find(B[w.first].begin(), B[w.first].end(), v) == B[w.first].end()) {
                    B[w.first].push_back(v);
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
                    //cycles.clear();
                    cycle_count = 0;
                    min_nr_lacking = nr_lacking;
                }
                if (nr_lacking <= min_nr_lacking && second_stage) {
                    //std::cerr << "L";
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
