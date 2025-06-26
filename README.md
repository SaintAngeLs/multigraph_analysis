# Multigraph Toolkit

**Exact & Approximation Algorithms for Large Multigraphs**

<p align="center">
  <a href="https://github.com/SaintAngeLs/multigraph_analysis/stargazers"><img src="https://img.shields.io/github/stars/SaintAngeLs/multigraph_analysis?style=social" alt="GitHub Stars"></a>
  <a href="https://github.com/SaintAngeLs/multigraph_analysis/actions/workflows/c-cpp.yml"><img src="https://github.com/SaintAngeLs/multigraph_analysis/actions/workflows/c-cpp.yml/badge.svg" alt="CI Status"></a>
  <a href="https://github.com/SaintAngeLs/multigraph_analysis/blob/main/LICENSE"><img src="https://img.shields.io/github/license/SaintAngeLs/multigraph_analysis.svg" alt="License"></a>
</p>

---

> **TL;DR** — *A blazing‑fast C/C++ library + CLI for analysing **multigraphs**. It automatically falls back to carefully‑designed approximation algorithms once the input size exceeds a tunable threshold.*

---

## 📚 Table of Contents

1. [Project Vision](#-project-vision)
2. [Mathematical Foundations](#-mathematical-foundations)
3. [Algorithmic Highlights](#-algorithmic-highlights)
4. [Building & Installation](#-building--installation)
5. [CLI Usage](#-cli-usage)
6. [Examples](#-examples)
7. [Benchmarks](#-benchmarks)
8. [Contributing](#-contributing)
9. [License](#-license)
10. [Citation](#-citation)

---

## 🚀 Project Vision

Real‑world graphs—from road networks to protein–protein interaction maps—often contain **multiple parallel edges** between the same pair of vertices. *Multigraph Toolkit* provides an algorithmic Swiss‑army knife for such structures:

* **Exact algorithms** with proven correctness for small graphs.
* **Approximate algorithms** (Monte‑Carlo, simulated annealing, heuristic extension) that stay responsive even for $|V| > 10^5$.
* A **hybrid dispatcher** that selects the optimal routine at runtime.

The project began as a laboratory assignment for the *Algorithms & Computability* course (Warsaw University of Technology) and has since grown into a production‑ready open‑source package.

## 🧮 Mathematical Foundations

Below we recap the core definitions using standard graph‑theoretic notation. All proofs can be found in [`documentation/main.pdf`](documentation/main.pdf).

### Definition 1 (Multigraph)

A **multigraph** is an ordered pair $G = (V,E)$ where
$E\subseteq \operatorname{Multiset}(V\times V),$
allowing repeated edges but disallowing loops $(v,v)$.

### Definition 2 (Graph Size)

The *size* of $G$ is
$|E| = \sum_{(u,v)\in V\times V} \operatorname{mult}((u,v),E).$

### Definition 3 (Cycle)

A **cycle** is a vertex sequence
$v_1,\dots,v_k,v_1$
with $v_i\neq v_j\ (i\neq j)$ and $(v_i,v_{i+1})\in E$ for all appropriate indices.

### Definition 4 (Hamiltonian Cycle)

A **Hamiltonian cycle** visits every vertex exactly once:
$|\{v_1,\dots,v_k\}| = |V|.$

### Definition 5 (Graph Metric)

Given two multigraphs $G,H$, the edit‑distance metric is
$d(G,H)=\min_{\text{edit sequences}}\sum_{e\in\text{seq}}\operatorname{cost}(e),$
with unit cost for edge/vertex insertion & deletion and zero cost for vertex relabelling.

*Formal proofs of theorems & algorithmic correctness are included in the documentation.*

## 🔬 Algorithmic Highlights

| Problem                       | Exact Algorithm                   | Complexity                | Approximate Counterpart      | Complexity             |
| ----------------------------- | --------------------------------- | ------------------------- | ---------------------------- | ---------------------- |
| Graph size `&#124;E&#124;`    | Double loop over adjacency matrix | `O(&#124;V&#124;^2)`      | *identical*                  | `O(&#124;V&#124;^2)`   |
| Cycle enumeration             | DFS with backtracking             | `O(&#124;V&#124;^3)`      | Random vertex sampling       | `O(T ⋅ d̄)`            |
| Hamiltonian cycles            | Exhaustive backtracking           | `O(&#124;V&#124;!)`       | Monte‑Carlo permutation test | `O(T ⋅ &#124;V&#124;)` |
| Graph metric                  | Factorial permutation search      | `O(&#124;V&#124;!)`       | Simulated annealing          | `O(T ⋅ &#124;V&#124;)` |
| Minimum Hamiltonian extension | Branch & bound                    | `O(2^m ⋅ &#124;V&#124;!)` | Heuristic minimum‑degree     | `O(&#124;V&#124;^2)`   |

*`T`* — number of Monte‑Carlo trials, *`d̄`* — average degree.## 🛠️ Building & Installation

```bash
# 1️⃣ Clone with submodules (if any)
git clone --recursive https://github.com/SaintAngeLs/multigraph_analysis.git
cd multigraph_analysis

# 2️⃣ Configure (Windows + MinGW)
cmake -G"MinGW Makefiles" -DCMAKE_TOOLCHAIN_FILE="../vcpkg/scripts/buildsystems/vcpkg.cmake" -DPKG_CONFIG_EXECUTABLE="../vcpkg/installed/x64-windows/tools/pkgconf/pkgconf.exe" .

# 3️⃣ Build
cmake --build . --config Release
```

> **Linux / macOS** users can omit the toolchain flags and simply run `cmake -Bbuild && cmake --build build -j$(nproc)`.

## 💻 CLI Usage

```text
./multigraph_analysis.exe <input_file> [comparison_file]
```

* **`<input_file>`** — mandatory; contains one or more multigraphs in the **plain adjacency matrix** format shown below.
* **`[comparison_file]`** — optional; if provided, the metric distance between the first graphs of each file is computed as well.

### Input File Format

```text
<number_of_graphs>
<number_of_vertices_for_graph_1>
<adjacency_matrix_for_graph_1>
<number_of_vertices_for_graph_2>
<adjacency_matrix_for_graph_2>
...
```

#### Minimal Example

```text
1
3
0 2 1
2 0 1
1 1 0
```

### Sample Run

```bash
./multigraph_analysis.exe ../Examples/input1.txt ../Examples/input2.txt
```

The program prints a comprehensive report:

```
Graph #1 ──────────────────────────────────────────
|E|............................ 8
All cycles..................... 3
Hamiltonian cycles............. 2
Min Hamiltonian extension...... 0
Max cycle length............... 3

Graph #2 ──────────────────────────────────────────
...
Distance(G₁,G₂)................ 6
(approximated)................ 6
```

## 🎯 Benchmarks

| \|V\| | Exact Hamiltonian search | Approx. Monte‑Carlo (T = 10⁴) | Speed‑up |
| :---: | :----------------------: | :---------------------------: | -------: |
|   8   |        **0.03 s**        |             0.02 s            |     1.5× |
|   12  |         **2.6 s**        |             0.07 s            |      37× |
|   15  |   **> 1 h (estimate)**   |             0.11 s            | >32 000× |

All experiments were executed on an **Intel i7‑13700K @ 5.1 GHz**, GCC 14, release build, Ubuntu 24.04.

## 🤝 Contributing

We 💛 contributions—bug reports, feature requests, new heuristics, better proofs, documentation tweaks… everything is welcome!

1. Fork 👉 Create branch 👉 **Write tests** 👉 Open PR.
2. Keep commits atomic; squash if necessary.
3. Run `./scripts/lint.sh` before pushing.

If you use this toolkit in academic work, please cite us (see below) and consider opening a PR to add your paper to the *Users* section.

## 🙏 Acknowledgements

Thanks to the following early contributors whose feedback, issue reports, and feature proposals shaped **Multigraph Toolkit**:

* [@eggwhat](https://github.com/eggwhat)
* [@an2508374](https://github.com/an2508374)
* [@olegkiprik](https://github.com/olegkiprik)

---

## 📜 License

This project is distributed under the **MIT License**—see [`LICENSE`](LICENSE) for details.

## 📝 Citation

```bibtex
@misc{multigraphtoolkit2025,
  author       = {Nowak, Amadeusz and Voznesenskyi, Andrii and Kiprik, Oleh and Padamczyk, Piotr},
  title        = {Multigraph Toolkit — Exact & Approximation Algorithms for Large Multigraphs},
  year         = {2025},
  howpublished = {\url{https://github.com/SaintAngeLs/multigraph_analysis}},
  note         = {Version 1.0.0}
}
```

---

<p align="center">
  <sub><em>Made with ❤ and graph‑theoretic rigour in Warsaw, Poland.</em></sub>
</p>
