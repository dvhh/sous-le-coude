CFLAGS+=-Wall -Werror -march=native -O2 -std=c99 -D_POSIX_C_SOURCE=200809L
OBJS=main.o serve/static.o serve/error.o serve/test.o serve/clientAddress.o
TARGET=./main

all: compile

compile: $(TARGET)


$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $^ -lcrypt
	strip $(TARGET)

clean:
	-rm -f $(OBJS) $(TARGET)

run: compile
	$(TARGET) 8080
