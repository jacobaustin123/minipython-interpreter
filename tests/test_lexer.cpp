#include <iostream>
#include <cassert>
#include <vector>
#include <string>
#include "../lexer.hpp"

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

// Helper to get token types from source
std::vector<TokenType> tokenTypes(const std::string& source) {
    Lexer lexer(source);
    auto tokens = lexer.tokenize();
    std::vector<TokenType> types;
    for (const auto& tok : tokens) {
        types.push_back(tok.type);
    }
    return types;
}

// Helper to get token lexemes from source
std::vector<std::string> tokenLexemes(const std::string& source) {
    Lexer lexer(source);
    auto tokens = lexer.tokenize();
    std::vector<std::string> lexemes;
    for (const auto& tok : tokens) {
        lexemes.push_back(tok.lexeme);
    }
    return lexemes;
}

//=============================================================================
// Integer Tests
//=============================================================================

TEST(integer_literal) {
    Lexer lexer("42");
    auto tokens = lexer.tokenize();
    ASSERT_EQ(tokens[0].type, TokenType::INTEGER);
    ASSERT_EQ(std::get<long long>(tokens[0].literal), 42LL);
}

TEST(multiple_integers) {
    auto types = tokenTypes("1 2 3");
    ASSERT_EQ(types[0], TokenType::INTEGER);
    ASSERT_EQ(types[1], TokenType::INTEGER);
    ASSERT_EQ(types[2], TokenType::INTEGER);
}

TEST(zero) {
    Lexer lexer("0");
    auto tokens = lexer.tokenize();
    ASSERT_EQ(std::get<long long>(tokens[0].literal), 0LL);
}

//=============================================================================
// Float Tests
//=============================================================================

TEST(float_literal) {
    Lexer lexer("3.14");
    auto tokens = lexer.tokenize();
    ASSERT_EQ(tokens[0].type, TokenType::FLOAT);
    ASSERT_TRUE(std::abs(std::get<double>(tokens[0].literal) - 3.14) < 0.001);
}

TEST(float_with_exponent) {
    Lexer lexer("1e10");
    auto tokens = lexer.tokenize();
    ASSERT_EQ(tokens[0].type, TokenType::FLOAT);
    ASSERT_TRUE(std::get<double>(tokens[0].literal) > 9e9);
}

TEST(float_with_negative_exponent) {
    Lexer lexer("1e-5");
    auto tokens = lexer.tokenize();
    ASSERT_EQ(tokens[0].type, TokenType::FLOAT);
    ASSERT_TRUE(std::get<double>(tokens[0].literal) < 0.001);
}

//=============================================================================
// String Tests
//=============================================================================

TEST(double_quoted_string) {
    Lexer lexer("\"hello\"");
    auto tokens = lexer.tokenize();
    ASSERT_EQ(tokens[0].type, TokenType::STRING);
    ASSERT_EQ(std::get<std::string>(tokens[0].literal), "hello");
}

TEST(single_quoted_string) {
    Lexer lexer("'world'");
    auto tokens = lexer.tokenize();
    ASSERT_EQ(tokens[0].type, TokenType::STRING);
    ASSERT_EQ(std::get<std::string>(tokens[0].literal), "world");
}

TEST(string_with_escapes) {
    Lexer lexer("\"line1\\nline2\"");
    auto tokens = lexer.tokenize();
    ASSERT_EQ(std::get<std::string>(tokens[0].literal), "line1\nline2");
}

TEST(string_with_tab_escape) {
    Lexer lexer("\"col1\\tcol2\"");
    auto tokens = lexer.tokenize();
    ASSERT_EQ(std::get<std::string>(tokens[0].literal), "col1\tcol2");
}

TEST(empty_string) {
    Lexer lexer("\"\"");
    auto tokens = lexer.tokenize();
    ASSERT_EQ(tokens[0].type, TokenType::STRING);
    ASSERT_EQ(std::get<std::string>(tokens[0].literal), "");
}

//=============================================================================
// Operator Tests
//=============================================================================

TEST(arithmetic_operators) {
    auto types = tokenTypes("+ - * / // % **");
    ASSERT_EQ(types[0], TokenType::PLUS);
    ASSERT_EQ(types[1], TokenType::MINUS);
    ASSERT_EQ(types[2], TokenType::STAR);
    ASSERT_EQ(types[3], TokenType::SLASH);
    ASSERT_EQ(types[4], TokenType::DOUBLE_SLASH);
    ASSERT_EQ(types[5], TokenType::PERCENT);
    ASSERT_EQ(types[6], TokenType::DOUBLE_STAR);
}

TEST(comparison_operators) {
    auto types = tokenTypes("== != < <= > >=");
    ASSERT_EQ(types[0], TokenType::EQ);
    ASSERT_EQ(types[1], TokenType::NE);
    ASSERT_EQ(types[2], TokenType::LT);
    ASSERT_EQ(types[3], TokenType::LE);
    ASSERT_EQ(types[4], TokenType::GT);
    ASSERT_EQ(types[5], TokenType::GE);
}

