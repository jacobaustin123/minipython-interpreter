# Test string literals
s = "Hello"
assert s == "Hello"

# Test string concatenation
first = "Hello"
second = "World"
assert first + " " + second == "Hello World"

# Test string repetition
assert "ab" * 3 == "ababab"
assert "-" * 5 == "-----"

# Test empty string
empty = ""
assert empty == ""

# Test string comparison
assert "abc" == "abc"
assert "abc" != "def"

# Test single quotes
t = 'single'
assert t == "single"

print("test_strings.py: All tests passed!")
