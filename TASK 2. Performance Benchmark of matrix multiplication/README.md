# TASK 2 – Performance Benchmark of Optimized and Sparse Matrix Multiplication

This folder contains the code and experiments for **Task 2**, where we benchmark different matrix multiplication algorithms in C++:

- Dense kernels:
  - `dense_ijk` (baseline triple loop)
  - `dense_block` (blocked / tiled version)
  - `dense_unroll` (manual loop unrolling)
- Sparse kernel:
  - `sparse_csr` (CSR sparse matrix × dense matrix)

All experiments are run from this task folder using a single script: `run_task2.sh`.

---

## 1. Project structure

Inside the **Task 2** folder you should have something like:

```text
TASK 2. Performance Benchmark of optimized and sparse matrix multiplication/
├─ cpp/
│  └─ src/
│     ├─ matrix.hpp          # Dense + CSR implementations
│     └─ bench_task2.cpp     # Benchmark main for Task 2
├─ results/                  # CSV files + plots will be created here
├─ paper/
│  └─ TASK_2__Performance_Benchmark_of_matrix_multiplication.pdf
├─ run_task2.sh              # Main script to run all experiments
├─ README.md            # This file
├─ plot_task2.py             # Script to generate plots from summary CSV
└── data/
    └── mc2depi/
        └── mc2depi.mtx  # (optional) SuiteSparse matrix in Matrix Market format
```
## 2. Requirements

You need:

- A Unix-like environment (Linux / macOS, or WSL on Windows).
- clang++ (or compatible C++17 compiler).
- bash (for running run_task2.sh).
- python3 with:
    - pandas
    - matplotlib
    - numpy

## 3. How to run the experiments (VERY IMPORTANT)
### 3.1. Go to the Task 2 folder

From the root of your repository, enter the Task 2 directory using quotes
(because the folder name contains spaces):

```bash
cd "TASK 2. Performance Benchmark of optimized and sparse matrix multiplication"
```
### 3.2. Create the results folder (if it does not exist)
```bash
mkdir -p results
```
### 3.3. Give execute permission to the script
```bash
chmod +x run_task2.sh
```
### 3.4. Run all benchmarks
```bash
./run_task2.sh
```

What this script does:

1. Compiles cpp/src/bench_task2.cpp into cpp/bench_task2.
2. Runs all dense benchmarks (dense_ijk, dense_block, dense_unroll) for
several matrix sizes (e.g. 512, 1024, 1536, 2048).
3. Runs all sparse benchmarks (sparse_csr) for:
    - sizes n = 5000 and n = 10000
    - densities d = 0.10, 0.01, 0.001, 0.0001.
4. If data/mc2depi/mc2depi.mtx exists, run an additional sparse experiment
using the SuiteSparse matrix mc2depi (CSR format) with a dense matrix
B having a small number of columns (to keep memory under control).
5. Aggregate all results into a single results/summary_task2.csv file.
6. Generate plots in results/plots/ using plot_task2.py.

## 4. Output files

After running:
```bash
./run_task2.sh
```

you should see:

In results/:

- dense_ijk_n512.csv, dense_ijk_n1024.csv, ...
- dense_block_b64_n512.csv, ...
- dense_unroll_n512.csv, ...
- sparse_csr_n5000_d0.0001.csv, sparse_csr_n5000_d0.001.csv, ...
- summary_task2.csv (aggregated metrics)

In results/plots/:

- time_vs_size_dense.png
- time_vs_size_dense_log.png
- time_vs_density_sparse_n5000.png
- time_vs_density_sparse_n10000.png
- memory_vs_density_sparse_n5000.png
- memory_vs_density_sparse_n10000.png

These plots are the ones referenced in the LaTeX report.

## 5. Notes

- If you change matrix sizes, densities or block size, edit run_task2.sh.

- If clang++ is not available but you have g++, you can modify the
line in run_task2.sh to use g++ -O3 -std=c++17 ....

- All paths in scripts are defined relative to this Task 2 folder,
so always start with:

```bash
cd "TASK 2. Performance Benchmark of optimized and sparse matrix multiplication"
```

before running any command.
```makefile
::contentReference[oaicite:0]{index=0}
```