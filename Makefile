# Convenience Makefile for the toy compiler/VM.
# CMake is the canonical build; this file is here for quick iteration.

CXX      ?= g++
CXXFLAGS ?= -std=c++17 -O2 -Wall -Wextra -Wpedantic -Iinclude
LDFLAGS  ?=

SRC := $(wildcard src/*.cpp)
OBJ := $(SRC:src/%.cpp=build/%.o)
BIN := build/compiler

.PHONY: all clean run

all: $(BIN)

build:
	@mkdir -p build

$(BIN): $(OBJ) | build
	$(CXX) $(LDFLAGS) -o $@ $^

build/%.o: src/%.cpp | build
	$(CXX) $(CXXFLAGS) -c $< -o $@

run: $(BIN)
	./$(BIN)

clean:
	rm -rf build
