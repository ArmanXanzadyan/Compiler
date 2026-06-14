# Compiler & Virtual Machine — Study Guide

This document explains how the project works, phase by phase, in simple words.
The second half is a list of questions your teacher may ask, each with an
answer you can learn. Read the explanation first, then practice the Q&A.

---

## Part 1 — How the code works

### The big idea

The project is a small **compiler** plus a **virtual machine (VM)**. You write
a program in a tiny C-like language. The compiler translates that text, step by
step, into simple numeric instructions. The virtual machine then runs those
instructions and gives back a result.

It works like a factory line: each stage takes the output of the previous stage,
does one job, and passes the result forward.

```
source code
   -> Lexer       (text  -> tokens)
   -> Parser      (tokens -> AST)
   -> IRGen       (AST -> intermediate code)
   -> LogicalCode (lower && || ! into branches)
   -> CodeGenIR   (IR -> binary instructions)
   -> Optimizer   (small cleanup)
   -> Assembler   (readable listing)
   -> ExecIO      (write .exec file)
   -> Loader + VM (run it)  -> result
```

### The source files

| File | Job |
|------|-----|
| `include/Common.h` | Shared types: tokens, AST nodes, opcodes, instruction format |
| `include/Lexer.h` | Lexer: turns source text into tokens |
| `include/Parser.h` | Parser: turns tokens into an AST |
| `src/IRGen.cpp` | Turns the AST into three-address intermediate code |
| `src/LogicalCode.cpp` | Rewrites `&&`, `||`, `!` into compares and branches |
| `src/CodeGenIR.cpp` | Turns IR into the VM's binary instructions |
| `src/optimizer.cpp` | Peephole optimization (removes dead writes) |
| `src/assembler.cpp` | Prints a human-readable disassembly |
| `src/ExecIO.cpp` | Writes/reads the `.exec` executable file |
| `src/loader.cpp` | Loads a program into the VM |
| `src/vm.cpp` | The virtual machine that executes instructions |
| `src/debugger.cpp` | Interactive debugger (step, breakpoints) |
| `src/main.cpp` | Wires all the phases together |

### Phase 1 — Lexer (lexical analysis)

The lexer reads the source **one character at a time** and groups characters
into **tokens**. A token is the smallest meaningful unit: a number, an
identifier, a keyword, or an operator.

Example: the text `int a = 5;` becomes the tokens
`INT_KW`, `VARIABLE(a)`, `ASSIGN`, `NUMBER(5)`, `SEMICOLON`.

It also skips spaces and `//` comments, and remembers line numbers so error
messages can point to the right line. Code: `include/Lexer.h`.

### Phase 2 — Parser (syntax analysis)

The parser takes the list of tokens and checks that they follow the grammar
rules of the language. As it does this, it builds an **Abstract Syntax Tree
(AST)** — a tree that shows the structure of the program.

It uses **recursive descent**: one function per grammar rule. Operator
precedence (for example, `*` binds tighter than `+`) is handled by a chain of
functions, one per precedence level. Code: `include/Parser.h`.

Example AST for `a + b`:

```
    BINOP "+"
    /       \
 VAR_REF   VAR_REF
   (a)       (b)
```

### Phase 3 — IRGen (intermediate code)

The AST is converted into **three-address code**: simple instructions that use
at most three operands, like `t0 = a + b`. This form is easy to optimize and
easy to translate to machine code. Temporary values get names like `t0`, `t1`.
Code: `src/IRGen.cpp`, types in `include/ir.h`.

### Phase 4 — LogicalCode (lowering logical operators)

The operators `&&`, `||`, and `!` use **short-circuit** behavior, so they can't
be a single arithmetic instruction. This pass rewrites them into **compares and
conditional branches**. After this pass the back end only has to deal with
simple jumps. Code: `src/LogicalCode.cpp`.

### Phase 5 — CodeGenIR (code generation)

