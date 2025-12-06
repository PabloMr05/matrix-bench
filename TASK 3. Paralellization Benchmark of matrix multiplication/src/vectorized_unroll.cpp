#include "matrix.hpp"
#include "/opt/homebrew/opt/libomp/include/omp.h"


Matrix multiply_unroll(const Matrix& A, const Matrix& B) {

    int n = A.size();
    Matrix C = zeros(n);

    #pragma omp parallel for schedule(static)
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {

            double sum = 0.0;
            int k = 0;

            // Unrolling x4
            for (; k <= n - 4; k += 4) {
                sum += A[i][k]     * B[k][j];
                sum += A[i][k + 1] * B[k + 1][j];
                sum += A[i][k + 2] * B[k + 2][j];
                sum += A[i][k + 3] * B[k + 3][j];
            }

            // Handle remainder
            for (; k < n; k++)
                sum += A[i][k] * B[k][j];

            C[i][j] = sum;
        }
    }

    return C;
}
