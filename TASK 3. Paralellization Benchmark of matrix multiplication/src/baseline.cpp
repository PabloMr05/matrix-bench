#include "matrix.hpp"

Matrix multiply_baseline(const Matrix& A, const Matrix& B) {

    int n = A.size();
    Matrix C = zeros(n);

    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            for (int k = 0; k < n; k++)
                C[i][j] += A[i][k] * B[k][j];

    return C;
}
