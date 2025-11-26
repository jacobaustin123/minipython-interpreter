#ifndef AST_HPP
#define AST_HPP

#include <memory>
#include <vector>
#include <string>
#include <variant>
#include "token.hpp"

// Forward declarations
struct BinaryExpr;
struct UnaryExpr;
struct LiteralExpr;
struct VariableExpr;
struct AssignExpr;
struct CallExpr;
struct GroupingExpr;

struct ExpressionStmt;
struct PrintStmt;
struct VarStmt;
struct BlockStmt;
struct IfStmt;
struct WhileStmt;
struct FunctionStmt;
struct ReturnStmt;
struct AssertStmt;

// Expression variant
using Expr = std::variant<
    std::unique_ptr<BinaryExpr>,
    std::unique_ptr<UnaryExpr>,
    std::unique_ptr<LiteralExpr>,
    std::unique_ptr<VariableExpr>,
    std::unique_ptr<AssignExpr>,
    std::unique_ptr<CallExpr>,
    std::unique_ptr<GroupingExpr>
>;

// Statement variant
using Stmt = std::variant<
    std::unique_ptr<ExpressionStmt>,
    std::unique_ptr<PrintStmt>,
    std::unique_ptr<VarStmt>,
    std::unique_ptr<BlockStmt>,
    std::unique_ptr<IfStmt>,
    std::unique_ptr<WhileStmt>,
    std::unique_ptr<FunctionStmt>,
    std::unique_ptr<ReturnStmt>,
    std::unique_ptr<AssertStmt>
>;

// Python value type
struct PyNone {};
struct PyFunction;

using PyValue = std::variant<
    PyNone,
    bool,
    long long,
    double,
    std::string,
    std::shared_ptr<PyFunction>
>;

// Expression nodes
struct BinaryExpr {
    Expr left;
    Token op;
    Expr right;

    BinaryExpr(Expr left, Token op, Expr right)
        : left(std::move(left)), op(std::move(op)), right(std::move(right)) {}
};

struct UnaryExpr {
    Token op;
    Expr operand;

    UnaryExpr(Token op, Expr operand)
        : op(std::move(op)), operand(std::move(operand)) {}
};

struct LiteralExpr {
    PyValue value;

    explicit LiteralExpr(PyValue value) : value(std::move(value)) {}
};

struct VariableExpr {
    Token name;

    explicit VariableExpr(Token name) : name(std::move(name)) {}
};

struct AssignExpr {
    Token name;
    Expr value;

    AssignExpr(Token name, Expr value)
        : name(std::move(name)), value(std::move(value)) {}
};

struct CallExpr {
    Expr callee;
    Token paren;  // For error reporting
    std::vector<Expr> arguments;

    CallExpr(Expr callee, Token paren, std::vector<Expr> arguments)
        : callee(std::move(callee)), paren(std::move(paren)), arguments(std::move(arguments)) {}
};

struct GroupingExpr {
    Expr expression;

    explicit GroupingExpr(Expr expression) : expression(std::move(expression)) {}
};

// Statement nodes
struct ExpressionStmt {
    Expr expression;

    explicit ExpressionStmt(Expr expression) : expression(std::move(expression)) {}
};

struct PrintStmt {
    std::vector<Expr> expressions;

    explicit PrintStmt(std::vector<Expr> expressions) : expressions(std::move(expressions)) {}
};

struct VarStmt {
    Token name;
    Expr initializer;

    VarStmt(Token name, Expr initializer)
        : name(std::move(name)), initializer(std::move(initializer)) {}
};

struct BlockStmt {
    std::vector<Stmt> statements;

    explicit BlockStmt(std::vector<Stmt> statements) : statements(std::move(statements)) {}
};

struct IfStmt {
    Expr condition;
    Stmt thenBranch;
    std::vector<std::pair<Expr, Stmt>> elifBranches;
    std::unique_ptr<Stmt> elseBranch;

    IfStmt(Expr condition, Stmt thenBranch,
           std::vector<std::pair<Expr, Stmt>> elifBranches,
           std::unique_ptr<Stmt> elseBranch)
        : condition(std::move(condition)), thenBranch(std::move(thenBranch)),
          elifBranches(std::move(elifBranches)), elseBranch(std::move(elseBranch)) {}
};

struct WhileStmt {
    Expr condition;
    Stmt body;

    WhileStmt(Expr condition, Stmt body)
        : condition(std::move(condition)), body(std::move(body)) {}
};

struct FunctionStmt {
    Token name;
    std::vector<Token> params;
    std::vector<Stmt> body;

    FunctionStmt(Token name, std::vector<Token> params, std::vector<Stmt> body)
        : name(std::move(name)), params(std::move(params)), body(std::move(body)) {}
};

struct ReturnStmt {
    Token keyword;
    std::unique_ptr<Expr> value;

    ReturnStmt(Token keyword, std::unique_ptr<Expr> value)
        : keyword(std::move(keyword)), value(std::move(value)) {}
};

struct AssertStmt {
    Token keyword;
    Expr condition;
    std::unique_ptr<Expr> message;  // Optional message

    AssertStmt(Token keyword, Expr condition, std::unique_ptr<Expr> message = nullptr)
        : keyword(std::move(keyword)), condition(std::move(condition)), message(std::move(message)) {}
};

// Function definition for runtime
struct PyFunction {
    std::string name;
    std::vector<std::string> params;
    const FunctionStmt* declaration;  // Points to the AST node

    PyFunction(std::string name, std::vector<std::string> params, const FunctionStmt* declaration)
        : name(std::move(name)), params(std::move(params)), declaration(declaration) {}
};

// Helper to convert PyValue to string
inline std::string pyValueToString(const PyValue& value) {
    return std::visit([](auto&& arg) -> std::string {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, PyNone>) {
            return "None";
        } else if constexpr (std::is_same_v<T, bool>) {
            return arg ? "True" : "False";
        } else if constexpr (std::is_same_v<T, long long>) {
            return std::to_string(arg);
        } else if constexpr (std::is_same_v<T, double>) {
            std::string s = std::to_string(arg);
            // Remove trailing zeros
            size_t dot = s.find('.');
            if (dot != std::string::npos) {
                size_t last = s.find_last_not_of('0');
                if (last > dot) {
                    s = s.substr(0, last + 1);
                } else {
                    s = s.substr(0, dot + 2);
                }
            }
            return s;
        } else if constexpr (std::is_same_v<T, std::string>) {
            return arg;
        } else if constexpr (std::is_same_v<T, std::shared_ptr<PyFunction>>) {
            return "<function " + arg->name + ">";
        }
    }, value);
}

// Helper to check truthiness
inline bool isTruthy(const PyValue& value) {
    return std::visit([](auto&& arg) -> bool {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, PyNone>) {
            return false;
        } else if constexpr (std::is_same_v<T, bool>) {
            return arg;
        } else if constexpr (std::is_same_v<T, long long>) {
            return arg != 0;
        } else if constexpr (std::is_same_v<T, double>) {
            return arg != 0.0;
        } else if constexpr (std::is_same_v<T, std::string>) {
            return !arg.empty();
        } else if constexpr (std::is_same_v<T, std::shared_ptr<PyFunction>>) {
            return true;
        }
    }, value);
}

#endif // AST_HPP
