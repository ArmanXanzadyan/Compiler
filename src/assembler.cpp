#include "assembler.h"

#include <map>
#include <sstream>

std::string Assembler::disassemble(const Program& prog) const {
    std::ostringstream out;
    for (size_t i = 0; i < prog.code.size(); ++i) {
        const Instruction& in = prog.code[i];
        const char* name = in.op < opCodeNameCount ? opCodeNames[in.op] : "?";
        out << i << ": " << name
            << " rd="  << in.rd
            << " rs1=" << in.rs1
            << " rs2=" << in.rs2 << "\n";
    }
    return out.str();
}

static std::map<std::string, OpCode> mnemonicMap() {
    std::map<std::string, OpCode> m;
    for (size_t i = 0; i < opCodeNameCount; ++i)
        m[opCodeNames[i]] = (OpCode)i;
    return m;
}

Program Assembler::assembleSource(const std::string& asmText) const {
    const auto mnem = mnemonicMap();
    Program prog;
    std::istringstream in(asmText);
    std::string line;
    while (std::getline(in, line)) {
        if (line.empty() || line[0] == ';') continue;
        std::istringstream ls(line);
        std::string op, a, b, c;
        ls >> op >> a >> b >> c;
        auto it = mnem.find(op);
        if (it == mnem.end()) continue;
        Instruction ins{};
        ins.op = (uint32_t)it->second;
        try {
            ins.rd  = a.empty() ? 0 : (uint32_t)std::stoul(a);
            ins.rs1 = b.empty() ? 0 : (uint32_t)std::stoul(b);
            ins.rs2 = c.empty() ? 0 : (uint32_t)std::stoul(c);
        } catch (...) { continue; }
        prog.code.push_back(ins);
    }
    return prog;
}
