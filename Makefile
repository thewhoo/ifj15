CC=gcc
CFLAGS=-Wall -Wextra -pedantic -g -std=c99
TARGET=ifj
MKDIR_P=mkdir -p
BINDIR=bin

all: directories $(TARGET)

$(TARGET): main.o vector.o
	$(CC) $(CFLAGS) -o $(BINDIR)/$(TARGET) $(BINDIR)/main.o $(BINDIR)/vector.o

main.o: main.c vector.h enums.h
	$(CC) $(CFLAGS) -c main.c -o $(BINDIR)/main.o

vector.o: vector.c vector.h enums.h
	$(CC) $(CFLAGS) -c vector.c -o $(BINDIR)/vector.o

directories:
	$(MKDIR_P) $(BINDIR)

clean:
	$(RM) $(BINDIR)/*
