import time
import numpy as np
import pandas as pd
from distributed_mm import generate_matrix, multiply_chunk

sizes = [500, 1000, 1250, 1500]
workers_list = [1, 2, 4, 8]

results = []

for n in sizes:

    A = generate_matrix(n)
    B = generate_matrix(n)

    for workers in workers_list:

        chunk = n // workers
        start = time.time()
        C = np.zeros((n, n))

        current = 0
        for w in range(workers):
            row_start = current
            row_end = n if w == workers - 1 else current + chunk

            C[row_start:row_end] = multiply_chunk(A, B, row_start, row_end)
            current = row_end

        end = time.time()
        elapsed = (end - start) * 1000
        mem = C.nbytes / (1024 * 1024)

        print(f"✔ Python Distributed: n={n} workers={workers} time={elapsed:.3f} ms mem={mem:.2f} MB")

        results.append([n, workers, elapsed, mem])

df = pd.DataFrame(results, columns=["n", "workers", "time_ms", "memory_mb"])
df.to_csv("results/task4_python_results.csv", index=False)
