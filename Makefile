CC=g++
CFLAGS=-std=c++11 -c -g
BIN=bin
SRC=src
SRCS=$(wildcard $(SRC)/*.cpp)
EXES=$(SRCS:$(SRC)/%.cpp=%)

.SECONDARY:



all : $(EXES)

% : $(SRC)/%.o
	$(CC) $< -o $(BIN)/$@

%.o : %.cpp
	$(CC) $(CFLAGS) $< -o $@

listflags :
	@echo CC=$(CC)
	@echo CFLAGS=$(CFLAGS)
	@echo BIN=$(BIN)
	@echo SRC=$(SRCS)
	@echo SRCS=$(SRCS)
	@echo EXES=$(EXES)

clean :
	rm -rf $(SRC)/*.o
	rm -rf $(BIN)/*

