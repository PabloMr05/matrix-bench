import java.io.File;
import java.io.FileWriter;

public class DistributedRowColumnMajor {

    public static void main(String[] args) throws Exception {

        if (args.length != 2) {
            System.out.println("Usage: java DistributedRowColumnMajor <n> <workers>");
            return;
        }

        int n = Integer.parseInt(args[0]);
        int workers = Integer.parseInt(args[1]);

        double[][] A = MatrixUtils.generateMatrix(n);
        double[][] B = MatrixUtils.generateMatrix(n);
        double[][] C = new double[n][n];

        int chunk = n / workers;
        long start = System.nanoTime();

        int current = 0;

        for (int w = 0; w < workers; w++) {
            int rowStart = current;
            int rowEnd = (w == workers - 1) ? n : current + chunk;

            ChunkTask task = new ChunkTask(A, B, rowStart, rowEnd);
            double[][] part = task.compute();

            for (int i = 0; i < part.length; i++)
                System.arraycopy(part[i], 0, C[rowStart + i], 0, n);

            current = rowEnd;
        }

        long end = System.nanoTime();
        double ms = (end - start) / 1e6;

        File file = new File("results/task4_java_results.csv");
        if (!file.exists()) {
            FileWriter fw = new FileWriter(file);
            fw.write("n,workers,time_ms\n");
            fw.close();
        }

        FileWriter fw = new FileWriter(file, true);
        fw.write(n + "," + workers + "," + ms + "\n");
        fw.close();

        System.out.println("✔ Java Distributed: n=" + n + " workers=" + workers + " time_ms=" + ms);
    }
}
