# =============================================
# Benchmark Automático: Python, Java y C++
# =============================================

# --- CONFIGURACIÓN ---
SIZES=(128 256 384 512)   # tamaños de matriz
RUNS=5
SEED=42

RESULTS_DIR="results"
PYTHON_DIR="python"
JAVA_DIR="java"
CPP_DIR="cpp"

mkdir -p $RESULTS_DIR

echo "=============================================="
echo " Ejecutando benchmarks en varios lenguajes..."
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
cd ../../..
echo

# --- C++ ---
echo "C++"
cd ${CPP_DIR}
g++ -O3 -march=native src/bench_main.cpp -o bench
for N in "${SIZES[@]}"; do
    OUT_FILE="../${RESULTS_DIR}/cpp_${N}.csv"
    echo "  → n=${N}, runs=${RUNS}"
    ./bench --n ${N} --runs ${RUNS} --seed ${SEED} --out ${OUT_FILE}
done
cd ..
echo

# --- Generar resumen combinado ---
echo "Creando resumen en results/summary.csv..."
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
echo "Benchmark completado. Resultados en la carpeta 'results/'"
echo "----------------------------------------------"
echo "Archivos generados:"
ls results
echo "----------------------------------------------"
echo "Tabla resumen guardada en: results/summary.csv"
