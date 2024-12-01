CC = gcc
PKG_CONFIG_PATH = /usr/lib/x86_64-linux-gnu/pkgconfig
CFLAGS = -Wall -Iinclude `pkg-config --cflags glib-2.0`
LDFLAGS = `pkg-config --libs glib-2.0`
BUILD_DIR = build
SRC_DIR = src
INCLUDE_DIR = include
TEST_DIR = test
SRC = $(SRC_DIR)/main.c $(SRC_DIR)/graph.c $(SRC_DIR)/utils.c $(SRC_DIR)/graph_factory.c $(SRC_DIR)/graph_algorithm.c $(INCLUDE_DIR)/config.c
OBJ = $(patsubst %.c, $(BUILD_DIR)/%.o, $(notdir $(SRC)))
TARGET = multigraph_analysis

TEST_SRC = $(TEST_DIR)/graph_gen.c
TEST_OBJ = $(patsubst %.c, $(BUILD_DIR)/%.o, $(notdir $(TEST_SRC)))
TEST_TARGET = graph_gen

all: $(TARGET) $(TEST_TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $(addprefix $(BUILD_DIR)/, $(notdir $(OBJ))) $(LDFLAGS)

$(TEST_TARGET): $(TEST_OBJ)
	$(CC) $(CFLAGS) -o $@ $(addprefix $(BUILD_DIR)/, $(notdir $(TEST_OBJ)))

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/%.o: $(INCLUDE_DIR)/%.c
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/%.o: $(TEST_DIR)/%.c
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR) $(TARGET) $(TEST_TARGET)

debug: CFLAGS += -g
debug: clean all

release: CFLAGS += -O2
release: clean all

test: all
	@echo "Running all tests..."
	@bash ./test/test_runner.sh

tests: $(TEST_TARGET)
	@echo "Running graph generator tests..."
	./$(TEST_TARGET) complete 1 10 10 ./data/test_complete_graph.txt
	./$(TEST_TARGET) sparse 1 10 10 ./data/test_sparse_graph.txt 0.2
	./$(TEST_TARGET) bipartite 1 10 10 ./data/test_bipartite_graph.txt
	./$(TEST_TARGET) multigraph 1 10 10 ./data/test_multigraph.txt 3
	@echo "Test graphs generated in ./data directory."
