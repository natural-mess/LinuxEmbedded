.PHONY : all chat clean

CC = gcc
CFLAGS = -Wall -g -Iinclude
LDFLAGS = -pthread

all: chat

chat: main.c chatDrv/chat.c server/server_socket.c client/client_socket.c ui/ui.c
	$(CC) $(CFLAGS) -o chat $^ $(LDFLAGS)

clean:
	rm -f chat