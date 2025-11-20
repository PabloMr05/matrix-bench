import pandas as pd
import matplotlib.pyplot as plt
import numpy as np
from pathlib import Path

ROOT = Path(__file__).resolve().parent
summary_path = ROOT / "results" / "summary_task2.csv"
df = pd.read_csv(summary_path)

plots_dir = ROOT / "results" / "plots"
plots_dir.mkdir(parents=True, exist_ok=True)

dense_algos = ["dense_ijk", "dense_block", "dense_unroll"]
sparse_algos = ["sparse_csr"]

colors = {
    "dense_ijk":   "#1f77b4",
    "dense_block": "#ff7f0e",
    "dense_unroll":"#2ca02c",
    "sparse_csr":  "#d62728",
}

markers = {
    "dense_ijk":   "o",
    "dense_block": "s",
    "dense_unroll":"^",
    "sparse_csr":  "x",
}

# ============================
# 1) Tiempo vs tamaño (densos)
# ============================
df_dense = df[df["algo"].isin(dense_algos)]

plt.figure(figsize=(8,5))
for algo in dense_algos:
    sub = df_dense[df_dense["algo"] == algo].sort_values("n")
    if sub.empty:
        continue
    plt.plot(
        sub["n"],
        sub["mean_seconds"],
        marker=markers[algo],
        color=colors[algo],
        linewidth=2,
        label=algo
    )

plt.title("Average execution time vs size (dense algorithms)")
plt.xlabel("Matrix size n (n × n)")
plt.ylabel("Average time (seconds)")
plt.grid(True, linestyle="--", alpha=0.6)
plt.legend()
plt.tight_layout()
out = plots_dir / "time_vs_size_dense.png"
plt.savefig(out, dpi=300)
plt.close()
print(f"Saved: {out}")

# Escala log
plt.figure(figsize=(8,5))
for algo in dense_algos:
    sub = df_dense[df_dense["algo"] == algo].sort_values("n")
    if sub.empty:
        continue
    plt.plot(
        sub["n"],
        sub["mean_seconds"],
        marker=markers[algo],
        color=colors[algo],
        linewidth=2,
        label=algo
    )

plt.yscale("log")
plt.title("Average execution time vs size (dense, log scale)")
plt.xlabel("Matrix size n (n × n)")
plt.ylabel("Average time (seconds, log)")
plt.grid(True, linestyle="--", alpha=0.6)
plt.legend()
plt.tight_layout()
out = plots_dir / "time_vs_size_dense_log.png"
plt.savefig(out, dpi=300)
plt.close()
print(f"Saved: {out}")

# ============================
# 2) Tiempo y memoria vs densidad (sparse)
# ============================
df_sparse = df[df["algo"].isin(sparse_algos)]

if not df_sparse.empty:
    for n_val in sorted(df_sparse["n"].unique()):
        sub = df_sparse[df_sparse["n"] == n_val].sort_values("density")
        if sub.empty:
            continue

        # Tiempo vs densidad
        plt.figure(figsize=(8,5))
        plt.plot(
            sub["density"],
            sub["mean_seconds"],
            marker=markers["sparse_csr"],
            color=colors["sparse_csr"],
            linewidth=2
        )
        plt.xscale("log")
        plt.title(f"Average time vs density (sparse_csr, n={n_val})")
        plt.xlabel("Density (fraction of non-zero entries, log scale)")
        plt.ylabel("Average time (seconds)")
        plt.grid(True, linestyle="--", alpha=0.6)
        plt.tight_layout()
        out = plots_dir / f"time_vs_density_sparse_n{n_val}.png"
        plt.savefig(out, dpi=300)
        plt.close()
        print(f"Saved: {out}")

        # Memoria vs densidad
        plt.figure(figsize=(8,5))
        plt.plot(
            sub["density"],
            sub["mean_memory_mb"],
            marker=markers["sparse_csr"],
            color=colors["sparse_csr"],
            linewidth=2
        )
        plt.xscale("log")
        plt.title(f"Average memory vs density (sparse_csr, n={n_val})")
        plt.xlabel("Density (fraction of non-zero entries, log scale)")
        plt.ylabel("Average memory (MB)")
        plt.grid(True, linestyle="--", alpha=0.6)
        plt.tight_layout()
        out = plots_dir / f"memory_vs_density_sparse_n{n_val}.png"
        plt.savefig(out, dpi=300)
        plt.close()
        print(f"Saved: {out}")

print("\nAll Task 2 graphics generated correctly.")
