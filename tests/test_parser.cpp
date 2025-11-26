#include <iostream>
#include <cassert>
#include <vector>
#include <string>
#include "../lexer.hpp"
#include "../parser.hpp"

// Simple test framework
#define TEST(name) void test_##name()
#define RUN_TEST(name) do { \
    std::cout << "  " << #name << "... "; \
    test_##name(); \
    std::cout << "PASS" << std::endl; \
} while(0)

#define ASSERT_EQ(a, b) do { \
    if ((a) != (b)) { \
        std::cerr << "FAIL at line " << __LINE__ << std::endl; \
        assert(false); \
    } \
} while(0)

#define ASSERT_TRUE(x) assert(x)
#define ASSERT_FALSE(x) assert(!(x))

// Helper to parse source code
std::vector<Stmt> parse(const std::string& source) {
    Lexer lexer(source);
    auto tokens = lexer.tokenize();
    Parser parser(tokens);
    return parser.parse();
}

// Helper to check if parsing succeeds
bool parses(const std::string& source) {
    try {
        parse(source);
        return true;
    } catch (...) {
        return false;
    }
}

// Helper to check statement type
template<typename T>
bool isStmtType(const Stmt& stmt) {
    return std::holds_alternative<std::unique_ptr<T>>(stmt);
}

// Helper to check expression type
template<typename T>
bool isExprType(const Expr& expr) {
    return std::holds_alternative<std::unique_ptr<T>>(expr);
}

//=============================================================================
// Literal Expression Tests
//=============================================================================

TEST(integer_expression) {
    auto stmts = parse("42\n");
    ASSERT_EQ(stmts.size(), 1u);
    ASSERT_TRUE(isStmtType<ExpressionStmt>(stmts[0]));

    auto& exprStmt = std::get<std::unique_ptr<ExpressionStmt>>(stmts[0]);
    ASSERT_TRUE(isExprType<LiteralExpr>(exprStmt->expression));

    auto& literal = std::get<std::unique_ptr<LiteralExpr>>(exprStmt->expression);
    ASSERT_TRUE(std::holds_alternative<long long>(literal->value));
    ASSERT_EQ(std::get<long long>(literal->value), 42LL);
}

TEST(float_expression) {
    auto stmts = parse("3.14\n");
    auto& exprStmt = std::get<std::unique_ptr<ExpressionStmt>>(stmts[0]);
    auto& literal = std::get<std::unique_ptr<LiteralExpr>>(exprStmt->expression);
    ASSERT_TRUE(std::holds_alternative<double>(literal->value));
}

TEST(string_expression) {
    auto stmts = parse("\"hello\"\n");
    auto& exprStmt = std::get<std::unique_ptr<ExpressionStmt>>(stmts[0]);
    auto& literal = std::get<std::unique_ptr<LiteralExpr>>(exprStmt->expression);
    ASSERT_TRUE(std::holds_alternative<std::string>(literal->value));
    ASSERT_EQ(std::get<std::string>(literal->value), "hello");
}

TEST(boolean_true) {
    auto stmts = parse("True\n");
    auto& exprStmt = std::get<std::unique_ptr<ExpressionStmt>>(stmts[0]);
    auto& literal = std::get<std::unique_ptr<LiteralExpr>>(exprStmt->expression);
    ASSERT_TRUE(std::holds_alternative<bool>(literal->value));
    ASSERT_TRUE(std::get<bool>(literal->value));
}

TEST(boolean_false) {
    auto stmts = parse("False\n");
    auto& exprStmt = std::get<std::unique_ptr<ExpressionStmt>>(stmts[0]);
    auto& literal = std::get<std::unique_ptr<LiteralExpr>>(exprStmt->expression);
    ASSERT_FALSE(std::get<bool>(literal->value));
}

TEST(none_literal) {
    auto stmts = parse("None\n");
    auto& exprStmt = std::get<std::unique_ptr<ExpressionStmt>>(stmts[0]);
    auto& literal = std::get<std::unique_ptr<LiteralExpr>>(exprStmt->expression);
    ASSERT_TRUE(std::holds_alternative<PyNone>(literal->value));
}

//=============================================================================
// Binary Expression Tests
//=============================================================================

