import re
import numpy as np
import matplotlib.pyplot as plt
import pandas as pd

# Compile regex patterns for parsing data
pattern_graph = re.compile(r"Graph (\d+):")
pattern_size = re.compile(r"Graph size: (\d+)")
pattern_exact_cycle_time = re.compile(r"Time taken for exact cycle detection: ([\d.]+) microseconds")
pattern_approx_cycle_time = re.compile(r"Time taken for approximate cycle detection: ([\d.]+) microseconds")
pattern_exact_ham_cycle_time = re.compile(r"Time taken for exact Hamiltonian cycle detection: ([\d.]+) microseconds")
pattern_approx_ham_cycle_time = re.compile(r"Time taken for approximate Hamiltonian cycle detection: ([\d.]+) microseconds")
pattern_final_ham_cycles = re.compile(r"Hamiltonian cycles:\s*(\d+)")
pattern_exact_max_cycle_len = re.compile(r"Exact maximal cycle length: (\d+)")
pattern_approx_max_cycle_len = re.compile(r"Approximate maximal cycle length: (\d+)")
pattern_exact_max_cycle_count = re.compile(r"Maximal cycles:\s*(\d+)")
pattern_approx_max_cycle_count = re.compile(r"Approximate maximal cycle count:\s*(\d+)")
pattern_exact_min_ext = re.compile(r"Exact minimal extension required: (\d+) edges")
pattern_exact_min_ext_time = re.compile(r"Time taken for exact minimal extension detection: ([\d.]+) microseconds")
pattern_approx_min_ext = re.compile(r"Approximate minimal extension required: (\d+) edges")
pattern_approx_min_ext_time = re.compile(r"Time taken for approximate minimal extension detection: ([\d.]+) microseconds")
pattern_no_ext = re.compile(r"Graph already has a Hamiltonian cycle\. No edges needed\.")

data = []

with open("test_output.txt", "r") as file:
    lines = file.readlines()
    graph_data = {}
    for line in lines:
        graph_match = pattern_graph.search(line)
        size_match = pattern_size.search(line)
        exact_cycle_time_match = pattern_exact_cycle_time.search(line)
        approx_cycle_time_match = pattern_approx_cycle_time.search(line)
        exact_ham_cycle_time_match = pattern_exact_ham_cycle_time.search(line)
        approx_ham_cycle_time_match = pattern_approx_ham_cycle_time.search(line)
        final_ham_cycles_match = pattern_final_ham_cycles.search(line)
        exact_max_cycle_len_match = pattern_exact_max_cycle_len.search(line)
        approx_max_cycle_len_match = pattern_approx_max_cycle_len.search(line)
        exact_max_cycle_count_match = pattern_exact_max_cycle_count.search(line)
        approx_max_cycle_count_match = pattern_approx_max_cycle_count.search(line)
        exact_min_ext_match = pattern_exact_min_ext.search(line)
        exact_min_ext_time_match = pattern_exact_min_ext_time.search(line)
        approx_min_ext_match = pattern_approx_min_ext.search(line)
        approx_min_ext_time_match = pattern_approx_min_ext_time.search(line)
        no_ext_match = pattern_no_ext.search(line)

        if graph_match:
            if graph_data:
                data.append(graph_data)
            graph_data = {"Graf": int(graph_match.group(1))}
        if size_match:
            graph_data["Rozmiar"] = int(size_match.group(1))
        if exact_cycle_time_match:
            graph_data["Czas dokładnego wykrycia cyklu"] = float(exact_cycle_time_match.group(1))
        if approx_cycle_time_match:
            graph_data["Czas przybliżonego wykrycia cyklu"] = float(approx_cycle_time_match.group(1))
        if exact_ham_cycle_time_match:
            graph_data["Czas dokładnego wykrycia Hamiltonian cycles"] = float(exact_ham_cycle_time_match.group(1))
        if approx_ham_cycle_time_match:
            graph_data["Czas przybliżonego wykrycia Hamiltonian cycles"] = float(approx_ham_cycle_time_match.group(1))
        if final_ham_cycles_match:
            graph_data["Liczba cykli Hamiltona (dokładna)"] = int(final_ham_cycles_match.group(1))
        else:
            if "Liczba cykli Hamiltona (dokładna)" not in graph_data:
                graph_data["Liczba cykli Hamiltona (dokładna)"] = 0
        # Assuming approximate Hamiltonian cycles are reported separately
        # If not present, default to 0.
        if "Liczba cykli Hamiltona (przybliżona)" not in graph_data:
            graph_data["Liczba cykli Hamiltona (przybliżona)"] = 0

        if exact_max_cycle_len_match:
            graph_data["Dokładna maksymalna długość cyklu"] = int(exact_max_cycle_len_match.group(1))
        if approx_max_cycle_len_match:
            graph_data["Przybliżona maksymalna długość cyklu"] = int(approx_max_cycle_len_match.group(1))
        if exact_max_cycle_count_match:
            graph_data["Dokładna liczba maksymalnych cykli"] = int(exact_max_cycle_count_match.group(1))
        if approx_max_cycle_count_match:
            graph_data["Przybliżona liczba maksymalnych cykli"] = int(approx_max_cycle_count_match.group(1))
        if no_ext_match:
            graph_data["Dokładne minimalne rozszerzenie"] = 0
            graph_data["Czas dokładnego minimalnego rozszerzenia"] = 0.0
            graph_data["Przybliżone minimalne rozszerzenie"] = 0
            graph_data["Czas przybliżonego minimalnego rozszerzenia"] = 0.0
        else:
            if exact_min_ext_match:
                graph_data["Dokładne minimalne rozszerzenie"] = int(exact_min_ext_match.group(1))
            if exact_min_ext_time_match:
                graph_data["Czas dokładnego minimalnego rozszerzenia"] = float(exact_min_ext_time_match.group(1))
            if approx_min_ext_match:
                graph_data["Przybliżone minimalne rozszerzenie"] = int(approx_min_ext_match.group(1))
            if approx_min_ext_time_match:
                graph_data["Czas przybliżonego minimalnego rozszerzenia"] = float(approx_min_ext_time_match.group(1))
    if graph_data:
        data.append(graph_data)

