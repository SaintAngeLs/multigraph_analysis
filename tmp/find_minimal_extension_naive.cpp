#include "all.hpp"

//std::map<std::size_t, std::list<std::size_t>>  
//

// not used
struct StackValue {
    std::size_t v;
    //std::vector<bool> flags;
    std::size_t count;

    //StackValue(std::size_t v, const std::vector<bool>& flags) : v(v), flags(flags) {}
    StackValue(std::size_t v, std::size_t count) : v(v), count(count) {}
};

// add sccs
std::vector<std::vector<int>> find_minimal_extension(const std::vector<std::vector<int>>& multigraph) {
    // find least nr of edges for an extension
    const std::size_t n = multigraph.size();
    assert(n);
    std::size_t least_nr_edges = n*n;

    std::list<std::size_t> stack;
    stack.emplace_back(0);

    std::vector<bool> blocked(n);
    blocked[0] = true;

    std::size_t now = 1;
    while (!stack.empty()) {
        if (blocked.all()) {
            // Hamilton cycle created
            // Count lacking edges
            std::size_t nr_lacking = 0;
            for (auto pv = stack.begin(); pv != stack.end(); ++pv) {
                auto pvnext = pv;
                ++pvnext;
                if (pvnext == stack.end()) {
                    pvnext = stack.begin();
                }
                if (!multigraph[*pv][*pvnext]) {
                    ++nr_lacking;
                }
            }
            if (nr_lacking < least_nr_edges) {
                least_nr_edges = nr_lacking;
            }
        }

        // add new vertex
        bool added = false;
        for (std::size_t i = now; i < n; ++i) {
            if (!blocked[i]) {

                // add reject condition for performance

                stack.push_back(i);
                blocked[i] = true;
                added = true;
                break;
            }
        }
        if (!added) {
            // delete the used vertex
            now = stack.back()+1;
            stack.pop_back();
        }
    }
}
