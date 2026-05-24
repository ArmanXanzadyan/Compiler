#pragma once

#include "Common.h"
#include "ir.h"

#include <map>
#include <string>
#include <vector>

// Lowers an AST into linear three-address IR.
class IRGen {
public:
    IRProgram generate(const ASTNode* prog);

private:
    IRProgram ir_;
    int       tmp_   = 0;
    int       label_ = 0;

    std::vector<std::map<std::string, std::string>> scopes_;
    std::map<std::string, std::string>              globals_;

    std::string fresh();
    std::string label();
    void        emit(IRInstruction i);

    void        emitDecl(const ASTNode* n);
    void        emitStmt(const ASTNode* n);
    std::string emitExpr(const ASTNode* n);
};
