#include "lexer.hpp"
#include <cctype>
#include <sstream>

const std::unordered_map<std::string, TokenType> Lexer::keywords = {
    {"def", TokenType::DEF},
    {"return", TokenType::RETURN},
    {"if", TokenType::IF},
    {"elif", TokenType::ELIF},
    {"else", TokenType::ELSE},
    {"while", TokenType::WHILE},
    {"for", TokenType::FOR},
    {"in", TokenType::IN},
    {"and", TokenType::AND},
    {"or", TokenType::OR},
    {"not", TokenType::NOT},
    {"True", TokenType::TRUE},
    {"False", TokenType::FALSE},
    {"None", TokenType::NONE},
    {"print", TokenType::PRINT},
    {"assert", TokenType::ASSERT}
};

Lexer::Lexer(std::string source) : source(std::move(source)) {
    indentStack.push_back(0);
}

bool Lexer::isAtEnd() const {
    return current >= source.length();
}

char Lexer::peek() const {
    if (isAtEnd()) return '\0';
    return source[current];
}

char Lexer::peekNext() const {
    if (current + 1 >= source.length()) return '\0';
    return source[current + 1];
}

char Lexer::advance() {
    char c = source[current++];
    column++;
    return c;
}

bool Lexer::match(char expected) {
    if (isAtEnd()) return false;
    if (source[current] != expected) return false;
    current++;
    column++;
    return true;
}

void Lexer::error(const std::string& message) {
    throw LexerError(message, line, startColumn);
}

void Lexer::addToken(TokenType type) {
    std::string text = source.substr(start, current - start);
    tokens.emplace_back(type, text, line, startColumn);
}

void Lexer::addToken(TokenType type, long long value) {
    std::string text = source.substr(start, current - start);
    tokens.emplace_back(type, text, value, line, startColumn);
}

void Lexer::addToken(TokenType type, double value) {
    std::string text = source.substr(start, current - start);
    tokens.emplace_back(type, text, value, line, startColumn);
}

void Lexer::addToken(TokenType type, const std::string& value) {
    std::string text = source.substr(start, current - start);
    tokens.emplace_back(type, text, value, line, startColumn);
}

void Lexer::handleIndentation() {
    int indent = 0;
    while (!isAtEnd() && (peek() == ' ' || peek() == '\t')) {
        if (peek() == ' ') {
            indent++;
        } else {
            // Tab counts as 8 spaces (simplified)
            indent += 8;
        }
        advance();
    }

    // Skip blank lines and comment-only lines
    if (isAtEnd() || peek() == '\n' || peek() == '#') {
        return;
    }

    int currentIndent = indentStack.back();

    if (indent > currentIndent) {
        indentStack.push_back(indent);
        start = current;
        startColumn = column;
        tokens.emplace_back(TokenType::INDENT, "", line, startColumn);
    } else if (indent < currentIndent) {
        while (!indentStack.empty() && indentStack.back() > indent) {
            indentStack.pop_back();
            start = current;
            startColumn = column;
            tokens.emplace_back(TokenType::DEDENT, "", line, startColumn);
        }
        if (indentStack.empty() || indentStack.back() != indent) {
            error("Inconsistent indentation");
        }
    }

    atLineStart = false;
}

void Lexer::number() {
    bool isFloat = false;

    while (std::isdigit(peek())) advance();

    // Check for decimal point
    if (peek() == '.' && std::isdigit(peekNext())) {
        isFloat = true;
        advance(); // consume '.'
        while (std::isdigit(peek())) advance();
    }

    // Check for exponent
    if (peek() == 'e' || peek() == 'E') {
        isFloat = true;
        advance();
        if (peek() == '+' || peek() == '-') advance();
        if (!std::isdigit(peek())) {
            error("Invalid number: expected digit after exponent");
        }
        while (std::isdigit(peek())) advance();
    }

    std::string numStr = source.substr(start, current - start);
    if (isFloat) {
        addToken(TokenType::FLOAT, std::stod(numStr));
    } else {
        addToken(TokenType::INTEGER, std::stoll(numStr));
    }
}

void Lexer::identifier() {
    while (std::isalnum(peek()) || peek() == '_') advance();

    std::string text = source.substr(start, current - start);
    auto it = keywords.find(text);
    if (it != keywords.end()) {
        addToken(it->second);
    } else {
        addToken(TokenType::IDENTIFIER);
    }
}

