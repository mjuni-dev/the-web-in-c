CC = gcc
CFLAGS = -Wall -Wextra -pthread -g
OBJ = bin/main.o bin/server.o

all: server

server: $(OBJ)
	$(CC) $(CFLAGS) -o bin/server $(OBJ)

bin/main.o: src/main.c src/server/server.h
	$(CC) $(CFLAGS) -c src/main.c -o bin/main.o

bin/server.o: src/server/server.c src/server/server.h
	$(CC) $(CFLAGS) -c src/server/server.c -o bin/server.o

clean:
	rm -f *.o **/*.o server