TEST(addition) {
    auto stmts = parse("1 + 2\n");
    auto& exprStmt = std::get<std::unique_ptr<ExpressionStmt>>(stmts[0]);
    ASSERT_TRUE(isExprType<BinaryExpr>(exprStmt->expression));

    auto& binExpr = std::get<std::unique_ptr<BinaryExpr>>(exprStmt->expression);
    ASSERT_EQ(binExpr->op.type, TokenType::PLUS);
}

TEST(subtraction) {
    auto stmts = parse("5 - 3\n");
    auto& exprStmt = std::get<std::unique_ptr<ExpressionStmt>>(stmts[0]);
    auto& binExpr = std::get<std::unique_ptr<BinaryExpr>>(exprStmt->expression);
    ASSERT_EQ(binExpr->op.type, TokenType::MINUS);
}

TEST(multiplication) {
    auto stmts = parse("2 * 3\n");
    auto& exprStmt = std::get<std::unique_ptr<ExpressionStmt>>(stmts[0]);
    auto& binExpr = std::get<std::unique_ptr<BinaryExpr>>(exprStmt->expression);
    ASSERT_EQ(binExpr->op.type, TokenType::STAR);
}

TEST(division) {
    auto stmts = parse("10 / 2\n");
    auto& exprStmt = std::get<std::unique_ptr<ExpressionStmt>>(stmts[0]);
    auto& binExpr = std::get<std::unique_ptr<BinaryExpr>>(exprStmt->expression);
    ASSERT_EQ(binExpr->op.type, TokenType::SLASH);
}

TEST(floor_division) {
    auto stmts = parse("10 // 3\n");
    auto& exprStmt = std::get<std::unique_ptr<ExpressionStmt>>(stmts[0]);
    auto& binExpr = std::get<std::unique_ptr<BinaryExpr>>(exprStmt->expression);
    ASSERT_EQ(binExpr->op.type, TokenType::DOUBLE_SLASH);
}

TEST(power) {
    auto stmts = parse("2 ** 3\n");
    auto& exprStmt = std::get<std::unique_ptr<ExpressionStmt>>(stmts[0]);
    auto& binExpr = std::get<std::unique_ptr<BinaryExpr>>(exprStmt->expression);
    ASSERT_EQ(binExpr->op.type, TokenType::DOUBLE_STAR);
}

TEST(comparison_operators) {
    ASSERT_TRUE(parses("1 < 2\n"));
    ASSERT_TRUE(parses("1 <= 2\n"));
    ASSERT_TRUE(parses("1 > 2\n"));
    ASSERT_TRUE(parses("1 >= 2\n"));
    ASSERT_TRUE(parses("1 == 2\n"));
    ASSERT_TRUE(parses("1 != 2\n"));
}

TEST(logical_and) {
    auto stmts = parse("True and False\n");
    auto& exprStmt = std::get<std::unique_ptr<ExpressionStmt>>(stmts[0]);
    auto& binExpr = std::get<std::unique_ptr<BinaryExpr>>(exprStmt->expression);
    ASSERT_EQ(binExpr->op.type, TokenType::AND);
}

TEST(logical_or) {
    auto stmts = parse("True or False\n");
    auto& exprStmt = std::get<std::unique_ptr<ExpressionStmt>>(stmts[0]);
    auto& binExpr = std::get<std::unique_ptr<BinaryExpr>>(exprStmt->expression);
    ASSERT_EQ(binExpr->op.type, TokenType::OR);
}

//=============================================================================
// Unary Expression Tests
//=============================================================================

TEST(unary_minus) {
    auto stmts = parse("-5\n");
    auto& exprStmt = std::get<std::unique_ptr<ExpressionStmt>>(stmts[0]);
    ASSERT_TRUE(isExprType<UnaryExpr>(exprStmt->expression));

    auto& unaryExpr = std::get<std::unique_ptr<UnaryExpr>>(exprStmt->expression);
    ASSERT_EQ(unaryExpr->op.type, TokenType::MINUS);
}

TEST(logical_not) {
    auto stmts = parse("not True\n");
    auto& exprStmt = std::get<std::unique_ptr<ExpressionStmt>>(stmts[0]);
    auto& unaryExpr = std::get<std::unique_ptr<UnaryExpr>>(exprStmt->expression);
    ASSERT_EQ(unaryExpr->op.type, TokenType::NOT);
}

//=============================================================================
// Operator Precedence Tests
//=============================================================================

