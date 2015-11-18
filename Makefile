CC=gcc
CFLAGS=-Wall -Wextra -pedantic -g -std=c99
PROJECT=ifj

SRC_FILES = $(wildcard *.c)
HEADER_FILES = $(wildcard *.h)
OBJ_FILES = $(patsubst %.c, %.o, $(SRC_FILES))

DBG_OBJ_FILES = $(patsubst %.c, %.dbg.o, $(SRC_FILES))

.PHONY: all pack clean

all: clean release

debug: clean dbg

release: $(OBJ_FILES)
	$(CC) $(CFLAGS) $^ -o $(PROJECT)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

dbg: $(DBG_OBJ_FILES)
	$(CC) $(CFLAGS)  $^ -o $(PROJECT)

%.dbg.o: %.c
	$(CC) $(CFLAGS) -DDEBUG_MODE -c $< -o $@

pack:
	tar -czf xposto02.tgz $(SRC_FILES) $(HEADER_FILES) Makefile

clean:
	$(RM) $(PROJECT) $(TEST) $(OBJ_FILES) $(DBG_OBJ_FILES)
