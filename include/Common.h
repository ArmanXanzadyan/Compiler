#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

// ---------------------------------------------------------------------------
//  Lexer tokens
// ---------------------------------------------------------------------------
enum class TokenType {
    NUMBER, VARIABLE, STRING_LIT,
    PLUS, MINUS, STAR, SLASH, PERCENT,
    ASSIGN,
    EQ, NEQ, LT, GT, LTE, GTE,
    AND, OR, NOT,
    LPAREN, RPAREN, LBRACE, RBRACE, LBRACKET, RBRACKET,
    SEMICOLON, COMMA, DOT,
    IF, ELSE, WHILE, DO, FOR, RETURN, BREAK, CONTINUE,
    INT_KW, FLOAT_KW, VOID_KW,
    SWITCH, CASE, DEFAULT,
    END_OF_FILE
};

struct Token {
    TokenType   type = TokenType::END_OF_FILE;
    std::string value;
    int         line = 0;

    Token() = default;
    Token(TokenType t, std::string v, int ln = 0)
        : type(t), value(std::move(v)), line(ln) {}
};

// ---------------------------------------------------------------------------
//  AST
// ---------------------------------------------------------------------------
enum class NodeKind {
    // Expressions
    NUM_LIT, VAR_REF, BINOP, UNOP, ASSIGN_EXPR, CALL,
    // Statements
    EXPR_STMT, IF_STMT, WHILE_STMT, DO_WHILE_STMT,
    FOR_STMT, RETURN_STMT, BREAK_STMT, CONTINUE_STMT,
    BLOCK,
    // Declarations
    VAR_DECL, FUNC_DECL, PROGRAM,
    // Switch
    SWITCH_STMT, CASE_CLAUSE, DEFAULT_CLAUSE
};

struct ASTNode {
    NodeKind    kind = NodeKind::PROGRAM;
    std::string text;        // operator, identifier, literal text
    double      numVal = 0;  // value when kind == NUM_LIT

    std::vector<std::unique_ptr<ASTNode>> children;

    explicit ASTNode(NodeKind k, std::string t = "")
        : kind(k), text(std::move(t)) {}
    explicit ASTNode(double v) : kind(NodeKind::NUM_LIT), numVal(v) {}
};

// ---------------------------------------------------------------------------
//  Virtual ISA (RISC-V-inspired teaching instruction set)
// ---------------------------------------------------------------------------
enum class OpCode : uint32_t {
    MOV_CONST = 0, LOAD_VAR, STORE_VAR,
    ADD, SUB, MUL, DIV, MOD,
    CMP,
    BR_EQ, BR_NEQ, BR_LT, BR_GT, BR_LTE, BR_GTE,
    JMP,
    CALL, RET,
    PUSH, POP,
    LOAD_MEM, STORE_MEM,
    PUSH_BP, SET_BP_SP, ALLOC_STACK, RESTORE_BP,
    LOAD_LOCAL, STORE_LOCAL,
    JMP_TABLE,
    HEAP_ALLOC,
    HALT
};

// A fixed 4-byte instruction word (5 + 9 + 9 + 9 = 32 bits).
struct Instruction {
    uint32_t op : 5;
    uint32_t rd : 9;   // destination register
    uint32_t rs1 : 9;   // source 1
    uint32_t rs2 : 9;   // source 2
};
static_assert(sizeof(Instruction) == 4, "Instruction must be 4 bytes");

// ---------------------------------------------------------------------------
//  EXEC file layout (on-disk structures shared by ExecIO and tooling)
// ---------------------------------------------------------------------------
struct SectionHeader {
    uint32_t type;    // 0=Code, 1=Data, 2=SymbolTable
    uint32_t size;
    uint32_t offset;
};

struct SymbolEntry {
    char     name[32];
    uint32_t address;
    uint8_t  type;   // 0=func 1=var
};

struct RelocEntry {
    uint32_t instrOffset;
    uint32_t symbolIndex;
};
