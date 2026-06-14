#include "linker.h"

#include <map>
#include <string>

bool Linker::link(Program& prog) const {
    unresolved_ = 0;

    // Build a name -> address index from the symbol table for O(log n) lookups.
    std::map<std::string, uint32_t> symbolAddr;
    for (const auto& s : prog.symbols)
        symbolAddr[s.name] = s.address;

    // Patch each call site with the resolved target address.
    for (const auto& r : prog.relocs) {
        auto it = symbolAddr.find(r.targetName);
        if (it == symbolAddr.end()) {
            ++unresolved_;
            continue;
        }
        if (r.instrIndex < prog.code.size())
            prog.code[r.instrIndex].rs1 = it->second;
    }

    return unresolved_ == 0;
}
