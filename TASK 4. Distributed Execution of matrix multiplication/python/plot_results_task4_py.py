import pandas as pd
import matplotlib.pyplot as plt

# Load results
df = pd.read_csv("results/task4_python_results.csv")

# Unique values
sizes = sorted(df["n"].unique())
workers_list = sorted(df["workers"].unique())

# ----------------------------------------------------------
# 1. PLOT TIME vs SIZE (one curve per worker count)
# ----------------------------------------------------------
plt.figure(figsize=(10, 6))
for w in workers_list:
    subset = df[df["workers"] == w]
    plt.plot(
        subset["n"],
        subset["time_ms"],
        marker="o",
        label=f"{w} workers"
    )

plt.title("Python Distributed Matrix Multiplication - Time vs Size")
plt.xlabel("Matrix Size (n)")
plt.ylabel("Execution Time (ms)")
plt.grid(True, linestyle="--", alpha=0.5)
plt.legend()
plt.tight_layout()
plt.savefig("results/task4_python_time_vs_size.png")
plt.close()

# ----------------------------------------------------------
# 2. PLOT MEMORY vs SIZE (one curve per worker count)
# ----------------------------------------------------------
plt.figure(figsize=(10, 6))
for w in workers_list:
    subset = df[df["workers"] == w]
    plt.plot(
        subset["n"],
        subset["memory_mb"],
        marker="s",
        label=f"{w} workers"
    )

plt.title("Python Distributed Matrix Multiplication - Memory vs Size")
plt.xlabel("Matrix Size (n)")
plt.ylabel("Memory (MB)")
plt.grid(True, linestyle="--", alpha=0.5)
plt.legend()
plt.tight_layout()
plt.savefig("results/task4_python_memory_vs_size.png")
plt.close()

print("Plots saved in results/:")
print(" - task4_python_time_vs_size.png")
print(" - task4_python_memory_vs_size.png")
