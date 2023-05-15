CC = clang
CFLAGS = -Wall -Wextra -std=c11 -pedantic
BIN = clarge
OUTPUT = ./build
OPTIM = -O3
SRC = clarge.c file_item.c helpers.c

.PHONY: build run prod static clean

build:
	$(CC) $(CFLAGS) -g -o $(OUTPUT)/$(BIN) $(SRC)

run: build
	@$(OUTPUT)/$(BIN) $(DIR)

prod:
	@$(CC) $(CFLAGS) $(OPTIM) -o $(OUTPUT)/$(BIN) $(SRC)

static:
	@$(CC) $(CFLAGS) -static $(OPTIM) -o $(OUTPUT)/$(BIN) $(SRC)

clean:
	@rm -rf *.txt *.o *.out ./build/* ./logs/*
