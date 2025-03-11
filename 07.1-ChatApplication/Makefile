.PHONY := all chat clean

CC = gcc
CFLAGS = -Wall -g -Iinclude
LDFLAGS = -pthread

all: chat

chat: main.c chat/chat.c server/server_socket.c client/client_socket.c ui/ui.c
	$(CC) $(CFLAGS) -o chatApp $^ $(LDFLAGS)

clean:
	rm -f chatApp
