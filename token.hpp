#ifndef TOKEN_HPP
#define TOKEN_HPP

#include <string>
#include <variant>

enum class TokenType {
    // Literals
    INTEGER,
    FLOAT,
    IDENTIFIER,
    STRING,

    // Operators
    PLUS,
    MINUS,
    STAR,
    SLASH,
    DOUBLE_SLASH,  // //
    PERCENT,       // %
    DOUBLE_STAR,   // **

    // Comparison
    EQ,            // ==
    NE,            // !=
    LT,            // <
    LE,            // <=
    GT,            // >
    GE,            // >=

    // Assignment
    ASSIGN,        // =
    PLUS_ASSIGN,   // +=
    MINUS_ASSIGN,  // -=
    STAR_ASSIGN,   // *=
    SLASH_ASSIGN,  // /=

    // Delimiters
    LPAREN,
    RPAREN,
    COLON,
    COMMA,
    NEWLINE,
    INDENT,
    DEDENT,

    // Keywords
    DEF,
    RETURN,
    IF,
    ELIF,
    ELSE,
    WHILE,
    FOR,
    IN,
    AND,
    OR,
    NOT,
    TRUE,
    FALSE,
    NONE,
    PRINT,
    ASSERT,

    // Special
    END_OF_FILE,
    INVALID
};

struct Token {
    TokenType type;
    std::string lexeme;
    std::variant<std::monostate, long long, double, std::string> literal;
    int line;
    int column;

    Token(TokenType type, std::string lexeme, int line, int column)
        : type(type), lexeme(std::move(lexeme)), line(line), column(column) {}

    Token(TokenType type, std::string lexeme, long long value, int line, int column)
        : type(type), lexeme(std::move(lexeme)), literal(value), line(line), column(column) {}

    Token(TokenType type, std::string lexeme, double value, int line, int column)
        : type(type), lexeme(std::move(lexeme)), literal(value), line(line), column(column) {}

    Token(TokenType type, std::string lexeme, std::string value, int line, int column)
        : type(type), lexeme(std::move(lexeme)), literal(std::move(value)), line(line), column(column) {}
};

inline std::string tokenTypeToString(TokenType type) {
    switch (type) {
        case TokenType::INTEGER: return "INTEGER";
        case TokenType::FLOAT: return "FLOAT";
        case TokenType::IDENTIFIER: return "IDENTIFIER";
        case TokenType::STRING: return "STRING";
        case TokenType::PLUS: return "PLUS";
        case TokenType::MINUS: return "MINUS";
        case TokenType::STAR: return "STAR";
        case TokenType::SLASH: return "SLASH";
        case TokenType::DOUBLE_SLASH: return "DOUBLE_SLASH";
        case TokenType::PERCENT: return "PERCENT";
        case TokenType::DOUBLE_STAR: return "DOUBLE_STAR";
        case TokenType::EQ: return "EQ";
        case TokenType::NE: return "NE";
        case TokenType::LT: return "LT";
        case TokenType::LE: return "LE";
        case TokenType::GT: return "GT";
        case TokenType::GE: return "GE";
        case TokenType::ASSIGN: return "ASSIGN";
        case TokenType::PLUS_ASSIGN: return "PLUS_ASSIGN";
        case TokenType::MINUS_ASSIGN: return "MINUS_ASSIGN";
        case TokenType::STAR_ASSIGN: return "STAR_ASSIGN";
        case TokenType::SLASH_ASSIGN: return "SLASH_ASSIGN";
        case TokenType::LPAREN: return "LPAREN";
        case TokenType::RPAREN: return "RPAREN";
        case TokenType::COLON: return "COLON";
        case TokenType::COMMA: return "COMMA";
        case TokenType::NEWLINE: return "NEWLINE";
        case TokenType::INDENT: return "INDENT";
        case TokenType::DEDENT: return "DEDENT";
        case TokenType::DEF: return "DEF";
        case TokenType::RETURN: return "RETURN";
        case TokenType::IF: return "IF";
        case TokenType::ELIF: return "ELIF";
        case TokenType::ELSE: return "ELSE";
        case TokenType::WHILE: return "WHILE";
        case TokenType::FOR: return "FOR";
        case TokenType::IN: return "IN";
        case TokenType::AND: return "AND";
        case TokenType::OR: return "OR";
        case TokenType::NOT: return "NOT";
        case TokenType::TRUE: return "TRUE";
        case TokenType::FALSE: return "FALSE";
        case TokenType::NONE: return "NONE";
        case TokenType::PRINT: return "PRINT";
        case TokenType::ASSERT: return "ASSERT";
        case TokenType::END_OF_FILE: return "EOF";
        case TokenType::INVALID: return "INVALID";
        default: return "UNKNOWN";
    }
}

#endif // TOKEN_HPP
