CC=gcc
CFLAGS=-Wall -Wextra -pedantic -O2 -std=c99
CFLAGS_DBG=-Wall -Wextra -pedantic -g -std=c99
# coverage tool - gcovr
COVERAGE_FLAGS=-Wall -Wextra -pedantic -fprofile-arcs -ftest-coverage -fPIC -O0 -std=c99
PROJECT=ifj
RM=rm -f

SRC_FILES = $(wildcard *.c)
HEADER_FILES = $(wildcard *.h)
OBJ_FILES = $(patsubst %.c, %.o, $(SRC_FILES))

DBG_OBJ_FILES = $(patsubst %.c, %.dbg.o, $(SRC_FILES))

.PHONY: all debug dbg coverage release pack clean

all: clean release

debug: clean dbg

coverage: CFLAGS = $(COVERAGE_FLAGS)
coverage: clean release

release: $(OBJ_FILES)
	$(CC) $(CFLAGS) $^ -o $(PROJECT)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

dbg: $(DBG_OBJ_FILES)
	$(CC) $(CFLAGS_DBG)  $^ -o $(PROJECT)

%.dbg.o: %.c
	$(CC) $(CFLAGS_DBG) -DDEBUG_MODE -c $< -o $@

pack:
	@cp doc/dokumentace.pdf .
	tar -czf xposto02.tgz $(SRC_FILES) $(HEADER_FILES) rozdeleni dokumentace.pdf Makefile
	@rm -f dokumentace.pdf

clean:
	$(RM) $(PROJECT) $(OBJ_FILES) $(DBG_OBJ_FILES)
