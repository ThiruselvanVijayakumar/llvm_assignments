#!/bin/bash

set -e

# Directory containing this script
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

# LLVM build directory
LLVM_BUILD="$SCRIPT_DIR/llvm-project/build"

# Use the clang and opt built from our LLVM project
CLANG="$LLVM_BUILD/bin/clang"
OPT="$LLVM_BUILD/bin/opt"

# Check that they exist
if [[ ! -x "$CLANG" ]]; then
    echo "Error: clang not found at $CLANG"
    exit 1
fi

if [[ ! -x "$OPT" ]]; then
    echo "Error: opt not found at $OPT"
    exit 1
fi

# Optimization to run
OPTIMIZATION="$1"

if [[ -z "$OPTIMIZATION" ]]; then
    echo "Usage: $0 {constprop|instcombine|dce|strengthreduction|cse|all}"
    exit 1
fi

# Directories
TEST_DIR="$SCRIPT_DIR/test_cases"
OUTPUT_DIR="$SCRIPT_DIR/output"

mkdir -p "$OUTPUT_DIR"


run_optimization()
{
    local OPT_NAME="$1"
    local INPUT="$TEST_DIR/${OPT_NAME}.c"

    echo
    echo "Running: $OPT_NAME"

    echo "1: Generating LLVM IR from C..."

    "$CLANG" \
        -S -emit-llvm \
        -O0 \
        -Xclang -disable-O0-optnone \
        "$INPUT" \
        -o "$OUTPUT_DIR/${OPT_NAME}_raw.ll"


    echo "2: Preparing simple SSA form..."

    "$OPT" \
        -passes=mem2reg \
        "$OUTPUT_DIR/${OPT_NAME}_raw.ll" \
        -S \
        -o "$OUTPUT_DIR/${OPT_NAME}.ll"


    echo "3: Running HelloWorld pass..."

    "$OPT" \
        -passes=helloworld \
        -hello-opt="$OPT_NAME" \
        "$OUTPUT_DIR/${OPT_NAME}.ll" \
        -S \
        -o "$OUTPUT_DIR/${OPT_NAME}_optimized.ll"


    echo "4: Optimized LLVM IR:"
    echo

    cat "$OUTPUT_DIR/${OPT_NAME}_optimized.ll"
}


if [[ "$OPTIMIZATION" == "all" ]]; then

    for OPT_NAME in constprop instcombine dce strength cse
    do
        run_optimization "$OPT_NAME"
    done

else

    case "$OPTIMIZATION" in
        constprop|instcombine|dce|strengthreduction|cse)
            run_optimization "$OPTIMIZATION"
            ;;
        *)
            echo "Unknown optimization: $OPTIMIZATION"
            echo "Usage: $0 {constprop|instcombine|dce|strengthreduction|cse|all}"
            exit 1
            ;;
    esac

fi