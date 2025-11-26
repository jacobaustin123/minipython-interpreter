# Test boolean literals
assert True
assert not False

# Test boolean operations
assert True and True
assert not (True and False)
assert not (False and True)
assert not (False and False)

assert True or True
assert True or False
assert False or True
assert not (False or False)

assert not False
assert not not True

# Test comparison operators
assert 1 < 2
assert 2 > 1
assert 1 <= 1
assert 1 >= 1
assert 1 == 1
assert 1 != 2

# Test truthiness
assert 1
assert not 0
assert "hello"
assert not ""

# Test None
assert None == None
assert not None

print("test_booleans.py: All tests passed!")
