CC=gcc
CFLAGS=-Wall -Wextra -pedantic -g -std=c99
PROJECT=ifj
TEST=ifj_test

TEST_FILE = main_test.c
TEST_OBJ_FILES = $(filter-out main.o, $(patsubst %.c, %.o, $(wildcard *.c)))

SRC_FILES = $(filter-out $(TEST_FILE), $(wildcard *.c))
HEADER_FILES = $($(wildcard *.h))
OBJ_FILES = $(patsubst %.c, %.o, $(SRC_FILES))

.PHONY: all test pack clean

all: clean $(PROJECT)
test: clean $(TEST)

$(PROJECT): $(OBJ_FILES)
	$(CC) $(CFLAGS) $^ -o $(PROJECT)

$(TEST): $(TEST_OBJ_FILES)
	$(CC) $(CFLAGS) $^ -o $(TEST)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

pack:
	tar -czf xposto02.tgz $(SRC_FILES) $(HEADER_FILES) Makefile

clean:
	$(RM) $(PROJECT) $(TEST) $(OBJ_FILES) $(TEST_OBJ_FILES)
