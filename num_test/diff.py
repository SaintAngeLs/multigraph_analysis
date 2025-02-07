import re

# Patterns for exact and approximate Hamiltonian cycle counts
pattern_graph = re.compile(r"Graph (\d+):")
pattern_exact_ham_cycles = re.compile(r"Hamiltonian cycles:\s*(\d+)")
pattern_approx_ham_cycle_count = re.compile(r"Approximate Hamiltonian cycle count:\s*(\d+)")

# Variables to count graphs and differences
total_graphs = 0
diff_count = 0

# Open and read the output file
with open("test_output.txt", "r") as file:
    lines = file.readlines()

# Variables to hold counts for the current graph
current_exact_count = None
current_approx_count = None

for line in lines:
    graph_match = pattern_graph.search(line)
    if graph_match:
        # If we already collected data for a previous graph, compare and update counter
        if current_exact_count is not None and current_approx_count is not None:
            total_graphs += 1
            if current_exact_count != current_approx_count:
                diff_count += 1
        # Start new graph block; reset counts
        current_exact_count = None
        current_approx_count = None

    # Search for exact Hamiltonian cycle count
    exact_match = pattern_exact_ham_cycles.search(line)
    if exact_match:
        current_exact_count = int(exact_match.group(1))
    
    # Search for approximate Hamiltonian cycle count
    approx_match = pattern_approx_ham_cycle_count.search(line)
    if approx_match:
        current_approx_count = int(approx_match.group(1))

# After the loop, check the last graph block if available
if current_exact_count is not None and current_approx_count is not None:
    total_graphs += 1
    if current_exact_count != current_approx_count:
        diff_count += 1

print("Total number of graphs processed:", total_graphs)
print("Number of graphs where exact and approximate Hamiltonian cycle counts differ:", diff_count)
