#include "interpreter.hpp"
#include <cmath>
#include <sstream>

Interpreter::Interpreter() {
    globalEnv = std::make_shared<Environment>();
    currentEnv = globalEnv;
}

void Interpreter::interpret(std::vector<Stmt> statements) {
    // Store statements to keep AST alive (for function bodies)
    storedStatements.push_back(std::move(statements));

    // Execute from the stored copy
    for (const auto& stmt : storedStatements.back()) {
        execute(stmt);
    }
}

PyValue Interpreter::evaluate(const Expr& expr) {
    return std::visit([this](auto&& arg) -> PyValue {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::unique_ptr<BinaryExpr>>) {
            return visitBinaryExpr(*arg);
        } else if constexpr (std::is_same_v<T, std::unique_ptr<UnaryExpr>>) {
            return visitUnaryExpr(*arg);
        } else if constexpr (std::is_same_v<T, std::unique_ptr<LiteralExpr>>) {
            return visitLiteralExpr(*arg);
        } else if constexpr (std::is_same_v<T, std::unique_ptr<VariableExpr>>) {
            return visitVariableExpr(*arg);
        } else if constexpr (std::is_same_v<T, std::unique_ptr<AssignExpr>>) {
            return visitAssignExpr(*arg);
        } else if constexpr (std::is_same_v<T, std::unique_ptr<CallExpr>>) {
            return visitCallExpr(*arg);
        } else if constexpr (std::is_same_v<T, std::unique_ptr<GroupingExpr>>) {
            return visitGroupingExpr(*arg);
        }
    }, expr);
}

void Interpreter::execute(const Stmt& stmt) {
    std::visit([this](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::unique_ptr<ExpressionStmt>>) {
            visitExpressionStmt(*arg);
        } else if constexpr (std::is_same_v<T, std::unique_ptr<PrintStmt>>) {
            visitPrintStmt(*arg);
        } else if constexpr (std::is_same_v<T, std::unique_ptr<VarStmt>>) {
            visitVarStmt(*arg);
        } else if constexpr (std::is_same_v<T, std::unique_ptr<BlockStmt>>) {
            visitBlockStmt(*arg);
        } else if constexpr (std::is_same_v<T, std::unique_ptr<IfStmt>>) {
            visitIfStmt(*arg);
        } else if constexpr (std::is_same_v<T, std::unique_ptr<WhileStmt>>) {
            visitWhileStmt(*arg);
        } else if constexpr (std::is_same_v<T, std::unique_ptr<FunctionStmt>>) {
            visitFunctionStmt(*arg);
        } else if constexpr (std::is_same_v<T, std::unique_ptr<ReturnStmt>>) {
            visitReturnStmt(*arg);
        } else if constexpr (std::is_same_v<T, std::unique_ptr<AssertStmt>>) {
            visitAssertStmt(*arg);
        }
    }, stmt);
}

// Expression visitors

