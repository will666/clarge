CC = clang
CFLAGS = -Wall -Wextra -std=c11 -pedantic
OUTPUT = ./bin/main
OPTIM = -O3
SRC = main.c file_item.c helpers.c

build:
	$(CC) $(CFLAGS) -g -o $(OUTPUT) $(SRC)

run: build
	$(OUTPUT) $(DIR)

prod:
	$(CC) $(CFLAGS) $(OPTIM) -o $(OUTPUT) $(SRC)

static:
	$(CC) $(CFLAGS) -static $(OPTIM) -o $(OUTPUT) $(SRC)

clean:
	rm -rf *.txt *.o *.out ./bin/*
