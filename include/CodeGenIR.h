#pragma once

#include "Program.h"
#include "ir.h"

#include <map>
#include <string>

// Lowers linear IR (after LogicalCode) into the VM's binary instruction set.
class CodeGenIR {
public:
    Program generate(const IRProgram& ir);

private:
    Program                         prog_;
    std::map<std::string, int>      vars_;
    std::map<std::string, uint32_t> labels_;
    std::map<std::string, uint32_t> funcAddrs_;
    int                             nextReg_ = 1;

    int      regOf(const std::string& name);
    void     emit(Instruction i);
    uint32_t here() const;
    int      newConst(double v);
};
