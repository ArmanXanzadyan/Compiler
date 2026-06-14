# Compiler

A small educational C-like compiler and virtual machine, written in C++17.
It walks a tiny C subset through every classical phase so you can read each
layer on its own.

## Pipeline

1. `Lexer` -> tokens
2. `Parser` -> AST
3. `IRGen` -> three-address IR
4. `LogicalCode` -> lowers `&&`, `||`, `!` to branches
5. `CodeGenIR` -> binary `Instruction`s + const pool, symbols, relocs, jump tables
6. `Optimizer` -> peephole cleanup
7. `Assembler` -> textual disassembly
8. `ExecIO` -> writes a self-describing `.exec` file
9. `Loader` + `VM` -> executes (with optional `Debugger` monitor)

## Build

The project builds in two standard configurations: an optimized **release**
build and a fully instrumented **debug** build.

With CMake (configuration chosen by `CMAKE_BUILD_TYPE`):

```bash
# Release (optimized, NDEBUG)
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j
./build/compiler

# Debug (-O0 -g3, DEBUG_BUILD diagnostics)
cmake -S . -B build-debug -DCMAKE_BUILD_TYPE=Debug
cmake --build build-debug -j
./build-debug/compiler

# Optional Address/UB sanitizers for the Debug build
cmake -S . -B build-debug -DCMAKE_BUILD_TYPE=Debug -DENABLE_SANITIZERS=ON
```

Or the plain Makefile (separate output trees under `build/`):

```bash
make            # release -> build/release/compiler
make debug      # debug   -> build/debug/compiler  (sanitizers on by default)
make debug SANITIZE=0   # debug without sanitizers
make run        # build + run release
make run-debug  # build + run debug
make clean
```

The debug build defines `DEBUG_BUILD`, which turns on a per-instruction trace
in the VM (printed to `stderr`) so you can watch the whole execution without
stepping through it by hand. It also keeps full symbols for use under `gdb`.

## Run

```text
./compiler                       # runs the built-in demo program (returns 15)
./compiler path/to/program.c     # compile, dump asm, write program.exec, run
./compiler --exec program.exec   # load a prebuilt EXEC and run it
./compiler --debug program.c     # step through in the interactive debugger
./compiler -o out.exec prog.c    # choose the output EXEC path
```

Debugger commands: `s` step, `c` continue, `r` registers, `m` segments,
`b N` set breakpoint at PC `N`, `q` quit.

## Layout

```
include/    public headers
src/        translation units
CMakeLists.txt
Makefile
```
