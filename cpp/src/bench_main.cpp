#include <iostream>
#include <fstream>
#include <chrono>
#include "matrix.hpp"

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
    ofs << "language,n,run,seconds\n";

    // warmup
    (void) matmul_ijk(A,B,n);

    for (int r=1; r<=runs; r++) {
        auto t0 = std::chrono::high_resolution_clock::now();
        auto C  = matmul_ijk(A,B,n);
        auto t1 = std::chrono::high_resolution_clock::now();
        double secs = std::chrono::duration<double>(t1 - t0).count();
        ofs << "cpp," << n << "," << r << "," << secs << "\n";
    }
}
