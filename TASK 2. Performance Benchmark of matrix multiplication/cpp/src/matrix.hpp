#pragma once

#include <vector>
#include <random>
#include <cstdint>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <stdexcept>

// =====================
//  DENSE MATRICES
// =====================

inline std::vector<double> random_matrix(int n, std::uint64_t seed) {
    std::mt19937_64 rng(seed);
    std::uniform_real_distribution<double> U(0.0, 1.0);
    std::vector<double> M(n * n);
    for (int i = 0; i < n * n; ++i) {
        M[i] = U(rng);
    }
    return M;
}

// C = A * B, matrices densas n x n en formato fila-major
inline std::vector<double> matmul_ijk(const std::vector<double>& A,
                                      const std::vector<double>& B,
                                      int n) {
    std::vector<double> C(n * n, 0.0);

    for (int i = 0; i < n; ++i) {
        const double* Ai = &A[i * n];
        double* Ci = &C[i * n];
        for (int k = 0; k < n; ++k) {
            double a_ik = Ai[k];
            const double* Bk = &B[k * n];
            for (int j = 0; j < n; ++j) {
                Ci[j] += a_ik * Bk[j];
            }
        }
    }
    return C;
}

// C = A * B con bloque (tiling) simple
inline std::vector<double> matmul_blocked(const std::vector<double>& A,
                                          const std::vector<double>& B,
                                          int n,
                                          int block) {
    std::vector<double> C(n * n, 0.0);

    for (int ii = 0; ii < n; ii += block) {
        int i_max = std::min(ii + block, n);
        for (int kk = 0; kk < n; kk += block) {
            int k_max = std::min(kk + block, n);
            for (int jj = 0; jj < n; jj += block) {
                int j_max = std::min(jj + block, n);

                for (int i = ii; i < i_max; ++i) {
                    double* Ci = &C[i * n];
                    const double* Ai = &A[i * n];
                    for (int k = kk; k < k_max; ++k) {
                        double a_ik = Ai[k];
                        const double* Bk = &B[k * n];
                        // bucle interno normal
                        for (int j = jj; j < j_max; ++j) {
                            Ci[j] += a_ik * Bk[j];
                        }
                    }
                }
            }
        }
    }
    return C;
}

// C = A * B con desenrollado manual del bucle interno (sobre j)
inline std::vector<double> matmul_unrolled(const std::vector<double>& A,
                                           const std::vector<double>& B,
                                           int n) {
    std::vector<double> C(n * n, 0.0);

    const int UNROLL = 4;

    for (int i = 0; i < n; ++i) {
        const double* Ai = &A[i * n];
        double* Ci = &C[i * n];
        for (int k = 0; k < n; ++k) {
            double a_ik = Ai[k];
            const double* Bk = &B[k * n];

            int j = 0;
            for (; j + UNROLL - 1 < n; j += UNROLL) {
                Ci[j + 0] += a_ik * Bk[j + 0];
                Ci[j + 1] += a_ik * Bk[j + 1];
                Ci[j + 2] += a_ik * Bk[j + 2];
                Ci[j + 3] += a_ik * Bk[j + 3];
            }
            for (; j < n; ++j) {
                Ci[j] += a_ik * Bk[j];
            }
        }
    }
    return C;
}

// =====================
//  SPARSE CSR MATRIX
// =====================

struct CSRMatrix {
    int n;                      // número de filas
    int n_cols;                 // número de columnas
    std::vector<int> row_ptr;   // tamaño n+1
    std::vector<int> col_idx;   // tamaño nnz
    std::vector<double> values; // tamaño nnz
};