PyValue Interpreter::visitBinaryExpr(const BinaryExpr& expr) {
    PyValue left = evaluate(expr.left);
    PyValue right = evaluate(expr.right);

    switch (expr.op.type) {
        case TokenType::PLUS: {
            // Handle string concatenation
            if (std::holds_alternative<std::string>(left) &&
                std::holds_alternative<std::string>(right)) {
                return std::get<std::string>(left) + std::get<std::string>(right);
            }

            // Numeric addition
            if (std::holds_alternative<double>(left) ||
                std::holds_alternative<double>(right)) {
                double l = std::holds_alternative<double>(left)
                    ? std::get<double>(left)
                    : static_cast<double>(std::get<long long>(left));
                double r = std::holds_alternative<double>(right)
                    ? std::get<double>(right)
                    : static_cast<double>(std::get<long long>(right));
                return l + r;
            }

            if (std::holds_alternative<long long>(left) &&
                std::holds_alternative<long long>(right)) {
                return std::get<long long>(left) + std::get<long long>(right);
            }

            throw RuntimeError("Operands must be numbers or strings", expr.op.line);
        }

        case TokenType::MINUS: {
            if (std::holds_alternative<double>(left) ||
                std::holds_alternative<double>(right)) {
                double l = std::holds_alternative<double>(left)
                    ? std::get<double>(left)
                    : static_cast<double>(std::get<long long>(left));
                double r = std::holds_alternative<double>(right)
                    ? std::get<double>(right)
                    : static_cast<double>(std::get<long long>(right));
                return l - r;
            }

            if (std::holds_alternative<long long>(left) &&
                std::holds_alternative<long long>(right)) {
                return std::get<long long>(left) - std::get<long long>(right);
            }

            throw RuntimeError("Operands must be numbers", expr.op.line);
        }

        case TokenType::STAR: {
            // Handle string repetition
            if (std::holds_alternative<std::string>(left) &&
                std::holds_alternative<long long>(right)) {
                std::string result;
                long long times = std::get<long long>(right);
                for (long long i = 0; i < times; i++) {
                    result += std::get<std::string>(left);
                }
                return result;
            }

            if (std::holds_alternative<double>(left) ||
                std::holds_alternative<double>(right)) {
                double l = std::holds_alternative<double>(left)
                    ? std::get<double>(left)
                    : static_cast<double>(std::get<long long>(left));
                double r = std::holds_alternative<double>(right)
                    ? std::get<double>(right)
                    : static_cast<double>(std::get<long long>(right));
                return l * r;
            }

            if (std::holds_alternative<long long>(left) &&
                std::holds_alternative<long long>(right)) {
                return std::get<long long>(left) * std::get<long long>(right);
            }

            throw RuntimeError("Operands must be numbers", expr.op.line);
        }

        case TokenType::SLASH: {
            double l = std::holds_alternative<double>(left)
                ? std::get<double>(left)
                : static_cast<double>(std::get<long long>(left));
            double r = std::holds_alternative<double>(right)
                ? std::get<double>(right)
                : static_cast<double>(std::get<long long>(right));

            if (r == 0.0) {
                throw RuntimeError("Division by zero", expr.op.line);
            }

            return l / r;
        }

        case TokenType::DOUBLE_SLASH: {
            double l = std::holds_alternative<double>(left)
                ? std::get<double>(left)
                : static_cast<double>(std::get<long long>(left));
            double r = std::holds_alternative<double>(right)
                ? std::get<double>(right)
                : static_cast<double>(std::get<long long>(right));

            if (r == 0.0) {
                throw RuntimeError("Division by zero", expr.op.line);
            }

            double result = std::floor(l / r);

            // If both operands were integers, return integer
            if (std::holds_alternative<long long>(left) &&
                std::holds_alternative<long long>(right)) {
                return static_cast<long long>(result);
            }

            return result;
        }

        case TokenType::PERCENT: {
            if (std::holds_alternative<long long>(left) &&
                std::holds_alternative<long long>(right)) {
                long long r = std::get<long long>(right);
                if (r == 0) {
                    throw RuntimeError("Modulo by zero", expr.op.line);
                }
                return std::get<long long>(left) % r;
            }

            double l = std::holds_alternative<double>(left)
                ? std::get<double>(left)
                : static_cast<double>(std::get<long long>(left));
            double r = std::holds_alternative<double>(right)
                ? std::get<double>(right)
                : static_cast<double>(std::get<long long>(right));

            if (r == 0.0) {
                throw RuntimeError("Modulo by zero", expr.op.line);
            }

            return std::fmod(l, r);
        }

        case TokenType::DOUBLE_STAR: {
            double l = std::holds_alternative<double>(left)
                ? std::get<double>(left)
                : static_cast<double>(std::get<long long>(left));
            double r = std::holds_alternative<double>(right)
                ? std::get<double>(right)
                : static_cast<double>(std::get<long long>(right));

            double result = std::pow(l, r);

            // Return integer if both operands were integers and result fits
            if (std::holds_alternative<long long>(left) &&
                std::holds_alternative<long long>(right) &&
                std::get<long long>(right) >= 0 &&
                result == std::floor(result)) {
                return static_cast<long long>(result);
            }

            return result;
        }

        case TokenType::EQ: {
            // Handle equality comparison
            if (std::holds_alternative<PyNone>(left) && std::holds_alternative<PyNone>(right)) {
                return true;
            }
            if (std::holds_alternative<bool>(left) && std::holds_alternative<bool>(right)) {
                return std::get<bool>(left) == std::get<bool>(right);
            }
            if (std::holds_alternative<std::string>(left) && std::holds_alternative<std::string>(right)) {
                return std::get<std::string>(left) == std::get<std::string>(right);
            }
            if (std::holds_alternative<std::shared_ptr<PyFunction>>(left) &&
                std::holds_alternative<std::shared_ptr<PyFunction>>(right)) {
                return std::get<std::shared_ptr<PyFunction>>(left).get() ==
                       std::get<std::shared_ptr<PyFunction>>(right).get();
            }
            // Numeric comparison
            if ((std::holds_alternative<long long>(left) || std::holds_alternative<double>(left)) &&
                (std::holds_alternative<long long>(right) || std::holds_alternative<double>(right))) {
                double l = std::holds_alternative<double>(left)
                    ? std::get<double>(left)
                    : static_cast<double>(std::get<long long>(left));
                double r = std::holds_alternative<double>(right)
                    ? std::get<double>(right)
                    : static_cast<double>(std::get<long long>(right));
                return l == r;
            }
            return false;
        }

        case TokenType::NE: {
            // Handle inequality - just negate equality
            if (std::holds_alternative<PyNone>(left) && std::holds_alternative<PyNone>(right)) {
                return false;
            }
            if (std::holds_alternative<bool>(left) && std::holds_alternative<bool>(right)) {
                return std::get<bool>(left) != std::get<bool>(right);
            }
            if (std::holds_alternative<std::string>(left) && std::holds_alternative<std::string>(right)) {
                return std::get<std::string>(left) != std::get<std::string>(right);
            }
            if (std::holds_alternative<std::shared_ptr<PyFunction>>(left) &&
                std::holds_alternative<std::shared_ptr<PyFunction>>(right)) {
                return std::get<std::shared_ptr<PyFunction>>(left).get() !=
                       std::get<std::shared_ptr<PyFunction>>(right).get();
            }
            // Numeric comparison
            if ((std::holds_alternative<long long>(left) || std::holds_alternative<double>(left)) &&
                (std::holds_alternative<long long>(right) || std::holds_alternative<double>(right))) {
                double l = std::holds_alternative<double>(left)
                    ? std::get<double>(left)
                    : static_cast<double>(std::get<long long>(left));
                double r = std::holds_alternative<double>(right)
                    ? std::get<double>(right)
                    : static_cast<double>(std::get<long long>(right));
                return l != r;
            }
            return true;  // Different types are not equal
        }

        case TokenType::LT:
        case TokenType::LE:
        case TokenType::GT:
        case TokenType::GE: {
            double l, r;

            if (std::holds_alternative<long long>(left)) {
                l = static_cast<double>(std::get<long long>(left));
            } else if (std::holds_alternative<double>(left)) {
                l = std::get<double>(left);
            } else {
                throw RuntimeError("Operands must be numbers", expr.op.line);
            }

            if (std::holds_alternative<long long>(right)) {
                r = static_cast<double>(std::get<long long>(right));
            } else if (std::holds_alternative<double>(right)) {
                r = std::get<double>(right);
            } else {
                throw RuntimeError("Operands must be numbers", expr.op.line);
            }

            switch (expr.op.type) {
                case TokenType::LT: return l < r;
                case TokenType::LE: return l <= r;
                case TokenType::GT: return l > r;
                case TokenType::GE: return l >= r;
                default: return false;
            }
        }

        case TokenType::AND:
            return isTruthy(left) ? right : left;

        case TokenType::OR:
            return isTruthy(left) ? left : right;

        default:
            throw RuntimeError("Unknown binary operator", expr.op.line);
    }
}

