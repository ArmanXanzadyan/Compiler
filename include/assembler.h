#pragma once

#include "Program.h"

#include <string>

// Render a Program as text and parse simple textual assembly back into a Program.
class Assembler {
public:
    std::string disassemble(const Program& prog) const;
    Program     assembleSource(const std::string& asmText) const;
};
