#include "matrix.hpp"
#include <omp.h>

/**
 * OpenMP SIMD vectorized version — vectorizes the k-loop
 */
Matrix multiply_omp_simd(const Matrix& A, const Matrix& B) {

    int n = A.size();
    Matrix C = zeros(n);

    #pragma omp parallel for
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {

            #pragma omp simd
            for (int k = 0; k < n; k++)
                C[i][j] += A[i][k] * B[k][j];

        }
    }

    return C;
}