This turns the IR into the VM's real **binary instructions**. Each instruction
is a fixed 4-byte word with an opcode and up to three register fields. This
phase also:
- assigns IR names to **registers**,
- builds a **constant pool** for literal numbers,
- records **symbols** (function names) and **relocations** (call targets to be
  fixed up), and
- builds **jump tables** for `switch`.

Code: `src/CodeGenIR.cpp`.

### Phase 6 — Optimizer

A tiny **peephole optimizer**: it scans short windows of instructions and
removes obvious waste. Here it drops a `MOV_CONST` if the very next instruction
overwrites the same register (a dead store). Code: `src/optimizer.cpp`.

### Phase 7 — Assembler

Prints a readable listing of the instructions (the disassembly) so a human can
inspect what was generated. It can also parse that text back into a program.
Code: `src/assembler.cpp`.

### Phase 8 — ExecIO (the executable file)

Writes a self-describing `.exec` file, similar in spirit to a real ELF file. It
has a **header** (signature `EXEC`, word size, section table) followed by
**sections**: code, data (constants), symbol table, relocations, and jump
tables. It can also load such a file back. Code: `src/ExecIO.cpp`.

### Phase 9 — Loader + Virtual Machine

The **loader** places the program into the VM and resets the registers. The
**virtual machine** then runs it using the classic cycle:

1. **Fetch** — read the instruction at the program counter `pc`.
2. **Decode** — figure out the opcode and operands.
3. **Execute** — perform the operation, update registers/memory, advance `pc`.

Repeat until a `HALT` instruction. The VM is **register-based**: it has 32
registers, a program counter, a stack pointer (`sp`), a base pointer (`bp`),
and a flags register. Memory (1 MB) is split into segments: **Code, Data, Heap,
Stack**. It supports arithmetic, comparison, branches, function calls with
stack frames, push/pop, memory load/store, heap allocation, and jump tables.
Code: `include/vm.h`, `src/vm.cpp`.

### How everything runs together

`src/main.cpp` calls each phase in order and prints what happens. The built-in
demo program is:

```c
int main() {
    int a = 5;
    int b = 10;
    int c = 0;
    if (a < b) {
        c = a + b;
    }
    return c;
}
```

Because `a < b` is true, `c` becomes `15`, and the VM returns **R0 = 15**.

### The two builds (Release and Debug)

- **Release**: `make` -> `build/release/compiler`, optimized with `-O2 -DNDEBUG`.
- **Debug**: `make debug` -> `build/debug/compiler`, built with `-O0 -g3
  -DDEBUG_BUILD` plus Address/UB sanitizers and full `gdb` symbols.

The debug build defines `DEBUG_BUILD`, which turns on a **per-instruction trace**
in the VM. Every instruction is printed as it is fetched, so you can watch the
fetch-decode-execute loop run. The same two builds also exist in CMake through
`-DCMAKE_BUILD_TYPE=Debug` or `Release`.

---

## Part 2 — Questions and answers to learn

### Basic concepts

**Q1. What is a compiler?**
A compiler is a program that translates source code written in one language into
another form — usually lower-level instructions — that a machine can run. Our
compiler translates a small C-like language into instructions for our virtual
machine.

**Q2. What is the difference between a compiler and an interpreter?**
A compiler translates the whole program first and produces output (instructions
or a file) that runs later. An interpreter executes the source directly, line by
line, without producing a separate output file. Our project compiles to
instructions and then a separate virtual machine runs them.

**Q3. What are the main phases of a compiler?**
Lexical analysis (lexer), syntax analysis (parser), intermediate code
generation, optimization, and code generation. Some compilers add semantic
analysis between parsing and IR. Our project follows this structure.

**Q4. What is the front end, middle end, and back end?**
Front end understands the source (lexer, parser). Middle end works on a
language-independent intermediate form (IR generation, optimization). Back end
produces the final target code (code generation, executable file).

