# =============================================
# Task 2 - Optimized & Sparse Matrix Multiplication (C++)
# =============================================

set -e

# --- Rutas base ---
ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
CPP_DIR="${ROOT_DIR}/cpp"
SRC_DIR="${CPP_DIR}/src"
RESULTS_DIR="${ROOT_DIR}/results"

mkdir -p "${RESULTS_DIR}"

# --- Parámetros de los experimentos ---
SIZES_DENSE=(512 1024 1536 2048)
RUNS=5
SEED=42

SIZES_SPARSE=(5000 10000)
DENSITIES=(0.10 0.01 0.001 0.0001)

BLOCK_SIZE=64

# --- Compilación ---
echo "[*] Compilando cpp/src/bench_task2.cpp ..."

CXX="${CXX:-clang++}"
CXXFLAGS="-O3 -std=c++17 -DNDEBUG -std=c++17"

cd "${CPP_DIR}"
${CXX} ${CXXFLAGS} -o bench_task2 "${SRC_DIR}/bench_task2.cpp"
cd "${ROOT_DIR}"

echo "[*] Compilación completada."
echo

# -------------------------
# 1) Experimentos DENSOS
# -------------------------
echo "=== Experimentos DENSOS ==="

for n in "${SIZES_DENSE[@]}"; do
  for algo in dense_ijk dense_block dense_unroll; do
    out_file="${RESULTS_DIR}/${algo}_n${n}.csv"
    echo "  -> Ejecutando ${algo}, n=${n}"
    if [[ "${algo}" == "dense_block" ]]; then
      "${CPP_DIR}/bench_task2" \
        --algo "${algo}" \
        --n "${n}" \
        --runs "${RUNS}" \
        --seed "${SEED}" \
        --block "${BLOCK_SIZE}" \
        --density 0.0 \
        --out "${out_file}"
    else
      "${CPP_DIR}/bench_task2" \
        --algo "${algo}" \
        --n "${n}" \
        --runs "${RUNS}" \
        --seed "${SEED}" \
        --density 0.0 \
        --out "${out_file}"
    fi
  done
done

# -------------------------
# 2) Experimentos SPARSE (aleatorias)
# -------------------------
echo
echo "=== Experimentos SPARSE (aleatorias) ==="

for n in "${SIZES_SPARSE[@]}"; do
  for dens in "${DENSITIES[@]}"; do
    out_file="${RESULTS_DIR}/sparse_csr_n${n}_d${dens}.csv"
    echo "  -> Ejecutando sparse_csr, n=${n}, density=${dens}"
    "${CPP_DIR}/bench_task2" \
      --algo sparse_csr \
      --n "${n}" \
      --runs "${RUNS}" \
      --seed "${SEED}" \
      --density "${dens}" \
      --out "${out_file}"
  done
done

# -------------------------
# 3) Experimento con mc2depi (si existe)
# -------------------------
echo
echo "=== Experimento SPARSE con mc2depi (si está disponible) ==="
MC2_PATH="${ROOT_DIR}/data/mc2depi/mc2depi.mtx"

if [[ -f "${MC2_PATH}" ]]; then
  echo "  -> Encontrado ${MC2_PATH}, ejecutando sparse_csr sobre mc2depi"
  "${CPP_DIR}/bench_task2" \
    --algo sparse_csr \
    --runs 3 \
    --seed "${SEED}" \
    --density 0.0 \
    --matrix_file "${MC2_PATH}" \
    --out "${RESULTS_DIR}/sparse_csr_mc2depi.csv"
else
  echo "  (mc2depi.mtx no encontrado en data/mc2depi/, se omite este experimento)"
fi

# -------------------------
# 4) Resumen con pandas
# -------------------------
echo
echo "[*] Generando resumen summary_task2.csv..."

python3 - << 'EOF'
import pandas as pd
from pathlib import Path

root = Path(__file__).resolve().parent
results_dir = root / "results"

csv_files = [p for p in results_dir.glob("*.csv") if p.name != "summary_task2.csv"]

if not csv_files:
    print("No se encontraron CSVs de resultados.")
    raise SystemExit(0)

dfs = [pd.read_csv(p) for p in csv_files]
df = pd.concat(dfs, ignore_index=True)

group_cols = ["algo", "n", "density", "block"]
agg = {
    "time_sec": ["mean", "std"],
    "memory_mb": "mean",
    "cpu_pct": "mean",
    "nnz_or_n2": "max",
}

g = df.groupby(group_cols, as_index=False).agg(agg)
g.columns = [
    "algo", "n", "density", "block",
    "avg_time_sec", "std_time_sec",
    "avg_memory_mb", "avg_cpu_pct",
    "nnz_or_n2"
]

out_path = results_dir / "summary_task2.csv"
g.to_csv(out_path, index=False)
print(f"Resumen guardado en: {out_path}")
EOF

echo
echo "Benchmark Task 2 completado. Resultados en 'results/'."
echo "----------------------------------------------"
ls "${RESULTS_DIR}"
echo "----------------------------------------------"

# -------------------------
# 5) Gráficas
# -------------------------
if [ -f "${ROOT_DIR}/plot_task2.py" ]; then
  echo
  echo "[*] Generando gráficas con plot_task2.py..."
  python3 "${ROOT_DIR}/plot_task2.py"
  echo "Gráficas guardadas en results/plots/"
fi
