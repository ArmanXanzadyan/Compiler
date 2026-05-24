#pragma once

#include "Program.h"
#include "vm.h"

#include <string>

// Read/write the simple ELF-ish EXEC container the toy compiler produces.
class ExecIO {
public:
    static bool write(const std::string& path, const Program& prog);
    static bool load(const std::string& path, VM& vm);
};
