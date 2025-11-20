#include <iostream>
#include <fstream>
#include <chrono>
#include <string>
#include <sys/resource.h>
#include <vector>
#include <algorithm>
#include "matrix.hpp"

// ============================
// Utilidades de medida
// ============================

double get_memory_mb() {
    struct rusage usage{};
    getrusage(RUSAGE_SELF, &usage);
#ifdef __APPLE__
    return usage.ru_maxrss / (1024.0 * 1024.0);
#else
    return usage.ru_maxrss / 1024.0;
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

int main(int argc, char** argv) {
    int n = 1024;
    int runs = 5;
    uint64_t seed = 123;
    std::string algo = "dense_ijk";   // dense_ijk, dense_block, dense_unroll, sparse_csr
    int block = 64;                   // tamaño de bloque por defecto
    double density = 0.01;            // fracción de elementos NO cero para sparse
    // AHORA: carpeta results junto a cpp dentro del TASK 2
    std::string out = "../results/cpp_task2.csv";

    // Parseo rápido de argumentos
    for (int i = 1; i < argc; ++i) {
        std::string a = argv[i];
        if (a == "--n") n = std::stoi(argv[++i]);
        else if (a == "--runs") runs = std::stoi(argv[++i]);
        else if (a == "--seed") seed = std::stoull(argv[++i]);
        else if (a == "--algo") algo = argv[++i];
        else if (a == "--block") block = std::stoi(argv[++i]);
        else if (a == "--density") density = std::stod(argv[++i]);
        else if (a == "--out") out = argv[++i];
    }

    std::ofstream ofs(out);
    if (!ofs) {
        std::cerr << "ERROR: no se puede abrir el fichero de salida: " << out << "\n";
        return 1;
    }

    ofs << "algo,n,density,block_size,run,seconds,memory_mb,cpu_percent,nnz\n";

    // Preparamos matrices según el algoritmo
    std::vector<double> A_dense;
    std::vector<double> B_dense;
    CSRMatrix A_sparse;
    int nnz = 0;

    if (algo == "sparse_csr") {
        A_sparse = random_sparse_csr(n, density, seed);
        B_dense  = random_matrix(n, seed + 1);
        nnz = static_cast<int>(A_sparse.values.size());
        auto Cw = spmm_csr_dense(A_sparse, B_dense, n);
        (void)Cw;
    } else {
        A_dense = random_matrix(n, seed);
        B_dense = random_matrix(n, seed + 1);

        if (algo == "dense_ijk") {
            auto Cw = matmul_ijk(A_dense, B_dense, n);
            (void)Cw;
        } else if (algo == "dense_block") {
            auto Cw = matmul_blocked(A_dense, B_dense, n, block);
            (void)Cw;
        } else if (algo == "dense_unroll") {
            auto Cw = matmul_unrolled(A_dense, B_dense, n);
            (void)Cw;
        } else {
            std::cerr << "ERROR: algoritmo desconocido: " << algo << "\n";
            return 1;
        }
    }

    // Bucle de runs
    for (int r = 1; r <= runs; ++r) {
        double mem_before = get_memory_mb();
        double cpu_before = get_cpu_time_seconds();

        auto t0 = std::chrono::high_resolution_clock::now();

        std::vector<double> C;
        if (algo == "sparse_csr") {
            C = spmm_csr_dense(A_sparse, B_dense, n);
        } else if (algo == "dense_ijk") {
            C = matmul_ijk(A_dense, B_dense, n);
        } else if (algo == "dense_block") {
            C = matmul_blocked(A_dense, B_dense, n, block);
        } else if (algo == "dense_unroll") {
            C = matmul_unrolled(A_dense, B_dense, n);
        }

        auto t1 = std::chrono::high_resolution_clock::now();
        double mem_after = get_memory_mb();
        double cpu_after = get_cpu_time_seconds();

        double secs = std::chrono::duration<double>(t1 - t0).count();
        double mem_used = std::max(mem_before, mem_after);
        double cpu_pct = (cpu_after - cpu_before) / secs * 100.0;

        ofs << algo << "," << n << "," << density << "," << block << ","
            << r << "," << secs << "," << mem_used << "," << cpu_pct << ","
            << (algo == "sparse_csr" ? nnz : (n * n)) << "\n";
    }

    std::cout << "Benchmark Task 2 completado para algo=" << algo
              << ", n=" << n << ", density=" << density << "\n";

    return 0;
}