df = pd.DataFrame(data)

# Compute additional metrics for Hamiltonian cycles
df["Dokładność długości cyklu (%)"] = (df["Przybliżona maksymalna długość cyklu"] / df["Dokładna maksymalna długość cyklu"]) * 100
df["Dokładność liczby cykli (%)"] = (df["Przybliżona liczba maksymalnych cykli"] / df["Dokładna liczba maksymalnych cykli"]) * 100
df["Stosunek czasu (cykle)"] = df["Czas przybliżonego wykrycia cyklu"] / df["Czas dokładnego wykrycia cyklu"]

# Metrics for minimal extension
df["Stosunek czasu (minimalne rozszerzenie)"] = df["Czas przybliżonego minimalnego rozszerzenia"] / df["Czas dokładnego minimalnego rozszerzenia"]
df["Dokładność minimalnego rozszerzenia (%)"] = np.where(
    df["Dokładne minimalne rozszerzenie"] > 0,
    (df["Przybliżone minimalne rozszerzenie"] / df["Dokładne minimalne rozszerzenie"]) * 100,
    100
)

# Save to CSV
df.to_csv("multigraf_analysis.csv", index=False)
print(df)

# Plot settings
plt.style.use("default")
plt.rcParams.update({
    "font.size": 12,
    "figure.figsize": (12, 6),
    "savefig.dpi": 300,
    "figure.dpi": 300,
    "axes.facecolor": "white",
    "savefig.facecolor": "white",
    "grid.color": "gray",
    "grid.linestyle": "--",
    "grid.linewidth": 0.5
})

