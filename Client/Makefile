CC=gcc
CFLAGS=-Wall -g
LDFLAGS=-lpthread
SRC=$(wildcard *.c)
OBJ=$(SRC:.c=.o)
EXEC=chat_client

all: $(EXEC)

$(EXEC): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f $(OBJ) $(EXEC)

.PHONY: all clean
