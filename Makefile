CC=gcc
CFLAGS=-Wall -Wextra -pedantic -g -std=c99
TARGET=ifj
TEST=ifj_test

all: clean $(TARGET)
test: clean $(TEST)

$(TARGET):
	$(CC) $(CFLAGS) -o $(TARGET) error.c galloc.c ial.c ilist.c main.c parser.c stack.c string.c

$(TEST):
	$(CC) $(CFLAGS) -o $(TEST) error.c galloc.c ial.c ilist.c main_test.c parser.c stack.c string.c

clean:
	$(RM) $(TARGET) $(TEST)
