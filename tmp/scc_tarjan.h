// https://www.geeksforgeeks.org/tarjans-algorithm-in-c/
// 1 Dec 2024

// int disc[MAX], low[MAX], stackMember[MAX], stack[MAX];
//int time = 0, top = -1;

// A utility function to find the SCCs using DFS
void SCCUtil(int* graph, int n, int u, int* disc, int* low, int* stackMember, int* stack, int* top, int* time, int* results, int* label)
{
    // Initialize discovery and low values
    disc[u] = low[u] = ++*time;
    
    // Push the vertex onto the stack
    stack[++*top] = u;
    stackMember[u] = 1;

    // Go through all vertices adjacent to this vertex
    for (int v = 0; v < n; v++) {
        // If v is not visited yet, recur for it
        if (graph[u*n+v]) {
            if (disc[v] == -1) {
                SCCUtil(graph, n, v, disc, low, stackMember, stack, top, time, results, label);
                // Check if the subtree rooted at v has a
                // connection back to one of the ancestors
                // of u
                low[u]
                    = (low[u] < low[v]) ? low[u] : low[v];
            }
            // Update low value of u for parent function
            // calls
            else if (stackMember[v]) {
                low[u]
                    = (low[u] < disc[v]) ? low[u] : disc[v];
            }
        }
    }
    // To store stack extracted vertices
    int w = 0;
    // If u is a root node, pop all vertices from the stack
    // and print them
    if (low[u] == disc[u]) {
        while (stack[*top] != u) {
            w = stack[(*top)--];
            results[w] = *label;
            stackMember[w] = 0;
        }
        w = stack[(*top)--];
        results[w] = *label;
        stackMember[w] = 0;
    }

    ++*label;
}

// Function to find and print all SCCs
void SCC(int* graph, int n, int* disc, int* low, int* stackMember, int* stack, int* top, int* time, int* results, int* label)
{
    // Initialize discovery and low values to -1
    for (int i = 0; i < n; i++) {
        disc[i] = -1;
        low[i] = -1;
        stackMember[i] = 0;
    }

    // Call the recursive helper function to find SCCs in
    // DFS tree rooted with vertex i
    for (int i = 0; i < n; i++) {
        if (disc[i] == -1) {
            SCCUtil(graph, n, i, disc, low, stackMember, stack, top, time, results, label);
        }
    }
}

