#!/bin/bash

EXECUTABLE=./multigraph_analysis
DATA_DIR=./data
RESULTS_DIR=./results
GRAPH_GENERATOR=./graph_gen

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
    "metric.txt"
)

echo "Running tests on predefined cases..."

for TEST_FILE in "${TEST_CASES[@]}"; do
    INPUT_FILE="$DATA_DIR/$TEST_FILE"
    OUTPUT_FILE="$RESULTS_DIR/${TEST_FILE%.txt}_output.txt"

    echo "Processing $INPUT_FILE..."
    $EXECUTABLE $INPUT_FILE > $OUTPUT_FILE

    echo "Results saved to $OUTPUT_FILE"
done

echo "Running custom graph generator tests..."

$GRAPH_GENERATOR complete 10 10 $DATA_DIR/test_complete_graph.txt
$EXECUTABLE $DATA_DIR/test_complete_graph.txt > $RESULTS_DIR/test_complete_graph_output.txt
echo "Complete graph test saved to $RESULTS_DIR/test_complete_graph_output.txt"

$GRAPH_GENERATOR sparse 10 10 $DATA_DIR/test_sparse_graph.txt 0.2
$EXECUTABLE $DATA_DIR/test_sparse_graph.txt > $RESULTS_DIR/test_sparse_graph_output.txt
echo "Sparse graph test saved to $RESULTS_DIR/test_sparse_graph_output.txt"

$GRAPH_GENERATOR bipartite 10 10 $DATA_DIR/test_bipartite_graph.txt
$EXECUTABLE $DATA_DIR/test_bipartite_graph.txt > $RESULTS_DIR/test_bipartite_graph_output.txt
echo "Bipartite graph test saved to $RESULTS_DIR/test_bipartite_graph_output.txt"

echo "All tests completed. Check the results in $RESULTS_DIR."
