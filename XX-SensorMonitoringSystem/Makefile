# Compiler and flags
CC = gcc
CFLAGS = -g -Wall -pthread -Iinclude -Isrc
LDFLAGS = -lsqlite3 -pthread

# Project structure
SRC_DIR = src
OBJ_DIR = build
BIN = sensor_gateway

# Source and object files
SRCS = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRCS))

# Default target
all: $(BIN)

# Build binary
$(BIN): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

# Compile source files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Clean
clean:
	rm -f $(BIN)
	rm -rf $(OBJ_DIR)
	rm -f logs/gateway.log db/sensors.db

# Run
run: $(BIN)
	./$(BIN) 1234

# Memory check
valgrind: $(BIN)
	valgrind --leak-check=full ./$(BIN) 1234

.PHONY: all clean run valgrind