PyValue Interpreter::visitUnaryExpr(const UnaryExpr& expr) {
    PyValue operand = evaluate(expr.operand);

    switch (expr.op.type) {
        case TokenType::MINUS:
            if (std::holds_alternative<long long>(operand)) {
                return -std::get<long long>(operand);
            }
            if (std::holds_alternative<double>(operand)) {
                return -std::get<double>(operand);
            }
            throw RuntimeError("Operand must be a number", expr.op.line);

        case TokenType::NOT:
            return !isTruthy(operand);

        default:
            throw RuntimeError("Unknown unary operator", expr.op.line);
    }
}

PyValue Interpreter::visitLiteralExpr(const LiteralExpr& expr) {
    return expr.value;
}

PyValue Interpreter::visitVariableExpr(const VariableExpr& expr) {
    return currentEnv->get(expr.name.lexeme);
}

PyValue Interpreter::visitAssignExpr(const AssignExpr& expr) {
    PyValue value = evaluate(expr.value);
    currentEnv->assign(expr.name.lexeme, value);
    return value;
}

PyValue Interpreter::visitCallExpr(const CallExpr& expr) {
    PyValue callee = evaluate(expr.callee);

    std::vector<PyValue> arguments;
    for (const auto& arg : expr.arguments) {
        arguments.push_back(evaluate(arg));
    }

    if (!std::holds_alternative<std::shared_ptr<PyFunction>>(callee)) {
        throw RuntimeError("Can only call functions", expr.paren.line);
    }

    auto function = std::get<std::shared_ptr<PyFunction>>(callee);
    return callFunction(function, arguments, expr.paren);
}

