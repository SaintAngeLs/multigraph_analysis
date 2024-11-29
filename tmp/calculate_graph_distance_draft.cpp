#include <iostream>
#include <vector>
#include <set>
#include <list>
#include <map>
#include <unordered_set>
#include <unordered_map>
#include <cstdlib>
#include <cstddef>
#include <climits>
#include <cstdint>
#include <memory>

template<class T>
using double_vector = std::vector<std::vector<T>>;

struct HeapItem {
    float priority;
    double_vector<int>* graph;
};

struct StorageItem {
    double_vector<int> graph;
    std::size_t heap_index;
};

struct PriorityQueue {
    std::map<std::size_t, HeapItem> heap;
    std::list<double_vector<int>> graph_storage;
    
    void insert(
};

// wiki
int a_star(const double_vector<int>& start, const double_vector<int>& goal, float (*h)(const double_vector<int>&)) {
        


    std::vector<T> openSet;
    openSet.push(start);
    std::map<T, T> cameFrom;
    
    sym_map<T, int> gScore;
    gScore.insert(start, 0);
    
    sym_map<T, float> fScore;
    fScore.insert(start, h(start));

    while (!openSet.empty()) {
        auto& current = fScore.get_min_second();
        if (equal(current, goal)) {
            return gScore.get_first(current);
        }

        openSet.erase(current);

        std::map<T, int> neighbors = current.get_neighbors();
        for (auto& neighbor : neighbors) {
            int tentative_gScore = gScore.get_first(current) + neighbor.second;
            if (tentative_gScore < gScore.get_first(neighbor)) {
                cameFrom[neighbor] = current;
                gScore.insert(neighbor, tentative_gScore);
                fScore.insert(neighbor, tentative_gScore + h(neighbor));
                openSet.push(neighbor);
            }
        }
    }

    return INT_MAX;
}

int calculateGraphDistance(double_vector<int> A, double_vector<int> B) {
    if (A.size() > B.size()) {
        std::swap(A, B);
    }
    
    std::size_t n = B.size();

    if (A.size() < B.size()) {
        A.resize(n, std::vector<int>(n, 0));
        for (int i = 0; i < n; ++i) {
            A[i].resize(n, 0);
        }
    }

    std::vector<int> degs_B(n, 0);
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            degs_B[i] += B[i][j];
        }
    } 

    std::sort(degs_B.begin(), degs_B.end());
    auto h = [&deg_B, &B, n](const double_vector<int>& g) {
        std::vector<int> degs_g(n, 0);
        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < n; ++j) {
                degs_g[i] += g[i][j];
            }
        }
        std::sort(degs_g.begin(), degs_g.end());

        int dist = 0;
        for (int i = 0; i < n; ++i) {
            d = std::abs(degs_B[i] - degs_g[i]);
            dist += d * d;
        }

        return dist;
    }

    return a_star(A, B, h);
}

/*
template<class T>
struct Node {
    T value;
    Node<T>* parent = nullptr;
    Node<T>* left = nullptr;
    Node<T>* right = nullptr;
};

template<class T>
void free_tree(Node<T>* root) {
    while (root->left || root->right || root->parent) {
        if (root->left) {
            root = root->left;
        } else if (root->right) {
            root = root->right;
        } else {
            Node<T>* parent = root->parent;
            (parent->left == root ?
             parent->left :
             parent->right) = nullptr;
            delete root;
            root = parent;
        }
    }
    delete root;
}

template<class T, class LessPrior>
void add_with_priority(Node<T>* root, const T& val) {
    
}*/


