import re
import numpy as np
import matplotlib.pyplot as plt
import pandas as pd

# Kompilacja wyrażeń regularnych do parsowania danych
pattern_graph = re.compile(r"Graph (\d+):")
pattern_size = re.compile(r"Graph size: (\d+)")
pattern_exact_cycle_time = re.compile(r"Time taken for exact cycle detection: ([\d.]+) microseconds")
pattern_approx_cycle_time = re.compile(r"Time taken for approximate cycle detection: ([\d.]+) microseconds")
pattern_exact_max_cycle_len = re.compile(r"Maximal cycle length: (\d+)")
pattern_approx_max_cycle_len = re.compile(r"Approximate maximal cycle length: (\d+)")
pattern_exact_max_cycle_count = re.compile(r"Maximal cycles: (\d+)")
pattern_approx_max_cycle_count = re.compile(r"Approximate maximal cycle count: (\d+)")

data = []

with open("test_output.txt", "r") as file:  
    lines = file.readlines()
    graph_data = {}

    for line in lines:
        graph_match = pattern_graph.search(line)
        size_match = pattern_size.search(line)
        exact_cycle_time_match = pattern_exact_cycle_time.search(line)
        approx_cycle_time_match = pattern_approx_cycle_time.search(line)
        exact_max_cycle_len_match = pattern_exact_max_cycle_len.search(line)
        approx_max_cycle_len_match = pattern_approx_max_cycle_len.search(line)
        exact_max_cycle_count_match = pattern_exact_max_cycle_count.search(line)
        approx_max_cycle_count_match = pattern_approx_max_cycle_count.search(line)

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
        if exact_max_cycle_len_match:
            graph_data["Dokładna maksymalna długość cyklu"] = int(exact_max_cycle_len_match.group(1))
        if approx_max_cycle_len_match:
            graph_data["Przybliżona maksymalna długość cyklu"] = int(approx_max_cycle_len_match.group(1))
        if exact_max_cycle_count_match:
            graph_data["Dokładna liczba maksymalnych cykli"] = int(exact_max_cycle_count_match.group(1))
        if approx_max_cycle_count_match:
            graph_data["Przybliżona liczba maksymalnych cykli"] = int(approx_max_cycle_count_match.group(1))
    if graph_data:
        data.append(graph_data)

df = pd.DataFrame(data)

df["Dokładność długości cyklu (%)"] = (df["Przybliżona maksymalna długość cyklu"] / df["Dokładna maksymalna długość cyklu"]) * 100
df["Dokładność liczby cykli (%)"] = (df["Przybliżona liczba maksymalnych cykli"] / df["Dokładna liczba maksymalnych cykli"]) * 100
df["Stosunek czasu (przybliżony/dokładny)"] = df["Czas przybliżonego wykrycia cyklu"] / df["Czas dokładnego wykrycia cyklu"]

df.to_csv("multigraf_analysis.csv", index=False)
print(df)

# Ustawienia globalne: używamy stylu 'default' i ręcznie konfigurujemy białe tło oraz siatkę.
plt.style.use("default")
plt.rcParams.update({
    "font.size": 12,
    "figure.figsize": (12, 6),
    "savefig.dpi": 300,
    "figure.dpi": 300,
    "axes.facecolor": "white",    # Białe tło dla osi
    "savefig.facecolor": "white",   # Białe tło dla zapisywanych obrazów
    "grid.color": "gray",           # Kolor siatki
    "grid.linestyle": "--",
    "grid.linewidth": 0.5
})

# Wykres 1: Czas wykrywania cyklu
plt.figure()
plt.scatter(df["Rozmiar"], df["Czas dokładnego wykrycia cyklu"], label="Dokładny czas wykrycia", marker="o", s=10)
plt.scatter(df["Rozmiar"], df["Czas przybliżonego wykrycia cyklu"], label="Przybliżony czas wykrycia", marker="s", s=10)
plt.xlabel("Rozmiar grafu")
plt.ylabel("Czas (mikrosekundy)")
plt.title("Czas wykrywania cyklu")
plt.legend()
plt.grid(True)
plt.savefig("czas_wykrycia_cyklu.png")
plt.show()

# Wykres 2: Maksymalna długość cyklu
plt.figure()
plt.scatter(df["Rozmiar"], df["Dokładna maksymalna długość cyklu"], label="Dokładna długość cyklu", marker="o", s=10)
plt.scatter(df["Rozmiar"], df["Przybliżona maksymalna długość cyklu"], label="Przybliżona długość cyklu", marker="s", s=10)
plt.xlabel("Rozmiar grafu")
plt.ylabel("Maksymalna długość cyklu")
plt.title("Maksymalna długość cyklu")
plt.legend()
plt.grid(True)
plt.savefig("maksymalna_dlugosc_cyklu.png")
plt.show()

# Wykres 3: Liczba maksymalnych cykli
plt.figure()
plt.scatter(df["Rozmiar"], df["Dokładna liczba maksymalnych cykli"], label="Dokładna liczba cykli", marker="o", s=10)
plt.scatter(df["Rozmiar"], df["Przybliżona liczba maksymalnych cykli"], label="Przybliżona liczba cykli", marker="s", s=10)
plt.xlabel("Rozmiar grafu")
plt.ylabel("Liczba maksymalnych cykli")
plt.title("Liczba maksymalnych cykli")
plt.legend()
plt.grid(True)
plt.savefig("liczba_maksymalnych_cykli.png")
plt.show()

# Wykres 4: Dokładność algorytmu aproksymacyjnego
plt.figure()
plt.scatter(df["Rozmiar"], df["Dokładność długości cyklu (%)"], label="Dokładność długości cyklu", marker="o", s=10)
plt.scatter(df["Rozmiar"], df["Dokładność liczby cykli (%)"], label="Dokładność liczby cykli", marker="s", s=10)
plt.xlabel("Rozmiar grafu")
plt.ylabel("Dokładność (%)")
plt.title("Dokładność algorytmu aproksymacyjnego")
plt.legend()
plt.grid(True)
plt.savefig("dokladnosc_algorytmu.png")
plt.show()

# Wykres 5: Stosunek czasu wykrycia cyklu (przybliżony/dokładny)
plt.figure()
plt.scatter(df["Rozmiar"], df["Stosunek czasu (przybliżony/dokładny)"], label="Stosunek czasu", marker="o", s=10)
plt.xlabel("Rozmiar grafu")
plt.ylabel("Stosunek czasu")
plt.title("Stosunek czasu wykrycia cyklu (przybliżony/dokładny)")
plt.legend()
plt.grid(True)
plt.savefig("stosunek_czasu.png")
plt.show()
