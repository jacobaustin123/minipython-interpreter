#ifndef PARSER_HPP
#define PARSER_HPP

#include <vector>
#include <stdexcept>
#include "token.hpp"
#include "ast.hpp"

class ParseError : public std::runtime_error {
public:
    Token token;
    ParseError(const std::string& msg, Token token)
        : std::runtime_error(msg), token(std::move(token)) {}
};

class Parser {
public:
    explicit Parser(std::vector<Token> tokens);
    std::vector<Stmt> parse();

private:
    std::vector<Token> tokens;
    size_t current = 0;

    // Utility methods
    bool isAtEnd() const;
    const Token& peek() const;
    const Token& previous() const;
    Token advance();
    bool check(TokenType type) const;
    bool match(TokenType type);
    template<typename... Types>
    bool match(TokenType first, Types... rest);
    Token consume(TokenType type, const std::string& message);
    ParseError error(const Token& token, const std::string& message);
    void synchronize();

    // Skip newlines helper
    void skipNewlines();

    // Grammar rules - Statements
    Stmt declaration();
    Stmt statement();
    Stmt expressionStatement();
    Stmt printStatement();
    Stmt ifStatement();
    Stmt whileStatement();
    Stmt functionDeclaration();
    Stmt returnStatement();
    Stmt assertStatement();
    std::vector<Stmt> block();

    // Grammar rules - Expressions
    Expr expression();
    Expr assignment();
    Expr orExpr();
    Expr andExpr();
    Expr notExpr();
    Expr comparison();
    Expr term();
    Expr factor();
    Expr unary();
    Expr power();
    Expr call();
    Expr primary();
    Expr finishCall(Expr callee);
};

#endif // PARSER_HPP
