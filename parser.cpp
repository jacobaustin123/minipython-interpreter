#include "parser.hpp"
#include <sstream>

Parser::Parser(std::vector<Token> tokens) : tokens(std::move(tokens)) {}

bool Parser::isAtEnd() const {
    return peek().type == TokenType::END_OF_FILE;
}

const Token& Parser::peek() const {
    return tokens[current];
}

const Token& Parser::previous() const {
    return tokens[current - 1];
}

Token Parser::advance() {
    if (!isAtEnd()) current++;
    return previous();
}

bool Parser::check(TokenType type) const {
    if (isAtEnd()) return false;
    return peek().type == type;
}

bool Parser::match(TokenType type) {
    if (check(type)) {
        advance();
        return true;
    }
    return false;
}

template<typename... Types>
bool Parser::match(TokenType first, Types... rest) {
    if (match(first)) return true;
    if constexpr (sizeof...(rest) > 0) {
        return match(rest...);
    }
    return false;
}

Token Parser::consume(TokenType type, const std::string& message) {
    if (check(type)) return advance();
    throw error(peek(), message);
}

ParseError Parser::error(const Token& token, const std::string& message) {
    std::ostringstream oss;
    oss << "[line " << token.line << "] Error";
    if (token.type == TokenType::END_OF_FILE) {
        oss << " at end";
    } else {
        oss << " at '" << token.lexeme << "'";
    }
    oss << ": " << message;
    return ParseError(oss.str(), token);
}

void Parser::synchronize() {
    advance();

    while (!isAtEnd()) {
        if (previous().type == TokenType::NEWLINE) return;

        switch (peek().type) {
            case TokenType::DEF:
            case TokenType::IF:
            case TokenType::WHILE:
            case TokenType::FOR:
            case TokenType::RETURN:
            case TokenType::PRINT:
                return;
            default:
                break;
        }

        advance();
    }
}

void Parser::skipNewlines() {
    while (match(TokenType::NEWLINE)) {}
}

std::vector<Stmt> Parser::parse() {
    std::vector<Stmt> statements;

    skipNewlines();

    while (!isAtEnd()) {
        try {
            statements.push_back(declaration());
        } catch (const ParseError& e) {
            throw; // Re-throw for now; could collect errors instead
        }
        skipNewlines();
    }

    return statements;
}

Stmt Parser::declaration() {
    if (match(TokenType::DEF)) {
        return functionDeclaration();
    }
    return statement();
}

Stmt Parser::statement() {
    if (match(TokenType::PRINT)) {
        return printStatement();
    }
    if (match(TokenType::IF)) {
        return ifStatement();
    }
    if (match(TokenType::WHILE)) {
        return whileStatement();
    }
    if (match(TokenType::RETURN)) {
        return returnStatement();
    }
    if (match(TokenType::ASSERT)) {
        return assertStatement();
    }
    return expressionStatement();
}

Stmt Parser::expressionStatement() {
    Expr expr = expression();

    // Handle compound assignment
    if (match(TokenType::PLUS_ASSIGN, TokenType::MINUS_ASSIGN,
              TokenType::STAR_ASSIGN, TokenType::SLASH_ASSIGN)) {
        Token op = previous();

        // expr must be a variable
        if (!std::holds_alternative<std::unique_ptr<VariableExpr>>(expr)) {
            throw error(op, "Invalid assignment target");
        }

        auto& varExpr = std::get<std::unique_ptr<VariableExpr>>(expr);
        Token name = varExpr->name;

        Expr value = expression();

        // Convert compound assignment to binary operation
        TokenType binOp;
        switch (op.type) {
            case TokenType::PLUS_ASSIGN: binOp = TokenType::PLUS; break;
            case TokenType::MINUS_ASSIGN: binOp = TokenType::MINUS; break;
            case TokenType::STAR_ASSIGN: binOp = TokenType::STAR; break;
            case TokenType::SLASH_ASSIGN: binOp = TokenType::SLASH; break;
            default: binOp = TokenType::PLUS; break;
        }

        Token binToken(binOp, op.lexeme.substr(0, 1), op.line, op.column);

        Expr varRef = std::make_unique<VariableExpr>(name);
        Expr binExpr = std::make_unique<BinaryExpr>(
            std::move(varRef), binToken, std::move(value));

        // Use AssignExpr to update existing variable (not create new one)
        Expr assignExpr = std::make_unique<AssignExpr>(name, std::move(binExpr));

        consume(TokenType::NEWLINE, "Expected newline after statement");

        return std::make_unique<ExpressionStmt>(std::move(assignExpr));
    }

    consume(TokenType::NEWLINE, "Expected newline after expression");
    return std::make_unique<ExpressionStmt>(std::move(expr));
}

