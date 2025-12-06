
# TASK 3 – Parallel (and Vectorized) Matrix Multiplication in C++

This directory contains the **C++ implementation and benchmark** for **Task 3: Parallel (and Vectorized) Matrix Multiplication**, as part of a matrix-multiplication performance study.

The goal of this task is to:
- Implement several optimized versions of dense matrix multiplication in C++.
- Compare them against a sequential baseline.
- Measure **execution time**, **speedup**, and **parallel efficiency**.
- Generate plots and a written report explaining the results.

---

## 1. Project Structure

```
TASK 3. Parallel (and Vectorized) Matrix Multiplication/
├── src/
│   ├── matrix.hpp              # Matrix helpers (allocation, initialization, utilities)
│   ├── baseline.cpp            # Sequential baseline implementation
│   ├── omp_parallel.cpp        # OpenMP parallel implementation (2/4/8 threads)
│   ├── omp_collapse.cpp        # OpenMP implementation using loop collapse
│   ├── omp_simd.cpp            # SIMD-oriented implementation (OpenMP simd + unrolling)
│   ├── benchmark.cpp           # Main benchmark driver (runs all versions and records results)
│
├── results/
│   └── task3_cpp_results.csv   # Benchmark output: times, speedup, efficiency, etc.
│
├── plot_results_cpp.py         # Python script to generate plots from the CSV
├── report_task3_cpp.tex        # LaTeX report (analysis, figures, discussion)
└── README.md                   # This file
```

> **Note:** File names may be slightly different depending on the final version, but the structure and roles are as above.

---

## 2. Implementations Overview

### 2.1 `baseline.cpp` – Sequential Reference

- Classical triple-nested loop matrix multiplication.
- No threads, no SIMD, no manual optimizations.
- Used as reference to compute **speedup** for all other implementations.

Conceptually:

```cpp
for (int i = 0; i < n; ++i) {
    for (int j = 0; j < n; ++j) {
        double sum = 0.0;
        for (int k = 0; k < n; ++k) {
            sum += A[i*n + k] * B[k*n + j];
        }
        C[i*n + j] = sum;
    }
}
```

### 2.2 `omp_parallel.cpp` – OpenMP Parallel

- Uses **OpenMP** to parallelize the outer loop(s).
- Evaluates different thread counts (e.g. 2, 4, 8).
- Demonstrates the effect of thread-level parallelism.

Example pattern:

```cpp
#pragma omp parallel for schedule(static)
for (int i = 0; i < n; ++i) {
    // compute row i of C
}
```

### 2.3 `omp_collapse.cpp` – OpenMP with Loop Collapsing

- Uses `#pragma omp parallel for collapse(2)` to parallelize **two nested loops** simultaneously.
- Exposes more parallelism by flattening the 2D iteration space `(i, j)` into one.
- May help for some sizes, but can negatively impact cache locality in others.

Key idea:

```cpp
#pragma omp parallel for collapse(2)
for (int i = 0; i < n; ++i) {
    for (int j = 0; j < n; ++j) {
        // inner k-loop as usual
    }
}
```

### 2.4 `omp_simd.cpp` – SIMD-Oriented Implementation

- Uses `#pragma omp simd` to hint the compiler to vectorize the inner loop.
- Takes advantage of **SIMD units** (vector instructions) of the CPU.
- Often combined with loop unrolling and careful memory access to maximise throughput.

Example pattern:

```cpp
double sum = 0.0;
#pragma omp simd
for (int k = 0; k < n; ++k) {
    sum += A[i*n + k] * B[k*n + j];
}
```

### 2.5 `benchmark.cpp` – Benchmark Orchestrator

- Contains the `main()` function.
- Generates input matrices (e.g. random or fixed).
- Runs each implementation (baseline, parallel, collapse, simd, etc.).
- Measures execution time using a high-resolution clock.
- Computes:
  - **Speedup** = `T_baseline / T_impl`
  - **Efficiency** (when applicable)
- Writes all results to `results/task3_cpp_results.csv`.

The CSV typically contains columns like:

- `method` – implementation name (baseline, omp_parallel_2, omp_parallel_4, omp_collapse, simd, ...)
- `n` – matrix size
- `threads` – number of threads used (if applicable)
- `time_ms` – execution time in milliseconds
- `speedup` – relative to baseline
- `efficiency` – speedup / threads (if applicable)

---

## 3. Requirements

### 3.1 Compiler and Tools

- **C++ compiler** with OpenMP support:
  - On macOS (Apple Silicon/Intel): `clang++` + `libomp` (via Homebrew)
  - On Linux: `g++` with `-fopenmp` usually works out of the box.
- **Python 3** with:
  - `pandas`
  - `matplotlib`

### 3.2 macOS – Installing `libomp` (if needed)

On macOS with Homebrew:

```bash
brew install libomp
```

Then you may need to expose headers and libs (Homebrew normally hints this):

```bash
export LDFLAGS="-L/opt/homebrew/opt/libomp/lib"
export CPPFLAGS="-I/opt/homebrew/opt/libomp/include"
```

---

## 4. Build Instructions

From inside the directory:

```bash
cd "TASK 3. Parallel (and Vectorized) Matrix Multiplication"
```

