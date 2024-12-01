CC = gcc
PKG_CONFIG_PATH = /usr/lib/x86_64-linux-gnu/pkgconfig
CFLAGS = -Wall -Iinclude `pkg-config --cflags glib-2.0`
LDFLAGS = `pkg-config --libs glib-2.0`
BUILD_DIR = build
SRC_DIR = src
INCLUDE_DIR = include
SRC = $(SRC_DIR)/main.c $(SRC_DIR)/graph.c $(SRC_DIR)/utils.c $(SRC_DIR)/graph_factory.c $(SRC_DIR)/graph_algorithm.c $(INCLUDE_DIR)/config.c
OBJ = $(patsubst %.c, $(BUILD_DIR)/%.o, $(notdir $(SRC)))
TARGET = multigraph_analysis

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $(addprefix $(BUILD_DIR)/, $(notdir $(OBJ))) $(LDFLAGS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/%.o: $(INCLUDE_DIR)/%.c
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR) $(TARGET)

debug: CFLAGS += -g
debug: clean all

release: CFLAGS += -O2
release: clean all

test:
	@bash ./test/test_runner.sh