// Genera una matriz dispersa n x n en CSR con densidad aproximada "density"
inline CSRMatrix random_sparse_csr(int n, double density, std::uint64_t seed) {
    if (density <= 0.0) {
        density = 0.0;
    }
    if (density > 1.0) {
        density = 1.0;
    }

    std::mt19937_64 rng(seed);
    std::uniform_real_distribution<double> Uval(0.0, 1.0);
    std::uniform_int_distribution<int> Ucol(0, n - 1);

    CSRMatrix A;
    A.n = n;
    A.n_cols = n;
    A.row_ptr.assign(n + 1, 0);

    // número esperado de no ceros por fila
    int target_per_row = static_cast<int>(density * n);
    if (target_per_row <= 0 && density > 0.0) target_per_row = 1;
    if (target_per_row > n) target_per_row = n;

    std::vector<int> cols;
    cols.reserve(target_per_row > 0 ? target_per_row : 1);

    for (int i = 0; i < n; ++i) {
        cols.clear();

        int nnz_row = target_per_row;
        if (density == 0.0) {
            nnz_row = 0;
        }

        // elegimos columnas únicas aleatorias para esta fila
        while ((int)cols.size() < nnz_row) {
            int c = Ucol(rng);
            bool exists = false;
            for (int x : cols) {
                if (x == c) {
                    exists = true;
                    break;
                }
            }
            if (!exists) cols.push_back(c);
        }

        std::sort(cols.begin(), cols.end());

        A.row_ptr[i + 1] = A.row_ptr[i] + static_cast<int>(cols.size());
        for (int c : cols) {
            A.col_idx.push_back(c);
            A.values.push_back(Uval(rng));
        }
    }

    return A;
}

// Producto A (CSR, n x n) * B (densa, n x n_cols) = C (n x n_cols)
inline std::vector<double> spmm_csr_dense(const CSRMatrix& A,
                                          const std::vector<double>& B,
                                          int n_cols) {
    const int n = A.n;
    std::vector<double> C(n * n_cols, 0.0);

    for (int i = 0; i < n; ++i) {
        const int row_start = A.row_ptr[i];
        const int row_end   = A.row_ptr[i + 1];

        double* Ci = &C[i * n_cols];

        for (int idx = row_start; idx < row_end; ++idx) {
            int k = A.col_idx[idx];
            double a_ik = A.values[idx];
            const double* Bk = &B[k * n_cols];

            for (int j = 0; j < n_cols; ++j) {
                Ci[j] += a_ik * Bk[j];
            }
        }
    }
    return C;
}

// =============================
//  CARGA MATRIX MARKET (.mtx)
// =============================

inline CSRMatrix load_matrix_market(const std::string& filename) {
    std::ifstream in(filename);
    if (!in) {
        throw std::runtime_error("No se puede abrir el fichero: " + filename);
    }

    std::string line;

    // Saltar cabecera y comentarios (líneas que empiezan por '%')
    do {
        if (!std::getline(in, line)) {
            throw std::runtime_error("Fichero MatrixMarket inválido: " + filename);
        }
    } while (!line.empty() && line[0] == '%');

    // Línea con: nrows ncols nnz
    int nrows = 0, ncols = 0;
    std::size_t nnz = 0;
    {
        std::istringstream iss(line);
        iss >> nrows >> ncols >> nnz;
    }

    std::vector<int> row(nnz), col(nnz);
    std::vector<double> val(nnz);

    int i, j;
    double x;
    std::size_t idx = 0;

    while (in >> i >> j >> x) {
        if (idx >= nnz) break;
        // MatrixMarket es 1-based
        row[idx] = i - 1;
        col[idx] = j - 1;
        val[idx] = x;
        ++idx;
    }

    if (idx != nnz) {
        nnz = idx; // por si el header no coincide exactamente
        row.resize(nnz);
        col.resize(nnz);
        val.resize(nnz);
    }

    CSRMatrix A;
    A.n = nrows;
    A.n_cols = ncols;
    A.row_ptr.assign(nrows + 1, 0);
    A.col_idx.resize(nnz);
    A.values.resize(nnz);

    // Contar por fila
    for (std::size_t k = 0; k < nnz; ++k) {
        int r = row[k];
        if (r < 0 || r >= nrows) {
            throw std::runtime_error("Fila fuera de rango en " + filename);
        }
        A.row_ptr[r + 1]++;
    }

    // Prefijos
    for (int r = 0; r < nrows; ++r) {
        A.row_ptr[r + 1] += A.row_ptr[r];
    }

    // Posición de escritura por fila
    std::vector<int> offset(A.row_ptr.begin(), A.row_ptr.end());

    for (std::size_t k = 0; k < nnz; ++k) {
        int r = row[k];
        int pos = offset[r]++;
        A.col_idx[pos] = col[k];
        A.values[pos] = val[k];
    }

    return A;
}
