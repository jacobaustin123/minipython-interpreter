#!/bin/bash

# Test runner for the MiniPython interpreter
# Usage: ./run_tests.sh

set -e

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
PYINTERP="$SCRIPT_DIR/pyinterp"
TEST_DIR="$SCRIPT_DIR/tests"

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Build the interpreter first
echo "Building interpreter..."
make -C "$SCRIPT_DIR" > /dev/null 2>&1
echo ""

# Count results
PASSED=0
FAILED=0
TOTAL=0

echo "Running tests..."
echo "================"
echo ""

for test_file in "$TEST_DIR"/test_*.py; do
    if [ -f "$test_file" ]; then
        test_name=$(basename "$test_file")
        TOTAL=$((TOTAL + 1))

        if "$PYINTERP" "$test_file" > /dev/null 2>&1; then
            echo -e "${GREEN}✓ PASS${NC}: $test_name"
            PASSED=$((PASSED + 1))
        else
            echo -e "${RED}✗ FAIL${NC}: $test_name"
            # Run again to show the error
            "$PYINTERP" "$test_file" 2>&1 | head -5
            FAILED=$((FAILED + 1))
        fi
    fi
done

echo ""
echo "================"
echo -e "Results: ${GREEN}$PASSED passed${NC}, ${RED}$FAILED failed${NC}, $TOTAL total"

if [ $FAILED -eq 0 ]; then
    echo -e "${GREEN}All tests passed!${NC}"
    exit 0
else
    echo -e "${RED}Some tests failed!${NC}"
    exit 1
fi
