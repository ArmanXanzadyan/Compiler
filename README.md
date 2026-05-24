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

With CMake:

```bash
cmake -S . -B build
cmake --build build -j
./build/compiler
```

Or the plain Makefile:

```bash
make
./build/compiler
```

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
