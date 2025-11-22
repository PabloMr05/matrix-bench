#include <iostream>
#include <fstream>
#include <chrono>
#include <string>
#include <sys/resource.h>
#include <sys/time.h>
#include <vector>
#include <algorithm>
#include <cstdint>

#include "matrix.hpp"

// ============================
// Utilidades de medida
// ============================

double get_memory_mb() {
    struct rusage usage{};
    getrusage(RUSAGE_SELF, &usage);
#ifdef __APPLE__
    return usage.ru_maxrss / (1024.0 * 1024.0); // bytes → MB
#else
    return usage.ru_maxrss / 1024.0; // KB → MB
#endif
}


double get_cpu_time_seconds() {
    struct rusage usage{};
    getrusage(RUSAGE_SELF, &usage);
    double user = usage.ru_utime.tv_sec + usage.ru_utime.tv_usec / 1e6;
    double sys  = usage.ru_stime.tv_sec + usage.ru_stime.tv_usec / 1e6;
    return user + sys;
}

// ============================
// Programa principal
// ============================

int main(int argc, char** argv) {
    std::string algo = "dense_ijk";
    int n = 512;
    int runs = 5;
    std::uint64_t seed = 42;
    int block = 64;
    double density = 0.01;
    std::string out_file = "results_task2.csv";
    std::string matrix_file;

    // Parseo sencillo de argumentos
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--algo" && i + 1 < argc) {
            algo = argv[++i];
        } else if (arg == "--n" && i + 1 < argc) {
            n = std::stoi(argv[++i]);
        } else if (arg == "--runs" && i + 1 < argc) {
            runs = std::stoi(argv[++i]);
        } else if (arg == "--seed" && i + 1 < argc) {
            seed = static_cast<std::uint64_t>(std::stoll(argv[++i]));
        } else if (arg == "--block" && i + 1 < argc) {
            block = std::stoi(argv[++i]);
        } else if (arg == "--density" && i + 1 < argc) {
            density = std::stod(argv[++i]);
        } else if (arg == "--out" && i + 1 < argc) {
            out_file = argv[++i];
        } else if (arg == "--matrix_file" && i + 1 < argc) {
            matrix_file = argv[++i];
        } else {
            std::cerr << "Argumento desconocido o incompleto: " << arg << "\n";
            return 1;
        }
    }

    std::ofstream ofs(out_file);
    if (!ofs) {
        std::cerr << "No se pudo abrir el fichero de salida: " << out_file << "\n";
        return 1;
    }

    ofs << "algo,n,density,block,run,time_sec,memory_mb,cpu_pct,nnz_or_n2\n";

    // Preparar datos iniciales
    std::vector<double> A_dense;
    std::vector<double> B_dense;
    CSRMatrix A_sparse;
    bool use_sparse = (algo == "sparse_csr");

    if (!use_sparse) {
        // Dense: matrices n x n
        A_dense = random_matrix(n, seed);
        B_dense = random_matrix(n, seed + 1);
    } else {
        if (!matrix_file.empty()) {
            // Cargar mc2depi u otra matriz desde Matrix Market
            A_sparse = load_matrix_market(matrix_file);
            n = A_sparse.n; // número de filas
            // Densidad real de la matriz
            double nnz = static_cast<double>(A_sparse.values.size());
            density = nnz / (static_cast<double>(A_sparse.n) * A_sparse.n_cols);

            // Para no petar memoria con mc2depi, usamos pocas columnas en B
            int n_cols = std::min(64, std::max(1, A_sparse.n_cols));
            // Si la matriz es enorme, podemos fijar por ejemplo 32 columnas
            if (A_sparse.n > 200000) {
                n_cols = 32;
            }

            std::mt19937_64 rng(seed + 123);
            std::uniform_real_distribution<double> U(0.0, 1.0);
            B_dense.assign(static_cast<std::size_t>(A_sparse.n) * n_cols, 0.0);
            for (double& v : B_dense) v = U(rng);

        } else {
            // Matriz dispersa aleatoria n x n y B densa n x n
            A_sparse = random_sparse_csr(n, density, seed);
            B_dense = random_matrix(n, seed + 1);
        }
    }

    // Ejecutar runs
    for (int r = 0; r < runs; ++r) {
        double mem_before = get_memory_mb();
        double cpu_before = get_cpu_time_seconds();
        auto t0 = std::chrono::high_resolution_clock::now();

        long long nnz_or_n2 = 0;

        if (!use_sparse) {
            std::vector<double> C;
            if (algo == "dense_ijk") {
                C = matmul_ijk(A_dense, B_dense, n);
            } else if (algo == "dense_block") {
                C = matmul_blocked(A_dense, B_dense, n, block);
            } else if (algo == "dense_unroll") {
                C = matmul_unrolled(A_dense, B_dense, n);
            } else {
                std::cerr << "Algoritmo denso desconocido: " << algo << "\n";
                return 1;
            }
            (void)C; // evitar warning
            nnz_or_n2 = static_cast<long long>(n) * static_cast<long long>(n);
        } else {
            // Sparse CSR * densa
            int n_cols_B;
            if (!matrix_file.empty()) {
                // mc2depi: B_dense ya tiene tamaño A_sparse.n x n_cols_B
                n_cols_B = static_cast<int>(
                    B_dense.size() / static_cast<std::size_t>(A_sparse.n));
            } else {
                // aleatoria cuadrada
                n_cols_B = n;
            }

            std::vector<double> C = spmm_csr_dense(A_sparse, B_dense, n_cols_B);
            (void)C;
            nnz_or_n2 = static_cast<long long>(A_sparse.values.size());
        }

        auto t1 = std::chrono::high_resolution_clock::now();
        double mem_after = get_memory_mb();
        double cpu_after = get_cpu_time_seconds();

        double secs = std::chrono::duration<double>(t1 - t0).count();
        double mem_used = std::max(mem_before, mem_after);
        double cpu_pct = (secs > 0.0)
                         ? (cpu_after - cpu_before) / secs * 100.0
                         : 0.0;

        ofs << algo << ","
            << n << ","
            << density << ","
            << (use_sparse ? 0 : block) << ","
            << r << ","
            << secs << ","
            << mem_used << ","
            << cpu_pct << ","
            << nnz_or_n2 << "\n";
    }

    std::cout << "Benchmark Task 2 completado para algo=" << algo
              << ", n=" << n << ", density=" << density << "\n";

    return 0;
}
