CC=gcc
CFLAGS=-g -Wall -pthread -Iinclude
LDFLAGS=-lsqlite3 -pthread

all: sensor_gateway

sensor_gateway: src/main.o src/connection_manager.o src/data_manager.o src/storage_manager.o src/log_process.o src/sbuffer.o src/sensor_node.o
	$(CC) $(LDFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f sensor_gateway src/*.o logs/gateway.log db/sensors.db

run: sensor_gateway
	./sensor_gateway 1234

valgrind: sensor_gateway
	valgrind --leak-check=full ./sensor_gateway 1234

.PHONY: all clean run valgrind