PyValue Interpreter::visitGroupingExpr(const GroupingExpr& expr) {
    return evaluate(expr.expression);
}

// Statement visitors

void Interpreter::visitExpressionStmt(const ExpressionStmt& stmt) {
    lastValue = evaluate(stmt.expression);
    lastValueSet = true;
}

void Interpreter::visitPrintStmt(const PrintStmt& stmt) {
    bool first = true;
    for (const auto& expr : stmt.expressions) {
        if (!first) std::cout << " ";
        first = false;

        PyValue value = evaluate(expr);
        std::cout << pyValueToString(value);
    }
    std::cout << std::endl;
    lastValueSet = false;
}

void Interpreter::visitVarStmt(const VarStmt& stmt) {
    PyValue value = evaluate(stmt.initializer);
    currentEnv->define(stmt.name.lexeme, value);
    lastValueSet = false;
}

void Interpreter::visitBlockStmt(const BlockStmt& stmt) {
    executeBlock(stmt.statements, std::make_shared<Environment>(currentEnv));
}

void Interpreter::visitIfStmt(const IfStmt& stmt) {
    if (isTruthy(evaluate(stmt.condition))) {
        execute(stmt.thenBranch);
    } else {
        // Check elif branches
        for (const auto& [condition, branch] : stmt.elifBranches) {
            if (isTruthy(evaluate(condition))) {
                execute(branch);
                return;
            }
        }

        // Execute else branch if present
        if (stmt.elseBranch) {
            execute(*stmt.elseBranch);
        }
    }
}

void Interpreter::visitWhileStmt(const WhileStmt& stmt) {
    while (isTruthy(evaluate(stmt.condition))) {
        execute(stmt.body);
    }
}

void Interpreter::visitFunctionStmt(const FunctionStmt& stmt) {
    std::vector<std::string> paramNames;
    for (const auto& param : stmt.params) {
        paramNames.push_back(param.lexeme);
    }

    auto function = std::make_shared<PyFunction>(
        stmt.name.lexeme,
        paramNames,
        &stmt  // Store pointer to the AST node
    );

    // Store the function in the environment
    currentEnv->define(stmt.name.lexeme, function);
    lastValueSet = false;
}

void Interpreter::visitReturnStmt(const ReturnStmt& stmt) {
    PyValue value = PyNone{};
    if (stmt.value) {
        value = evaluate(*stmt.value);
    }
    throw ReturnException(value);
}

void Interpreter::visitAssertStmt(const AssertStmt& stmt) {
    PyValue condition = evaluate(stmt.condition);

    if (!isTruthy(condition)) {
        std::string message = "AssertionError";
        if (stmt.message) {
            PyValue msgValue = evaluate(*stmt.message);
            message = "AssertionError: " + pyValueToString(msgValue);
        }
        throw AssertionError(message, stmt.keyword.line);
    }
    lastValueSet = false;
}

void Interpreter::executeBlock(const std::vector<Stmt>& statements,
                                std::shared_ptr<Environment> env) {
    auto previous = currentEnv;
    currentEnv = env;

    try {
        for (const auto& stmt : statements) {
            execute(stmt);
        }
    } catch (...) {
        currentEnv = previous;
        throw;
    }

    currentEnv = previous;
}

PyValue Interpreter::callFunction(std::shared_ptr<PyFunction> function,
                                   const std::vector<PyValue>& arguments,
                                   const Token& paren) {
    if (arguments.size() != function->params.size()) {
        std::ostringstream oss;
        oss << "Expected " << function->params.size()
            << " arguments but got " << arguments.size();
        throw RuntimeError(oss.str(), paren.line);
    }

    auto env = std::make_shared<Environment>(globalEnv);

    for (size_t i = 0; i < function->params.size(); i++) {
        env->define(function->params[i], arguments[i]);
    }

    try {
        executeBlock(function->declaration->body, env);
    } catch (const ReturnException& ret) {
        return ret.value;
    }

    return PyNone{};
}
