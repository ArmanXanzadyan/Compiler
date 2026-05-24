#include "optimizer.h"

#include <cstddef>

void Optimizer::optimize(Program& prog) const {
    // Drop the first of two consecutive MOV_CONSTs that target the same
    // register: the second write makes the first dead.
    for (size_t i = 0; i + 1 < prog.code.size(); ) {
        const Instruction& a = prog.code[i];
        const Instruction& b = prog.code[i + 1];
        const bool deadStore =
            (OpCode)a.op == OpCode::MOV_CONST &&
            (OpCode)b.op == OpCode::MOV_CONST &&
            a.rd == b.rd;
        if (deadStore) {
            prog.code.erase(prog.code.begin() + (ptrdiff_t)i);
        } else {
            ++i;
        }
    }
}
