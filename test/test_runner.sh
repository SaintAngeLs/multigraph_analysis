#!/bin/bash

EXECUTABLE=./multigraph_analysis
DATA_DIR=./data
RESULTS_DIR=./test/results

mkdir -p $RESULTS_DIR

TEST_CASES=(
    "multiple_graphs_case1.txt"
    "multiple_graphs_case2.txt"
    "multiple_graphs_case3.txt"
    "multiple_graphs_case4.txt"
)

echo "Running tests on data cases..."

for TEST_FILE in "${TEST_CASES[@]}"; do
    INPUT_FILE="$DATA_DIR/$TEST_FILE"
    OUTPUT_FILE="$RESULTS_DIR/${TEST_FILE%.txt}_output.txt"

    echo "Processing $INPUT_FILE..."
    $EXECUTABLE $INPUT_FILE > $OUTPUT_FILE

    echo "Results saved to $OUTPUT_FILE"
done

echo "All tests completed. Check the results in $RESULTS_DIR."
