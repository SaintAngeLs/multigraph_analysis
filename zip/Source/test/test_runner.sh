#!/bin/bash

# Paths to executables and directories
EXECUTABLE=../multigraph_analysis
DATA_DIR=../../Examples/data
RESULTS_DIR=../../Examples/results
GRAPH_GENERATOR=../graph_gen

mkdir -p $RESULTS_DIR

if [[ $# -gt 0 ]]; then
    case $1 in
        "graph_gen")
            echo "Running graph generator with specified arguments..."
            $GRAPH_GENERATOR "${@:2}"
            exit 0
            ;;
        "multigraph_analysis")
            if [[ $# -lt 3 ]]; then
                echo "Usage: $0 multigraph_analysis <input_file> <output_file>"
                exit 1
            fi
            INPUT_FILE=$2
            OUTPUT_FILE=$3
            echo "Running multigraph analysis on $INPUT_FILE..."
            $EXECUTABLE $INPUT_FILE > $OUTPUT_FILE
            echo "Results saved to $OUTPUT_FILE"
            exit 0
            ;;
        *)
            echo "Invalid option. Supported: graph_gen, multigraph_analysis"
            exit 1
            ;;
    esac
fi

TEST_CASES=(
    "multiple_graphs_case1.txt"
    "multiple_graphs_case2.txt"
    "multiple_graphs_case3.txt"
    "multiple_graphs_case4.txt"
    "multiple_graphs_case5.txt"
    "multiple_graphs_case6.txt"
    "multiple_graphs_case7.txt"
    "metric.txt"
)

echo "Running predefined test cases..."
for TEST_FILE in "${TEST_CASES[@]}"; do
    INPUT_FILE="$DATA_DIR/$TEST_FILE"
    OUTPUT_FILE="$RESULTS_DIR/${TEST_FILE%.txt}_output.txt"

    if [[ -f $INPUT_FILE ]]; then
        echo "Processing $INPUT_FILE..."
        $EXECUTABLE $INPUT_FILE > $OUTPUT_FILE
        echo "Results saved to $OUTPUT_FILE"
    else
        echo "Test file $INPUT_FILE not found, skipping."
    fi
done

echo "Running custom graph generator tests..."

$GRAPH_GENERATOR complete 1 10 10 $DATA_DIR/test_complete_graph.txt 3
$EXECUTABLE $DATA_DIR/test_complete_graph.txt > $RESULTS_DIR/test_complete_graph_output.txt
echo "Complete graph test saved to $RESULTS_DIR/test_complete_graph_output.txt"

$GRAPH_GENERATOR sparse 1 10 10 $DATA_DIR/test_sparse_graph.txt 0.2 5
$EXECUTABLE $DATA_DIR/test_sparse_graph.txt > $RESULTS_DIR/test_sparse_graph_output.txt
echo "Sparse graph test saved to $RESULTS_DIR/test_sparse_graph_output.txt"

$GRAPH_GENERATOR bipartite 1 10 10 $DATA_DIR/test_bipartite_graph.txt 3
$EXECUTABLE $DATA_DIR/test_bipartite_graph.txt > $RESULTS_DIR/test_bipartite_graph_output.txt
echo "Bipartite graph test saved to $RESULTS_DIR/test_bipartite_graph_output.txt"

$GRAPH_GENERATOR complete 1 10 10 $DATA_DIR/test_multigraph.txt 3
$EXECUTABLE $DATA_DIR/test_multigraph.txt > $RESULTS_DIR/test_multigraph_output.txt
echo "Multigraph test saved to $RESULTS_DIR/test_multigraph_output.txt"

echo "All tests completed. Check results in $RESULTS_DIR."
