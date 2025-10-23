
import pandas as pd
import matplotlib.pyplot as plt
import numpy as np
from pathlib import Path

summary_path = Path("results/summary.csv")
output_dir = Path("results/plots")
output_dir.mkdir(exist_ok=True)

df = pd.read_csv(summary_path)

df = df.sort_values(by=["n", "language"])

colors = {
    "python": "#FFB000",  
    "java": "#0072B2",    
    "cpp": "#009E73",     
}

markers = {
    "python": "o",
    "java": "s",
    "cpp": "D",
}

plt.figure(figsize=(8,5))
for lang in df["language"].unique():
    d = df[df["language"] == lang]
    plt.plot(d["n"], d["mean_seconds"], marker=markers[lang],
             color=colors[lang], linewidth=2, label=lang.capitalize())

plt.title("Average execution time comparison")
plt.xlabel("Matrix size (n × n)")
plt.ylabel("Average time (seconds)")
plt.legend(title="Language")
plt.grid(True, linestyle="--", alpha=0.6)
plt.tight_layout()

out_file = output_dir / "time_vs_size.png"
plt.savefig(out_file, dpi=300)
print(f" Chart saved in: {out_file}")

plt.figure(figsize=(8,5))
for lang in df["language"].unique():
    d = df[df["language"] == lang]
    plt.plot(d["n"], d["mean_seconds"], marker=markers[lang],
             color=colors[lang], linewidth=2, label=lang.capitalize())

plt.yscale("log")
plt.title("Average time (logarithmic scale)")
plt.xlabel("Matrix size (n × n)")
plt.ylabel("Average time (seconds, log)")
plt.legend(title="Language")
plt.grid(True, linestyle="--", alpha=0.6)
plt.tight_layout()

out_file_log = output_dir / "time_vs_size_log.png"
plt.savefig(out_file_log, dpi=300)
print(f" Log. chart saved in: {out_file_log}")

plt.figure(figsize=(9,5))
n_values = sorted(df["n"].unique())
x = np.arange(len(n_values))
width = 0.25

for i, lang in enumerate(["python", "java", "cpp"]):
    d = df[df["language"] == lang]
    plt.bar(x + (i-1)*width, d["mean_seconds"], width, label=lang.capitalize(),
            color=colors[lang], edgecolor="black")

plt.title("Comparison by size (bars)")
plt.xlabel("Matrix size (n × n)")
plt.ylabel("Average time (seconds)")
plt.xticks(x, n_values)
plt.legend(title="Language")
plt.grid(axis="y", linestyle="--", alpha=0.6)
plt.tight_layout()

out_file_bar = output_dir / "comparison_bars.png"
plt.savefig(out_file_bar, dpi=300)
print(f" Bar chart saved in: {out_file_bar}")

print("\n All graphics generated correctly.")
