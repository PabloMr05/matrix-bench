public class ChunkTask {

    private final double[][] A, B;
    private final int rowStart, rowEnd;

    public ChunkTask(double[][] A, double[][] B, int rowStart, int rowEnd) {
        this.A = A;
        this.B = B;
        this.rowStart = rowStart;
        this.rowEnd = rowEnd;
    }

    public double[][] compute() {
        int n = B.length;
        int rows = rowEnd - rowStart;
        double[][] C = new double[rows][n];

        for (int i = 0; i < rows; i++) {
            int globalRow = rowStart + i;
            for (int j = 0; j < n; j++) {
                double sum = 0;
                for (int k = 0; k < n; k++)
                    sum += A[globalRow][k] * B[k][j];
                C[i][j] = sum;
            }
        }
        return C;
    }
}
