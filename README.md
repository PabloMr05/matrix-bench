# matrix-bench

## 1. Overview

This project evaluates the performance of a basic **matrix multiplication algorithm (O(n³))** implemented in **Python**, **Java**, and **C++**.  
The objective is to analyze the computational efficiency of each language when performing large-scale arithmetic operations, focusing on **execution time**, **efficiency**, and **scalability**.

---

## 2. Methodology

### Algorithm
All implementations use the classical triple-nested loop for matrix multiplication in `i–j–k` order:

for i in range(n):
for j in range(n):
for k in range(n):
C[i][j] += A[i][k] * B[k][j]


### Parameters
- **Matrix sizes:** 128×128, 256×256, 384×384, 512×512  
- **Runs per size:** 5  
- **Data:** Random floating-point numbers in [0, 1)  
- **Metrics:** Mean and standard deviation of execution time (seconds)  
- **Warm-up:** Included for Java to stabilize JIT compilation  

### Environment
Experiments were executed using:
- **Python 3.10+**
- **Java 17+**
- **C++17 (clang++/g++)**
- Operating System: macOS (64-bit)

---

## 3. Project Structure

matrix-bench/
├─ python/ # Python implementation and benchmark
├─ java/ # Java implementation and benchmark
├─ cpp/ # C++ implementation and benchmark
├─ results/ # CSV data and generated plots
├─ run_all.sh # Automation script for all benchmarks
├─ plot_results.py # Chart generation script
└─ paper/ # Report and supporting documentation


---

## 4. Execution Procedure

To reproduce the results:

```bash
./run_all.sh

```
This script performs the following:

1. Executes benchmarks for Python, Java, and C++.

2. Saves individual results in results/*.csv.

3. Creates a consolidated summary (results/summary.csv).

4. Generates comparative charts (results/plots/).

---

## 6. Discussion

C++ achieves the lowest execution time due to native compilation and memory locality.

Java performs competitively after JIT warm-up, showing consistent execution.

Python is substantially slower but remains advantageous for readability and rapid prototyping.

Differences highlight how compilation strategies influence CPU-bound performance.

---

## 7. Output Files

- results/*.csv: Raw benchmark data for each language

- results/summary.csv: Combined mean and standard deviation

- results/plots/: Visual comparisons (time vs. matrix size, logarithmic scale, and bar charts)

---

## 8. Repository

GitHub repository: https://github.com/PabloMr05/matrix-bench

---