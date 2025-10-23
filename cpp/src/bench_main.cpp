#include <iostream>
#include <fstream>
#include <chrono>
#include <string>
#include <sys/resource.h>
#include "matrix.hpp"


double get_memory_mb() {
    struct rusage usage{};
    getrusage(RUSAGE_SELF, &usage);
    // ru_maxrss is in kilobytes on Linux, bytes on macOS — normalize
#ifdef __APPLE__
    return usage.ru_maxrss / (1024.0 * 1024.0); 
#else
    return usage.ru_maxrss / 1024.0;
#endif
}

double get_cpu_percent() {
    struct rusage usage{};
    getrusage(RUSAGE_SELF, &usage);
    double user = usage.ru_utime.tv_sec + usage.ru_utime.tv_usec / 1e6;
    double sys  = usage.ru_stime.tv_sec + usage.ru_stime.tv_usec / 1e6;
    double total_cpu = user + sys;
    return total_cpu;
}



int main(int argc, char** argv) {
    int n = 512, runs = 5;
    uint64_t seed = 123;
    std::string out = "../results/cpp.csv";
    for (int i=1;i<argc;i++) {
        std::string a = argv[i];
        if (a=="--n") n = std::stoi(argv[++i]);
        else if (a=="--runs") runs = std::stoi(argv[++i]);
        else if (a=="--seed") seed = std::stoull(argv[++i]);
        else if (a=="--out") out = argv[++i];
    }

    auto A = random_matrix(n, seed);
    auto B = random_matrix(n, seed+1);

    std::ofstream ofs(out);
    ofs << "language,n,run,seconds,memory_mb,cpu_percent\n";

    // warmup
    (void) matmul_ijk(A,B,n);

    for (int r=1; r<=runs; r++) {
        auto mem_before = get_memory_mb();
        auto cpu_before = get_cpu_percent();

        auto t0 = std::chrono::high_resolution_clock::now();
        auto C  = matmul_ijk(A,B,n);
        auto t1 = std::chrono::high_resolution_clock::now();

        auto mem_after = get_memory_mb();
        auto cpu_after = get_cpu_percent();

        double secs = std::chrono::duration<double>(t1 - t0).count();
        double mem_used = std::max(mem_before, mem_after);
        double cpu_used = (cpu_after - cpu_before) / secs * 100.0;
        
        ofs << "cpp," << n << "," << r << "," << secs << "," << mem_used << "," << cpu_used <<  "\n";
    }
}
