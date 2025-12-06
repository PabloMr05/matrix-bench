import pandas as pd
import matplotlib.pyplot as plt

df = pd.read_csv("results/task3_cpp_results.csv")

methods = df["method"].unique()
sizes = sorted(df["n"].unique())

# ----- EXECUTION TIME -----
plt.figure(figsize=(12,6))
for method in methods:
    sub = df[df["method"] == method]
    plt.plot(sub["n"], sub["time_ms"], marker="o", label=method)

plt.title("Execution Time (C++ Task 3)")
plt.xlabel("Matrix size n x n")
plt.ylabel("Time (ms)")
plt.grid(True)
plt.legend()
plt.tight_layout()
plt.savefig("results/time_plot_cpp.png", dpi=300)
plt.close()


# ----- SPEEDUP -----
baseline = df[df["method"] == "baseline"].set_index("n")["time_ms"]

plt.figure(figsize=(12,6))
for method in methods:
    if method == "baseline":
        continue
    sub = df[df["method"] == method].set_index("n")["time_ms"]
    speed = baseline / sub
    plt.plot(sizes, speed, marker="o", label=method)

plt.title("Speedup vs Baseline")
plt.xlabel("Matrix size n x n")
plt.ylabel("Speedup")
plt.grid(True)
plt.legend()
plt.tight_layout()
plt.savefig("results/speedup_plot_cpp.png", dpi=300)
plt.close()


# ----- EFFICIENCY -----
thread_map = {
    "baseline": 1,
    "omp_parallel": 1,
    "omp_parallel_2": 2,
    "omp_parallel_4": 4,
    "omp_parallel_8": 8,
    "omp_collapse": 1,
    "omp_simd": 1,
    "unroll": 1
}

plt.figure(figsize=(12,6))
for method in methods:
    if method == "baseline":
        continue
    threads = thread_map[method]
    sub = df[df["method"] == method].set_index("n")["time_ms"]
    speed = baseline / sub
    eff = speed / threads
    plt.plot(sizes, eff, marker="o", label=f"{method} (p={threads})")

plt.title("Parallel Efficiency (C++)")
plt.xlabel("Matrix size n x n")
plt.ylabel("Efficiency")
plt.grid(True)
plt.legend()
plt.tight_layout()
plt.savefig("results/efficiency_plot_cpp.png", dpi=300)
plt.close()

print("\nGenerated:")
print("  results/time_plot_cpp.png")
print("  results/speedup_plot_cpp.png")
print("  results/efficiency_plot_cpp.png")
