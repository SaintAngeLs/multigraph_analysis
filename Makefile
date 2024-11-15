CC = gcc
CFLAGS = -Wall -Wpedantic -Werror -std=c99 -Iinclude
BUILD_DIR = build
SRC = src/graph.c  src/graph_algorithm.c  src/graph_factory.c  src/main.c  src/utils.c include/config.c 
OBJ = $(SRC:src/%.c=$(BUILD_DIR)/%.o)
TARGET = multigraph_analysis

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

$(BUILD_DIR)/%.o: src/%.c
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/config.o: include/config.c
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR) $(TARGET)

debug: CFLAGS += -g -fsanitize=address,undefined
debug: clean all

release: CFLAGS += -O2
release: clean all