TEST(assignment_operators) {
    auto types = tokenTypes("= += -= *= /=");
    ASSERT_EQ(types[0], TokenType::ASSIGN);
    ASSERT_EQ(types[1], TokenType::PLUS_ASSIGN);
    ASSERT_EQ(types[2], TokenType::MINUS_ASSIGN);
    ASSERT_EQ(types[3], TokenType::STAR_ASSIGN);
    ASSERT_EQ(types[4], TokenType::SLASH_ASSIGN);
}

//=============================================================================
// Keyword Tests
//=============================================================================

TEST(keywords) {
    auto types = tokenTypes("def return if elif else while for in");
    ASSERT_EQ(types[0], TokenType::DEF);
    ASSERT_EQ(types[1], TokenType::RETURN);
    ASSERT_EQ(types[2], TokenType::IF);
    ASSERT_EQ(types[3], TokenType::ELIF);
    ASSERT_EQ(types[4], TokenType::ELSE);
    ASSERT_EQ(types[5], TokenType::WHILE);
    ASSERT_EQ(types[6], TokenType::FOR);
    ASSERT_EQ(types[7], TokenType::IN);
}

TEST(boolean_keywords) {
    auto types = tokenTypes("True False None and or not");
    ASSERT_EQ(types[0], TokenType::TRUE);
    ASSERT_EQ(types[1], TokenType::FALSE);
    ASSERT_EQ(types[2], TokenType::NONE);
    ASSERT_EQ(types[3], TokenType::AND);
    ASSERT_EQ(types[4], TokenType::OR);
    ASSERT_EQ(types[5], TokenType::NOT);
}

TEST(print_and_assert) {
    auto types = tokenTypes("print assert");
    ASSERT_EQ(types[0], TokenType::PRINT);
    ASSERT_EQ(types[1], TokenType::ASSERT);
}

//=============================================================================
// Identifier Tests
//=============================================================================

TEST(simple_identifier) {
    Lexer lexer("foo");
    auto tokens = lexer.tokenize();
    ASSERT_EQ(tokens[0].type, TokenType::IDENTIFIER);
    ASSERT_EQ(tokens[0].lexeme, "foo");
}

TEST(identifier_with_underscore) {
    Lexer lexer("my_variable");
    auto tokens = lexer.tokenize();
    ASSERT_EQ(tokens[0].type, TokenType::IDENTIFIER);
    ASSERT_EQ(tokens[0].lexeme, "my_variable");
}

TEST(identifier_with_numbers) {
    Lexer lexer("var123");
    auto tokens = lexer.tokenize();
    ASSERT_EQ(tokens[0].type, TokenType::IDENTIFIER);
    ASSERT_EQ(tokens[0].lexeme, "var123");
}

TEST(identifier_starting_with_underscore) {
    Lexer lexer("_private");
    auto tokens = lexer.tokenize();
    ASSERT_EQ(tokens[0].type, TokenType::IDENTIFIER);
    ASSERT_EQ(tokens[0].lexeme, "_private");
}

//=============================================================================
// Delimiter Tests
//=============================================================================

TEST(delimiters) {
    auto types = tokenTypes("( ) : ,");
    ASSERT_EQ(types[0], TokenType::LPAREN);
    ASSERT_EQ(types[1], TokenType::RPAREN);
    ASSERT_EQ(types[2], TokenType::COLON);
    ASSERT_EQ(types[3], TokenType::COMMA);
}

//=============================================================================
// Indentation Tests
//=============================================================================

TEST(indent_dedent) {
    std::string source = "if x:\n    y\nz";
    auto types = tokenTypes(source);
    // if x : NEWLINE INDENT y NEWLINE DEDENT z NEWLINE EOF
    ASSERT_EQ(types[0], TokenType::IF);
    ASSERT_EQ(types[1], TokenType::IDENTIFIER);  // x
    ASSERT_EQ(types[2], TokenType::COLON);
    ASSERT_EQ(types[3], TokenType::NEWLINE);
    ASSERT_EQ(types[4], TokenType::INDENT);
    ASSERT_EQ(types[5], TokenType::IDENTIFIER);  // y
    ASSERT_EQ(types[6], TokenType::NEWLINE);
    ASSERT_EQ(types[7], TokenType::DEDENT);
    ASSERT_EQ(types[8], TokenType::IDENTIFIER);  // z
}

TEST(nested_indent) {
    std::string source = "if a:\n    if b:\n        c\nd";
    auto types = tokenTypes(source);
    // Should have two INDENTs and two DEDENTs
    int indents = 0, dedents = 0;
    for (auto t : types) {
        if (t == TokenType::INDENT) indents++;
        if (t == TokenType::DEDENT) dedents++;
    }
    ASSERT_EQ(indents, 2);
    ASSERT_EQ(dedents, 2);
}

//=============================================================================
// Comment Tests
//=============================================================================

