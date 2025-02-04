import numpy as np

def generate_random_graph(num_vertices, max_weight=3, directed=False):
    adjacency_matrix = np.random.randint(0, max_weight + 1, size=(num_vertices, num_vertices))
    
    if not directed:
        adjacency_matrix = np.tril(adjacency_matrix) + np.tril(adjacency_matrix, -1).T  

    np.fill_diagonal(adjacency_matrix, 0)  
    return adjacency_matrix

def save_graphs_to_file(filename, num_graphs, min_vertices=3, max_vertices=9):
    graphs = []
    
    for _ in range(num_graphs):
        num_vertices = np.random.randint(min_vertices, max_vertices + 1)
        graph = generate_random_graph(num_vertices)
        graphs.append((num_vertices, graph))

    graphs.sort(key=lambda x: x[0])  

    with open(filename, "w") as file:
        file.write(f"{num_graphs}\n\n")  
        
        for num_vertices, graph in graphs:
            file.write(f"{num_vertices}\n")
            for row in graph:
                file.write(" ".join(map(str, row)) + "\n")
            file.write("\n")  

if __name__ == "__main__":
    save_graphs_to_file("random_graphs.txt", num_graphs=128, min_vertices=3, max_vertices=9)
    print("Plik random_graphs.txt został wygenerowany.")
