CFLAGS=-Wall -Werror -march=native -O2
all: compile

compile: main.c
	$(CC) $(CFLAGS) -o main main.c
	strip main