TEST(comment_ignored) {
    auto types = tokenTypes("x # this is a comment\ny");
    // x NEWLINE y NEWLINE EOF
    ASSERT_EQ(types[0], TokenType::IDENTIFIER);
    ASSERT_EQ(types[1], TokenType::NEWLINE);
    ASSERT_EQ(types[2], TokenType::IDENTIFIER);
}

TEST(comment_only_line) {
    auto types = tokenTypes("x\n# comment\ny");
    // Comments on their own line shouldn't produce extra tokens
    ASSERT_EQ(types[0], TokenType::IDENTIFIER);  // x
    ASSERT_EQ(types[1], TokenType::NEWLINE);
    ASSERT_EQ(types[2], TokenType::IDENTIFIER);  // y
}

//=============================================================================
// Complex Expression Tests
//=============================================================================

TEST(function_call) {
    auto types = tokenTypes("foo(1, 2)");
    ASSERT_EQ(types[0], TokenType::IDENTIFIER);
    ASSERT_EQ(types[1], TokenType::LPAREN);
    ASSERT_EQ(types[2], TokenType::INTEGER);
    ASSERT_EQ(types[3], TokenType::COMMA);
    ASSERT_EQ(types[4], TokenType::INTEGER);
    ASSERT_EQ(types[5], TokenType::RPAREN);
}

TEST(arithmetic_expression) {
    auto types = tokenTypes("2 + 3 * 4");
    ASSERT_EQ(types[0], TokenType::INTEGER);
    ASSERT_EQ(types[1], TokenType::PLUS);
    ASSERT_EQ(types[2], TokenType::INTEGER);
    ASSERT_EQ(types[3], TokenType::STAR);
    ASSERT_EQ(types[4], TokenType::INTEGER);
}

//=============================================================================
// Error Tests
//=============================================================================

TEST(unterminated_string_throws) {
    bool threw = false;
    try {
        Lexer lexer("\"unterminated");
        lexer.tokenize();
    } catch (const LexerError& e) {
        threw = true;
    }
    ASSERT_TRUE(threw);
}

TEST(unexpected_character_throws) {
    bool threw = false;
    try {
        Lexer lexer("@");
        lexer.tokenize();
    } catch (const LexerError& e) {
        threw = true;
    }
    ASSERT_TRUE(threw);
}

//=============================================================================
// Line/Column Tracking Tests
//=============================================================================

TEST(line_numbers) {
    Lexer lexer("a\nb\nc");
    auto tokens = lexer.tokenize();
    ASSERT_EQ(tokens[0].line, 1);  // a
    ASSERT_EQ(tokens[2].line, 2);  // b
    ASSERT_EQ(tokens[4].line, 3);  // c
}

//=============================================================================
// Main
//=============================================================================

int main() {
    std::cout << "Running Lexer Tests..." << std::endl;
    std::cout << std::endl;

    std::cout << "Integer Tests:" << std::endl;
    RUN_TEST(integer_literal);
    RUN_TEST(multiple_integers);
    RUN_TEST(zero);

    std::cout << "\nFloat Tests:" << std::endl;
    RUN_TEST(float_literal);
    RUN_TEST(float_with_exponent);
    RUN_TEST(float_with_negative_exponent);

    std::cout << "\nString Tests:" << std::endl;
    RUN_TEST(double_quoted_string);
    RUN_TEST(single_quoted_string);
    RUN_TEST(string_with_escapes);
    RUN_TEST(string_with_tab_escape);
    RUN_TEST(empty_string);

    std::cout << "\nOperator Tests:" << std::endl;
    RUN_TEST(arithmetic_operators);
    RUN_TEST(comparison_operators);
    RUN_TEST(assignment_operators);

    std::cout << "\nKeyword Tests:" << std::endl;
    RUN_TEST(keywords);
    RUN_TEST(boolean_keywords);
    RUN_TEST(print_and_assert);

    std::cout << "\nIdentifier Tests:" << std::endl;
    RUN_TEST(simple_identifier);
    RUN_TEST(identifier_with_underscore);
    RUN_TEST(identifier_with_numbers);
    RUN_TEST(identifier_starting_with_underscore);

    std::cout << "\nDelimiter Tests:" << std::endl;
    RUN_TEST(delimiters);

    std::cout << "\nIndentation Tests:" << std::endl;
    RUN_TEST(indent_dedent);
    RUN_TEST(nested_indent);

    std::cout << "\nComment Tests:" << std::endl;
    RUN_TEST(comment_ignored);
    RUN_TEST(comment_only_line);

    std::cout << "\nComplex Expression Tests:" << std::endl;
    RUN_TEST(function_call);
    RUN_TEST(arithmetic_expression);

    std::cout << "\nError Tests:" << std::endl;
    RUN_TEST(unterminated_string_throws);
    RUN_TEST(unexpected_character_throws);

    std::cout << "\nLine/Column Tests:" << std::endl;
    RUN_TEST(line_numbers);

    std::cout << "\n========================================" << std::endl;
    std::cout << "All Lexer tests passed!" << std::endl;

    return 0;
}