### Lexer

**Q5. What does the lexer do?**
It reads the source text character by character and groups characters into
tokens (numbers, identifiers, keywords, operators), skipping whitespace and
comments.

**Q6. What is a token?**
A token is the smallest meaningful unit of the source: for example the keyword
`if`, the identifier `count`, the number `42`, or the operator `==`.

**Q7. How does the lexer tell a keyword from a variable name?**
It reads a whole word made of letters, digits, and underscores, then checks it
against a list of keywords. If it matches (`if`, `while`, `int`, ...) it is a
keyword token; otherwise it is a `VARIABLE` (identifier) token.

**Q8. How are two-character operators like `==` or `&&` handled?**
After reading the first character, the lexer "peeks" at the next character. If
the pair forms a known operator (`==`, `!=`, `<=`, `>=`, `&&`, `||`), it consumes
both characters and returns one token.

### Parser

**Q9. What does the parser do?**
It checks that the tokens follow the grammar and builds an Abstract Syntax Tree
(AST) that represents the structure of the program.

**Q10. What is an AST?**
An Abstract Syntax Tree is a tree representation of the program's structure.
Each node is a construct (an `if`, a binary operation, a variable reference),
and child nodes are its parts.

**Q11. What parsing technique do you use?**
Recursive descent: one function per grammar rule, calling each other to match
nested structures. Expression precedence is handled by a chain of functions,
from lowest precedence (assignment) to highest (primary).

**Q12. How is operator precedence handled, so `2 + 3 * 4` is correct?**
Each precedence level has its own function. `parseAddSub` calls `parseMulDiv`
for its operands, so multiplication is grouped before addition. This makes
`3 * 4` evaluate first, giving `2 + 12 = 14`.

**Q13. How does the parser report errors?**
When it expects a specific token and finds something else, it throws an error
message that includes the line number and what was expected, for example
"expected ';'".

### Intermediate code

**Q14. What is intermediate representation (IR)?**
IR is a simple, machine-independent form of the program between the AST and the
final code. It is easier to optimize and to translate than either the AST or raw
machine code.

**Q15. What is three-address code?**
Instructions that have at most three operands, typically one result and two
sources, like `t0 = a + b`. Complex expressions are broken into several such
instructions using temporary variables.

**Q16. Why do you lower `&&`, `||`, `!` separately?**
Because they use short-circuit evaluation: the second operand may not run at
all. They cannot be a single arithmetic instruction, so they are rewritten into
compares and conditional branches.

### Code generation and the executable

**Q17. What does the code generator produce?**
Binary instructions for the virtual machine, plus a constant pool, a symbol
table, relocations, and jump tables. Each instruction is a fixed 4-byte word.

**Q18. What is a constant pool?**
A list that stores literal values (like the numbers 5 and 10). Instructions
refer to a constant by its index instead of embedding the full value.

**Q19. What is a relocation?**
A note that says "this instruction refers to a symbol whose address is not known
yet." After all addresses are known, the loader or linker fills in the real
address. We use it for function call targets.

**Q20. What does the optimizer do here?**
It is a peephole optimizer: it looks at a small window of instructions and
removes obvious waste. In our case, if two `MOV_CONST` instructions write the
same register in a row, the first is dead and is removed.

**Q21. What is in the `.exec` file?**
A header with a signature (`EXEC`), the word size, and a section table; then
sections for code, data (constants), symbols, relocations, and jump tables. It
is a simplified version of a real executable format like ELF.

### Virtual machine

**Q22. What is a virtual machine?**
A program that imitates a CPU. It has registers and memory and executes its own
instruction set in software, so the compiled program runs the same way on any
real computer.

**Q23. What is the fetch-decode-execute cycle?**
The basic loop of a processor: fetch the next instruction from memory, decode it
to find the operation and operands, execute it, then move to the next
instruction. Our VM repeats this until it hits `HALT`.

