# Compiler and flags
CC = gcc
CFLAGS = -g -Wall -pthread -Iinclude -Isrc
LDFLAGS = -lsqlite3 -pthread

# Project structure
SRC_DIR = src
OBJ_DIR = build
BIN = sensor_gateway
SENSOR_NODE_BIN = node

# Source and object files for sensor_gateway
SRCS = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRCS))

# Source file for sensor_node (assuming it's in sensor_node directory)
SENSOR_NODE_SRC = sensor_node/sensor_node.c
SENSOR_NODE_OBJ = $(OBJ_DIR)/sensor_node.o

# Default target
all: $(BIN) $(SENSOR_NODE_BIN)

# Build sensor_gateway binary
$(BIN): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

# Build sensor_node binary
$(SENSOR_NODE_BIN): $(SENSOR_NODE_OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

# Compile source files for sensor_gateway
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Compile sensor_node source file
$(SENSOR_NODE_OBJ): $(SENSOR_NODE_SRC)
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Clean
clean:
	rm -f $(BIN) $(SENSOR_NODE_BIN)
	rm -rf $(OBJ_DIR)
	rm -f logs/gateway.log db/sensors.db

# Run sensor_gateway
run_gateway: $(BIN)
	./$(BIN) 1234

# Run sensor_node
run_sensor_node: $(SENSOR_NODE_BIN)
	./$(SENSOR_NODE_BIN)

# Memory check for sensor_gateway
valgrind_gateway: $(BIN)
	valgrind --leak-check=full ./$(BIN) 1234

# Memory check for sensor_node
valgrind_sensor_node: $(SENSOR_NODE_BIN)
	valgrind --leak-check=full ./$(SENSOR_NODE_BIN)

.PHONY: all clean run_gateway run_sensor_node valgrind_gateway valgrind_sensor_node
