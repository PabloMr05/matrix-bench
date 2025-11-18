#pragma once
#include <vector>
#include <random>

inline std::vector<double> random_matrix(int n, uint64_t seed) {
    std::mt19937_64 rng(seed);
    std::uniform_real_distribution<double> U(0.0,1.0);
    std::vector<double> M(n* n);
    for (int i=0;i<n*n;i++) M[i] = U(rng);
    return M;
}

inline std::vector<double> matmul_ijk(const std::vector<double>& A,
                                      const std::vector<double>& B,
                                      int n) {
    std::vector<double> C(n* n, 0.0);
    for (int i=0;i<n;i++) {
        for (int k=0;k<n;k++) {
            double aik = A[i*n + k];
            const double* Bk = &B[k*n];
            double* Ci = &C[i*n];
            for (int j=0;j<n;j++) {
                Ci[j] += aik * Bk[j];
            }
        }
    }
    return C;
}
