#include "matrix.hpp"
#include "/opt/homebrew/opt/libomp/include/omp.h"


/**
 * OpenMP collapse(2) version — parallelizes (i, j)
 */
Matrix multiply_omp_collapse(const Matrix& A, const Matrix& B) {

    int n = A.size();
    Matrix C = zeros(n);

    #pragma omp parallel for collapse(2)
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {

            for (int k = 0; k < n; k++)
                C[i][j] += A[i][k] * B[k][j];

        }
    }
    return C;
}
