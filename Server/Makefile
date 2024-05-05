CC=gcc
CFLAGS=-Wall -g
LDFLAGS=-lpthread
OBJ=main.o client_handler.o message.o chat_node.o properties.o
EXEC=server

all: $(EXEC)

$(EXEC): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

main.o: main.c main.h client_handler.h chat_node.h properties.h
	$(CC) $(CFLAGS) -c main.c

client_handler.o: client_handler.c client_handler.h message.h chat_node.h
	$(CC) $(CFLAGS) -c client_handler.c

message.o: message.c message.h chat_node.h
	$(CC) $(CFLAGS) -c message.c

chat_node.o: chat_node.c chat_node.h
	$(CC) $(CFLAGS) -c chat_node.c

properties.o: properties.c properties.h
	$(CC) $(CFLAGS) -c properties.c

clean:
	rm -f $(OBJ) $(EXEC)

.PHONY: all clean
