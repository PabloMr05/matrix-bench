#pragma once
#include <vector>
#include <random>
#include <cstdint>
#include <algorithm>

// =====================
//  DENSE MATRICES
// =====================

inline std::vector<double> random_matrix(int n, uint64_t seed) {
    std::mt19937_64 rng(seed);
    std::uniform_real_distribution<double> U(0.0,1.0);
    std::vector<double> M(n * n);
    for (int i = 0; i < n * n; ++i) M[i] = U(rng);
    return M;
}

// Baseline (i-k-j, buena localidad de caché en row-major)
inline std::vector<double> matmul_ijk(const std::vector<double>& A,
                                      const std::vector<double>& B,
                                      int n) {
    std::vector<double> C(n * n, 0.0);
    for (int i = 0; i < n; ++i) {
        double* Ci = &C[i * n];
        for (int k = 0; k < n; ++k) {
            double aik = A[i * n + k];
            const double* Bk = &B[k * n];
            for (int j = 0; j < n; ++j) {
                Ci[j] += aik * Bk[j];
            }
        }
    }
    return C;
}

// ---------------------
// Versión bloqueada (tiling / blocking)
// ---------------------
inline std::vector<double> matmul_blocked(const std::vector<double>& A,
                                          const std::vector<double>& B,
                                          int n,
                                          int block) {
    std::vector<double> C(n * n, 0.0);

    for (int ii = 0; ii < n; ii += block) {
        for (int kk = 0; kk < n; kk += block) {
            for (int jj = 0; jj < n; jj += block) {

                int i_max = std::min(ii + block, n);
                int k_max = std::min(kk + block, n);
                int j_max = std::min(jj + block, n);

                for (int i = ii; i < i_max; ++i) {
                    double* Ci = &C[i * n];
                    for (int k = kk; k < k_max; ++k) {
                        double aik = A[i * n + k];
                        const double* Bk = &B[k * n];
                        for (int j = jj; j < j_max; ++j) {
                            Ci[j] += aik * Bk[j];
                        }
                    }
                }
            }
        }
    }
    return C;
}

// ---------------------
// Versión con loop unrolling (desenrollado simple)
// ---------------------
inline std::vector<double> matmul_unrolled(const std::vector<double>& A,
                                           const std::vector<double>& B,
                                           int n) {
    std::vector<double> C(n * n, 0.0);

    for (int i = 0; i < n; ++i) {
        double* Ci = &C[i * n];
        for (int k = 0; k < n; ++k) {
            double aik = A[i * n + k];
            const double* Bk = &B[k * n];

            int j = 0;
            // desenrollamos de 4 en 4
            for (; j + 3 < n; j += 4) {
                Ci[j]     += aik * Bk[j];
                Ci[j + 1] += aik * Bk[j + 1];
                Ci[j + 2] += aik * Bk[j + 2];
                Ci[j + 3] += aik * Bk[j + 3];
            }
            // resto
            for (; j < n; ++j) {
                Ci[j] += aik * Bk[j];
            }
        }
    }
    return C;
}

// =====================
//  SPARSE MATRICES (CSR)
// =====================

struct CSRMatrix {
    int rows;
    int cols;
    std::vector<int> row_ptr;   // size = rows + 1
    std::vector<int> col_idx;   // size = nnz
    std::vector<double> values; // size = nnz
};

// Genera matriz dispersa aleatoria en CSR con densidad "density" (0..1 de NO ceros)
inline CSRMatrix random_sparse_csr(int n, double density, uint64_t seed) {
    CSRMatrix S;
    S.rows = n;
    S.cols = n;
    S.row_ptr.assign(n + 1, 0);

    std::mt19937_64 rng(seed);
    std::uniform_real_distribution<double> U(0.0, 1.0);

    std::vector<int>    col_idx;
    std::vector<double> values;

    int nnz = 0;
    for (int i = 0; i < n; ++i) {
        S.row_ptr[i] = nnz;
        for (int j = 0; j < n; ++j) {
            double r = U(rng);
            if (r < density) {  // valor no nulo
                col_idx.push_back(j);
                values.push_back(U(rng));
                ++nnz;
            }
        }
    }
    S.row_ptr[n] = nnz;
    S.col_idx = std::move(col_idx);
    S.values  = std::move(values);
    return S;
}

// C = A_csr * B_densa   (A: n×n sparse, B: n×n dense, C: n×n dense)
inline std::vector<double> spmm_csr_dense(const CSRMatrix& A,
                                          const std::vector<double>& B,
                                          int n) {
    std::vector<double> C(n * n, 0.0);

    for (int i = 0; i < A.rows; ++i) {
        double* Ci = &C[i * n];
        int row_start = A.row_ptr[i];
        int row_end   = A.row_ptr[i + 1];

        for (int idx = row_start; idx < row_end; ++idx) {
            int k = A.col_idx[idx];
            double a_ik = A.values[idx];
            const double* Bk = &B[k * n];

            for (int j = 0; j < n; ++j) {
                Ci[j] += a_ik * Bk[j];
            }
        }
    }
    return C;
}
