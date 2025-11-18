import argparse, random, time, csv, psutil, os
from matrix import matmul_ijk

def gen_matrix(n, rng):
    return [[rng.random() for _ in range(n)] for _ in range(n)]

if __name__ == "__main__":
    ap = argparse.ArgumentParser()
    ap.add_argument("--n", type=int, default=256)
    ap.add_argument("--runs", type=int, default=5)
    ap.add_argument("--seed", type=int, default=123)
    ap.add_argument("--out", type=str, default="../results/python.csv")
    args = ap.parse_args()

    rng = random.Random(args.seed)
    A = gen_matrix(args.n, rng)
    B = gen_matrix(args.n, rng)


    proc = psutil.Process(os.getpid())


    with open(args.out, "w", newline="") as f:
        w = csv.writer(f)
        w.writerow(["language","n","run","seconds", "memory_mb", "cpu_percent"])
        # Warmup ligero
        matmul_ijk(A[:min(8,args.n)], B[:min(8,args.n)])
        for r in range(1, args.runs+1):
            proc.cpu_percent(None)
            t0 = time.perf_counter()
            C = matmul_ijk(A, B)
            t1 = time.perf_counter()
            mem_mb = proc.memory_info().rss / (1024 * 1024)
            cpu_pct = proc.cpu_percent(None)

            w.writerow(["python", args.n, r, f"{t1 - t0:.6f}", f"{mem_mb:.2f}", f"{cpu_pct:.1f}"])