TEST(mult_before_add) {
    // 1 + 2 * 3 should parse as 1 + (2 * 3)
    auto stmts = parse("1 + 2 * 3\n");
    auto& exprStmt = std::get<std::unique_ptr<ExpressionStmt>>(stmts[0]);
    auto& binExpr = std::get<std::unique_ptr<BinaryExpr>>(exprStmt->expression);

    // Top level should be +
    ASSERT_EQ(binExpr->op.type, TokenType::PLUS);
    // Right side should be *
    ASSERT_TRUE(isExprType<BinaryExpr>(binExpr->right));
    auto& rightExpr = std::get<std::unique_ptr<BinaryExpr>>(binExpr->right);
    ASSERT_EQ(rightExpr->op.type, TokenType::STAR);
}

TEST(parentheses_override_precedence) {
    // (1 + 2) * 3 should parse as (1 + 2) * 3
    auto stmts = parse("(1 + 2) * 3\n");
    auto& exprStmt = std::get<std::unique_ptr<ExpressionStmt>>(stmts[0]);
    auto& binExpr = std::get<std::unique_ptr<BinaryExpr>>(exprStmt->expression);

    // Top level should be *
    ASSERT_EQ(binExpr->op.type, TokenType::STAR);
    // Left side should be grouping containing +
    ASSERT_TRUE(isExprType<GroupingExpr>(binExpr->left));
}

//=============================================================================
// Variable Tests
//=============================================================================

TEST(variable_reference) {
    auto stmts = parse("foo\n");
    auto& exprStmt = std::get<std::unique_ptr<ExpressionStmt>>(stmts[0]);
    ASSERT_TRUE(isExprType<VariableExpr>(exprStmt->expression));

    auto& varExpr = std::get<std::unique_ptr<VariableExpr>>(exprStmt->expression);
    ASSERT_EQ(varExpr->name.lexeme, "foo");
}

TEST(assignment) {
    auto stmts = parse("x = 5\n");
    auto& exprStmt = std::get<std::unique_ptr<ExpressionStmt>>(stmts[0]);
    ASSERT_TRUE(isExprType<AssignExpr>(exprStmt->expression));

    auto& assignExpr = std::get<std::unique_ptr<AssignExpr>>(exprStmt->expression);
    ASSERT_EQ(assignExpr->name.lexeme, "x");
}

TEST(compound_assignment) {
    ASSERT_TRUE(parses("x += 1\n"));
    ASSERT_TRUE(parses("x -= 1\n"));
    ASSERT_TRUE(parses("x *= 2\n"));
    ASSERT_TRUE(parses("x /= 2\n"));
}

//=============================================================================
// Function Call Tests
//=============================================================================

TEST(function_call_no_args) {
    auto stmts = parse("foo()\n");
    auto& exprStmt = std::get<std::unique_ptr<ExpressionStmt>>(stmts[0]);
    ASSERT_TRUE(isExprType<CallExpr>(exprStmt->expression));

    auto& callExpr = std::get<std::unique_ptr<CallExpr>>(exprStmt->expression);
    ASSERT_EQ(callExpr->arguments.size(), 0u);
}

TEST(function_call_one_arg) {
    auto stmts = parse("foo(1)\n");
    auto& exprStmt = std::get<std::unique_ptr<ExpressionStmt>>(stmts[0]);
    auto& callExpr = std::get<std::unique_ptr<CallExpr>>(exprStmt->expression);
    ASSERT_EQ(callExpr->arguments.size(), 1u);
}

TEST(function_call_multiple_args) {
    auto stmts = parse("foo(1, 2, 3)\n");
    auto& exprStmt = std::get<std::unique_ptr<ExpressionStmt>>(stmts[0]);
    auto& callExpr = std::get<std::unique_ptr<CallExpr>>(exprStmt->expression);
    ASSERT_EQ(callExpr->arguments.size(), 3u);
}

//=============================================================================
// Print Statement Tests
//=============================================================================

TEST(print_no_args) {
    auto stmts = parse("print()\n");
    ASSERT_TRUE(isStmtType<PrintStmt>(stmts[0]));

    auto& printStmt = std::get<std::unique_ptr<PrintStmt>>(stmts[0]);
    ASSERT_EQ(printStmt->expressions.size(), 0u);
}

