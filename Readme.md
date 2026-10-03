# LLVM Optimization Assignment

This project implements five educational LLVM optimizations in a single
`HelloWorld.cpp` pass:

1. Constant Propagation
2. Instruction Combining
3. Dead Code Elimination
4. Strength Reduction
5. Common Subexpression Elimination

Each optimization can be selected from the command line. No changes to
`HelloWorld.cpp` are required when switching between optimizations.

## Repository Structure

```text
.
└── llvm-project/
    └── llvm/
        └── lib/
            └── Transforms/
                └── Utils/
                    └── HelloWorld.cpp
├── run.sh
├── test_cases/
│   ├── constprop.c
│   ├── instcombine.c
│   ├── dce.c
│   ├── strengthreduction.c
│   └── cse.c
└── output/
    └── generated LLVM IR files
```

`output/` is generated automatically by `run.sh`.

## Prerequisites

The LLVM build containing the `helloworld` pass must be available.

The following commands should work:

```bash
clang --version
opt --version
```


## Running an Optimization

Make the script executable once:

```bash
chmod +x run.sh
```

Then run any optimization:

```bash
./run.sh constprop
```

```bash
./run.sh instcombine
```

```bash
./run.sh dce
```

```bash
./run.sh strength
```

```bash
./run.sh cse
```

To run all five test cases:

```bash
./run.sh all
```

## What the Script Does

For example:

```bash
./run.sh constprop
```

The original and optimized LLVM IR are both saved in `output/`.


## Generated Files

For example, after:

```bash
./run.sh cse
```

the following files are generated:

```text
output/cse.ll
output/cse_optimized.ll
```

