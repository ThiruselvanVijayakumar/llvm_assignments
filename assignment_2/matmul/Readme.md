# Assignment 2 — Matrix Multiplication Optimization

## Overview

This assignment implements and benchmarks different loop optimizations for matrix multiplication.

The following four implementations are included:

1. **Naive Matrix Multiplication**
2. **Loop Interchange**
3. **Loop Unrolling**
4. **Loop Interchange + Loop Unrolling**

All implementations perform matrix multiplication on `512 × 512` integer matrices and are verified against the naive implementation to ensure correctness.

## Device Specifications

* **CPU:** AMD Ryzen AI MAX+ 395 with Radeon 8060S
* **Architecture:** x86_64
* **CPU Cores:** 16
* **Threads:** 32
* **Maximum CPU Frequency:** 5.19 GHz
* **L2 Cache:** 16 MiB
* **L3 Cache:** 64 MiB
* **Memory:** 62 GiB
* **Operating System:** Ubuntu 24.04.4 LTS
* **Compiler Optimization:** `-O2`

## Compilation

Compile using:

```bash
gcc -O2 -o matmul matmul.c
```

## Execution

Run the benchmark using:

```bash
./matmul
```

The program:

* Initializes the input matrices.
* Runs each matrix multiplication implementation.
* Repeats each benchmark 3 times.
* Verifies the optimized implementations against the naive implementation.
* Reports the average execution time.
* Reports speedup relative to the naive implementation.

## Configuration

The matrix size and number of benchmark repetitions are defined in the source code:

```c
#define N 512
#define REPEAT 3
```

## Correctness Verification

The results from all optimized implementations are compared with the result produced by the naive implementation.

The expected output is:

```text
Verification:
Naive:                 PASS
Loop interchange:      PASS
Loop unrolling:        PASS
Interchange + unroll:  PASS
```

## Benchmark Results

Benchmark results can be added below after running the program:

```text
Benchmark Results

Benchmark Results
Naive:                 0.274132 seconds
Loop interchange:      0.135693 seconds
Loop unrolling:        0.168536 seconds
Interchange + unroll:  0.146818 seconds

Speedup vs Naive
Naive:                 1.00x
Loop interchange:      2.02x
Loop unrolling:        1.63x
Interchange + unroll:  1.87x
```
