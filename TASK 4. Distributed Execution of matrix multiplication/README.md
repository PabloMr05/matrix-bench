# Task 4 – Distributed Execution of Matrix Multiplication

This project implements distributed matrix multiplication in **Python** and **Java**.  
It includes benchmarking, CSV result logging, and performance visualization.

---

## Features

### Python Distributed Multiplication
- Uses multiprocessing to parallelize row-block multiplication
- Benchmarks multiple matrix sizes and workers
- Tracks execution time and memory usage
- Saves performance results to CSV
- Generates plots automatically

### Java Distributed Multiplication
- Custom thread‑based execution model
- Benchmarks multiple worker configurations
- Logs results into CSV
- Plotting via Python scripts for consistency

---

## Project Structure (expected)

```
Task_4/
│
├── python/
│   ├── distributed_mm.py
│   ├── benchmark_task4_py.py
│   ├── plot_results_task4_py.py
│   └── results/
│       ├── performance_python.csv
│       └── python_plots.png
│
├── java/
│   ├── src/
│   │   ├── DistributedMatrixMultiplication.java
│   │   ├── MatrixUtils.java
│   │   └── RowBlockTask.java
│   ├── bin/
│   ├── compile.sh
│   ├── run_java_benchmark.sh
│   ├── plot_results_task4_java.py
│   └── results/
│       ├── performance_java.csv
│       └── java_plots.png
│
└── README.md
```

---

## How to Run

### Python
```bash
cd python
python3 benchmark_task4_py.py
python3 plot_results_task4_py.py
```

### Java
```bash
cd java
chmod +x compile.sh
./compile.sh

java -cp bin DistributedMatrixMultiplication <size> <workers>

python3 plot_results_task4_java.py
```

---

## 📊 Outputs
- Execution time comparisons
- Memory usage trends
- Scalability analysis
- CSV logs & plots
