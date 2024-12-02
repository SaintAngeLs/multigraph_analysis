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

struct AdjacencyList {

static std::vector<std::map<int, int>> getAdjList(const std::vector<std::vector<int>>& adjMatrix) {
    std::vector<std::map<int, int>> list(adjMatrix.size());

    for (int i = 0; i < adjMatrix.size(); ++i) {
        for (int j = 0; j < adjMatrix[i].size(); ++j) {
            if (adjMatrix[i][j]) {
                list[i].insert(std::pair<int,int>(j, adjMatrix[i][j]));
            }
        }
    }

    return list;
}

};

struct SCCResult {
    std::unordered_set<int> nodeIdsOfSCC;
    std::vector<std::map<int, int>> adjList;
    int lowestNodeId = -1;

    SCCResult(const std::vector<std::map<int, int>>& adjList, int lowestNodeId) : adjList(adjList), lowestNodeId(lowestNodeId) {
        //if (adjList.size() > 0)
        for (int i = lowestNodeId; i < adjList.size(); ++i) {
            if (adjList[i].size() > 0) {
                nodeIdsOfSCC.insert(i);
            }
        }
    }

    SCCResult() {
        
    }
};

struct StrongConnectedComponents {
    std::vector<std::map<int, int>> adjListOriginal;
    std::vector<std::map<int, int>> adjList;
    std::vector<bool> visited;
    std::forward_list<int> stack;
    std::vector<int> lowlink;
    std::vector<int> number;
    int sccCounter = 0;
    std::vector<std::vector<int>> currentSCCs;

    StrongConnectedComponents(const std::vector<std::map<int, int>>& adjList) {
        adjListOriginal = adjList;
    }

    SCCResult getAdjacencyList(int node) {
        auto alos = adjListOriginal.size();
        visited.clear();
        visited.resize(alos);
        lowlink.clear();
        lowlink.resize(alos);
        number.clear();
        number.resize(alos);
        stack.clear();
        currentSCCs.clear();

        makeAdjListSubgraph(node);

        for (int i = node; i < adjListOriginal.size(); ++i) {
            if (!visited[i]) {
                getStrongConnectedComponents(i);
                std::vector<int> nodes = getLowestIdComponent();
                if (!nodes.empty() && std::find(nodes.begin(), nodes.end(), node) == nodes.end() &&
                                      std::find(nodes.begin(), nodes.end(), node+1) == nodes.end()) {
                    return getAdjacencyList(node + 1);
                } else {
                    std::vector<std::map<int, int>> adjList1 = getAdjList(nodes);
                    if (!adjList1.empty()) {
                        for (int j = 0; j < adjListOriginal.size(); ++j) {
                            if (adjList1[j].size() > 0) {
                                return SCCResult(adjList1, j);
                            }
                        }
                    }
                }
            }
        }

        return {};
    }

    void makeAdjListSubgraph(int node) {
        adjList.clear();
        adjList.resize(adjListOriginal.size());

        for (int i = node; i < adjList.size(); ++i) {
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
    std::list<std::vector<int>> cycles;
    std::vector<std::map<int, int>> adjList;
    std::vector<int> graphNodes;
    std::vector<bool> blocked;
    std::vector<std::vector<int>> B;
    std::list<int> stack;

    int max_cycle_size = 0;
    int max_cycle_width = 0;

    ElementaryCyclesSearch(const std::vector<std::vector<int>>& matrix, const std::vector<int>& graphNodes) :
        graphNodes(graphNodes) {
        
        adjList = AdjacencyList::getAdjList(matrix);
    }

    std::list<std::vector<int>> getElementaryCycles() {
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


        return cycles;
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
                    cycles.clear();
                    max_cycle_size = stack.size();
                    max_cycle_width = 0;
                }

                std::vector<int> cycle;
                int prev = stack.back();
                int curr_min_width = INT_MAX;
                for (int now : stack) {
                    curr_min_width = std::min(curr_min_width, adjList[prev].find(now)->second);
                    if (curr_min_width < max_cycle_width) {
                        cycle.clear();
                        break;
                    }
                    cycle.push_back(graphNodes[now]);
                    prev = now;
                }

                if (curr_min_width > max_cycle_width) {
                    cycles.clear();
                    max_cycle_width = curr_min_width;
                }

                if (!cycle.empty())
                    cycles.push_back(cycle);
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
        //stack.pop_back();
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
    adjMatrix[7][4] = 1;

    adjMatrix[3][5] = 1;
    adjMatrix[4][2] = 1;

    ElementaryCyclesSearch ecs(adjMatrix, nodes);
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
