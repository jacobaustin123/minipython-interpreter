#ifndef LEXER_HPP
#define LEXER_HPP

#include <string>
#include <vector>
#include <unordered_map>
#include <stdexcept>
#include "token.hpp"

class LexerError : public std::runtime_error {
public:
    int line, column;
    LexerError(const std::string& msg, int line, int column)
        : std::runtime_error(msg), line(line), column(column) {}
};

class Lexer {
public:
    explicit Lexer(std::string source);
    std::vector<Token> tokenize();

private:
    std::string source;
    std::vector<Token> tokens;
    size_t start = 0;
    size_t current = 0;
    int line = 1;
    int column = 1;
    int startColumn = 1;

    // For indentation tracking
    std::vector<int> indentStack;
    bool atLineStart = true;

    static const std::unordered_map<std::string, TokenType> keywords;

    bool isAtEnd() const;
    char peek() const;
    char peekNext() const;
    char advance();
    bool match(char expected);

    void scanToken();
    void handleIndentation();
    void number();
    void identifier();
    void string(char quote);
    void skipComment();

    void addToken(TokenType type);
    void addToken(TokenType type, long long value);
    void addToken(TokenType type, double value);
    void addToken(TokenType type, const std::string& value);

    void error(const std::string& message);
};

#endif // LEXER_HPP
