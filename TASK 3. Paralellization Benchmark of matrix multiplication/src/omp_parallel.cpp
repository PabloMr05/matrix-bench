#include "matrix.hpp"
#include <omp.h>

/**
 * OpenMP basic parallel-for version
 */
Matrix multiply_omp_parallel(const Matrix& A, const Matrix& B) {

    int n = A.size();
    Matrix C = zeros(n);

    #pragma omp parallel for
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            for (int k = 0; k < n; k++)
                C[i][j] += A[i][k] * B[k][j];

    return C;
}
