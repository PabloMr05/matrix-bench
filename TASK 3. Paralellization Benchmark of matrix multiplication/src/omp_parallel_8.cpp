#include "/opt/homebrew/opt/libomp/include/omp.h"
#include "matrix.hpp"

Matrix multiply_omp_parallel_8(const Matrix& A, const Matrix& B) {

    int n = A.size();
    Matrix C = zeros(n);

    #pragma omp parallel for num_threads(8) schedule(static)
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {

            double sum = 0.0;

            for (int k = 0; k < n; k++)
                sum += A[i][k] * B[k][j];

            C[i][j] = sum;
        }
    }

    return C;
}
