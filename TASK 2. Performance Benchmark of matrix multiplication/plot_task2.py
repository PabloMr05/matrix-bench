import pandas as pd
import matplotlib.pyplot as plt
from pathlib import Path

ROOT = Path(__file__).resolve().parent
SUMMARY = ROOT / "results" / "summary_task2.csv"
PLOTS_DIR = ROOT / "results" / "plots"

PLOTS_DIR.mkdir(parents=True, exist_ok=True)

print(f"[plot_task2] Leyendo resumen desde: {SUMMARY}")
df = pd.read_csv(SUMMARY)

# Algoritmos
DENSE_ALGOS = ["dense_ijk", "dense_block", "dense_unroll"]
SPARSE_ALGOS = ["sparse_csr"]


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
    plt.xlabel("Matrix Size (n)")
    plt.ylabel("Average time (s)")
    plt.title("Average time vs size (dense methods)")
    plt.grid(True, linestyle="--", alpha=0.6)
    plt.legend()
    out = PLOTS_DIR / "dense_time_vs_n.png"
    plt.tight_layout()
    plt.savefig(out, dpi=300)
    plt.close()
    print(f"[plot_task2] Saved: {out}")

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
    plt.xlabel("Matrix Size (n)")
    plt.ylabel("Average memory (MB)")
    plt.title("Average memory vs size (dense methods)")
    plt.grid(True, linestyle="--", alpha=0.6)
    plt.legend()
    out = PLOTS_DIR / "dense_memory_vs_n.png"
    plt.tight_layout()
    plt.savefig(out, dpi=300)
    plt.close()
    print(f"[plot_task2] Saved: {out}")
else:
    print("[plot_task2] No hay datos para métodos densos en summary_task2.csv")


df_sparse = df[df["algo"].isin(SPARSE_ALGOS)].copy()

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
        plt.xlabel("Density (fraction of non-null elements, log scale)")
        plt.ylabel("Average time (s)")
        plt.title(f"Average time vs density (sparse_csr, n={n_val})")
        plt.grid(True, linestyle="--", alpha=0.6)
        plt.tight_layout()
        out = PLOTS_DIR / f"time_vs_density_sparse_n{n_val}.png"
        plt.savefig(out, dpi=300)
        plt.close()
        print(f"[plot_task2] Saved: {out}")

        # Memoria vs densidad (escala log)
        plt.figure()
        plt.plot(sub["density"], sub["avg_memory_mb"], marker="o")
        plt.xscale("log")
        plt.xlabel("Density (fraction of non-null elements, log scale)")
        plt.ylabel("Average memory (MB)")
        plt.title(f"Average memory  vs density (sparse_csr, n={n_val})")
        plt.grid(True, linestyle="--", alpha=0.6)
        plt.tight_layout()
        out = PLOTS_DIR / f"memory_vs_density_sparse_n{n_val}.png"
        plt.savefig(out, dpi=300)
        plt.close()
        print(f"[plot_task2] Saved: {out}")
else:
    print("[plot_task2] No hay suficientes densidades distintas para generar gráficos SPARSE aleatorios")

print("[plot_task2] Task 2 graphs generated correctly.")