Stmt Parser::printStatement() {
    consume(TokenType::LPAREN, "Expected '(' after 'print'");

    std::vector<Expr> expressions;

    if (!check(TokenType::RPAREN)) {
        do {
            expressions.push_back(expression());
        } while (match(TokenType::COMMA));
    }

    consume(TokenType::RPAREN, "Expected ')' after print arguments");
    consume(TokenType::NEWLINE, "Expected newline after print statement");

    return std::make_unique<PrintStmt>(std::move(expressions));
}

Stmt Parser::ifStatement() {
    Expr condition = expression();
    consume(TokenType::COLON, "Expected ':' after if condition");
    consume(TokenType::NEWLINE, "Expected newline after ':'");
    consume(TokenType::INDENT, "Expected indented block after if");

    std::vector<Stmt> thenStatements = block();
    Stmt thenBranch = std::make_unique<BlockStmt>(std::move(thenStatements));

    std::vector<std::pair<Expr, Stmt>> elifBranches;

    while (match(TokenType::ELIF)) {
        Expr elifCondition = expression();
        consume(TokenType::COLON, "Expected ':' after elif condition");
        consume(TokenType::NEWLINE, "Expected newline after ':'");
        consume(TokenType::INDENT, "Expected indented block after elif");

        std::vector<Stmt> elifStatements = block();
        Stmt elifBranch = std::make_unique<BlockStmt>(std::move(elifStatements));
        elifBranches.emplace_back(std::move(elifCondition), std::move(elifBranch));
    }

    std::unique_ptr<Stmt> elseBranch = nullptr;
    if (match(TokenType::ELSE)) {
        consume(TokenType::COLON, "Expected ':' after else");
        consume(TokenType::NEWLINE, "Expected newline after ':'");
        consume(TokenType::INDENT, "Expected indented block after else");

        std::vector<Stmt> elseStatements = block();
        elseBranch = std::make_unique<Stmt>(
            std::make_unique<BlockStmt>(std::move(elseStatements)));
    }

    return std::make_unique<IfStmt>(
        std::move(condition), std::move(thenBranch),
        std::move(elifBranches), std::move(elseBranch));
}

Stmt Parser::whileStatement() {
    Expr condition = expression();
    consume(TokenType::COLON, "Expected ':' after while condition");
    consume(TokenType::NEWLINE, "Expected newline after ':'");
    consume(TokenType::INDENT, "Expected indented block after while");

    std::vector<Stmt> bodyStatements = block();
    Stmt body = std::make_unique<BlockStmt>(std::move(bodyStatements));

    return std::make_unique<WhileStmt>(std::move(condition), std::move(body));
}

Stmt Parser::functionDeclaration() {
    Token name = consume(TokenType::IDENTIFIER, "Expected function name");
    consume(TokenType::LPAREN, "Expected '(' after function name");

    std::vector<Token> params;
    if (!check(TokenType::RPAREN)) {
        do {
            params.push_back(consume(TokenType::IDENTIFIER, "Expected parameter name"));
        } while (match(TokenType::COMMA));
    }

    consume(TokenType::RPAREN, "Expected ')' after parameters");
    consume(TokenType::COLON, "Expected ':' after parameters");
    consume(TokenType::NEWLINE, "Expected newline after ':'");
    consume(TokenType::INDENT, "Expected indented block for function body");

    std::vector<Stmt> body = block();

    return std::make_unique<FunctionStmt>(name, std::move(params), std::move(body));
}

Stmt Parser::returnStatement() {
    Token keyword = previous();
    std::unique_ptr<Expr> value = nullptr;

    if (!check(TokenType::NEWLINE)) {
        value = std::make_unique<Expr>(expression());
    }

    consume(TokenType::NEWLINE, "Expected newline after return");

    return std::make_unique<ReturnStmt>(keyword, std::move(value));
}

Stmt Parser::assertStatement() {
    Token keyword = previous();
    Expr condition = expression();

    std::unique_ptr<Expr> message = nullptr;
    if (match(TokenType::COMMA)) {
        message = std::make_unique<Expr>(expression());
    }

    consume(TokenType::NEWLINE, "Expected newline after assert");

    return std::make_unique<AssertStmt>(keyword, std::move(condition), std::move(message));
}

std::vector<Stmt> Parser::block() {
    std::vector<Stmt> statements;

    while (!check(TokenType::DEDENT) && !isAtEnd()) {
        skipNewlines();
        if (check(TokenType::DEDENT)) break;
        statements.push_back(declaration());
    }

    if (!isAtEnd()) {
        consume(TokenType::DEDENT, "Expected dedent at end of block");
    }

    return statements;
}

// Expression parsing

