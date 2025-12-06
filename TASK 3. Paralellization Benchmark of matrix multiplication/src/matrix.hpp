#ifndef MATRIX_HPP
#define MATRIX_HPP

#include <vector>
#include <random>

using Matrix = std::vector<std::vector<double>>;

/** Creates an n x n matrix of zeros */
inline Matrix zeros(int n) {
    return Matrix(n, std::vector<double>(n, 0.0));
}

/** Generates a random n x n matrix */
inline Matrix generate_matrix(int n) {
    Matrix M(n, std::vector<double>(n));

    std::mt19937 gen(42);
    std::uniform_real_distribution<> dist(0.0, 5.0);

    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            M[i][j] = dist(gen);

    return M;
}

#endif