TEST(print_one_arg) {
    auto stmts = parse("print(42)\n");
    auto& printStmt = std::get<std::unique_ptr<PrintStmt>>(stmts[0]);
    ASSERT_EQ(printStmt->expressions.size(), 1u);
}

TEST(print_multiple_args) {
    auto stmts = parse("print(1, 2, 3)\n");
    auto& printStmt = std::get<std::unique_ptr<PrintStmt>>(stmts[0]);
    ASSERT_EQ(printStmt->expressions.size(), 3u);
}

//=============================================================================
// If Statement Tests
//=============================================================================

TEST(if_statement) {
    auto stmts = parse("if True:\n    x\n");
    ASSERT_TRUE(isStmtType<IfStmt>(stmts[0]));
}

TEST(if_else_statement) {
    auto stmts = parse("if True:\n    x\nelse:\n    y\n");
    auto& ifStmt = std::get<std::unique_ptr<IfStmt>>(stmts[0]);
    ASSERT_TRUE(ifStmt->elseBranch != nullptr);
}

TEST(if_elif_else_statement) {
    auto stmts = parse("if a:\n    x\nelif b:\n    y\nelse:\n    z\n");
    auto& ifStmt = std::get<std::unique_ptr<IfStmt>>(stmts[0]);
    ASSERT_EQ(ifStmt->elifBranches.size(), 1u);
    ASSERT_TRUE(ifStmt->elseBranch != nullptr);
}

TEST(multiple_elif) {
    auto stmts = parse("if a:\n    x\nelif b:\n    y\nelif c:\n    z\n");
    auto& ifStmt = std::get<std::unique_ptr<IfStmt>>(stmts[0]);
    ASSERT_EQ(ifStmt->elifBranches.size(), 2u);
}

//=============================================================================
// While Statement Tests
//=============================================================================

TEST(while_statement) {
    auto stmts = parse("while True:\n    x\n");
    ASSERT_TRUE(isStmtType<WhileStmt>(stmts[0]));
}

//=============================================================================
// Function Definition Tests
//=============================================================================

TEST(function_def_no_params) {
    auto stmts = parse("def foo():\n    return 1\n");
    ASSERT_TRUE(isStmtType<FunctionStmt>(stmts[0]));

    auto& funcStmt = std::get<std::unique_ptr<FunctionStmt>>(stmts[0]);
    ASSERT_EQ(funcStmt->name.lexeme, "foo");
    ASSERT_EQ(funcStmt->params.size(), 0u);
}

TEST(function_def_one_param) {
    auto stmts = parse("def foo(x):\n    return x\n");
    auto& funcStmt = std::get<std::unique_ptr<FunctionStmt>>(stmts[0]);
    ASSERT_EQ(funcStmt->params.size(), 1u);
    ASSERT_EQ(funcStmt->params[0].lexeme, "x");
}

TEST(function_def_multiple_params) {
    auto stmts = parse("def foo(a, b, c):\n    return a\n");
    auto& funcStmt = std::get<std::unique_ptr<FunctionStmt>>(stmts[0]);
    ASSERT_EQ(funcStmt->params.size(), 3u);
}

//=============================================================================
// Return Statement Tests
//=============================================================================

TEST(return_with_value) {
    auto stmts = parse("def f():\n    return 42\n");
    auto& funcStmt = std::get<std::unique_ptr<FunctionStmt>>(stmts[0]);
    ASSERT_EQ(funcStmt->body.size(), 1u);
    ASSERT_TRUE(isStmtType<ReturnStmt>(funcStmt->body[0]));

    auto& retStmt = std::get<std::unique_ptr<ReturnStmt>>(funcStmt->body[0]);
    ASSERT_TRUE(retStmt->value != nullptr);
}

TEST(return_without_value) {
    auto stmts = parse("def f():\n    return\n");
    auto& funcStmt = std::get<std::unique_ptr<FunctionStmt>>(stmts[0]);
    auto& retStmt = std::get<std::unique_ptr<ReturnStmt>>(funcStmt->body[0]);
    ASSERT_TRUE(retStmt->value == nullptr);
}

//=============================================================================
// Assert Statement Tests
//=============================================================================

TEST(assert_simple) {
    auto stmts = parse("assert True\n");
    ASSERT_TRUE(isStmtType<AssertStmt>(stmts[0]));
}