Expr Parser::expression() {
    return assignment();
}

Expr Parser::assignment() {
    Expr expr = orExpr();

    if (match(TokenType::ASSIGN)) {
        Token equals = previous();
        Expr value = assignment();

        if (std::holds_alternative<std::unique_ptr<VariableExpr>>(expr)) {
            auto& varExpr = std::get<std::unique_ptr<VariableExpr>>(expr);
            Token name = varExpr->name;
            return std::make_unique<AssignExpr>(name, std::move(value));
        }

        throw error(equals, "Invalid assignment target");
    }

    return expr;
}

Expr Parser::orExpr() {
    Expr expr = andExpr();

    while (match(TokenType::OR)) {
        Token op = previous();
        Expr right = andExpr();
        expr = std::make_unique<BinaryExpr>(std::move(expr), op, std::move(right));
    }

    return expr;
}

Expr Parser::andExpr() {
    Expr expr = notExpr();

    while (match(TokenType::AND)) {
        Token op = previous();
        Expr right = notExpr();
        expr = std::make_unique<BinaryExpr>(std::move(expr), op, std::move(right));
    }

    return expr;
}

Expr Parser::notExpr() {
    if (match(TokenType::NOT)) {
        Token op = previous();
        Expr operand = notExpr();
        return std::make_unique<UnaryExpr>(op, std::move(operand));
    }

    return comparison();
}

Expr Parser::comparison() {
    Expr expr = term();

    while (match(TokenType::LT, TokenType::LE, TokenType::GT,
                 TokenType::GE, TokenType::EQ, TokenType::NE)) {
        Token op = previous();
        Expr right = term();
        expr = std::make_unique<BinaryExpr>(std::move(expr), op, std::move(right));
    }

    return expr;
}

Expr Parser::term() {
    Expr expr = factor();

    while (match(TokenType::PLUS, TokenType::MINUS)) {
        Token op = previous();
        Expr right = factor();
        expr = std::make_unique<BinaryExpr>(std::move(expr), op, std::move(right));
    }

    return expr;
}

Expr Parser::factor() {
    Expr expr = unary();

    while (match(TokenType::STAR, TokenType::SLASH,
                 TokenType::DOUBLE_SLASH, TokenType::PERCENT)) {
        Token op = previous();
        Expr right = unary();
        expr = std::make_unique<BinaryExpr>(std::move(expr), op, std::move(right));
    }

    return expr;
}

Expr Parser::unary() {
    if (match(TokenType::MINUS)) {
        Token op = previous();
        Expr operand = unary();
        return std::make_unique<UnaryExpr>(op, std::move(operand));
    }

    return power();
}

Expr Parser::power() {
    Expr expr = call();

    if (match(TokenType::DOUBLE_STAR)) {
        Token op = previous();
        Expr right = unary(); // Right-associative
        expr = std::make_unique<BinaryExpr>(std::move(expr), op, std::move(right));
    }

    return expr;
}

Expr Parser::call() {
    Expr expr = primary();

    while (true) {
        if (match(TokenType::LPAREN)) {
            expr = finishCall(std::move(expr));
        } else {
            break;
        }
    }

    return expr;
}

Expr Parser::finishCall(Expr callee) {
    std::vector<Expr> arguments;

    if (!check(TokenType::RPAREN)) {
        do {
            arguments.push_back(expression());
        } while (match(TokenType::COMMA));
    }

    Token paren = consume(TokenType::RPAREN, "Expected ')' after arguments");

    return std::make_unique<CallExpr>(std::move(callee), paren, std::move(arguments));
}

Expr Parser::primary() {
    if (match(TokenType::TRUE)) {
        return std::make_unique<LiteralExpr>(true);
    }
    if (match(TokenType::FALSE)) {
        return std::make_unique<LiteralExpr>(false);
    }
    if (match(TokenType::NONE)) {
        return std::make_unique<LiteralExpr>(PyNone{});
    }

    if (match(TokenType::INTEGER)) {
        return std::make_unique<LiteralExpr>(
            std::get<long long>(previous().literal));
    }
    if (match(TokenType::FLOAT)) {
        return std::make_unique<LiteralExpr>(
            std::get<double>(previous().literal));
    }
    if (match(TokenType::STRING)) {
        return std::make_unique<LiteralExpr>(
            std::get<std::string>(previous().literal));
    }

    if (match(TokenType::IDENTIFIER)) {
        return std::make_unique<VariableExpr>(previous());
    }

    if (match(TokenType::LPAREN)) {
        Expr expr = expression();
        consume(TokenType::RPAREN, "Expected ')' after expression");
        return std::make_unique<GroupingExpr>(std::move(expr));
    }

    throw error(peek(), "Expected expression");
}
