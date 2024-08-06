CC = gcc
CFLAGS = -std=c99 -Wall -Wpedantic -Werror -o3 #-luuid
OS := $(shell uname)

mkBinDir := $(shell mkdir -p bin)
mkObjDir := $(shell mkdir -p obj)

BIN = ./bin/main.run

MAIN = 	./obj/main.o

OBJS = 	./obj/scanner.o \
		./obj/file.o \
		./obj/vm.o \
		./obj/table.o \

default: reset $(BIN)
ifeq ($(OS),Darwin) 
	$(BIN)
else 
	valgrind --track-origins=yes --leak-check=full $(BIN)  
endif

reset: clear link

run: reset $(BIN)
	$(BIN)

link: $(OBJS) $(MAIN)
	$(CC) $(CFLAGS) $(OBJS) $(MAIN) -o $(BIN)

./obj/%.o: ./src/%.c ./src/%.h #./src/common.h
	$(CC) $(CFLAGS) -c $< -o $@

./obj/main.o: ./src/main.c ./src/common.h
	$(CC) $(CFLAGS) -c ./src/main.c -o ./obj/main.o
	
clear: clear-bin clear-obj

clear-bin:
	-rm ./bin/*

clear-obj:
	-rm ./obj/*
