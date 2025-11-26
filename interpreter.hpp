#ifndef INTERPRETER_HPP
#define INTERPRETER_HPP

#include <memory>
#include <vector>
#include <iostream>
#include "ast.hpp"
#include "environment.hpp"

// Exception for return statements
class ReturnException : public std::exception {
public:
    PyValue value;
    explicit ReturnException(PyValue value) : value(std::move(value)) {}
};

class Interpreter {
public:
    Interpreter();

    void interpret(std::vector<Stmt> statements);
    PyValue evaluate(const Expr& expr);
    void execute(const Stmt& stmt);

    // For REPL: get the value of the last expression
    bool hasLastValue() const { return lastValueSet; }
    PyValue getLastValue() const { return lastValue; }
    void clearLastValue() { lastValueSet = false; }

private:
    std::shared_ptr<Environment> globalEnv;
    std::shared_ptr<Environment> currentEnv;
    PyValue lastValue;
    bool lastValueSet = false;

    // Store AST to keep function bodies alive
    std::vector<std::vector<Stmt>> storedStatements;

    // Expression evaluation
    PyValue visitBinaryExpr(const BinaryExpr& expr);
    PyValue visitUnaryExpr(const UnaryExpr& expr);
    PyValue visitLiteralExpr(const LiteralExpr& expr);
    PyValue visitVariableExpr(const VariableExpr& expr);
    PyValue visitAssignExpr(const AssignExpr& expr);
    PyValue visitCallExpr(const CallExpr& expr);
    PyValue visitGroupingExpr(const GroupingExpr& expr);

    // Statement execution
    void visitExpressionStmt(const ExpressionStmt& stmt);
    void visitPrintStmt(const PrintStmt& stmt);
    void visitVarStmt(const VarStmt& stmt);
    void visitBlockStmt(const BlockStmt& stmt);
    void visitIfStmt(const IfStmt& stmt);
    void visitWhileStmt(const WhileStmt& stmt);
    void visitFunctionStmt(const FunctionStmt& stmt);
    void visitReturnStmt(const ReturnStmt& stmt);
    void visitAssertStmt(const AssertStmt& stmt);

    // Helpers
    void executeBlock(const std::vector<Stmt>& statements,
                      std::shared_ptr<Environment> env);
    PyValue callFunction(std::shared_ptr<PyFunction> function,
                         const std::vector<PyValue>& arguments,
                         const Token& paren);
};

#endif // INTERPRETER_HPP