# --- Plots for maximal cycles ---
plt.figure()
plt.scatter(df["Rozmiar"], df["Czas dokładnego wykrycia cyklu"], label="Dokładny czas wykrywania maks. cyklu", marker="o", s=10)
plt.scatter(df["Rozmiar"], df["Czas przybliżonego wykrycia cyklu"], label="Przybliżony czas wykrywania maks. cyklu", marker="s", s=10)
plt.xlabel("Rozmiar grafu")
plt.ylabel("Czas (mikrosekundy)")
plt.title("Czas wykrywania maksymalnego cyklu")
plt.legend()
plt.grid(True)
plt.savefig("czas_wykrywania_maks_cyklu.png")
plt.show()

plt.figure()
plt.scatter(df["Rozmiar"], df["Dokładna maksymalna długość cyklu"], label="Dokładna długość maks. cyklu", marker="o", s=10)
plt.scatter(df["Rozmiar"], df["Przybliżona maksymalna długość cyklu"], label="Przybliżona długość maks. cyklu", marker="s", s=10)
plt.xlabel("Rozmiar grafu")
plt.ylabel("Maksymalna długość cyklu")
plt.title("Maksymalna długość maksymalnego cyklu")
plt.legend()
plt.grid(True)
plt.savefig("dlugosc_maks_cyklu.png")
plt.show()

plt.figure()
plt.scatter(df["Rozmiar"], df["Dokładna liczba maksymalnych cykli"], label="Dokładna liczba maks. cykli", marker="o", s=10)
plt.scatter(df["Rozmiar"], df["Przybliżona liczba maksymalnych cykli"], label="Przybliżona liczba maks. cykli", marker="s", s=10)
plt.xlabel("Rozmiar grafu")
plt.ylabel("Liczba maksymalnych cykli")
plt.title("Liczba maksymalnych cykli")
plt.legend()
plt.grid(True)
plt.savefig("liczba_maks_cyklu.png")
plt.show()

plt.figure()
plt.scatter(df["Rozmiar"], df["Dokładność długości cyklu (%)"], label="Dokładność długości cyklu (maks. cykle)", marker="o", s=10)
plt.scatter(df["Rozmiar"], df["Dokładność liczby cykli (%)"], label="Dokładność liczby cykli (maks. cykle)", marker="s", s=10)
plt.xlabel("Rozmiar grafu")
plt.ylabel("Dokładność (%)")
plt.title("Dokładność algorytmu aproksymacyjnego dla maksymalnych cykli")
plt.legend()
plt.grid(True)
plt.savefig("dokladnosc_maks_cyklu.png")
plt.show()

plt.figure()
plt.scatter(df["Rozmiar"], df["Czas przybliżonego wykrycia cyklu"] / df["Czas dokładnego wykrycia cyklu"], label="Stosunek czasu (maks. cykle)", marker="o", s=10)
plt.xlabel("Rozmiar grafu")
plt.ylabel("Stosunek czasu")
plt.title("Stosunek czasu wykrywania maksymalnego cyklu (przybliżony/dokładny)")
plt.legend()
plt.grid(True)
plt.savefig("stosunek_czasu_maks_cyklu.png")
plt.show()

# --- Plots for Hamiltonian cycles ---
plt.figure()
plt.scatter(df["Rozmiar"], df["Czas dokładnego wykrycia Hamiltonian cycles"], label="Dokładny czas wykrywania cykli Hamiltona", marker="o", s=10)
plt.scatter(df["Rozmiar"], df["Czas przybliżonego wykrycia Hamiltonian cycles"], label="Przybliżony czas wykrywania cykli Hamiltona", marker="s", s=10)
plt.xlabel("Rozmiar grafu")
plt.ylabel("Czas (mikrosekundy)")
plt.title("Czas wykrywania cykli Hamiltona")
plt.legend()
plt.grid(True)
plt.savefig("czas_wykrycia_hamiltonian.png")
plt.show()

plt.figure()
plt.scatter(df["Rozmiar"], df["Dokładna maksymalna długość cyklu"], label="Dokładna długość cyklu Hamiltona", marker="o", s=10)
plt.scatter(df["Rozmiar"], df["Przybliżona maksymalna długość cyklu"], label="Przybliżona długość cyklu Hamiltona", marker="s", s=10)
plt.xlabel("Rozmiar grafu")
plt.ylabel("Długość cyklu Hamiltona")
plt.title("Długość cyklu Hamiltona")
plt.legend()
plt.grid(True)
plt.savefig("dlugosc_cyklu_hamiltonian.png")
plt.show()

