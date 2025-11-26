# MiniPython Interpreter

A simple Python interpreter written in C++17. Implements a tree-walking interpreter with lexer, recursive descent parser, and AST evaluator.

## Features

- **Data types**: integers, floats, strings, booleans, None
- **Arithmetic**: `+`, `-`, `*`, `/`, `//` (floor div), `%`, `**` (power)
- **Comparisons**: `==`, `!=`, `<`, `<=`, `>`, `>=`
- **Boolean logic**: `and`, `or`, `not`
- **Variables**: assignment, compound assignment (`+=`, `-=`, etc.)
- **Control flow**: `if`/`elif`/`else`, `while` loops
- **Functions**: `def`, `return`, recursion, closures
- **Built-ins**: `print`, `assert`
- **Python-style indentation** with INDENT/DEDENT tokens

## Building

```bash
make
```

## Usage

**REPL mode:**
```bash
./pyinterp
```

**Execute a file:**
```bash
./pyinterp script.py
```

## Example

```python
def fib(n):
    if n <= 1:
        return n
    return fib(n - 1) + fib(n - 2)

print(fib(10))  # 55
```

## Testing

```bash
make test          # Run all tests
make test-cpp      # C++ unit tests only
make test-python   # Python integration tests only
```

## Project Structure

```
├── token.hpp        # Token types and Token struct
├── lexer.hpp/cpp    # Tokenizer with indentation handling
├── ast.hpp          # AST node definitions, PyValue type
├── parser.hpp/cpp   # Recursive descent parser
├── environment.hpp  # Variable scoping
├── interpreter.hpp/cpp  # Tree-walking evaluator
├── main.cpp         # REPL and file execution
└── tests/           # C++ and Python tests
```
