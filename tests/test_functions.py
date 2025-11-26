# Test basic function definition and call
def add(a, b):
    return a + b

assert add(2, 3) == 5
assert add(10, 20) == 30
assert add(-5, 5) == 0

# Test function with no arguments
def get_five():
    return 5

assert get_five() == 5

# Test function with no return (returns None)
def no_return():
    x = 1

assert no_return() == None

# Test recursive function
def factorial(n):
    if n <= 1:
        return 1
    return n * factorial(n - 1)

assert factorial(0) == 1
assert factorial(1) == 1
assert factorial(5) == 120
assert factorial(6) == 720

# Test fibonacci
def fib(n):
    if n <= 1:
        return n
    return fib(n - 1) + fib(n - 2)

assert fib(0) == 0
assert fib(1) == 1
assert fib(2) == 1
assert fib(5) == 5
assert fib(10) == 55

# Test nested function calls
def double(x):
    return x * 2

def quadruple(x):
    return double(double(x))

assert quadruple(3) == 12

# Test function with multiple parameters
def sum3(a, b, c):
    return a + b + c

assert sum3(1, 2, 3) == 6

print("test_functions.py: All tests passed!")
