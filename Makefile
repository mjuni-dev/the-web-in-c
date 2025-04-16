CC = gcc
CFLAGS = \
	-Wall \
	-Wextra \
	-pthread \
	-g \
	-I./src \
	-I./src/server \
	-I./src/server/router \
	-I./src/server/template \
	-I./src/server/features/auth \
	-I./src/server/features/landing

OBJS = \
	bin/main.o \
	bin/utils.o \
	bin/server.o \
	bin/request.o \
	bin/mime.o \
	bin/response.o \
	bin/radix_router.o \
	bin/auth.o \
	bin/landing.o \
	bin/template.o

all: clean server


server: $(OBJS)
	$(CC) $(CFLAGS) -o bin/server $(OBJS)

bin/main.o: src/main.c src/server/server.h
	$(CC) $(CFLAGS) -c src/main.c -o bin/main.o

bin/utils.o: src/utils.c src/utils.h
	$(CC) $(CFLAGS) -c src/utils.c -o bin/utils.o

bin/server.o: src/server/server.c src/server/server.h
	$(CC) $(CFLAGS) -c src/server/server.c -o bin/server.o

bin/request.o: src/server/request.c src/server/request.h
	$(CC) $(CFLAGS) -c src/server/request.c -o bin/request.o

bin/mime.o: src/server/mime.c src/server/mime.h
	$(CC) $(CFLAGS) -c src/server/mime.c -o bin/mime.o

bin/response.o: src/server/response.c src/server/response.h
	$(CC) $(CFLAGS) -c src/server/response.c -o bin/response.o

bin/radix_router.o: src/server/router/radix_tree_router.c src/server/router/radix_tree_router.h
	$(CC) $(CFLAGS) -c src/server/router/radix_tree_router.c -o bin/radix_router.o

bin/auth.o: src/server/features/auth/auth.c src/server/features/auth/auth.h
	$(CC) $(CFLAGS) -c src/server/features/auth/auth.c -o bin/auth.o

bin/landing.o: src/server/features/landing/landing.c src/server/features/landing/landing.h
	$(CC) $(CFLAGS) -c src/server/features/landing/landing.c -o bin/landing.o

bin/template.o: src/server/template/template.c src/server/template/template.h
		$(CC) $(CFLAGS) -c src/server/template/template.c -o bin/template.o

clean:
	rm -f *.o bin/*.o bin/server

serve:
	make clean && make server && ./bin/server
