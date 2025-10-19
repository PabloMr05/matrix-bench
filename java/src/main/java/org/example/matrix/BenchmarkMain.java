package org.example.matrix;

import java.io.FileWriter;
import java.io.PrintWriter;

public class BenchmarkMain {
    public static void main(String[] args) throws Exception {
        int n = 512, runs = 5;
        long seed = 123L;
        String out = "../results/java.csv";
        for (int i=0;i<args.length;i++) {
            switch (args[i]) {
                case "-n": n = Integer.parseInt(args[++i]); break;
                case "-runs": runs = Integer.parseInt(args[++i]); break;
                case "-seed": seed = Long.parseLong(args[++i]); break;
                case "-out": out = args[++i]; break;
            }
        }

        double[][] A = Matrix.randomMatrix(n, seed);
        double[][] B = Matrix.randomMatrix(n, seed+1);

        try (PrintWriter pw = new PrintWriter(new FileWriter(out))) {
            pw.println("language,n,run,seconds");
            // Warmup JIT
            Matrix.matmul_ijk(A, B);
            Matrix.matmul_ijk(A, B);

            for (int r=1; r<=runs; r++) {
                long t0 = System.nanoTime();
                double[][] C = Matrix.matmul_ijk(A, B);
                long t1 = System.nanoTime();
                double secs = (t1 - t0)/1e9;
                pw.printf("java,%d,%d,%.6f%n", n, r, secs);
            }
        }
    }
}

