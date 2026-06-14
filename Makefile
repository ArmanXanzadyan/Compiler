# Convenience Makefile for the toy compiler/VM.
# CMake is the canonical build; this file mirrors it for quick iteration and
# supports two standard configurations: optimized "release" and debuggable
# "debug".
#
#   make            # release build  -> build/release/compiler
#   make release    # same as above
#   make debug      # debug build    -> build/debug/compiler
#   make run        # build + run the release binary
#   make run-debug  # build + run the debug binary
#   make clean      # remove all build artifacts
#
# Override the toolchain or flags from the command line, e.g.
#   make CXX=clang++ debug

CXX      ?= g++
WARNINGS := -Wall -Wextra -Wpedantic
CXXSTD   := -std=c++17
CPPFLAGS ?= -Iinclude
LDFLAGS  ?=

# Per-configuration flags. Release is optimized with assertions disabled.
# Debug keeps full symbols, no optimization, assertions on, and a DEBUG_BUILD
# define that enables extra diagnostics in the sources.
RELEASE_FLAGS := -O2 -DNDEBUG
DEBUG_FLAGS   := -O0 -g3 -DDEBUG_BUILD -fno-omit-frame-pointer

# Address/UB sanitizers are on by default for debug builds; disable with
#   make debug SANITIZE=0
SANITIZE ?= 1
ifeq ($(SANITIZE),1)
DEBUG_FLAGS  += -fsanitize=address,undefined
DEBUG_LDLIBS := -fsanitize=address,undefined
endif

SRC := $(wildcard src/*.cpp)

REL_OBJ := $(SRC:src/%.cpp=build/release/%.o)
DBG_OBJ := $(SRC:src/%.cpp=build/debug/%.o)
REL_BIN := build/release/compiler
DBG_BIN := build/debug/compiler

.PHONY: all release debug run run-debug clean

all: release
release: $(REL_BIN)
debug:   $(DBG_BIN)

build/release build/debug:
	@mkdir -p $@

$(REL_BIN): $(REL_OBJ) | build/release
	$(CXX) $(LDFLAGS) -o $@ $^

$(DBG_BIN): $(DBG_OBJ) | build/debug
	$(CXX) $(LDFLAGS) -o $@ $^ $(DEBUG_LDLIBS)

build/release/%.o: src/%.cpp | build/release
	$(CXX) $(CXXSTD) $(WARNINGS) $(CPPFLAGS) $(RELEASE_FLAGS) -c $< -o $@

build/debug/%.o: src/%.cpp | build/debug
	$(CXX) $(CXXSTD) $(WARNINGS) $(CPPFLAGS) $(DEBUG_FLAGS) -c $< -o $@

run: $(REL_BIN)
	./$(REL_BIN)

run-debug: $(DBG_BIN)
	./$(DBG_BIN)

clean:
	rm -rf build
