# Optimal implementations

## Size of a graph

sqrt(sum_v(d(v)^2)) + sum_cycle(size_of_cycle)

## Graph distance (with a metric)

Graph edit distance, see wiki

## Nr of longest cycles (with best edge width)

Use Johnson's algorithm with rejecting smaller cycles. Calculate the length of the cycles. Count the widest cycles by
rejecting more narrow ones.

## Minimal extension + nr of Hamiltonian cycles

Enumerate all possible Hamiltonian cycles, calculate nr of lacking edges, reject cycles with less covering, count the
widest by rejecting more narrow ones.

# Heuristic implementations

## Size of a graph

## Graph distance (with a metric)

## Nr of longest cycles (with best edge width)

## Minimal extension + nr of Hamiltonian cycles

Cover all edges with probabilities that they are in the Hamiltonian cycle we would encounter after the extension.

For each v in V run DFS, find the longest path from v, for each edge in the path assign the probability path_length/n.

Calculate the average probability of all paths for all edges.

For each unvisited edge calculate its probability by using adjacent edges' probabilities if they exist, otherwise let it
be 0.5

Run a random walk from some v and prefer existing edges along with their width and our probabilities. Finish when
further steps are redundant and connect the last vertex with the first of the path.

So if we have the incomplete cycle, then for each unwalked vertex find such edge in the path that replacing it with two
steps through the vertex would be most beneficial.

Count a nr of lacking edges to recreate the Hamiltonial cycle.
