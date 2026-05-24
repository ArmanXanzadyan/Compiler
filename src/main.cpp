#include "CodeGenIR.h"
#include "Common.h"
#include "ExecIO.h"
#include "IRGen.h"
#include "Lexer.h"
#include "LogicalCode.h"
#include "Parser.h"
#include "assembler.h"
#include "debugger.h"
#include "loader.h"
#include "optimizer.h"
#include "vm.h"

#include <fstream>
#include <iostream>
#include <string>

static const char* DEMO_SOURCE = R"(
int main() {
    int a = 5;
    int b = 10;
    int c = 0;
    if (a < b) {
        c = a + b;
    }
    return c;
}
)";

static void printUsage(const char* prog) {
    std::cout
        << "Usage: " << prog << " [options] [source.c | program.exec]\n"
        << "  --exec           Treat the input path as a prebuilt EXEC image\n"
        << "  --debug          Drop into the interactive debugger after loading\n"
        << "  -o <path>        Output EXEC path (default: program.exec)\n"
        << "  -h, --help       Show this message\n";
}

int main(int argc, char* argv[]) {
    bool        useExec   = false;
    bool        debugMode = false;
    std::string execPath  = "program.exec";
    std::string inputPath;

    for (int i = 1; i < argc; ++i) {
        std::string a = argv[i];
        if      (a == "--exec")  useExec   = true;
        else if (a == "--debug") debugMode = true;
        else if (a == "-h" || a == "--help") { printUsage(argv[0]); return 0; }
        else if (a == "-o" && i + 1 < argc)  { execPath = argv[++i]; }
        else if (!a.empty() && a[0] == '-') {
            std::cerr << "Unknown option: " << a << "\n";
            printUsage(argv[0]);
            return 1;
        }
        else inputPath = a;
    }

    if (useExec && !inputPath.empty()) execPath = inputPath;

    std::string source = DEMO_SOURCE;
    if (!useExec && !inputPath.empty()) {
        std::ifstream f(inputPath);
        if (!f) {
            std::cerr << "Cannot open source file: " << inputPath << "\n";
            return 1;
        }
        source.assign(std::istreambuf_iterator<char>(f),
                      std::istreambuf_iterator<char>());
    }

    std::cout << "=== Full curriculum pipeline ===\n\n";

    std::cout << "[1] Lexer + Parser -> AST\n";
    Lexer  lexer(source);
    auto   tokens = lexer.tokenize();
    Parser parser(std::move(tokens));
    auto   ast = parser.parseProgram();

    std::cout << "[2] AST -> Intermediate Code (IR)\n";
    IRGen     irGen;
    IRProgram ir = irGen.generate(ast.get());

    std::cout << "[3] IR -> Logical Code (lower && || !)\n";
    LogicalCode::process(ir);

    std::cout << "[4] Logical Code -> Code Generator -> machine code\n";
    CodeGenIR backend;
    Program   prog = backend.generate(ir);
    Optimizer().optimize(prog);

    Assembler assembler;
    std::cout << "\n--- Assembler listing ---\n";
    std::cout << assembler.disassemble(prog);

    std::cout << "\n[5] Binary EXEC file (header, sections, sym, reloc, jump table)\n";
    if (ExecIO::write(execPath, prog))
        std::cout << "Wrote: " << execPath << "\n";
    else
        std::cerr << "Failed to write EXEC: " << execPath << "\n";

    VM       vm;
    Loader   loader;
    Debugger dbg;

    std::cout << "\n[6] Loader + VM monitor\n";
    if (useExec) {
        if (!loader.loadExec(execPath, vm)) {
            std::cerr << "Cannot load EXEC: " << execPath << "\n";
            return 1;
        }
    } else {
        loader.load(vm);
        vm.loadProgram(prog.code, prog.constPool, prog.jumpTables);
    }

    vm.setXLen(XLen::W64);
    std::cout << "Segments: Code[" << VM::CODE_BASE
              << "] Data["         << VM::DATA_BASE
              << "] Heap["         << VM::HEAP_BASE
              << "] Stack["        << VM::STACK_TOP << "]\n";

    dbg.dumpState(vm);
    if (debugMode) {
        dbg.monitor(vm);
    } else {
        vm.run();
        std::cout << "\nResult R0 = " << vm.regs[0] << " (expected 15)\n";
        dbg.dumpRegisters(vm);
    }
    return 0;
}
