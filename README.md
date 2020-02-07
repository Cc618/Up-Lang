# Up-Lang

Up is a programming language with Python like syntax, static typing and is transpiled in C.

## Up syntax

```
# This is an inline comment

# Create a variable with type int
int a = 42

# Create a variable with type automaticaly deducted
$b = 38.2

# Some operations
a++
a += 8

# Condition
a > 0 ?
    # Function call
    Exit()

# Loops
for i to 618:
    Log(i)
for i=3 to 8:
    ...
for i=10 to 1:
    ..

# yes means true
while yes:
    ...

# Same as
loop:
    ...

# Functions
int MyFunc(int a, int b=6)
    ...
    ret 42
```

## Depedencies

Up targets linux platforms, some programs are required to compile Up :

- make
- gcc
- g++
- flex
- bison
