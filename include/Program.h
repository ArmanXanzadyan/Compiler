#pragma once

#include "Common.h"

#include <cstdint>
#include <string>
#include <vector>

// A fully lowered program ready to be executed by the VM or written to disk.
// Produced by CodeGenIR, consumed by Optimizer, Assembler, ExecIO and VM.
struct Program {
    struct Symbol {
        std::string name;
        uint32_t    address = 0;
        uint8_t     type    = 0;  // 0 = function, 1 = variable
    };

    struct Reloc {
        uint32_t    instrIndex = 0;
        std::string targetName;
    };

    struct JumpTable {
        int32_t               minVal    = 0;
        uint32_t              tableId   = 0;
        uint32_t              switchReg = 0;
        std::vector<uint32_t> targets;
    };

    std::vector<Instruction> code;
    std::vector<double>      constPool;
    std::vector<std::string> strings;
    std::vector<Symbol>      symbols;
    std::vector<Reloc>       relocs;
    std::vector<JumpTable>   jumpTables;
};

// Pretty-printable names for OpCode values; defined in src/opcodes.cpp.
extern const char* const opCodeNames[];
constexpr size_t opCodeNameCount = 31;
