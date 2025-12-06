#include "matrix.hpp"
#include <iostream>
#include <fstream>
#include <chrono>
#include <functional>

// Include OpenMP header
#include "/opt/homebrew/opt/libomp/include/omp.h"

using namespace std;
using namespace chrono;

// DECLARATIONS OF ALL METHODS
Matrix multiply_baseline(const Matrix& A, const Matrix& B);
Matrix multiply_omp_parallel(const Matrix& A, const Matrix& B);
Matrix multiply_omp_parallel_2(const Matrix& A, const Matrix& B);
Matrix multiply_omp_parallel_4(const Matrix& A, const Matrix& B);
Matrix multiply_omp_parallel_8(const Matrix& A, const Matrix& B);
Matrix multiply_omp_collapse(const Matrix& A, const Matrix& B);
Matrix multiply_omp_simd(const Matrix& A, const Matrix& B);
Matrix multiply_unroll(const Matrix& A, const Matrix& B);

// TIME MEASUREMENT
long measure_time_ms(function<Matrix()> f) {
    auto start = high_resolution_clock::now();
    Matrix C = f();
    auto end = high_resolution_clock::now();
    return duration_cast<milliseconds>(end - start).count();
}

int main() {

    vector<int> sizes = {200, 300, 400, 500};

    ofstream file("results/task3_cpp_results.csv");
    file << "method,n,time_ms\n";

    cout << "Running complete C++ OpenMP benchmark...\n";

    for (int n : sizes) {

        cout << "\n==== Matrix size: " << n << "x" << n << " ====\n";

        Matrix A = generate_matrix(n);
        Matrix B = generate_matrix(n);

        // Define methods and names
        vector<pair<string, function<Matrix()>>> tests = {
            {"baseline",        [&]() { return multiply_baseline(A, B); }},
            {"omp_parallel",    [&]() { return multiply_omp_parallel(A, B); }},
            {"omp_parallel_2",  [&]() { return multiply_omp_parallel_2(A, B); }},
            {"omp_parallel_4",  [&]() { return multiply_omp_parallel_4(A, B); }},
            {"omp_parallel_8",  [&]() { return multiply_omp_parallel_8(A, B); }},
            {"omp_collapse",    [&]() { return multiply_omp_collapse(A, B); }},
            {"omp_simd",        [&]() { return multiply_omp_simd(A, B); }},
            {"unroll",          [&]() { return multiply_unroll(A, B); }},
        };

        // RUN ALL METHODS
        for (auto& t : tests) {
            string method = t.first;
            auto func = t.second;

            long time = measure_time_ms(func);

            cout << method << ": " << time << " ms\n";
            file << method << "," << n << "," << time << "\n";
        }
    }

    file.close();
    cout << "\nBenchmark finished. CSV saved.\n";

    return 0;
}
