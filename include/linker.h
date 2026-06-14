#pragma once

#include "Program.h"

// Resolves symbolic references into concrete addresses.
//
// The code generator records each function call as a relocation (an instruction
// index plus the target function name) and each function as a symbol (name plus
// address). The linker walks the relocation table, looks up every target in the
// symbol table, and patches the real address into the call instruction. After
// link() succeeds the program contains no unresolved references and is ready to
// run or to be written to disk.
class Linker {
public:
    // Patch every relocation in-place. Returns true if all targets resolved;
    // returns false (and leaves the offending instruction unpatched) if a
    // relocation names a symbol that is not in the symbol table.
    bool link(Program& prog) const;

    // Number of relocations that could not be resolved on the last link().
    int  unresolvedCount() const { return unresolved_; }

private:
    mutable int unresolved_ = 0;
};
