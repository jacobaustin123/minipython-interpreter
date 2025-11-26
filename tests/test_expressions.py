# Test complex expressions
assert (1 + 2) * (3 + 4) == 21
assert 2 ** 3 ** 2 == 512  # Right associative: 2^(3^2) = 2^9 = 512
assert 10 - 5 - 2 == 3  # Left associative
assert 100 / 10 / 2 == 5.0

# Test mixed operations
assert 1 + 2 * 3 - 4 == 3
assert (1 + 2) * (3 - 4) == -3
assert 2 * 3 + 4 * 5 == 26

# Test comparison chaining (evaluated left to right)
assert (1 < 2) == True
assert (2 > 1) == True
assert (1 == 1) == True

# Test logical operators with values
x = 5
y = 0
assert (x and y) == 0
assert (x or y) == 5
assert (y or x) == 5

# Test short-circuit evaluation
def returns_true():
    return True

def returns_false():
    return False

assert returns_true() or returns_false()
assert not (returns_false() and returns_true())

# Test parentheses
assert (((1 + 2))) == 3
assert ((1 + 2) * 3) == 9

# Test unary operators
assert -(-5) == 5
assert --5 == 5
assert not not True
assert not not 1

print("test_expressions.py: All tests passed!")
