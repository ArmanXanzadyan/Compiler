#pragma once

#include "Program.h"

// Tiny peephole pass: collapses runs of MOV_CONST writing the same register.
class Optimizer {
public:
    void optimize(Program& prog) const;
};
