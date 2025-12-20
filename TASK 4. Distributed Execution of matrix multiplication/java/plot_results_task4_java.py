import pandas as pd
import matplotlib.pyplot as plt

df = pd.read_csv("results/task4_java_results.csv")

plt.figure(figsize=(10,6))

for w in df["workers"].unique():
    subset = df[df["workers"] == w]
    plt.plot(subset["n"], subset["time_ms"], marker='o', label=f"{w} workers")

plt.xlabel("Matrix Size n")
plt.ylabel("Execution Time (ms)")
plt.title("Java Distributed Matrix Multiplication — Time vs Size")
plt.grid(True)
plt.legend()
plt.savefig("results/task4_java_time_vs_size.png", dpi=120)
plt.show()
