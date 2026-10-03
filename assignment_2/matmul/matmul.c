
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define N 512
#define REPEAT 3


//represent 2D by 1D, also initializes numbers for the input matrices
void initialize_matrix(int *M)
{
    for (int i = 0; i < N * N; i++)
        M[i] = i % 10;
}

//to be done for C matrix as we perform C[]+= rather that C[]=
void zero_matrix(int *M)
{
    for (int i = 0; i < N * N; i++)
        M[i] = 0;
}


int verify_result(int *A, int *B)
{
    for (int i = 0; i < N * N; i++) {
        if (A[i] != B[i])
            return 0;
    }
    return 1;
}

void matmul_naive(int *A, int *B, int *C)
{
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            for (int k = 0; k < N; k++) {
                C[i * N + j] += A[i * N + k] * B[k * N + j];
            }
        }
    }
}

void matmul_interchange(int *A, int *B, int *C)
{
    for (int i = 0; i < N; i++) {
        for (int k = 0; k < N; k++) {
            int Aik = A[i * N + k];
            for (int j = 0; j < N; j++) {
                C[i * N + j] += Aik * B[k * N + j];
            }
        }
    }
}

void matmul_unroll(int *A, int *B, int *C)
{
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            int sum = 0;
            int k = 0;
            for (; k <= N - 4; k += 4) {
                sum += A[i * N + k] * B[k * N + j];
                sum += A[i * N + k + 1] * B[(k + 1) * N + j];
                sum += A[i * N + k + 2] * B[(k + 2) * N + j];
                sum += A[i * N + k + 3] * B[(k + 3) * N + j];
            }

            for (; k < N; k++) {
                sum += A[i * N + k] * B[k * N + j];
            }
            C[i * N + j] = sum;
        }
    }
}

void matmul_interchange_unroll(int *A, int *B, int *C)
{
    for (int i = 0; i < N; i++) {
        for (int k = 0; k < N; k++) {
            int Aik = A[i * N + k];
            int j = 0;
            for (; j <= N - 4; j += 4) {
                C[i * N + j] += Aik * B[k * N + j];
                C[i * N + j + 1] += Aik * B[k * N + j + 1];
                C[i * N + j + 2] += Aik * B[k * N + j + 2];
                C[i * N + j + 3] += Aik * B[k * N + j + 3];
            }
            for (; j < N; j++) {
                C[i * N + j] += Aik * B[k * N + j];
            }
        }
    }
}

double benchmark(
    void (*matmul)(int *, int *, int *), int *A, int *B, int *C)
{
    double total = 0.0;
    for (int r = 0; r < REPEAT; r++) {
        zero_matrix(C);
        clock_t start = clock();
        matmul(A, B, C);
        clock_t end = clock();
        total +=
            (double)(end - start) / CLOCKS_PER_SEC;
    }
    return total / REPEAT;
}

int main()
{
    printf("Matrix size: %d x %d\n", N, N);
    printf("Repeats: %d\n\n", REPEAT);

    int *A = malloc(N * N * sizeof(int));
    int *B = malloc(N * N * sizeof(int));

    int *C_naive = malloc(N * N * sizeof(int));
    int *C_interchange = malloc(N * N * sizeof(int));
    int *C_unroll = malloc(N * N * sizeof(int));
    int *C_interchange_unroll = malloc(N * N * sizeof(int));

    if (!A || !B || !C_naive || !C_interchange || !C_unroll || !C_interchange_unroll) {
        printf("Memory allocation failed\n");
        return 1;
    }

    initialize_matrix(A);
    initialize_matrix(B);

    double t_naive = benchmark(matmul_naive, A, B, C_naive);
    double t_interchange = benchmark(matmul_interchange, A, B, C_interchange);
    double t_unroll = benchmark(matmul_unroll, A, B, C_unroll);
    double t_interchange_unroll = benchmark(matmul_interchange_unroll,A,B,C_interchange_unroll);

    printf("Verification:\n");
    printf("Naive:                 PASS\n");
    printf("Loop interchange:      %s\n", verify_result(C_naive, C_interchange)? "PASS" : "FAIL");

    printf("Loop unrolling:        %s\n", verify_result(C_naive, C_unroll) ? "PASS" : "FAIL");
    printf("Interchange + unroll:  %s\n\n", verify_result(C_naive, C_interchange_unroll) ? "PASS" : "FAIL");

    printf("Benchmark Results\n");
    printf("Naive:                 %.6f seconds\n", t_naive);
    printf("Loop interchange:      %.6f seconds\n", t_interchange);
    printf("Loop unrolling:        %.6f seconds\n", t_unroll);
    printf("Interchange + unroll:  %.6f seconds\n", t_interchange_unroll);

    printf("\n");
    printf("Speedup vs Naive\n");

    printf("Naive:                 %.2fx\n", t_naive / t_naive);
    printf("Loop interchange:      %.2fx\n", t_naive / t_interchange);
    printf("Loop unrolling:        %.2fx\n", t_naive / t_unroll);
    printf("Interchange + unroll:  %.2fx\n", t_naive / t_interchange_unroll);

    free(A);
    free(B);
    free(C_naive);
    free(C_interchange);
    free(C_unroll);
    free(C_interchange_unroll);
    return 0;
}

