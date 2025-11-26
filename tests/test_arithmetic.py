# Test basic arithmetic operations
assert 2 + 3 == 5
assert 10 - 4 == 6
assert 6 * 7 == 42
assert 15 / 3 == 5.0
assert 15 // 4 == 3
assert 17 % 5 == 2
assert 2 ** 10 == 1024

# Test operator precedence
assert 2 + 3 * 4 == 14
assert (2 + 3) * 4 == 20
assert 10 - 2 - 3 == 5

# Test negative numbers
assert -5 == -5
assert -5 + 10 == 5
assert 3 * -2 == -6

# Test division
assert 10 / 4 == 2.5
assert 7 // 2 == 3
assert -7 // 2 == -4

# Test power
assert 2 ** 3 == 8
assert 3 ** 2 == 9
assert 2 ** 0 == 1

print("test_arithmetic.py: All tests passed!")