void Lexer::string(char quote) {
    std::string value;

    while (!isAtEnd() && peek() != quote) {
        if (peek() == '\n') {
            error("Unterminated string");
        }
        if (peek() == '\\') {
            advance(); // consume backslash
            if (isAtEnd()) {
                error("Unterminated string");
            }
            char escaped = advance();
            switch (escaped) {
                case 'n': value += '\n'; break;
                case 't': value += '\t'; break;
                case 'r': value += '\r'; break;
                case '\\': value += '\\'; break;
                case '\'': value += '\''; break;
                case '"': value += '"'; break;
                default: value += escaped; break;
            }
        } else {
            value += advance();
        }
    }

    if (isAtEnd()) {
        error("Unterminated string");
    }

    advance(); // closing quote
    addToken(TokenType::STRING, value);
}

void Lexer::skipComment() {
    while (!isAtEnd() && peek() != '\n') {
        advance();
    }
}

void Lexer::scanToken() {
    start = current;
    startColumn = column;

    char c = advance();

    switch (c) {
        case '(': addToken(TokenType::LPAREN); break;
        case ')': addToken(TokenType::RPAREN); break;
        case ':': addToken(TokenType::COLON); break;
        case ',': addToken(TokenType::COMMA); break;

        case '+':
            if (match('=')) addToken(TokenType::PLUS_ASSIGN);
            else addToken(TokenType::PLUS);
            break;
        case '-':
            if (match('=')) addToken(TokenType::MINUS_ASSIGN);
            else addToken(TokenType::MINUS);
            break;
        case '*':
            if (match('*')) addToken(TokenType::DOUBLE_STAR);
            else if (match('=')) addToken(TokenType::STAR_ASSIGN);
            else addToken(TokenType::STAR);
            break;
        case '/':
            if (match('/')) addToken(TokenType::DOUBLE_SLASH);
            else if (match('=')) addToken(TokenType::SLASH_ASSIGN);
            else addToken(TokenType::SLASH);
            break;
        case '%': addToken(TokenType::PERCENT); break;

        case '=':
            if (match('=')) addToken(TokenType::EQ);
            else addToken(TokenType::ASSIGN);
            break;
        case '!':
            if (match('=')) addToken(TokenType::NE);
            else error("Unexpected character '!'");
            break;
        case '<':
            if (match('=')) addToken(TokenType::LE);
            else addToken(TokenType::LT);
            break;
        case '>':
            if (match('=')) addToken(TokenType::GE);
            else addToken(TokenType::GT);
            break;

        case '#':
            skipComment();
            break;

        case ' ':
        case '\t':
        case '\r':
            // Ignore whitespace (indentation already handled)
            break;

        case '\n':
            // Only add NEWLINE if there's meaningful content before it
            if (!tokens.empty() && tokens.back().type != TokenType::NEWLINE &&
                tokens.back().type != TokenType::INDENT) {
                addToken(TokenType::NEWLINE);
            }
            line++;
            column = 1;
            atLineStart = true;
            break;

        case '"':
        case '\'':
            string(c);
            break;

        default:
            if (std::isdigit(c)) {
                number();
            } else if (std::isalpha(c) || c == '_') {
                identifier();
            } else {
                error("Unexpected character");
            }
            break;
    }
}

std::vector<Token> Lexer::tokenize() {
    while (!isAtEnd()) {
        if (atLineStart) {
            handleIndentation();
            if (isAtEnd()) break;
            if (peek() == '\n' || peek() == '#') {
                // Empty line or comment-only line
                if (peek() == '#') skipComment();
                if (!isAtEnd() && peek() == '\n') {
                    advance();
                    line++;
                    column = 1;
                }
                continue;
            }
        }
        scanToken();
    }

    // Add remaining DEDENTs
    while (indentStack.size() > 1) {
        indentStack.pop_back();
        tokens.emplace_back(TokenType::DEDENT, "", line, column);
    }

    // Add final NEWLINE if needed
    if (!tokens.empty() && tokens.back().type != TokenType::NEWLINE) {
        tokens.emplace_back(TokenType::NEWLINE, "", line, column);
    }

    tokens.emplace_back(TokenType::END_OF_FILE, "", line, column);
    return tokens;
}
