import java.util.Random;

public class MatrixUtils {

    public static double[][] generateMatrix(int n) {
        double[][] m = new double[n][n];
        Random r = new Random();

        for (int i = 0; i < n; i++)
            for (int j = 0; j < n; j++)
                m[i][j] = r.nextDouble();

        return m;
    }

    public static double[][] multiplySequential(double[][] A, double[][] B) {
        int n = A.length;
        double[][] C = new double[n][n];

        for (int i = 0; i < n; i++)
            for (int j = 0; j < n; j++)
                for (int k = 0; k < n; k++)
                    C[i][j] += A[i][k] * B[k][j];

        return C;
    }
}
