package org.example.matrix;

import java.util.Random;

public class Matrix {
    public static double[][] randomMatrix(int n, long seed) {
        Random rng = new Random(seed);
        double[][] M = new double[n][n];
        for (int i=0;i<n;i++)
            for (int j=0;j<n;j++)
                M[i][j] = rng.nextDouble();
        return M;
    }

    public static double[][] matmul_ijk(double[][] A, double[][] B) {
        int n = A.length;
        double[][] C = new double[n][n];
        for (int i=0;i<n;i++) {
            for (int k=0;k<n;k++) {
                double aik = A[i][k];
                for (int j=0;j<n;j++) {
                    C[i][j] += aik * B[k][j];
                }
            }
        }
        return C;
    }
}