# Makefile for compiling a C code

CC = gcc
CFLAGS = -Wall -Wextra -g -I../hex_file_reader

all: main

main: main.c ../hex_file_reader/*.c
	$(CC) $(CFLAGS) -o main.exe main.c ../hex_file_reader/*.c

clean:
	rm -f main.exe