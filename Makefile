CXX = clang++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2

TARGET = pyinterp
SOURCES = main.cpp lexer.cpp parser.cpp interpreter.cpp
HEADERS = token.hpp lexer.hpp parser.hpp ast.hpp environment.hpp interpreter.hpp
OBJECTS = $(SOURCES:.cpp=.o)

# Test targets
TEST_LEXER = tests/test_lexer
TEST_PARSER = tests/test_parser

.PHONY: all clean run test test-lexer test-parser test-cpp test-python

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $^

%.o: %.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -c -o $@ $<

clean:
	rm -f $(TARGET) $(OBJECTS) $(TEST_LEXER) $(TEST_PARSER)

run: $(TARGET)
	./$(TARGET)

# Debug build
debug: CXXFLAGS = -std=c++17 -Wall -Wextra -g -O0 -DDEBUG
debug: clean $(TARGET)

# C++ Unit Tests
$(TEST_LEXER): tests/test_lexer.cpp lexer.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -o $@ tests/test_lexer.cpp lexer.cpp

$(TEST_PARSER): tests/test_parser.cpp lexer.cpp parser.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -o $@ tests/test_parser.cpp lexer.cpp parser.cpp

test-lexer: $(TEST_LEXER)
	./$(TEST_LEXER)

test-parser: $(TEST_PARSER)
	./$(TEST_PARSER)

test-cpp: test-lexer test-parser

test-python: $(TARGET)
	./run_tests.sh

test: test-cpp test-python