Then compile all C++ sources in `src/` into a single executable `benchmark`:

### 4.1 macOS (Clang + libomp)

```bash
clang++ -O3   -Xpreprocessor -fopenmp   -I/opt/homebrew/opt/libomp/include   -L/opt/homebrew/opt/libomp/lib   src/*.cpp -lomp -o benchmark
```

Notes:

- `-O3` enables aggressive optimizations (vectorization, unrolling, etc.).
- `-fopenmp` activates OpenMP pragmas.
- `-lomp` links the OpenMP runtime library.

### 4.2 Linux (g++ example)

```bash
g++ -O3 -fopenmp src/*.cpp -o benchmark
```

(Exact flags may vary depending on the environment.)

---

## 5. Running the Benchmark

From inside:

```bash
cd "TASK 3. Parallel (and Vectorized) Matrix Multiplication"
```

Execute:

```bash
./benchmark
```

This will:

1. Generate matrices for each size `n` (e.g. 200, 300, 400, 500).
2. Run:
   - baseline
   - OpenMP parallel versions (2/4/8 threads)
   - collapse
   - simd
3. Measure the execution time of each configuration.
4. Compute speedup and efficiency.
5. Save all results into:

```text
results/task3_cpp_results.csv
```

If the `results/` directory does not exist, ensure it is created before running:

```bash
mkdir -p results
```

---

## 6. Plotting Results

The file `plot_results_cpp.py` reads the CSV file and generates three plots:

- `time_plot_cpp.png` – Execution time vs. matrix size.
- `speedup_plot_cpp.png` – Speedup vs. matrix size.
- `efficiency_plot_cpp.png` – Efficiency vs. matrix size.

### 6.1 Running the plotting script

From the same directory:

```bash
python3 plot_results_cpp.py
```

Make sure you have the necessary Python packages installed:

```bash
pip install pandas matplotlib
```

After running, you should see the three PNG image files in the same folder.  
These are then included in the LaTeX report `report_task3_cpp.tex`.

---

## 7. Interpreting the Results

### 7.1 Execution Time (time_plot_cpp.png)

- The **baseline** shows cubic growth with respect to `n`, as expected from the $\Theta(n^3)$ complexity.
- OpenMP parallel versions significantly reduce execution time for all sizes.
- The **SIMD** version (`omp_simd`) and the most optimised variants typically have the lowest execution times.
- For small sizes, caches are more effective and optimisations show dramatic improvements.
- For larger matrices, performance becomes more **memory-bound** and the relative advantage of some optimisations shrinks.

### 7.2 Speedup (speedup_plot_cpp.png)

- Speedup is computed as `T_baseline / T_method`.
- Values greater than 1 mean the method is faster than the baseline.
- Occasionally, **superlinear speedup** (speedup > number of threads) appears:
  - due to improved cache locality,
  - reduced branch overhead,
  - compiler vectorization (FMA, unrolling, etc.).
- As `n` increases, speedup tends to flatten when memory bandwidth becomes the main bottleneck.

### 7.3 Efficiency (efficiency_plot_cpp.png)

- Efficiency is defined as `speedup / threads` for multi-threaded runs.
- High efficiency means each thread is being used effectively.
- Efficiency naturally decreases as the number of threads increases:
  - overheads (thread creation, scheduling),
  - contention for memory bandwidth,
  - cache invalidations between cores.
- Values above 1 (for SIMD or unrolled variants) indicate that the **baseline is relatively inefficient**, and that vectorized versions are exploiting the hardware much better.

---

## 8. LaTeX Report

The file:

```text
report_task3_cpp.tex
```

is a detailed written report that:

- Describes the mathematical background and complexity.
- Explains the hardware and memory model intuitively.
- Documents the different implementations and their rationale.
- Includes the generated plots:
  - `time_plot_cpp.png`
  - `speedup_plot_cpp.png`
  - `efficiency_plot_cpp.png`
- Provides an in-depth interpretation of:
  - why each method behaves as observed,
  - when parallelism helps,
  - when the memory wall limits performance,
  - and which optimisations provide the best trade-offs.

You can compile the report with:

```bash
pdflatex report_task3_cpp.tex
```

(or from Overleaf, uploading the `.tex` and the `.png` plots).

---

## 9. How to Extend This Work

If you want to push the project further, some ideas are:

- Add **tiled (blocked) matrix multiplication** to improve cache reuse.
- Implement explicit **SIMD intrinsics** (e.g. AVX2 / AVX-512) instead of relying only on `#pragma omp simd`.
- Compare against a reference BLAS implementation (OpenBLAS, BLIS, MKL).
- Port the benchmark to **CUDA** or **OpenCL** to analyse GPU performance.
- Explore different OpenMP scheduling strategies (`static`, `dynamic`, `guided`) and chunk sizes.

---

## 10. Summary

This Task 3 directory provides a complete mini-benchmarking suite for studying how different C++ implementation strategies affect the performance of dense matrix multiplication:

- From sequential to OpenMP parallel.
- From scalar to SIMD/vectorized.
- From naive loops to optimised inner kernels.

It is designed to be:
- **Reproducible** (fixed structure and scripts),
- **Extensible** (easy to add new variants),
- And **didactic** (clear mapping from code → CSV → plots → LaTeX report).