**Q24. Is your VM register-based or stack-based? What's the difference?**
It is register-based: operations work on numbered registers. A stack-based VM
(like the JVM) keeps operands on a stack instead. Register machines often need
fewer instructions per operation.

**Q25. What registers and pointers does the VM have?**
32 general registers, a program counter `pc`, a stack pointer `sp`, a base
pointer `bp`, and a flags register for comparison results.

**Q26. How is memory organized?**
One 1 MB address space split into segments: Code (the instructions), Data
(global constants), Heap (dynamic allocation), and Stack (function frames,
growing downward from the top).

**Q27. How does a function call work in the VM?**
On `CALL`, the VM saves the return address and the caller's base pointer, sets a
new frame, and jumps to the function. On `RET`, it restores them, puts the
return value in register 0, and continues after the call. Stack frame setup uses
`PUSH_BP`, `SET_BP_SP`, `ALLOC_STACK`, and `RESTORE_BP`.

**Q28. How are `if` and `while` executed at the machine level?**
They become a compare instruction that sets a result, followed by a conditional
branch (`BR_EQ`/`BR_NEQ`) that changes the `pc` if the condition is or isn't
met. A `while` loop also has an unconditional `JMP` back to the test.

**Q29. How does `switch` work?**
It uses a jump table: the switch value is used as an index into a table of target
addresses, and the VM jumps directly to the matching case.

### Builds and debugging

**Q30. What is the difference between the Release and Debug builds?**
Release is optimized (`-O2`) with assertions off (`-DNDEBUG`) for normal use.
Debug is unoptimized (`-O0 -g3`) with debugging symbols, the `DEBUG_BUILD`
define, and sanitizers, so it is easier to inspect and catches memory errors.

**Q31. What does the debug build add at runtime?**
It prints a trace line for every instruction the VM fetches, showing the program
counter, the opcode, and the operands. This makes the fetch-decode-execute loop
visible.

**Q32. What are AddressSanitizer and UBSan?**
Tools built into the compiler that check the program while it runs:
AddressSanitizer detects memory errors (out-of-bounds, use-after-free) and
UndefinedBehaviorSanitizer detects undefined behavior (like integer overflow).
They run in the debug build and report problems immediately.

**Q33. How do you build and run the project?**
With the Makefile: `make` then `./build/release/compiler`, or `make debug` then
`./build/debug/compiler`. With CMake: `cmake -S . -B build
-DCMAKE_BUILD_TYPE=Release` then `cmake --build build`.

### About the demo and a real fix

**Q34. What does the demo program return and why?**
It returns 15. It sets `a = 5`, `b = 10`, and because `a < b` is true it runs
`c = a + b`, so `c = 15`, which `main` returns into register 0.

**Q35. Describe a bug you fixed in this project.**
The IR generator had no case for assignment expressions, so an assignment used
as an expression (like `c = a + b;`) was silently dropped, and the demo wrongly
returned 0. I added the `ASSIGN_EXPR` case in `IRGen` to evaluate the right side
and store it into the left side, so the program now correctly returns 15.

**Q36. Why split the compiler into so many phases?**
Each phase has one clear responsibility, which makes the code easier to
understand, test, and change. It also lets phases be reused: for example, the
same IR could target a different machine by changing only the back end.

---

## Quick revision checklist

- Phases in order: Lexer, Parser, IRGen, LogicalCode, CodeGenIR, Optimizer,
  Assembler, ExecIO, Loader + VM.
- Lexer = text to tokens. Parser = tokens to AST.
- IR = simple three-address instructions; logical ops become branches.
- Code generator = binary instructions + constant pool + symbols + relocations.
- VM = register machine, 1 MB segmented memory, fetch-decode-execute until HALT.
- Two builds: Release (`-O2`) and Debug (`-O0 -g3 -DDEBUG_BUILD`, with trace +
  sanitizers).
- Demo returns 15.
