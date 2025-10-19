
import pandas as pd
import matplotlib.pyplot as plt
import numpy as np
from pathlib import Path

# === Configuración ===
summary_path = Path("results/summary.csv")
output_dir = Path("results/plots")
output_dir.mkdir(exist_ok=True)

# === Leer datos ===
df = pd.read_csv(summary_path)

# Ordenar datos para que los tamaños estén en orden creciente
df = df.sort_values(by=["n", "language"])

# === Colores y estilos por lenguaje ===
colors = {
    "python": "#FFB000",  # amarillo
    "java": "#0072B2",    # azul
    "cpp": "#009E73",     # verde
}

markers = {
    "python": "o",
    "java": "s",
    "cpp": "D",
}

# === Gráfico principal: Tiempo medio vs Tamaño ===
plt.figure(figsize=(8,5))
for lang in df["language"].unique():
    d = df[df["language"] == lang]
    plt.plot(d["n"], d["mean_seconds"], marker=markers[lang],
             color=colors[lang], linewidth=2, label=lang.capitalize())

plt.title("Comparativa de tiempo medio de ejecución")
plt.xlabel("Tamaño de la matriz (n × n)")
plt.ylabel("Tiempo medio (segundos)")
plt.legend(title="Lenguaje")
plt.grid(True, linestyle="--", alpha=0.6)
plt.tight_layout()

out_file = output_dir / "tiempo_vs_tamaño.png"
plt.savefig(out_file, dpi=300)
print(f" Gráfico guardado en: {out_file}")

# === Gráfico en escala logarítmica (opcional) ===
plt.figure(figsize=(8,5))
for lang in df["language"].unique():
    d = df[df["language"] == lang]
    plt.plot(d["n"], d["mean_seconds"], marker=markers[lang],
             color=colors[lang], linewidth=2, label=lang.capitalize())

plt.yscale("log")
plt.title("Tiempo medio (escala logarítmica)")
plt.xlabel("Tamaño de la matriz (n × n)")
plt.ylabel("Tiempo medio (segundos, log)")
plt.legend(title="Lenguaje")
plt.grid(True, linestyle="--", alpha=0.6)
plt.tight_layout()

out_file_log = output_dir / "tiempo_vs_tamaño_log.png"
plt.savefig(out_file_log, dpi=300)
print(f" Gráfico logarítmico guardado en: {out_file_log}")

# === Gráfico de barras: Tiempo medio por tamaño ===
plt.figure(figsize=(9,5))
n_values = sorted(df["n"].unique())
x = np.arange(len(n_values))
width = 0.25

for i, lang in enumerate(["python", "java", "cpp"]):
    d = df[df["language"] == lang]
    plt.bar(x + (i-1)*width, d["mean_seconds"], width, label=lang.capitalize(),
            color=colors[lang], edgecolor="black")

plt.title("Comparativa por tamaño (barras)")
plt.xlabel("Tamaño de matriz (n × n)")
plt.ylabel("Tiempo medio (segundos)")
plt.xticks(x, n_values)
plt.legend(title="Lenguaje")
plt.grid(axis="y", linestyle="--", alpha=0.6)
plt.tight_layout()

out_file_bar = output_dir / "barras_comparativa.png"
plt.savefig(out_file_bar, dpi=300)
print(f" Gráfico de barras guardado en: {out_file_bar}")

print("\n Todos los gráficos generados correctamente.")
