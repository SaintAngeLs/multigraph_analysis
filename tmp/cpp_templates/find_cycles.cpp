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

struct AdjacencyList {

static std::vector<std::vector<int>> getAdjList(const std::vector<std::vector<bool>>& adjMatrix) {
    std::vector<std::vector<int>> list(adjMatrix.size());

    for (int i = 0; i < adjMatrix.size(); ++i) {
        std::vector<int> v;
        for (int j = 0; j < adjMatrix[i].size(); ++j) {
            if (adjMatrix[i][j]) {
                v.push_back(j);
            }
        }
        list[i].clear();
        list[i].resize(v.size());
        for (int j = 0; j < v.size(); ++j) {
            int in = v[j];
            list[i][j] = in;
        }
    }

    return list;
}

};

struct SCCResult {
    std::unordered_set<int> nodeIdsOfSCC;
    std::vector<std::vector<int>> adjList;
    int lowestNodeId = -1;

    SCCResult(const std::vector<std::vector<int>>& adjList, int lowestNodeId) : adjList(adjList), lowestNodeId(lowestNodeId) {
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
    std::vector<std::vector<int>> adjListOriginal;
    std::vector<std::vector<int>> adjList;
    std::vector<bool> visited;
    std::forward_list<int> stack;
    std::vector<int> lowlink;
    std::vector<int> number;
    int sccCounter = 0;
    std::vector<std::vector<int>> currentSCCs;

    StrongConnectedComponents(const std::vector<std::vector<int>>& adjList) {
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
                    std::vector<std::vector<int>> adjList1 = getAdjList(nodes);
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
            std::vector<int> successors;
            for (int j = 0; j < adjListOriginal[i].size(); ++j) {
                if (adjListOriginal[i][j] >= node) {
                    successors.push_back(adjListOriginal[i][j]);
                }
            }

            if (successors.size() > 0) {
                adjList[i].clear();
                adjList[i].resize(successors.size());
                for (int j = 0; j < successors.size(); ++j) {
                    int succ = successors[j];
                    adjList[i][j] = succ;
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

    std::vector<std::vector<int>> getAdjList(const std::vector<int>& nodes) {
        std::vector<std::vector<int>> lowestIdAdjacencyList;

        if (!nodes.empty()) {
            lowestIdAdjacencyList.clear();
            lowestIdAdjacencyList.resize(adjList.size());
            for (int i = 0; i < lowestIdAdjacencyList.size(); ++i) {
                lowestIdAdjacencyList[i].clear();
            }
            for (int i = 0; i < nodes.size(); ++i) {
                int node = nodes[i];
                for (int j = 0; j < adjList[node].size(); ++j) {
                    int succ = adjList[node][j];
                    if (std::find(nodes.begin(), nodes.end(), succ) != nodes.end()) {
                        lowestIdAdjacencyList[node].push_back(succ);
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

        for (int i = 0; i < adjList[root].size(); ++i) {
            int w = adjList[root][i];
            if (!visited[w]) {
                getStrongConnectedComponents(w);
                lowlink[root] = std::min(lowlink[root], lowlink[w]);
            } else if (number[w] < number[root]) {
                if (std::find(stack.begin(), stack.end(), w) != stack.end())
                    lowlink[root] = std::min(lowlink[root], number[w]);
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
    std::vector<std::vector<int>> adjList;
    std::vector<int> graphNodes;
    std::vector<bool> blocked;
    std::vector<std::vector<int>> B;
    std::list<int> stack;

    ElementaryCyclesSearch(const std::vector<std::vector<bool>>& matrix, const std::vector<int>& graphNodes) :
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

    bool findCycles(int v, int s, const std::vector<std::vector<int>>& adjList) {
        bool f = false;
        stack.push_back(v);
        blocked[v] = true;

        for (int i = 0; i < adjList[v].size(); ++i) {
            int w = adjList[v][i];
            if (w == s) {
                std::vector<int> cycle;
                for (int now : stack) {
                    cycle.push_back(graphNodes[now]);
                }
                cycles.push_back(cycle);
                f = true;
            } else if (!blocked[w]) {
                if (findCycles(w, s, adjList)) {
                    f = true;
                }
            }
        }

        if (f) {
            unblock(v);
        } else {
            for (int i = 0; i < adjList[v].size(); ++i) {
                int w = adjList[v][i];
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
    const int N = 10;

    std::vector<int> nodes(N);
    for (int i = 0; i < N; ++i) {
        nodes[i] = i+1000;
    }

    std::vector<std::vector<bool>> adjMatrix(N, std::vector<bool>(N, false));
    adjMatrix[0][1] = true;
    adjMatrix[1][2] = true;
    adjMatrix[2][0] = true;
    adjMatrix[3][4] = true;
    adjMatrix[2][6] = true;
    adjMatrix[4][5] = true;
    adjMatrix[4][6] = true;
    adjMatrix[5][3] = true;
    adjMatrix[6][7] = true;
    adjMatrix[7][8] = true;
    adjMatrix[8][6] = true;
    adjMatrix[6][1] = true;

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
