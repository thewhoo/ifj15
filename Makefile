CC=gcc
CFLAGS=-Wall -Wextra -pedantic -g -std=c99
PROJECT=ifj

SRC_FILES = $(wildcard *.c)
HEADER_FILES = $(wildcard *.h)
OBJ_FILES = $(patsubst %.c, %.o, $(SRC_FILES))

.PHONY: all pack clean

all: clean $(PROJECT)

$(PROJECT): $(OBJ_FILES)
	$(CC) $(CFLAGS) $^ -o $(PROJECT)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

pack:
	tar -czf xposto02.tgz $(SRC_FILES) $(HEADER_FILES) Makefile

clean:
	$(RM) $(PROJECT) $(TEST) $(OBJ_FILES)
