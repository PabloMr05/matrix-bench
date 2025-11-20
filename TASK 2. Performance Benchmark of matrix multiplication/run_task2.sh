# =============================================
# Task 2 - Optimized & Sparse Matrix Multiplication (C++)
# =============================================

set -e

SIZES_DENSE=(512 1024 1536 2048)
RUNS=5
SEED=42

SIZES_SPARSE=(5000 10000)
DENSITIES=(0.10 0.01 0.001 0.0001)

# Ruta base = carpeta del Task 2
ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
RESULTS_DIR="${ROOT_DIR}/results"
CPP_DIR="${ROOT_DIR}/cpp"

mkdir -p "${RESULTS_DIR}"

echo "=============================================="
echo " Task 2: Benchmarks de algoritmos (C++)"
echo " Carpeta: ${ROOT_DIR}"
echo "=============================================="
echo

# --- Compilación ---
echo "[*] Compilando cpp/src/bench_task2.cpp ..."
cd "${CPP_DIR}"
clang++ -O3 -std=c++17 src/bench_task2.cpp -o bench_task2
cd "${ROOT_DIR}"
echo "Compilación OK."
echo

# --- Algoritmos densos ---
echo "[*] Ejecutando benchmarks DENSOS..."
for N in "${SIZES_DENSE[@]}"; do
  echo "  → n=${N}, runs=${RUNS}"

  "${CPP_DIR}/bench_task2" \
    --algo dense_ijk \
    --n "${N}" \
    --runs "${RUNS}" \
    --seed "${SEED}" \
    --out "${RESULTS_DIR}/dense_ijk_n${N}.csv"

  "${CPP_DIR}/bench_task2" \
    --algo dense_block \
    --n "${N}" \
    --runs "${RUNS}" \
    --seed "${SEED}" \
    --block 64 \
    --out "${RESULTS_DIR}/dense_block_b64_n${N}.csv"

  "${CPP_DIR}/bench_task2" \
    --algo dense_unroll \
    --n "${N}" \
    --runs "${RUNS}" \
    --seed "${SEED}" \
    --out "${RESULTS_DIR}/dense_unroll_n${N}.csv"
done
echo

# --- Algoritmo sparse ---
echo "[*] Ejecutando benchmarks SPARSE (CSR)..."
for N in "${SIZES_SPARSE[@]}"; do
  echo "  → n=${N}, runs=${RUNS}"
  for D in "${DENSITIES[@]}"; do
    echo "     - density=${D}"
    "${CPP_DIR}/bench_task2" \
      --algo sparse_csr \
      --n "${N}" \
      --runs "${RUNS}" \
      --seed "${SEED}" \
      --density "${D}" \
      --out "${RESULTS_DIR}/sparse_csr_n${N}_d${D}.csv"
  done
done
echo

echo "[*] Creando results/summary_task2.csv ..."
python3 - << 'EOF'
import pandas as pd
import glob
import numpy as np
from pathlib import Path

ROOT = Path(__file__).resolve().parent
files = glob.glob(str(ROOT / "results" / "*.csv"))
rows = []

for f in files:
    if "summary" in f:
        continue
    df = pd.read_csv(f)

    algo = df["algo"].iloc[0]
    n = int(df["n"].iloc[0])
    density = float(df["density"].iloc[0])
    block_size = int(df["block_size"].iloc[0])

    mean_secs = df["seconds"].astype(float).mean()
    std_secs  = df["seconds"].astype(float).std()
    mean_mem  = df["memory_mb"].astype(float).mean()
    mean_cpu  = df["cpu_percent"].astype(float).mean()
    mean_nnz  = df["nnz"].astype(float).mean()

    rows.append(
        (algo, n, density, block_size,
         mean_secs, std_secs, mean_mem, mean_cpu, mean_nnz)
    )

summary = pd.DataFrame(
    rows,
    columns=[
        "algo","n","density","block_size",
        "mean_seconds","std_seconds","mean_memory_mb","mean_cpu_percent","mean_nnz"
    ]
)

summary = summary.sort_values(by=["algo", "n", "density", "block_size"])
out_path = ROOT / "results" / "summary_task2.csv"
out_path.write_text(summary.to_csv(index=False))
print(summary)
EOF

echo
echo "Benchmark Task 2 completado. Resultados en 'results/'."
echo "----------------------------------------------"
ls "${RESULTS_DIR}"
echo "----------------------------------------------"

if [ -f "${ROOT_DIR}/plot_task2.py" ]; then
  echo
  echo "[*] Generando gráficas con plot_task2.py..."
  python3 "${ROOT_DIR}/plot_task2.py"
  echo "Gráficas guardadas en results/plots/"
fi
