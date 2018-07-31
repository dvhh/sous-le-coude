all: compile

compile: main.c
	$(CC) -Wall -Werror -march=native -o main -O2 main.c
	strip main