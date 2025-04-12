CC = gcc
CFLAGS = -Wall -Wextra -pthread -g
OBJ = bin/main.o bin/server.o bin/request.o bin/router.o bin/mime.o

all: server

server: $(OBJ)
	$(CC) $(CFLAGS) -o bin/server $(OBJ)

bin/main.o: src/main.c src/server/server.h
	$(CC) $(CFLAGS) -c src/main.c -o bin/main.o

bin/server.o: src/server/server.c src/server/server.h
	$(CC) $(CFLAGS) -c src/server/server.c -o bin/server.o

bin/request.o: src/server/request.c src/server/request.h
	$(CC) $(CFLAGS) -c src/server/request.c -o bin/request.o

bin/router.o: src/server/router.c src/server/router.h
	$(CC) $(CFLAGS) -c src/server/router.c -o bin/router.o

bin/mime.o: src/server/mime.c src/server/mime.h
	$(CC) $(CFLAGS) -c src/server/mime.c -o bin/mime.o

clean:
	rm -f *.o bin/*.o bin/server

serve:
	make clean && make server && ./bin/server
