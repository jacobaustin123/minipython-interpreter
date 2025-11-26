# Test if statements
x = 10
if x > 5:
    result = "big"
else:
    result = "small"
assert result == "big"

x = 3
if x > 5:
    result = "big"
else:
    result = "small"
assert result == "small"

# Test elif
def classify(n):
    if n < 0:
        return "negative"
    elif n == 0:
        return "zero"
    elif n < 10:
        return "small"
    else:
        return "big"

assert classify(-5) == "negative"
assert classify(0) == "zero"
assert classify(5) == "small"
assert classify(100) == "big"

# Test nested if
def nested_test(a, b):
    if a > 0:
        if b > 0:
            return "both positive"
        else:
            return "a positive, b not"
    else:
        if b > 0:
            return "b positive, a not"
        else:
            return "neither positive"

assert nested_test(1, 1) == "both positive"
assert nested_test(1, -1) == "a positive, b not"
assert nested_test(-1, 1) == "b positive, a not"
assert nested_test(-1, -1) == "neither positive"

# Test while loop
def sum_to_n(n):
    total = 0
    i = 1
    while i <= n:
        total += i
        i += 1
    return total

assert sum_to_n(1) == 1
assert sum_to_n(5) == 15
assert sum_to_n(10) == 55

# Test while with condition
def count_digits(n):
    count = 0
    while n > 0:
        count += 1
        n = n // 10
    return count

assert count_digits(1) == 1
assert count_digits(123) == 3
assert count_digits(99999) == 5

print("test_control_flow.py: All tests passed!")
