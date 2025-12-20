import numpy as np

def generate_matrix(n):
    return np.random.rand(n, n)

def multiply_chunk(A, B, start, end):
    C = np.zeros((end - start, A.shape[1]))
    for i in range(start, end):
        C[i - start] = A[i] @ B
    return C
