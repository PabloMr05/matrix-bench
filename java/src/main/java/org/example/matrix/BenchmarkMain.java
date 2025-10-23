package org.example.matrix;

import java.io.FileWriter;
import java.io.PrintWriter;
import java.util.Locale;


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
            pw.println("language,n,run,seconds,memory_mb,cpu_percent");
            // Warmup JIT
            Matrix.matmul_ijk(A, B);
            Matrix.matmul_ijk(A, B);

            java.lang.management.OperatingSystemMXBean osBean =
                    java.lang.management.ManagementFactory.getOperatingSystemMXBean();

            com.sun.management.OperatingSystemMXBean sunOsBean = null;
            if (osBean instanceof com.sun.management.OperatingSystemMXBean) {
                sunOsBean = (com.sun.management.OperatingSystemMXBean) osBean;
            }


            for (int r=1; r<=runs; r++) {
                
                Runtime runtime = Runtime.getRuntime();
                runtime.gc(); 
                long usedBefore = (runtime.totalMemory() - runtime.freeMemory()) / (1024 * 1024);

                double cpuBefore = (sunOsBean != null) ? sunOsBean.getProcessCpuLoad() : -1;
                
                long t0 = System.nanoTime();
                Matrix.matmul_ijk(A, B);
                long t1 = System.nanoTime();
                
                long usedAfter = (runtime.totalMemory() - runtime.freeMemory()) / (1024 * 1024);
                long memUsed = Math.max(usedBefore, usedAfter); // MB

                double cpuAfter = (sunOsBean != null) ? sunOsBean.getProcessCpuLoad() : -1;
                double avgCpu = (cpuBefore >= 0 && cpuAfter >= 0)
                        ? ((cpuBefore + cpuAfter) / 2.0) * 100.0 : -1.0;


                double secs = (t1 - t0)/1e9;
                pw.printf(Locale.US, "java,%d,%d,%.6f,%.2f,%.1f%n", n, r, secs, (double) memUsed, avgCpu);
            }
        }
    }
}

