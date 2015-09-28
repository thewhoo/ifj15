CC=gcc
CFLAGS=-Wall -Wextra -pedantic -g -std=c99
TARGET=ifj

all: clean $(TARGET)

$(TARGET):
	$(CC) $(CFLAGS) -o $(TARGET) *.c

clean:
	$(RM) $(TARGET)
