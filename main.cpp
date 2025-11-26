#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include "lexer.hpp"
#include "parser.hpp"
#include "interpreter.hpp"

int runFile(const std::string& path, Interpreter& interpreter);
void runRepl(Interpreter& interpreter);
void run(const std::string& source, Interpreter& interpreter, bool isRepl = false);

int main(int argc, char* argv[]) {
    Interpreter interpreter;

    if (argc > 2) {
        std::cerr << "Usage: pyinterp [script]" << std::endl;
        return 1;
    } else if (argc == 2) {
        return runFile(argv[1], interpreter);
    } else {
        runRepl(interpreter);
    }

    return 0;
}

int runFile(const std::string& path, Interpreter& interpreter) {
    std::ifstream file(path);
    if (!file) {
        std::cerr << "Error: Could not open file '" << path << "'" << std::endl;
        return 1;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();

    try {
        run(buffer.str(), interpreter);
    } catch (const AssertionError&) {
        return 1;  // Test failed
    }
    return 0;
}

void runRepl(Interpreter& interpreter) {
    std::cout << "MiniPython Interpreter v0.1" << std::endl;
    std::cout << "Type 'exit()' or Ctrl+D to quit" << std::endl;
    std::cout << std::endl;

    std::string line;
    std::string buffer;
    bool inBlock = false;
    int indentLevel = 0;

    while (true) {
        if (inBlock) {
            std::cout << "... ";
        } else {
            std::cout << ">>> ";
        }
        std::cout.flush();

        if (!std::getline(std::cin, line)) {
            std::cout << std::endl;
            break;
        }

        // Check for exit command
        if (!inBlock && (line == "exit()" || line == "quit()")) {
            break;
        }

        // Handle multi-line input (for function definitions, if statements, etc.)
        if (line.empty()) {
            if (inBlock) {
                // Empty line ends the block
                inBlock = false;
                run(buffer, interpreter, true);
                buffer.clear();
                indentLevel = 0;
            }
            continue;
        }

        // Check if this line starts a block
        bool startsBlock = false;
        if (!line.empty()) {
            // Trim trailing whitespace for checking
            std::string trimmed = line;
            while (!trimmed.empty() && (trimmed.back() == ' ' || trimmed.back() == '\t')) {
                trimmed.pop_back();
            }
            startsBlock = !trimmed.empty() && trimmed.back() == ':';
        }

        // Count leading spaces
        int currentIndent = 0;
        for (char c : line) {
            if (c == ' ') currentIndent++;
            else if (c == '\t') currentIndent += 4;
            else break;
        }

        if (inBlock) {
            buffer += line + "\n";
            if (startsBlock) {
                indentLevel++;
            }
        } else if (startsBlock) {
            inBlock = true;
            indentLevel = 1;
            buffer = line + "\n";
        } else {
            // Single line execution
            run(line, interpreter, true);
        }
    }
}

void run(const std::string& source, Interpreter& interpreter, bool isRepl) {
    try {
        Lexer lexer(source);
        std::vector<Token> tokens = lexer.tokenize();

        Parser parser(tokens);
        std::vector<Stmt> statements = parser.parse();

        interpreter.clearLastValue();
        interpreter.interpret(std::move(statements));

        // In REPL mode, print the result of expressions
        if (isRepl && interpreter.hasLastValue()) {
            PyValue value = interpreter.getLastValue();
            // Don't print None for expression statements in REPL
            if (!std::holds_alternative<PyNone>(value)) {
                std::cout << pyValueToString(value) << std::endl;
            }
        }
    } catch (const LexerError& e) {
        std::cerr << "Lexer Error [line " << e.line << ", col " << e.column << "]: "
                  << e.what() << std::endl;
    } catch (const ParseError& e) {
        std::cerr << e.what() << std::endl;
    } catch (const AssertionError& e) {
        std::cerr << e.what();
        if (e.line > 0) {
            std::cerr << " (line " << e.line << ")";
        }
        std::cerr << std::endl;
        throw;  // Re-throw to signal test failure
    } catch (const RuntimeError& e) {
        std::cerr << "Runtime Error";
        if (e.line > 0) {
            std::cerr << " [line " << e.line << "]";
        }
        std::cerr << ": " << e.what() << std::endl;
    }
}
