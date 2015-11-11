CC=gcc
CFLAGS=-Wall -Wextra -pedantic -g -std=c99
TARGET=ifj
TEST=ifj_test

all: clean $(TARGET)
test: clean $(TEST)

$(TARGET): *.c *.h
	$(CC) $(CFLAGS) -o $(TARGET) adt.c lex.c error.c galloc.c ial.c ilist.c interpret.c main.c parser.c stack.c string.c builtin.c

$(TEST): *.c *.h
	$(CC) $(CFLAGS) -o $(TEST)  adt.c lex.c error.c galloc.c ial.c ilist.c interpret.c main_test.c parser.c stack.c string.c builtin.c

clean:
	$(RM) $(TARGET) $(TEST)
