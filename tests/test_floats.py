# Test floating point operations
assert 3.14 > 3.0
assert 2.5 + 1.5 == 4.0
assert 5.0 - 2.0 == 3.0
assert 2.5 * 4.0 == 10.0
assert 7.5 / 2.5 == 3.0

# Test mixed int/float operations
assert 3 + 2.5 == 5.5
assert 10 / 4 == 2.5
assert 10.0 // 3.0 == 3.0

# Test float comparisons
assert 1.5 < 2.0
assert 2.0 > 1.5
assert 2.0 >= 2.0
assert 2.0 <= 2.0

# Test negative floats
assert -1.5 + 3.0 == 1.5
assert -2.5 * 2.0 == -5.0

print("test_floats.py: All tests passed!")
