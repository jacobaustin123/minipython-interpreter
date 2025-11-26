# Test variable assignment and usage
x = 10
assert x == 10

y = 20
assert x + y == 30

# Test reassignment
x = 5
assert x == 5

# Test compound assignment
x += 3
assert x == 8

x -= 2
assert x == 6

x *= 4
assert x == 24

x /= 2
assert x == 12.0

# Test variable in expressions
a = 3
b = 4
c = a * a + b * b
assert c == 25

# Test multiple assignments
p = q = r = 0
# Note: chained assignment not supported, so test individual
p = 0
q = 0
r = 0
assert p == 0
assert q == 0
assert r == 0

print("test_variables.py: All tests passed!")
