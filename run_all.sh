# =============================================
# Automatic Benchmark: Python, Java and C++
# =============================================

SIZES=(128 256 384 512)   
RUNS=5
SEED=42

RESULTS_DIR="results"
PYTHON_DIR="python"
JAVA_DIR="java"
CPP_DIR="cpp"

mkdir -p $RESULTS_DIR

echo "=============================================="
echo " Running benchmarks in multiple languages..."
echo "=============================================="
echo

# --- Python ---
echo "Python"
for N in "${SIZES[@]}"; do
    OUT_FILE="${RESULTS_DIR}/python_${N}.csv"
    echo "  → n=${N}, runs=${RUNS}"
    python3 ${PYTHON_DIR}/bench.py --n ${N} --runs ${RUNS} --seed ${SEED} --out ${OUT_FILE}
done
echo

# --- Java ---
echo "☕ Java"
cd ${JAVA_DIR}
mkdir -p out
javac -d out src/main/java/org/example/matrix/*.java
cd out
for N in "${SIZES[@]}"; do
    OUT_FILE="../../${RESULTS_DIR}/java_${N}.csv"
    echo "  → n=${N}, runs=${RUNS}"
    java org.example.matrix.BenchmarkMain -n ${N} -runs ${RUNS} -seed ${SEED} -out ${OUT_FILE}
done
cd ../..
echo

# --- C++ ---
echo "C++"
cd ${CPP_DIR}
clang++ -O3 -std=c++17 src/bench_main.cpp -o bench
for N in "${SIZES[@]}"; do
    OUT_FILE="../${RESULTS_DIR}/cpp_${N}.csv"
    echo "  → n=${N}, runs=${RUNS}"
    ./bench --n ${N} --runs ${RUNS} --seed ${SEED} --out ${OUT_FILE}
done
cd ..
echo

echo "Creating summary in results/summary.csv..."
python3 - <<'EOF'
import pandas as pd
import glob
import numpy as np

files = glob.glob("results/*.csv")
data = []
for f in files:
    if "summary" in f: continue
    df = pd.read_csv(f)
    lang = df["language"].iloc[0]
    n = df["n"].iloc[0]
    mean = df["seconds"].astype(float).mean()
    std = df["seconds"].astype(float).std()
    data.append((lang, n, mean, std))

summary = pd.DataFrame(data, columns=["language", "n", "mean_seconds", "std_seconds"])
summary = summary.sort_values(by=["n", "language"])
summary.to_csv("results/summary.csv", index=False)
print(summary)
EOF

echo
echo "Benchmark completed. Results in folder. 'results/'"
echo "----------------------------------------------"
echo "Generated files:"
ls results
echo "----------------------------------------------"
echo "Summary table saved in: results/summary.csv"

if [ -f "plot_results.py" ]; then
    echo
    echo "Generating charts withplot_results.py..."
    python3 plot_results.py
    echo "Charts saved in results/plots/"
fi
