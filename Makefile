CC=g++
CFLAGS=-std=c++11 -c -g
LIB=-I /usr/include/eigen3/
BIN=bin
SRC=src
BUILD=build
SRCS=$(wildcard $(SRC)/*.cpp)
EXES=$(SRCS:$(SRC)/%.cpp=%)

.SECONDARY:



all : $(EXES)

% : $(BUILD)/%.o
	$(CC) $< -o $(BIN)/$@

$(BUILD)/%.o : $(SRC)/%.*
	$(CC) $(CFLAGS) $(LIB) $< -o $@

listflags :
	@echo CC=$(CC)
	@echo CFLAGS=$(CFLAGS)
	@echo BIN=$(BIN)
	@echo SRC=$(SRC)
	@echo SRCS=$(SRCS)
	@echo EXES=$(EXES)
	@echo BUILD=$(BUILD)

clean :
	rm -rf $(BUILD)/*
	rm -rf $(BIN)/*

test :
	./bin/new_wall_finder data/bwi_lab.ply
