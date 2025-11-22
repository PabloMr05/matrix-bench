import pandas as pd
import matplotlib.pyplot as plt
from pathlib import Path

# Carpeta raíz = donde está este script
ROOT = Path(__file__).resolve().parent
SUMMARY = ROOT / "results" / "summary_task2.csv"
PLOTS_DIR = ROOT / "results" / "plots"

PLOTS_DIR.mkdir(parents=True, exist_ok=True)

print(f"[plot_task2] Leyendo resumen desde: {SUMMARY}")
df = pd.read_csv(SUMMARY)

# Algoritmos
DENSE_ALGOS = ["dense_ijk", "dense_block", "dense_unroll"]
SPARSE_ALGOS = ["sparse_csr"]

# -------------------------------------------------------------------
# 1) DENSOS: tiempo y memoria vs tamaño de la matriz
# -------------------------------------------------------------------
df_dense = df[df["algo"].isin(DENSE_ALGOS)].copy()
df_dense = df_dense.sort_values(["algo", "n"])

if not df_dense.empty:
    # Tiempo vs n
    plt.figure()
    for algo in DENSE_ALGOS:
        sub = df_dense[df_dense["algo"] == algo]
        if sub.empty:
            continue
        plt.plot(
            sub["n"],
            sub["avg_time_sec"],
            marker="o",
            label=algo,
        )
    plt.xlabel("Tamaño de la matriz (n)")
    plt.ylabel("Tiempo medio (s)")
    plt.title("Tiempo medio vs tamaño (métodos densos)")
    plt.grid(True, linestyle="--", alpha=0.6)
    plt.legend()
    out = PLOTS_DIR / "dense_time_vs_n.png"
    plt.tight_layout()
    plt.savefig(out, dpi=300)
    plt.close()
    print(f"[plot_task2] Guardado: {out}")

    # Memoria vs n
    plt.figure()
    for algo in DENSE_ALGOS:
        sub = df_dense[df_dense["algo"] == algo]
        if sub.empty:
            continue
        plt.plot(
            sub["n"],
            sub["avg_memory_mb"],
            marker="o",
            label=algo,
        )
    plt.xlabel("Tamaño de la matriz (n)")
    plt.ylabel("Memoria media (MB)")
    plt.title("Memoria media vs tamaño (métodos densos)")
    plt.grid(True, linestyle="--", alpha=0.6)
    plt.legend()
    out = PLOTS_DIR / "dense_memory_vs_n.png"
    plt.tight_layout()
    plt.savefig(out, dpi=300)
    plt.close()
    print(f"[plot_task2] Guardado: {out}")
else:
    print("[plot_task2] No hay datos para métodos densos en summary_task2.csv")

# -------------------------------------------------------------------
# 2) SPARSE: tiempo y memoria vs densidad (solo matrices aleatorias)
# -------------------------------------------------------------------
df_sparse = df[df["algo"].isin(SPARSE_ALGOS)].copy()

# Filtramos solo los n donde haya varias densidades distintas
# (así evitamos que mc2depi, con una sola densidad, genere gráficos raros)
def has_multiple_densities(g):
    return g["density"].nunique() > 1

df_sparse_rand = df_sparse.groupby("n", group_keys=False).filter(has_multiple_densities)
df_sparse_rand = df_sparse_rand.sort_values(["n", "density"])

if not df_sparse_rand.empty:
    for n_val in sorted(df_sparse_rand["n"].unique()):
        sub = df_sparse_rand[df_sparse_rand["n"] == n_val]
        if sub.empty:
            continue

        # Tiempo vs densidad (escala log)
        plt.figure()
        plt.plot(sub["density"], sub["avg_time_sec"], marker="o")
        plt.xscale("log")
        plt.xlabel("Densidad (fracción de elementos no nulos, escala log)")
        plt.ylabel("Tiempo medio (s)")
        plt.title(f"Tiempo medio vs densidad (sparse_csr, n={n_val})")
        plt.grid(True, linestyle="--", alpha=0.6)
        plt.tight_layout()
        out = PLOTS_DIR / f"time_vs_density_sparse_n{n_val}.png"
        plt.savefig(out, dpi=300)
        plt.close()
        print(f"[plot_task2] Guardado: {out}")

        # Memoria vs densidad (escala log)
        plt.figure()
        plt.plot(sub["density"], sub["avg_memory_mb"], marker="o")
        plt.xscale("log")
        plt.xlabel("Densidad (fracción de elementos no nulos, escala log)")
        plt.ylabel("Memoria media (MB)")
        plt.title(f"Memoria media vs densidad (sparse_csr, n={n_val})")
        plt.grid(True, linestyle="--", alpha=0.6)
        plt.tight_layout()
        out = PLOTS_DIR / f"memory_vs_density_sparse_n{n_val}.png"
        plt.savefig(out, dpi=300)
        plt.close()
        print(f"[plot_task2] Guardado: {out}")
else:
    print("[plot_task2] No hay suficientes densidades distintas para generar gráficos SPARSE aleatorios")

print("[plot_task2] Gráficas de Task 2 generadas correctamente.")