plt.figure()
plt.scatter(df["Rozmiar"], df["Liczba cykli Hamiltona (dokładna)"], label="Dokładna liczba cykli Hamiltona", marker="o", s=10)
plt.scatter(df["Rozmiar"], df["Liczba cykli Hamiltona (przybliżona)"], label="Przybliżona liczba cykli Hamiltona", marker="s", s=10)
plt.xlabel("Rozmiar grafu")
plt.ylabel("Liczba cykli Hamiltona")
plt.title("Liczba cykli Hamiltona")
plt.legend()
plt.grid(True)
plt.savefig("liczba_cykli_hamiltonian.png")
plt.show()

plt.figure()
plt.scatter(df["Rozmiar"], df["Dokładność długości cyklu (%)"], label="Dokładność długości cykli Hamiltona", marker="o", s=10)
plt.scatter(df["Rozmiar"], df["Dokładność liczby cykli (%)"], label="Dokładność liczby cykli Hamiltona", marker="s", s=10)
plt.xlabel("Rozmiar grafu")
plt.ylabel("Dokładność (%)")
plt.title("Dokładność algorytmu aproksymacyjnego dla cykli Hamiltona")
plt.legend()
plt.grid(True)
plt.savefig("dokladnosc_cykli_hamiltonian.png")
plt.show()

plt.figure()
plt.scatter(df["Rozmiar"], df["Czas przybliżonego wykrycia Hamiltonian cycles"] / df["Czas dokładnego wykrycia Hamiltonian cycles"], label="Stosunek czasu (cykle Hamiltona)", marker="o", s=10)
plt.xlabel("Rozmiar grafu")
plt.ylabel("Stosunek czasu")
plt.title("Stosunek czasu wykrywania cykli Hamiltona (przybliżony/dokładny)")
plt.legend()
plt.grid(True)
plt.savefig("stosunek_czasu_cykli_hamiltonian.png")
plt.show()

# --- Plots for minimal extension ---
plt.figure()
plt.scatter(df["Rozmiar"], df["Czas dokładnego minimalnego rozszerzenia"], label="Czas dokładnego minimalnego rozszerzenia", marker="o", s=10)
plt.scatter(df["Rozmiar"], df["Czas przybliżonego minimalnego rozszerzenia"], label="Czas przybliżonego minimalnego rozszerzenia", marker="s", s=10)
plt.xlabel("Rozmiar grafu")
plt.ylabel("Czas (mikrosekundy)")
plt.title("Czas wykrywania minimalnego rozszerzenia")
plt.legend()
plt.grid(True)
plt.savefig("czas_minimalnego_rozszerzenia.png")
plt.show()

plt.figure()
plt.scatter(df["Rozmiar"], df["Dokładne minimalne rozszerzenie"], label="Dokładne minimalne rozszerzenie", marker="o", s=10)
plt.scatter(df["Rozmiar"], df["Przybliżone minimalne rozszerzenie"], label="Przybliżone minimalne rozszerzenie", marker="s", s=10)
plt.xlabel("Rozmiar grafu")
plt.ylabel("Minimalne rozszerzenie (liczba krawędzi)")
plt.title("Minimalne rozszerzenie grafu")
plt.legend()
plt.grid(True)
plt.savefig("minimalne_rozszerzenie.png")
plt.show()

plt.figure()
plt.scatter(df["Rozmiar"], df["Czas przybliżonego minimalnego rozszerzenia"] / df["Czas dokładnego minimalnego rozszerzenia"], label="Stosunek czasu (minimalne rozszerzenie)", marker="o", s=10)
plt.xlabel("Rozmiar grafu")
plt.ylabel("Stosunek czasu")
plt.title("Stosunek czasu wykrywania minimalnego rozszerzenia (przybliżony/dokładny)")
plt.legend()
plt.grid(True)
plt.savefig("stosunek_czasu_minimalnego_rozszerzenia.png")
plt.show()