TEST(assert_with_message) {
    auto stmts = parse("assert False, \"error\"\n");
    auto& assertStmt = std::get<std::unique_ptr<AssertStmt>>(stmts[0]);
    ASSERT_TRUE(assertStmt->message != nullptr);
}

//=============================================================================
// Error Tests
//=============================================================================

TEST(missing_colon_after_if) {
    ASSERT_FALSE(parses("if True\n    x\n"));
}

TEST(missing_indent_after_if) {
    ASSERT_FALSE(parses("if True:\nx\n"));
}

TEST(unmatched_paren) {
    ASSERT_FALSE(parses("(1 + 2\n"));
}

TEST(missing_function_name) {
    ASSERT_FALSE(parses("def ():\n    pass\n"));
}

//=============================================================================
// Multiple Statements Tests
//=============================================================================

TEST(multiple_statements) {
    auto stmts = parse("x = 1\ny = 2\nz = 3\n");
    ASSERT_EQ(stmts.size(), 3u);
}

TEST(nested_blocks) {
    auto stmts = parse("if a:\n    if b:\n        x\n");
    ASSERT_EQ(stmts.size(), 1u);
}

//=============================================================================
// Main
//=============================================================================

int main() {
    std::cout << "Running Parser Tests..." << std::endl;
    std::cout << std::endl;

    std::cout << "Literal Expression Tests:" << std::endl;
    RUN_TEST(integer_expression);
    RUN_TEST(float_expression);
    RUN_TEST(string_expression);
    RUN_TEST(boolean_true);
    RUN_TEST(boolean_false);
    RUN_TEST(none_literal);

    std::cout << "\nBinary Expression Tests:" << std::endl;
    RUN_TEST(addition);
    RUN_TEST(subtraction);
    RUN_TEST(multiplication);
    RUN_TEST(division);
    RUN_TEST(floor_division);
    RUN_TEST(power);
    RUN_TEST(comparison_operators);
    RUN_TEST(logical_and);
    RUN_TEST(logical_or);

    std::cout << "\nUnary Expression Tests:" << std::endl;
    RUN_TEST(unary_minus);
    RUN_TEST(logical_not);

    std::cout << "\nOperator Precedence Tests:" << std::endl;
    RUN_TEST(mult_before_add);
    RUN_TEST(parentheses_override_precedence);

    std::cout << "\nVariable Tests:" << std::endl;
    RUN_TEST(variable_reference);
    RUN_TEST(assignment);
    RUN_TEST(compound_assignment);

    std::cout << "\nFunction Call Tests:" << std::endl;
    RUN_TEST(function_call_no_args);
    RUN_TEST(function_call_one_arg);
    RUN_TEST(function_call_multiple_args);

    std::cout << "\nPrint Statement Tests:" << std::endl;
    RUN_TEST(print_no_args);
    RUN_TEST(print_one_arg);
    RUN_TEST(print_multiple_args);

    std::cout << "\nIf Statement Tests:" << std::endl;
    RUN_TEST(if_statement);
    RUN_TEST(if_else_statement);
    RUN_TEST(if_elif_else_statement);
    RUN_TEST(multiple_elif);

    std::cout << "\nWhile Statement Tests:" << std::endl;
    RUN_TEST(while_statement);

    std::cout << "\nFunction Definition Tests:" << std::endl;
    RUN_TEST(function_def_no_params);
    RUN_TEST(function_def_one_param);
    RUN_TEST(function_def_multiple_params);

    std::cout << "\nReturn Statement Tests:" << std::endl;
    RUN_TEST(return_with_value);
    RUN_TEST(return_without_value);

    std::cout << "\nAssert Statement Tests:" << std::endl;
    RUN_TEST(assert_simple);
    RUN_TEST(assert_with_message);

    std::cout << "\nError Tests:" << std::endl;
    RUN_TEST(missing_colon_after_if);
    RUN_TEST(missing_indent_after_if);
    RUN_TEST(unmatched_paren);
    RUN_TEST(missing_function_name);

    std::cout << "\nMultiple Statements Tests:" << std::endl;
    RUN_TEST(multiple_statements);
    RUN_TEST(nested_blocks);

    std::cout << "\n========================================" << std::endl;
    std::cout << "All Parser tests passed!" << std::endl;

    return 0;
}
